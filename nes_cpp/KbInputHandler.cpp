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


#include "Globals.h"


     KbInputHandler::KbInputHandler(NES* nes, int id) {
        this->nes = nes;
        this->id = id;
        allKeysState = new vector<bool>(255);
        keyMapping = new int[KbInputHandler::NUM_KEYS];
    }

     short KbInputHandler::getKeyState(int padKey) {
        return (short) ((*allKeysState)[keyMapping[padKey]] ? 0x41 : 0x40);
    }

     void KbInputHandler::mapKey(int padKey, int kbKeycode) {
        keyMapping[padKey] = kbKeycode;
    }

     void KbInputHandler::keyPressed(KeyEvent* ke) {

        int kc = ke->getKeyCode();
        if (kc >= allKeysState->size()) {
            return;
        }

        (*allKeysState)[kc] = true;

        // Can't hold both left & right or up & down at same time:
        if (kc == keyMapping[KbInputHandler::KEY_LEFT]) {
            (*allKeysState)[keyMapping[KbInputHandler::KEY_RIGHT]] = false;
        } else if (kc == keyMapping[KbInputHandler::KEY_RIGHT]) {
            (*allKeysState)[keyMapping[KbInputHandler::KEY_LEFT]] = false;
        } else if (kc == keyMapping[KbInputHandler::KEY_UP]) {
            (*allKeysState)[keyMapping[KbInputHandler::KEY_DOWN]] = false;
        } else if (kc == keyMapping[KbInputHandler::KEY_DOWN]) {
            (*allKeysState)[keyMapping[KbInputHandler::KEY_UP]] = false;
        }
    }

     void KbInputHandler::keyReleased(KeyEvent* ke) {

        int kc = ke->getKeyCode();
        if (kc >= allKeysState->size()) {
            return;
        }

        (*allKeysState)[kc] = false;

        if (id == 0) {
            switch (kc) {
                case KeyEvent::VK_F5: {
                    // Reset game:
                    if (nes->isRunning()) {
                        nes->stopEmulation();
                        nes->reset();
                        nes->reloadRom();
                        nes->startEmulation();
                    }
                    break;
                }
                case KeyEvent::VK_F10: {
                    // Just using this to display the battery RAM contents to user.
                    if (nes->rom != NULL) {
                        nes->rom->closeRom();
                    }
                    break;
                }
                case KeyEvent::VK_F12: {
//                    JOptionPane.showInputDialog("Save Code for Resuming Game.", "Test");
                    break;
                }
            }
        }

    }

     void KbInputHandler::keyTyped(KeyEvent* ke) {
        // Ignore.
    }

     void KbInputHandler::reset() {
        size_t size = allKeysState->size();
        allKeysState->clear();
        allKeysState->resize(size);
    }

     void KbInputHandler::update() {
        // doesn't do anything.
    }

     void KbInputHandler::destroy() {
        nes = NULL;
    }
