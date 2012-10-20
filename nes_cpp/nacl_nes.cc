
#ifdef NACL

#include <stdbool.h>
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

#include "nacl_nes.h"
#include "nes_cpp.h"

using namespace std;

uint8_t string_to_half_byte(uint8_t byte) {
	// '0'(48) through '9'(57)
	if(byte >= 48 && byte <= 57) {
		byte -= 48;
	// 'A'(65) through 'F'(70)
	} else if(byte >= 65 && byte <= 70) {
		byte -= 55;
	}
	return byte;
}

uint8_t string_to_byte(uint8_t upper, uint8_t lower) {
	uint8_t retval = 0;
	upper = string_to_half_byte(upper);
	lower = string_to_half_byte(lower);
	retval = ((uint8_t)(upper << 4)) | lower;
	return retval;
}

namespace {
	const int kPthreadMutexSuccess = 0;
	const int kMaxPointCount = 1000000000;  // The total number of points to draw.
	const uint32_t kOpaqueColorMask = 0xff000000;  // Opaque pixels.
	const uint32_t kRedMask = 0xff0000;
	const uint32_t kBlueMask = 0xff;
	const uint32_t kRedShift = 16;
	const uint32_t kBlueShift = 0;

	// This is called by the browser when the 2D context has been flushed to the
	// browser window.
	void FlushCallback(void* data, int32_t result) {
		static_cast<nacl_nes::NaclNes*>(data)->set_flush_pending(false);
	}
}


namespace nacl_nes {
	NaclNes* NaclNes::g_nacl_nes = NULL;
	
	void NaclNes::log_to_browser(string message) {
		if(g_nacl_nes != NULL)
			g_nacl_nes->PostMessage(message.c_str());
	}
	
	// Declare Variables
	uint64_t g_frame_start = 0;
	uint64_t g_frame_end = 0;
	uint64_t g_fps_start = 0;
	static vNES* vnes;

// A small helper RAII class that implementes a scoped pthread_mutex lock.
class ScopedMutexLock {
pthread_mutex_t* mutex_;  // Weak reference.

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
NaclNes* image_owner_;  // Weak reference.
uint32_t* pixels_;  // Weak reference.

ScopedPixelLock();  // Not implemented, do not use.

public:
	explicit ScopedPixelLock(NaclNes* image_owner) : 
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

NaclNes::NaclNes(PP_Instance instance)
    : pp::Instance(instance),
      graphics_2d_context_(NULL),
      pixel_buffer_(NULL),
      flush_pending_(false),
      quit_(false),
      thread_(0) {
    //nacl_nes::g_nacl_nes = this;
    pthread_mutex_init(&pixel_buffer_mutex_, NULL);
}

NaclNes::~NaclNes() {
  quit_ = true;
  if (thread_) {
    if(vnes)
      vnes->stop();
    pthread_join(thread_, NULL);
  }
  DestroyContext();
  delete pixel_buffer_;
  pthread_mutex_destroy(&pixel_buffer_mutex_);
}

void NaclNes::DidChangeView(const pp::View& view) {
  pp::Rect position = view.GetRect();
  if (pixel_buffer_ && position.size() == pixel_buffer_->size())
    return;  // Size didn't change, no need to update anything.

  // Create a new device context with the new size.
  DestroyContext();
  CreateContext(position.size());
  // Delete the old pixel buffer and create a new one.
  ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
  delete pixel_buffer_;
  pixel_buffer_ = NULL;
  if (graphics_2d_context_ != NULL) {
    pixel_buffer_ = new pp::ImageData(this,
                                      PP_IMAGEDATAFORMAT_BGRA_PREMUL,
                                      graphics_2d_context_->size(),
                                      false);
  }
}

bool NaclNes::Init(uint32_t argc, const char* argn[], const char* argv[]) {
	//pthread_create(&thread_, NULL, start_main_loop, this);

  //pthread_create(&thread_, NULL, ComputePi, this);
  return true;
}

uint32_t* NaclNes::LockPixels() {
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

void NaclNes::HandleMessage(const pp::Var& var_message) {
	string message;
	if(var_message.is_string())
		message = var_message.AsString();

	if(message == "paint") {
//		if(!vnes->isPaused)
			Paint();
	} else if(message == "pause") {
//		vnes->isPaused = !vnes->isPaused;
	} else if(message.find("button_down:") == 0) {
		size_t sep_pos = message.find_first_of(":");
		int32_t button = 0;
		string str_button = message.substr(sep_pos + 1);
		istringstream ( str_button ) >> button;
		button_down(button);
	} else if(message.find("button_up:") == 0) {
		size_t sep_pos = message.find_first_of(":");
		int32_t button = 0;
		string str_button = message.substr(sep_pos + 1);
		istringstream ( str_button ) >> button;
		button_up(button);
	} else if(message == "get_fps") {
		stringstream out;
		out << "get_fps:";
		out << get_fps();
		nacl_nes::NaclNes::log_to_browser(out.str());
	} else if(message.find("load_rom:") == 0) {
		// Convert the rom data to bytes
		size_t sep_pos = message.find_first_of(":");
		string rom_string = message.substr(sep_pos + 1);
		const size_t ROM_DATA_LENGTH = rom_string.length()/2;
		uint8_t* rom_data = new uint8_t[ROM_DATA_LENGTH]();
		size_t j = 0;
		for(size_t i=0; i<rom_string.length(); i+=2) {
			rom_data[j] = string_to_byte(rom_string[i], rom_string[i+1]);
			j++;
		}

		// Make sure the ROM is valid
		if(rom_data[0] != 'N' || rom_data[1] != 'E' || rom_data[2] != 'S' || rom_data[3] != 0x1A) {
			nacl_nes::NaclNes::log_to_browser("Invalid ROM file!");
		} else {
			// Stop any previously running NES
			if(vnes != NULL) {
				vnes->stop();
				pthread_join(thread_, NULL);
				delete_n_null(vnes);
			}

			// Run the ROM
			vnes = new vNES();
			vnes->init_data((uint8_t*) rom_data);
			pthread_create(&thread_, NULL, start_main_loop, this);
			nacl_nes::NaclNes::log_to_browser("running");
		}
	} else {
		nacl_nes::NaclNes::log_to_browser("unknown message");
	}

}

void NaclNes::UnlockPixels() const {
  pthread_mutex_unlock(&pixel_buffer_mutex_);
}

void NaclNes::Paint() {
  ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
  if (!scoped_mutex.is_valid()) {
    return;
  }
  FlushPixelBuffer();
}

void NaclNes::CreateContext(const pp::Size& size) {
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

void NaclNes::DestroyContext() {
  ScopedMutexLock scoped_mutex(&pixel_buffer_mutex_);
  if (!scoped_mutex.is_valid()) {
    return;
  }
  if (!IsContextValid())
    return;
  delete graphics_2d_context_;
  graphics_2d_context_ = NULL;
}

void NaclNes::FlushPixelBuffer() {
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

void* NaclNes::start_main_loop(void* param) {
	//daNes->nacl_nes->PostMessage("start_main_loop");
	nacl_nes::NaclNes::log_to_browser("nes_RunCart");
	vnes->run();
	return 0;
}

}  // namespace nacl_nes

#endif
