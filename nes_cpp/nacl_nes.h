
#ifndef NACL_NES_H_
#define NACL_NES_H_

#include <pthread.h>
#include <map>
#include <vector>
#include <stdio.h>
#include "ppapi/cpp/graphics_2d.h"
#include "ppapi/cpp/image_data.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"

using namespace std;

class NaclNes;

namespace nacl_nes {
	class NaclNes : public pp::Instance {
		mutable pthread_mutex_t pixel_buffer_mutex_;
		pp::Graphics2D* graphics_2d_context_;
		pp::ImageData* pixel_buffer_;
		bool flush_pending_;
		bool quit_;
		uint32_t _fps;
		pthread_t thread_;
	
		// Create and initialize the 2D context used for drawing.
		void CreateContext(const pp::Size& size);
		// Destroy the 2D drawing context.
		void DestroyContext();
		// Push the pixels to the browser, then attempt to flush the 2D context.  If
		// there is a pending flush on the 2D context, then update the pixels only
		// and do not flush.
		void FlushPixelBuffer();
	
		bool IsContextValid() const {
			return graphics_2d_context_ != NULL;
		}
	
	public:
		explicit NaclNes(PP_Instance instance);
		virtual ~NaclNes();
	
		static NaclNes* g_nacl_nes;
		static void log_to_browser(string message);
	
		bool _button_a_down;
		bool _button_b_down;
		bool _button_start_down;
		bool _button_select_down;
		bool _button_up_down;
		bool _button_down_down;
		bool _button_left_down;
		bool _button_right_down;
	
		static void* start_main_loop(void* param);
		virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]);
		virtual void DidChangeView(const pp::View& view);
		virtual void HandleMessage(const pp::Var& var_message);
	
		uint32_t* LockPixels();
		void UnlockPixels() const;
		void Paint();
	
		bool quit() const {
			return quit_;
		}
	
		void button_down(int32_t key) {
			switch(key) {
				case(90): _button_a_down = true; break; // z = 90
				case(88): _button_b_down = true; break; // x = 88
				case(65): _button_start_down = true; break; // a = 65
				case(83): _button_select_down = true; break; // s = 83
				case(38): _button_up_down = true; break; // up = 38
				case(37): _button_left_down = true; break; // left = 37
				case(40): _button_down_down = true; break; // down = 40
				case(39): _button_right_down = true; break; // right = 39
			}
		}
		void button_up(int32_t key) {
			switch(key) {
				case(90): _button_a_down = false; break; // z = 90
				case(88): _button_b_down = false; break; // x = 88
				case(65): _button_start_down = false; break; // a = 65
				case(83): _button_select_down = false; break; // s = 83
				case(38): _button_up_down = false; break; // up = 38
				case(37): _button_left_down = false; break; // left = 37
				case(40): _button_down_down = false; break; // down = 40
				case(39): _button_right_down = false; break; // right = 39
			}
		}
	
		void set_fps(uint32_t value) {
			_fps = value;
		}
		uint32_t get_fps() {
			return _fps;
		}
	
		int width() const {
			return pixel_buffer_ ? pixel_buffer_->size().width() : 0;
		}
		int height() const {
		return pixel_buffer_ ? pixel_buffer_->size().height() : 0;
		}
	
		// Indicate whether a flush is pending.  This can only be called from the
		// main thread; it is not thread safe.
		bool flush_pending() const {
			return flush_pending_;
		}
		void set_flush_pending(bool flag) {
			flush_pending_ = flag;
		}
	};
}



#endif

