/*
SaltyNES Copyright (c) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>
vNES Copyright (c) 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SaltyNES.h"

const float InputHandler::AXES_DEAD_ZONE = 0.2;
const string InputHandler::KEYS[] = { "up", "down", "right", "left", "start", "select", "a", "b" };
const size_t InputHandler::KEYS_LENGTH = 8;

	InputHandler::InputHandler(NES* nes, int id) :
		_nes(nes),
		_id(id),
		_keys(255),
		_map(InputHandler::NUM_KEYS) {

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
	}

     InputHandler::~InputHandler() {
        _nes = NULL;
    }

     short InputHandler::getKeyState(int padKey) {
        return (short) (_keys[_map[padKey]] ? 0x41 : 0x40);
    }

     void InputHandler::mapKey(int padKey, int kbKeycode) {
        _map[padKey] = kbKeycode;
    }

     void InputHandler::poll_for_key_events() {
#ifdef SDL
        int32_t numberOfKeys;
        uint8_t* keystate = SDL_GetKeyState(&numberOfKeys);

        _keys[_map[InputHandler::KEY_UP]] =     keystate[SDLK_UP];
        _keys[_map[InputHandler::KEY_DOWN]] =   keystate[SDLK_DOWN];
        _keys[_map[InputHandler::KEY_RIGHT]] =  keystate[SDLK_RIGHT];
        _keys[_map[InputHandler::KEY_LEFT]] =   keystate[SDLK_LEFT];
        _keys[_map[InputHandler::KEY_START]] =  keystate[SDLK_RETURN];
        _keys[_map[InputHandler::KEY_SELECT]] = keystate[SDLK_RCTRL];
        _keys[_map[InputHandler::KEY_B]] =      keystate[SDLK_z];
        _keys[_map[InputHandler::KEY_A]] =      keystate[SDLK_x];
#endif

#ifdef NACL
        _keys[_map[InputHandler::KEY_UP]] =     _is_input_pressed["up"];
        _keys[_map[InputHandler::KEY_DOWN]] =   _is_input_pressed["down"];
        _keys[_map[InputHandler::KEY_RIGHT]] =  _is_input_pressed["right"];
        _keys[_map[InputHandler::KEY_LEFT]] =   _is_input_pressed["left"];
        _keys[_map[InputHandler::KEY_START]] =  _is_input_pressed["start"];
        _keys[_map[InputHandler::KEY_SELECT]] = _is_input_pressed["select"];
        _keys[_map[InputHandler::KEY_B]] =      _is_input_pressed["b"];
        _keys[_map[InputHandler::KEY_A]] =      _is_input_pressed["a"];
#endif

        // Can't hold both left & right or up & down at same time:
        if(_keys[_map[InputHandler::KEY_LEFT]]) {
            _keys[_map[InputHandler::KEY_RIGHT]] = false;
        } else if(_keys[_map[InputHandler::KEY_RIGHT]]) {
            _keys[_map[InputHandler::KEY_LEFT]] = false;
        }
        
        if(_keys[_map[InputHandler::KEY_UP]]) {
            _keys[_map[InputHandler::KEY_DOWN]] = false;
        } else if(_keys[_map[InputHandler::KEY_DOWN]]) {
            _keys[_map[InputHandler::KEY_UP]] = false;
        }
    }

     void InputHandler::reset() {
        size_t size = _keys.size();
        _keys.clear();
        _keys.resize(size);
    }
    
