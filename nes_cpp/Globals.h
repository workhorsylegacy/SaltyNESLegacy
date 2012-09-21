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

#ifndef CPP_NES_GLOBALS_
#define CPP_NES_GLOBALS_

#include <map>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

using namespace std;


class AppletUI;
class BlipBuffer;
class BufferView;
class ByteBuffer;
class ChannelDM;
class ChannelNoise;
class ChannelSquare;
class ChannelTriangle;
class CPU;
class CpuInfo;
class FileLoader;
class HiResTimer;
class InputHandler;
class KbInputHandler;
class Mapper001;
class MapperDefault;
class Memory;
class MemoryMapper;
class Misc;
class NameTable;
class NES;
class PaletteTable;
class PAPU;
class PapuChannel;
class PPU;
class Raster;
class ROM;
class Scale;
class ScreenView;
class Tile;
class UI;
class vNES;

class Point {
	uint32_t x;
	uint32_t y;
};

namespace Globals {

    static double CPU_FREQ_NTSC = 1789772.5d;
    static double CPU_FREQ_PAL = 1773447.4d;
    static int preferredFrameRate = 60;
    
    // Microseconds per frame:
    static int frameTime = 1000000 / preferredFrameRate;
    // What value to flush memory with on power-up:
    static short memoryFlushValue = 0xFF;

    static const bool debug = true;
    static const bool fsdebug = false;

    static bool appletMode = true;
    static bool disableSprites = false;
    static bool timeEmulation = true;
    static bool palEmulation;
    static bool enableSound = true;
    static bool focused = false;

    static std::map<string, uint32_t> keycodes; //Java key codes
    static std::map<string, string> controls; //vNES controls codes

    static NES* nes;

    static void println(string s) {
        printf("%s\n", s.c_str());
    }
};


class IUI {
public:
    virtual NES getNES();

    virtual InputHandler getJoy1() = 0;

    virtual class InputHandler getJoy2() = 0;

    virtual BufferView getScreenView() = 0;

    virtual BufferView getPatternView() = 0;

    virtual BufferView getSprPalView() = 0;

    virtual BufferView getNameTableView() = 0;

    virtual BufferView getImgPalView() = 0;

    virtual HiResTimer getTimer() = 0;

    virtual void imageReady(bool skipFrame) = 0;

    virtual void init(bool showGui) = 0;

    virtual string getWindowCaption() = 0;

    virtual void setWindowCaption(string s) = 0;

    virtual void setTitle(string s) = 0;

    virtual Point getLocation() = 0;

    virtual int getWidth() = 0;

    virtual int getHeight() = 0;

    virtual int getRomFileSize() = 0;

    virtual void destroy() = 0;

    virtual void println(string s) = 0;

    virtual void showLoadProgress(int percentComplete) = 0;

    virtual void showErrorMsg(string msg) = 0;
};

#endif

