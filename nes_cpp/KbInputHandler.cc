/*
vNES
Copyright © 2006-2011 Jamie Sanders

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


#include "nes_cpp.h"


     KbInputHandler::KbInputHandler(NES* nes, int id) {
        this->nes = nes;
        this->id = id;
        allKeysState = new vector<bool>(255);
        keyMapping = new vector<int>(KbInputHandler::NUM_KEYS);
    }

     KbInputHandler::~KbInputHandler() {
        nes = NULL;
    }

     short KbInputHandler::getKeyState(int padKey) {
        return (short) ((*allKeysState)[(*keyMapping)[padKey]] ? 0x41 : 0x40);
    }

     void KbInputHandler::mapKey(int padKey, int kbKeycode) {
        (*keyMapping)[padKey] = kbKeycode;
    }

     void KbInputHandler::poll_for_key_events() {
#ifdef SDL
        int32_t numberOfKeys;
        uint8_t* keystate = SDL_GetKeyState(&numberOfKeys);
        vector<bool>* s = this->allKeysState;
        vector<int>* m = this->keyMapping;

        (*s)[(*m)[KbInputHandler::KEY_A]] =      keystate[SDLK_z];
        (*s)[(*m)[KbInputHandler::KEY_B]] =      keystate[SDLK_x];
        (*s)[(*m)[KbInputHandler::KEY_START]] =  keystate[SDLK_RETURN];
        (*s)[(*m)[KbInputHandler::KEY_SELECT]] = keystate[SDLK_RCTRL];
        (*s)[(*m)[KbInputHandler::KEY_UP]] =     keystate[SDLK_UP];
        (*s)[(*m)[KbInputHandler::KEY_DOWN]] =   keystate[SDLK_DOWN];
        (*s)[(*m)[KbInputHandler::KEY_RIGHT]] =  keystate[SDLK_RIGHT];
        (*s)[(*m)[KbInputHandler::KEY_LEFT]] =   keystate[SDLK_LEFT];

        // Can't hold both left & right or up & down at same time:
        if ((*s)[(*m)[KbInputHandler::KEY_LEFT]]) {
            (*s)[(*m)[KbInputHandler::KEY_RIGHT]] = false;
        } else if ((*s)[(*m)[KbInputHandler::KEY_RIGHT]]) {
            (*s)[(*m)[KbInputHandler::KEY_LEFT]] = false;
        }
        
        if ((*s)[(*m)[KbInputHandler::KEY_UP]]) {
            (*s)[(*m)[KbInputHandler::KEY_DOWN]] = false;
        } else if ((*s)[(*m)[KbInputHandler::KEY_DOWN]]) {
            (*s)[(*m)[KbInputHandler::KEY_UP]] = false;
        }
#endif
    }

     void KbInputHandler::reset() {
        size_t size = allKeysState->size();
        allKeysState->clear();
        allKeysState->resize(size);
    }
    
