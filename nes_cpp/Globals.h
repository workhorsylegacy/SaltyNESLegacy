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

// Temp stub classes
class Point {
public:
	uint32_t x;
	uint32_t y;
};
class Image {};
class Graphics {};
class File {};
class BufferedImage {};
class VolatileImage {};
class Font {
public:
	static const int BOLD = 0;
	Font(string name, int style, int size) {
		
	}
};

// Forward declarations
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
class IInputHandler;
class IMemoryMapper;
class IPapuChannel;
class IUI;
class KbInputHandler;
class Mapper001;
class MapperDefault;
class Memory;
class Misc;
class NameTable;
class NES;
class PaletteTable;
class PAPU;
class PPU;
class Raster;
class ROM;
class Scale;
class ScreenView;
class Tile;
class vNES;

// Prototypes
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
    virtual NES* getNES();
    virtual IInputHandler* getJoy1() = 0;
    virtual IInputHandler* getJoy2() = 0;
    virtual BufferView* getScreenView() = 0;
    virtual BufferView* getPatternView() = 0;
    virtual BufferView* getSprPalView() = 0;
    virtual BufferView* getNameTableView() = 0;
    virtual BufferView* getImgPalView() = 0;
    virtual HiResTimer* getTimer() = 0;
    virtual void imageReady(bool skipFrame) = 0;
    virtual void init(bool showGui) = 0;
    virtual string getWindowCaption() = 0;
    virtual void setWindowCaption(string s) = 0;
    virtual void setTitle(string s) = 0;
    virtual Point* getLocation() = 0;
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getRomFileSize() = 0;
    virtual void destroy() = 0;
    virtual void println(string s) = 0;
    virtual void showLoadProgress(int percentComplete) = 0;
    virtual void showErrorMsg(string msg) = 0;
};

class AppletUI : public IUI {
    vNES* applet;
    NES* nes;
    KbInputHandler* kbJoy1;
    KbInputHandler* kbJoy2;
    ScreenView* vScreen;
    HiResTimer* timer;
    long t1, t2;
    int sleepTime;

public:
    AppletUI(vNES* applet);
    virtual void init(bool showGui);
    virtual void imageReady(bool skipFrame);
    virtual int getRomFileSize();
    virtual void showLoadProgress(int percentComplete);
    virtual void destroy();
    NES* getNES();
    virtual IInputHandler* getJoy1();
    virtual IInputHandler* getJoy2();
    virtual BufferView* getScreenView();
    virtual BufferView* getPatternView();
    virtual BufferView* getSprPalView();
    virtual BufferView* getNameTableView();
    virtual BufferView* getImgPalView();
    virtual HiResTimer* getTimer();
    virtual string getWindowCaption();
    virtual void setWindowCaption(string s);
    virtual void setTitle(string s);
    virtual Point* getLocation();
    virtual int getWidth();
    virtual int getHeight();
    virtual void println(string s);
    virtual void showErrorMsg(string msg);
};


class BlipBuffer {
	// These values must be set:
    int win_size;
    int smp_period;
    int sinc_periods;
    // Different samplings of bandlimited impulse:
    int** imp;
    // Difference buffer:
    int* diff;
    // Last position changed in buffer:
    int lastChanged;
    // Previous end absolute value:
    int prevSum;
    // DC removal:
    int dc_prev;
    int dc_diff;
    int dc_acc;
    
public:
    void init(int bufferSize, int windowSize, int samplePeriod, int sincPeriods);
    void impulse(int smpPos, int smpOffset, int magnitude);
    int integrate();
    void clear();
    static double sinc(double x);
};

class BufferView { /*: public JPanel { */
    // Scale modes:
    static const int SCALE_NONE = 0;
    static const int SCALE_HW2X = 1;
    static const int SCALE_HW3X = 2;
    static const int SCALE_NORMAL = 3;
    static const int SCALE_SCANLINE = 4;
    static const int SCALE_RASTER = 5;
    NES* nes;
     BufferedImage* img;
     VolatileImage* vimg;
     bool usingMenu;
     Graphics* gfx;
     int width;
     int height;
     int* pix;
     int* pix_scaled;
     int scaleMode;
    // FPS counter variables:
     bool showFPS;
     long prevFrameTime;
     string fps;
     int fpsCounter;
     Font* fpsFont;
     int bgColor;
     
public:
    // Constructor
    BufferView(NES* nes, int width, int height);
    void setBgColor(int color);
    void setScaleMode(int newMode);
    void init();
    void createView();
    void imageReady(bool skipFrame);
    Image* getImage();
    int* getBuffer();
    void update(Graphics* g);
    bool scalingEnabled();
    int getScaleMode();
    bool useNormalScaling();
    void paint(Graphics* g);
    void paint_scaled(Graphics* g);
    void setFPSEnabled(bool val);
    void paintFPS(int x, int y, Graphics* g);
    int getBufferWidth();
    int getBufferHeight();
    void setUsingMenu(bool val);
    bool useHWScaling();
    bool useHWScaling(int mode);
    int getScaleModeScale(int mode);
    void destroy();
};


class ByteBuffer {
	 static const bool debug = false;
	 static const int BO_BIG_ENDIAN = 0;
     static const int BO_LITTLE_ENDIAN = 1;
     int byteOrder;
     short* buf;
     int size;
     int curPos;
     bool hasBeenErrors;
     bool expandable;
     int expandBy;

public:	
    ByteBuffer(int size, int byteOrdering);
    ByteBuffer(int8_t* content, int byteOrdering);
    void setExpandable(bool exp);
    void setExpandBy(int expBy);
    void setByteOrder(int byteOrder);
    int8_t* getBytes();
    int getSize();
    int getPos();
    void error();
    bool hasHadErrors();
    void clear();
    void fill(int8_t value);
    bool fillRange(int start, int length, int8_t value);
    void resize(int length);
    void resizeToCurrentPos();
    void expand();
    void expand(int byHowMuch);
    void goTo(int position);
    void move(int howFar);
    bool inRange(int pos);
    bool inRange(int pos, int length);
    bool putBoolean(bool b);
    bool putBoolean(bool b, int pos);
    bool putByte(short var);
    bool putByte(short var, int pos);
    bool putShort(short var);
    bool putShort(short var, int pos);
    bool putInt(int var);
    bool putInt(int var, int pos);
    bool putString(string var);
    bool putString(string var, int pos);
    bool putChar(char var);
    bool putChar(char var, int pos);
    bool putCharAscii(char var);
    bool putCharAscii(char var, int pos);
    bool putStringAscii(string var);
    bool putStringAscii(string var, int pos);
    bool putByteArray(short* arr);
    bool readByteArray(short* arr);
    bool putShortArray(short* arr);
    string toString();
    string toStringAscii();
    bool readBoolean();
    bool readBoolean(int pos);
    short readByte();
    short readByte(int pos);
    short readShort();
    short readShort(int pos);
    int readInt();
    int readInt(int pos);
    char readChar();
    char readChar(int pos);
    char readCharAscii();
    char readCharAscii(int pos);
    string readString(int length);
    string readString(int pos, int length);
    string readStringWithShortLength();
    string readStringWithShortLength(int pos);
    string readStringAscii(int length);
    string readStringAscii(int pos, int length);
    string readStringAsciiWithShortLength();
    string readStringAsciiWithShortLength(int pos);
    short* expandShortArray(short* array, int size);
    void crop();
    static ByteBuffer* asciiEncode(ByteBuffer* buf);
    static ByteBuffer* asciiDecode(ByteBuffer* buf);
    static void saveToZipFile(File f, ByteBuffer* buf);
    static ByteBuffer* readFromZipFile(File f);
};

// FIXME: Add the rest of the class prototypes

#endif

