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

class ChannelTriangle : public IPapuChannel {
public:
    PAPU papu;
    bool isEnabled;
    bool sampleCondition;
    bool lengthCounterEnable;
    bool lcHalt;
    bool lcControl;
    int progTimerCount;
    int progTimerMax;
    int triangleCounter;
    int lengthCounter;
    int linearCounter;
    int lcLoadValue;
    int sampleValue;
    int tmp;

     ChannelTriangle(PAPU papu) {
        this.papu = papu;
    }

     void clockLengthCounter() {
        if (lengthCounterEnable && lengthCounter > 0) {
            lengthCounter--;
            if (lengthCounter == 0) {
                updateSampleCondition();
            }
        }
    }

     void clockLinearCounter() {

        if (lcHalt) {

            // Load:
            linearCounter = lcLoadValue;
            updateSampleCondition();

        } else if (linearCounter > 0) {

            // Decrement:
            linearCounter--;
            updateSampleCondition();

        }

        if (!lcControl) {

            // Clear halt flag:
            lcHalt = false;

        }

    }

     int getLengthStatus() {
        return ((lengthCounter == 0 || !isEnabled) ? 0 : 1);
    }

     int readReg(int address) {
        return 0;
    }

     void writeReg(int address, int value) {

        if (address == 0x4008) {

            // New values for linear counter:
            lcControl = (value & 0x80) != 0;
            lcLoadValue = value & 0x7F;

            // Length counter enable:
            lengthCounterEnable = !lcControl;

        } else if (address == 0x400A) {

            // Programmable timer:
            progTimerMax &= 0x700;
            progTimerMax |= value;

        } else if (address == 0x400B) {

            // Programmable timer, length counter
            progTimerMax &= 0xFF;
            progTimerMax |= ((value & 0x07) << 8);
            lengthCounter = papu.getLengthMax(value & 0xF8);
            lcHalt = true;

        }

        updateSampleCondition();

    }

     void clockProgrammableTimer(int nCycles) {

        if (progTimerMax > 0) {
            progTimerCount += nCycles;
            while (progTimerMax > 0 && progTimerCount >= progTimerMax) {
                progTimerCount -= progTimerMax;
                if (isEnabled && lengthCounter > 0 && linearCounter > 0) {
                    clockTriangleGenerator();
                }
            }
        }

    }

     void clockTriangleGenerator() {
        triangleCounter++;
        triangleCounter &= 0x1F;
    }

     void setEnabled(bool value) {
        isEnabled = value;
        if (!value) {
            lengthCounter = 0;
        }
        updateSampleCondition();
    }

     bool isEnabled() {
        return isEnabled;
    }

     void updateSampleCondition() {
        sampleCondition =
                isEnabled &&
                progTimerMax > 7 &&
                linearCounter > 0 &&
                lengthCounter > 0;
    }

     void reset() {

        progTimerCount = 0;
        progTimerMax = 0;
        triangleCounter = 0;
        isEnabled = false;
        sampleCondition = false;
        lengthCounter = 0;
        lengthCounterEnable = false;
        linearCounter = 0;
        lcLoadValue = 0;
        lcHalt = true;
        lcControl = false;
        tmp = 0;
        sampleValue = 0xF;

    }

     void destroy() {
        papu = NULL;
    }
};