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

class IMemoryMapper {
public:
     virtual void init(NES* nes) = 0;

     virtual void loadROM(ROM rom) = 0;

     virtual void write(int address, short value) = 0;

     virtual short load(int address) = 0;

     virtual short joy1Read() = 0;

     virtual short joy2Read() = 0;

     virtual void reset() = 0;

     virtual void setGameGenieState(bool value) = 0;

     virtual void clockIrqCounter() = 0;

     virtual void loadBatteryRam() = 0;

     virtual void destroy() = 0;

     virtual void stateLoad(ByteBuffer* buf) = 0;

     virtual void stateSave(ByteBuffer* buf) = 0;

     virtual void setMouseState(bool pressed, int x, int y) = 0;

     virtual void latchAccess(int address) = 0;
};