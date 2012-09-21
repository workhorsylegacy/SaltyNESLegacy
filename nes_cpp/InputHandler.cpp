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

class IInputHandler {
public:
    // Joypad keys:
    static const int KEY_A = 0;
    static const int KEY_B = 1;
    static const int KEY_START = 2;
    static const int KEY_SELECT = 3;
    static const int KEY_UP = 4;
    static const int KEY_DOWN = 5;
    static const int KEY_LEFT = 6;
    static const int KEY_RIGHT = 7;
    
    // Key count:
    static const int NUM_KEYS = 8;

    static virtual short getKeyState(int padKey) = 0;

    static virtual void mapKey(int padKey, int deviceKey) = 0;

    static virtual void reset() = 0;

    static virtual void update() = 0;
};