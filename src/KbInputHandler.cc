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

	KbInputHandler::KbInputHandler(NES* nes, int id) :
		_nes(nes),
		_id(id),
		_keys(255),
		_map(KbInputHandler::NUM_KEYS) {
	}

     KbInputHandler::~KbInputHandler() {
        _nes = NULL;
    }

     short KbInputHandler::getKeyState(int padKey) {
        return (short) (_keys[_map[padKey]] ? 0x41 : 0x40);
    }

     void KbInputHandler::mapKey(int padKey, int kbKeycode) {
        _map[padKey] = kbKeycode;
    }

     void KbInputHandler::poll_for_key_events() {
#ifdef SDL
        int32_t numberOfKeys;
        uint8_t* keystate = SDL_GetKeyState(&numberOfKeys);

        _keys[_map[KbInputHandler::KEY_UP]] =     keystate[SDLK_UP];
        _keys[_map[KbInputHandler::KEY_DOWN]] =   keystate[SDLK_DOWN];
        _keys[_map[KbInputHandler::KEY_RIGHT]] =  keystate[SDLK_RIGHT];
        _keys[_map[KbInputHandler::KEY_LEFT]] =   keystate[SDLK_LEFT];
        _keys[_map[KbInputHandler::KEY_START]] =  keystate[SDLK_RETURN];
        _keys[_map[KbInputHandler::KEY_SELECT]] = keystate[SDLK_RCTRL];
        _keys[_map[KbInputHandler::KEY_B]] =      keystate[SDLK_z];
        _keys[_map[KbInputHandler::KEY_A]] =      keystate[SDLK_x];
#endif

#ifdef NACL
        _keys[_map[KbInputHandler::KEY_UP]] =     this->_nes->_salty_nes->_is_input_pressed["up"];
        _keys[_map[KbInputHandler::KEY_DOWN]] =   this->_nes->_salty_nes->_is_input_pressed["down"];
        _keys[_map[KbInputHandler::KEY_RIGHT]] =  this->_nes->_salty_nes->_is_input_pressed["right"];
        _keys[_map[KbInputHandler::KEY_LEFT]] =   this->_nes->_salty_nes->_is_input_pressed["left"];
        _keys[_map[KbInputHandler::KEY_START]] =  this->_nes->_salty_nes->_is_input_pressed["start"];
        _keys[_map[KbInputHandler::KEY_SELECT]] = this->_nes->_salty_nes->_is_input_pressed["select"];
        _keys[_map[KbInputHandler::KEY_B]] =      this->_nes->_salty_nes->_is_input_pressed["b"];
        _keys[_map[KbInputHandler::KEY_A]] =      this->_nes->_salty_nes->_is_input_pressed["a"];
#endif

        // Can't hold both left & right or up & down at same time:
        if(_keys[_map[KbInputHandler::KEY_LEFT]]) {
            _keys[_map[KbInputHandler::KEY_RIGHT]] = false;
        } else if(_keys[_map[KbInputHandler::KEY_RIGHT]]) {
            _keys[_map[KbInputHandler::KEY_LEFT]] = false;
        }
        
        if(_keys[_map[KbInputHandler::KEY_UP]]) {
            _keys[_map[KbInputHandler::KEY_DOWN]] = false;
        } else if(_keys[_map[KbInputHandler::KEY_DOWN]]) {
            _keys[_map[KbInputHandler::KEY_UP]] = false;
        }
    }

     void KbInputHandler::reset() {
        size_t size = _keys.size();
        _keys.clear();
        _keys.resize(size);
    }
    
