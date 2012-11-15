
#ifdef NACL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <map>
#include <sstream>
#include <pthread.h>


#include <cassert>
#include <cmath>
#include <cstring>
#include <string>
#include <sys/time.h>
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_array_buffer.h"

#include "SaltyNES.h"

using namespace std;

// Globals
SaltyNES* SaltyNES::g_salty_nes = NULL;
const int kPthreadMutexSuccess = 0;
const float SaltyNES::AXES_DEAD_ZONE = 0.2;
const string SaltyNES::KEYS[] = { "up", "down", "right", "left", "start", "select", "a", "b" };
const size_t SaltyNES::KEYS_LENGTH = 8;

// This is called by the browser when the 2D context has been flushed to the
// browser window.
void FlushCallback(void* data, int32_t result) {
	static_cast<SaltyNES*>(data)->set_flush_pending(false);
}

void AudioCallback(void* samples, uint32_t buffer_size, void* data) {
	SaltyNES* salty_nes = reinterpret_cast<SaltyNES*>(data);
	PAPU* papu = salty_nes->vnes->nes->papu;

	if(papu == NULL || !papu->ready_for_buffer_write)
		return;

	const uint32_t channels = papu->stereo ? 2 : 1;
	uint8_t* buff = reinterpret_cast<uint8_t*>(samples);

	// Make sure we can't write outside the buffer.
	assert(buffer_size >= (sizeof(*buff) * channels * salty_nes->sample_frame_count_));

	size_t mix_len = buffer_size > ((size_t) papu->bufferIndex) ? ((size_t) papu->bufferIndex) : buffer_size;
	for(size_t i=0; i<mix_len; i++) {
		buff[i] = (*papu->sampleBuffer)[i];
	}

	papu->ready_for_buffer_write = false;
	papu->bufferIndex = 0;
}


// A small helper RAII class that implementes a scoped pthread_mutex lock.
class ScopedMutexLock {
	pthread_mutex_t* mutex_; // Weak reference.

public:
	explicit ScopedMutexLock(pthread_mutex_t* mutex) : 
		mutex_(mutex) {
		if(pthread_mutex_lock(mutex_) != kPthreadMutexSuccess) {
			mutex_ = NULL;
		}
	}
	
	~ScopedMutexLock() {
		if (mutex_)
			pthread_mutex_unlock(mutex_);
	}
	
	bool is_valid() const {
		return mutex_ != NULL;
	}
};

// A small helper RAII class used to acquire and release the pixel lock.
class ScopedPixelLock {
	SaltyNES* image_owner_;	// Weak reference.
	uint32_t* pixels_;	// Weak reference.

	ScopedPixelLock();	// Not implemented, do not use.

public:
	explicit ScopedPixelLock(SaltyNES* image_owner) : 
		image_owner_(image_owner), 
		pixels_(image_owner->LockPixels()) {
	}

	~ScopedPixelLock() {
		pixels_ = NULL;
		image_owner_->UnlockPixels();
	}

	uint32_t* pixels() const {
		return pixels_;
	}
};

SaltyNES::SaltyNES(PP_Instance instance)
		: pp::Instance(instance),
			graphics_2d_context_(NULL),
			pixel_buffer_(NULL),
			flush_pending_(false),
			quit_(false),
			thread_(0),
			thread_is_running_(false) {
	pthread_mutex_init(&pixel_buffer_mutex_, NULL);
	vnes = NULL;

	if(SaltyNES::g_salty_nes == NULL)
		SaltyNES::g_salty_nes = this;

	_is_gamepad_connected = false;
	_is_gamepad_used = false;
	_is_keyboard_used = false;

	_is_input_pressed["up"] = false;
	_is_input_pressed["down"] = false;
	_is_input_pressed["right"] = false;
	_is_input_pressed["left"] = false;
	_is_input_pressed["start"] = false;
	_is_input_pressed["select"] = false;
	_is_input_pressed["a"] = false;
	_is_input_pressed["b"] = false;

	pp::Module* module = pp::Module::Get();
	assert(module);
	gamepad_ = static_cast<const PPB_Gamepad*>(
		module->GetBrowserInterface(PPB_GAMEPAD_INTERFACE));
	assert(gamepad_);
	
	frequency_ = 440;
}

SaltyNES::~SaltyNES() {
	quit_ = true;

	if(vnes) {
		vnes->stop();
		if(thread_is_running_) {
			pthread_join(thread_, NULL);
			thread_is_running_ = false;
		}
		delete_n_null(vnes);
	}
	
	DestroyContext();
	delete_n_null(pixel_buffer_);
	pthread_mutex_destroy(&pixel_buffer_mutex_);
}

void SaltyNES::DidChangeView(const pp::View& view) {
	pp::Rect position = view.GetRect();
	if (pixel_buffer_ && position.size() == pixel_buffer_->size())
		return;	// Size didn't change, no need to update anything.

	// Create a new device context with the new size.
	DestroyContext();
	CreateContext(position.size());
	// Delete the old pixel buffer and create a new one.
	ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
	delete_n_null(pixel_buffer_);
	if (graphics_2d_context_ != NULL) {
		pixel_buffer_ = new pp::ImageData(
			this,
			PP_IMAGEDATAFORMAT_BGRA_PREMUL,
			graphics_2d_context_->size(),
			false
		);
	}
}

bool SaltyNES::Init(uint32_t argc, const char* argn[], const char* argv[]) {
	// Ask the device for an appropriate sample count size.
	const uint32_t desired_sample_frame_count = 2048; // Same as PAPU::bufferSize
	sample_frame_count_ = pp::AudioConfig::RecommendSampleFrameCount(
		this,
		PP_AUDIOSAMPLERATE_44100,
		desired_sample_frame_count
	);
	
	pp::AudioConfig config(
		this,
		PP_AUDIOSAMPLERATE_44100,
		sample_frame_count_
	);
	
	audio_ = pp::Audio(
		this,
		config,
		AudioCallback,
		this
	);

	return true;
}

void SaltyNES::HandleMessage(const pp::Var& var_message) {
	string message;
	if(var_message.is_string())
		message = var_message.AsString();

	// Handle messages that work with vnes running or not
	if(message.find("load_rom:") == 0) {
		// Convert the rom from base64 to bytes
		size_t sep_pos = message.find_first_of(":");
		string rom_base64 = message.substr(sep_pos + 1);
		string rom_data = base64_decode(rom_base64);

		// Make sure the ROM is valid
		if(rom_data[0] != 'N' || rom_data[1] != 'E' || rom_data[2] != 'S' || rom_data[3] != 0x1A) {
			log_to_browser("Invalid ROM file!");
		} else {
			// Stop any previously running NES
			if(vnes) {
				vnes->stop();
				if(thread_) {
					pthread_join(thread_, NULL);
					thread_is_running_ = false;
				}
				delete_n_null(vnes);
			}

			// Run the ROM
			vnes = new vNES();
			vnes->init_data((uint8_t*) rom_data.c_str(), (size_t)rom_data.length(), this);
			vnes->pre_run_setup();
			log_to_browser("running");
			pthread_create(&thread_, NULL, start_main_loop, this);
			audio_.StartPlayback();
			thread_is_running_ = true;
		}
		
		return;
		
	} else if(message == "get_gamepad_status") {
		// Get current gamepad data.
		PP_GamepadsSampleData gamepad_data;
		gamepad_->Sample(pp_instance(), &gamepad_data);
		_is_gamepad_connected = false;
		for(size_t i=0; i<gamepad_data.length; ++i) {
			if(gamepad_data.items[i].connected)
				_is_gamepad_connected = true;
		}
	
		// Tell the browser if the gamepad is connected or not
		stringstream out;
		out << "get_gamepad_status:";
		out << (_is_gamepad_connected ? "yes" : "no");
		out << ":" << _gamepad_vendor_id << _gamepad_product_id;
		log_to_browser(out.str());
		return;
	}

	// Just return if any messages require vnes, but it is not running
	if(vnes == NULL) return;

	// Handle messages that require vnes running
	if(message == "paint") {
		if(!vnes->nes->_is_paused)
			Paint();
	} else if(message == "pause") {
		vnes->nes->_is_paused = !vnes->nes->_is_paused;
	} else if(message.find("zoom:") == 0) {
		size_t sep_pos = message.find_first_of(":");
		int32_t zoom = 0;
		string str_zoom = message.substr(sep_pos + 1);
		istringstream ( str_zoom ) >> zoom;
		vnes->nes->ppu->_zoom = zoom;
	} else if(message.find("key_down:") == 0) {
		size_t sep_pos = message.find_first_of(":");
		int32_t button = 0;
		string str_button = message.substr(sep_pos + 1);
		istringstream ( str_button ) >> button;
		key_down(button);
	} else if(message.find("key_up:") == 0) {
		size_t sep_pos = message.find_first_of(":");
		int32_t button = 0;
		string str_button = message.substr(sep_pos + 1);
		istringstream ( str_button ) >> button;
		key_up(button);
	} else if(message == "get_fps") {
		stringstream out;
		out << "get_fps:";
		out << get_fps();
		log_to_browser(out.str());
	} else if(message.find("set_input_") == 0) {
		size_t sep_pos = message.find_first_of(":");
		size_t axes_pos = message.find("axes");
		size_t sign_pos = message.find_first_of("+-");
		string input = message.substr(10, sep_pos-10);

		// Axes
		if(axes_pos != string::npos && sign_pos != string::npos) {
			string str_axes = message.substr(sign_pos + 1);
			string sign = message.substr(sign_pos, 1);
			size_t axes = 0;
			istringstream ( str_axes ) >> axes;
			
			if(sign == "+") {
				_input_map_axes_pos[input].push_back(axes);
			} else if(sign == "-") {
				_input_map_axes_neg[input].push_back(axes);
			}
		// Button
		} else {
			string str_button = message.substr(sep_pos + 1);
			int button = 0;
			istringstream ( str_button ) >> button;
			_input_map_button[input].push_back(button);
		}
	} else if(message == "quit") {
		if(vnes != NULL) {
			audio_.StopPlayback();
			vnes->stop();
			if(thread_is_running_) {
				pthread_join(thread_, NULL);
				thread_is_running_ = false;
			}
			delete_n_null(vnes);
		}
		log_to_browser("quit");
	} else if(message == "get_sha256") {
		stringstream out;
		out << "get_sha256:";
		out << this->vnes->nes->memMapper->rom->_sha256;
		log_to_browser(out.str());
	} else {
		stringstream out;
		out << "unknown message:";
		out << message;
		log_to_browser(out.str());
	}

}

uint32_t* SaltyNES::LockPixels() {
	void* pixels = NULL;
	// Do not use a ScopedMutexLock here, since the lock needs to be held until
	// the matching UnlockPixels() call.
	if (pthread_mutex_lock(&pixel_buffer_mutex_) == kPthreadMutexSuccess) {
		if (pixel_buffer_ != NULL && !pixel_buffer_->is_null()) {
			pixels = pixel_buffer_->data();
		}
	}
	return reinterpret_cast<uint32_t*>(pixels);
}

void SaltyNES::UnlockPixels() const {
	pthread_mutex_unlock(&pixel_buffer_mutex_);
}

void SaltyNES::Paint() {
	ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
	if (!scoped_mutex.is_valid()) {
		return;
	}

	update_gamepad();
	
	FlushPixelBuffer();
}

void SaltyNES::update_gamepad() {
	// Get current gamepad data.
	PP_GamepadsSampleData gamepad_data;
	gamepad_->Sample(pp_instance(), &gamepad_data);
	
	for(size_t i=0; i<gamepad_data.length; ++i) {
		PP_GamepadSampleData& pad = gamepad_data.items[i];

		if(!pad.connected)
			continue;

		// Check if we are switching to the gamepad from the keyboard
		for(size_t j=0; j<pad.buttons_length; ++j) {
			if(pad.buttons[j]) {
				_is_gamepad_used = true;
				_is_keyboard_used = false;
			}
		}
		for(size_t j=0; j<pad.axes_length; ++j) {
			if(pad.axes[j] > AXES_DEAD_ZONE || pad.axes[j] < -AXES_DEAD_ZONE) {
				_is_gamepad_used = true;
				_is_keyboard_used = false;
			}
		}
		
		// Get the vendor, and product id
		if(_is_gamepad_used) {
			char id[128];
			for(int k=0; k<128; ++k) {
				id[k] = pad.id[k];
			}
			string sid = id;
			size_t vendor_pos = sid.find("Vendor: ");
			size_t product_pos = sid.find("Product: ");
			_gamepad_vendor_id = sid.substr(vendor_pos + strlen("Vendor: "), 4);
			_gamepad_product_id = sid.substr(product_pos + strlen("Product: "), 4);
		}

		// Get the key states
		if(_is_gamepad_used) {
			for(size_t j=0; j<KEYS_LENGTH; ++j) {
				string key = KEYS[j];
				_is_input_pressed[key] = false;
				// Button
				for(size_t k=0; k<_input_map_button[key].size(); ++k) {
					size_t number = _input_map_button[key][k];
					if(pad.buttons[number] > 0) {
						_is_input_pressed[key] = true;
					}
				}
				// Positive Axes
				for(size_t k=0; k<_input_map_axes_pos[key].size(); ++k) {
					size_t number = _input_map_axes_pos[key][k];
					if(pad.axes[number] > AXES_DEAD_ZONE) {
						_is_input_pressed[key] = true;
					}
				}
				// Negative Axes
				for(size_t k=0; k<_input_map_axes_neg[key].size(); ++k) {
					size_t number = _input_map_axes_neg[key][k];
					if(pad.axes[number] < -AXES_DEAD_ZONE) {
						_is_input_pressed[key] = true;
					}
				}
			}
		}
	}
}

void SaltyNES::CreateContext(const pp::Size& size) {
	ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
	if (!scoped_mutex.is_valid()) {
		return;
	}
	if (IsContextValid())
		return;
	graphics_2d_context_ = new pp::Graphics2D(this, size, false);
	if (!BindGraphics(*graphics_2d_context_)) {
		printf("Couldn't bind the device context\n");
	}
}

void SaltyNES::DestroyContext() {
	ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
	if (!scoped_mutex.is_valid()) {
		return;
	}
	if (!IsContextValid())
		return;
	delete_n_null(graphics_2d_context_);
}

void SaltyNES::FlushPixelBuffer() {
	if (!IsContextValid())
		return;
	// Note that the pixel lock is held while the buffer is copied into the
	// device context and then flushed.
	graphics_2d_context_->PaintImageData(*pixel_buffer_, pp::Point());
	if (flush_pending())
		return;
	set_flush_pending(true);
	graphics_2d_context_->Flush(pp::CompletionCallback(&FlushCallback, this));
}

void* SaltyNES::start_main_loop(void* param) {
	log_to_browser("start_main_loop");

	SaltyNES* salty_nes = reinterpret_cast<SaltyNES*>(param);
	salty_nes->vnes->run();
	return 0;
}

void SaltyNES::log(string message) {
	PostMessage(message);
}

#endif
