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
class IMemoryMapper;
class IPapuChannel;
class IUI;

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

// Interfaces
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

class IPapuChannel {
     virtual void writeReg(int address, int value) = 0;
     virtual void setEnabled(bool value) = 0;
     virtual bool isEnabled() = 0;
     virtual void reset() = 0;
     virtual int getLengthStatus() = 0;
};

class IUI {
public:
    virtual NES* getNES();
    virtual KbInputHandler* getJoy1() = 0;
    virtual KbInputHandler* getJoy2() = 0;
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

// Class Prototypes
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
    virtual KbInputHandler* getJoy1();
    virtual KbInputHandler* getJoy2();
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

class ChannelDM : public IPapuChannel {
    static const int MODE_NORMAL = 0;
    static const int MODE_LOOP = 1;
    static const int MODE_IRQ = 2;
    PAPU* papu;
    bool isEnabled;
    bool hasSample;
    bool irqGenerated = false;
    int playMode;
    int dmaFrequency;
    int dmaCounter;
    int deltaCounter;
    int playStartAddress;
    int playAddress;
    int playLength;
    int playLengthCounter;
    int shiftCounter;
    int reg4012, reg4013;
    int status;
    int sample;
    int dacLsb;
    int data;
public:
    ChannelDM(PAPU* papu);
    void clockDmc();
     void endOfSample();
     void nextSample();
    void writeReg(int address, int value);
    void setEnabled(bool value);
    bool isEnabled();
    int getLengthStatus();
    int getIrqStatus();
    void reset();
    void destroy();
};


class ChannelNoise : public IPapuChannel {
    PAPU* papu;
     bool isEnabled;
     bool envDecayDisable;
     bool envDecayLoopEnable;
     bool lengthCounterEnable;
     bool envReset;
     bool shiftNow;
     int lengthCounter;
     int progTimerCount;
     int progTimerMax;
     int envDecayRate;
     int envDecayCounter;
     int envVolume;
     int masterVolume;
     int shiftReg;
     int randomBit;
     int randomMode;
     int sampleValue;
     long accValue = 0;
     long accCount = 1;
     int tmp;
public:
     ChannelNoise(PAPU* papu);
     void clockLengthCounter();
     void clockEnvDecay();
     void updateSampleValue();
     void writeReg(int address, int value);
     void setEnabled(bool value);
     bool isEnabled();
     int getLengthStatus();
     void reset();
};

class ChannelSquare : public IPapuChannel {
    PAPU* papu;
    static int[] dutyLookup;
    static int[] impLookup;
    bool sqr1;
    bool isEnabled;
    bool lengthCounterEnable;
    bool sweepActive;
    bool envDecayDisable;
    bool envDecayLoopEnable;
    bool envReset;
    bool sweepCarry;
    bool updateSweepPeriod;
    int progTimerCount;
    int progTimerMax;
    int lengthCounter;
    int squareCounter;
    int sweepCounter;
    int sweepCounterMax;
    int sweepMode;
    int sweepShiftAmount;
    int envDecayRate;
    int envDecayCounter;
    int envVolume;
    int masterVolume;
    int dutyMode;
    int sweepResult;
    int sampleValue;
    int vol;
public:
    static int[] dutyLookup = {
                    0, 1, 0, 0, 0, 0, 0, 0,
                    0, 1, 1, 0, 0, 0, 0, 0,
                    0, 1, 1, 1, 1, 0, 0, 0,
                    1, 0, 0, 1, 1, 1, 1, 1};

    static int[] impLookup = {
                    1, -1, 0, 0, 0, 0, 0, 0,
                    1, 0, -1, 0, 0, 0, 0, 0,
                    1, 0, 0, 0, -1, 0, 0, 0,
                    -1, 0, 1, 0, 0, 0, 0, 0};

     ChannelSquare(PAPU* papu, bool square1);
     void clockLengthCounter();
     void clockEnvDecay();
     void clockSweep();
     void updateSampleValue();
     void writeReg(int address, int value);
     void setEnabled(bool value);
     bool isEnabled();
     int getLengthStatus();
     void reset();
     void destroy();
};

class ChannelTriangle : public IPapuChannel {
    PAPU* papu;
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
public:
     ChannelTriangle(PAPU* papu);
     void clockLengthCounter();
     void clockLinearCounter();
     int getLengthStatus();
     int readReg(int address);
     void writeReg(int address, int value);
     void clockProgrammableTimer(int nCycles);
     void clockTriangleGenerator();
     void setEnabled(bool value);
     bool isEnabled();
     void updateSampleCondition();
     void reset();
     void destroy();
};

class CPU : public Runnable {
	// Thread:
	Thread myThread;

	// References to other parts of NES :
	 NES* nes;
	 IMemoryMapper* mmap;
	 short[] mem;

	// CPU Registers:
	 int REG_ACC_NEW;
	 int REG_X_NEW;
	 int REG_Y_NEW;
	 int REG_STATUS_NEW;
	 int REG_PC_NEW;
	 int REG_SP;

	// Status flags:
	 int F_CARRY_NEW;
	 int F_ZERO_NEW;
	 int F_INTERRUPT_NEW;
	 int F_DECIMAL_NEW;
	 int F_BRK_NEW;
	 int F_NOTUSED_NEW;
	 int F_OVERFLOW_NEW;
	 int F_SIGN_NEW;

	// IRQ Types:
	 static const int IRQ_NORMAL = 0;
	 static const int IRQ_NMI    = 1;
	 static const int IRQ_RESET  = 2;

	// Interrupt notification:
	 bool irqRequested;
	 int irqType;

	// Op/Inst Data:
	 int[] opdata;

	// Misc vars:
	 int cyclesToHalt;
	 bool stopRunning;
	 bool crash;


public:
	 CPU(NES* nes);
	 void init();
	 void stateLoad(ByteBuffer* buf);
	 void stateSave(ByteBuffer* buf);
	 void reset();
	 synchronized void beginExecution();
	 synchronized void endExecution();
	 bool isRunning();
	 void run();
	 synchronized void initRun();
	 void emulate();
	 int load(int addr);
	 int load16bit(int addr);
	 void write(int addr, short val);
	 void requestIrq(int type);
	 void push(int value);
	 void stackWrap();
	 short pull();
	 bool pageCrossed(int addr1, int addr2);
	 void haltCycles(int cycles);
	 void doNonMaskableInterrupt(int status);
	 void doResetInterrupt();
	 void doIrq(int status);
	 int getStatus();
	 void setStatus(int st);
	 void setCrashed(bool value);
	 void setMapper(IMemoryMapper* mapper);
	 void destroy();
};

class CpuInfo {
public:
    // Opdata array:
     static int[] opdata;
    // Instruction names:
     static string[] instname;
    // Address mode descriptions:
     static string[] addrDesc;
     static int[] cycTable;
    // Instruction types:
    // -------------------------------- //
     static const int INS_ADC = 0;
     static const int INS_AND = 1;
     static const int INS_ASL = 2;
     static const int INS_BCC = 3;
     static const int INS_BCS = 4;
     static const int INS_BEQ = 5;
     static const int INS_BIT = 6;
     static const int INS_BMI = 7;
     static const int INS_BNE = 8;
     static const int INS_BPL = 9;
     static const int INS_BRK = 10;
     static const int INS_BVC = 11;
     static const int INS_BVS = 12;
     static const int INS_CLC = 13;
     static const int INS_CLD = 14;
     static const int INS_CLI = 15;
     static const int INS_CLV = 16;
     static const int INS_CMP = 17;
     static const int INS_CPX = 18;
     static const int INS_CPY = 19;
     static const int INS_DEC = 20;
     static const int INS_DEX = 21;
     static const int INS_DEY = 22;
     static const int INS_EOR = 23;
     static const int INS_INC = 24;
     static const int INS_INX = 25;
     static const int INS_INY = 26;
     static const int INS_JMP = 27;
     static const int INS_JSR = 28;
     static const int INS_LDA = 29;
     static const int INS_LDX = 30;
     static const int INS_LDY = 31;
     static const int INS_LSR = 32;
     static const int INS_NOP = 33;
     static const int INS_ORA = 34;
     static const int INS_PHA = 35;
     static const int INS_PHP = 36;
     static const int INS_PLA = 37;
     static const int INS_PLP = 38;
     static const int INS_ROL = 39;
     static const int INS_ROR = 40;
     static const int INS_RTI = 41;
     static const int INS_RTS = 42;
     static const int INS_SBC = 43;
     static const int INS_SEC = 44;
     static const int INS_SED = 45;
     static const int INS_SEI = 46;
     static const int INS_STA = 47;
     static const int INS_STX = 48;
     static const int INS_STY = 49;
     static const int INS_TAX = 50;
     static const int INS_TAY = 51;
     static const int INS_TSX = 52;
     static const int INS_TXA = 53;
     static const int INS_TXS = 54;
     static const int INS_TYA = 55;
     static const int INS_DUMMY = 56; // dummy instruction used for 'halting' the processor some cycles
    // -------------------------------- //
    // Addressing modes:
     static const int ADDR_ZP = 0;
     static const int ADDR_REL = 1;
     static const int ADDR_IMP = 2;
     static const int ADDR_ABS = 3;
     static const int ADDR_ACC = 4;
     static const int ADDR_IMM = 5;
     static const int ADDR_ZPX = 6;
     static const int ADDR_ZPY = 7;
     static const int ADDR_ABSX = 8;
     static const int ADDR_ABSY = 9;
     static const int ADDR_PREIDXIND = 10;
     static const int ADDR_POSTIDXIND = 11;
     static const int ADDR_INDABS = 12;

     static int[] getOpData();
     static string[] getInstNames();
     static string getInstName(int inst);
     static string[] getAddressModeNames();
     static string getAddressModeName(int addrMode);
     static void initOpData();
     static void setOp(int inst, int op, int addr, int size, int cycles);
     static void initInstNames();
     static void initAddrDesc();
};

class FileLoader {
public:
     short[] loadFile(string fileName, UI ui);
};

class HiResTimer {
public:
     long currentMicros();
     long currentTick();
     void sleepMicros(long time);
     void sleepMillisIdle(int millis);
     void yield();
};

class KbInputHandler : public KeyListener {
    bool[] allKeysState;
    int[] keyMapping;
    int id;
    NES* nes;
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
    static final int NUM_KEYS = 8;
    
     KbInputHandler(NES* nes, int id);
     short getKeyState(int padKey);
     void mapKey(int padKey, int kbKeycode);
     void keyPressed(KeyEvent ke);
     void keyReleased(KeyEvent ke);
     void keyTyped(KeyEvent ke);
     void reset();
     void update();
     void destroy();
};

class Mapper001 : public MapperDefault {
    // Register flags:

    // Register 0:
    int mirroring;
    int oneScreenMirroring;
    int prgSwitchingArea = 1;
    int prgSwitchingSize = 1;
    int vromSwitchingSize;

    // Register 1:
    int romSelectionReg0;

    // Register 2:
    int romSelectionReg1;

    // Register 3:
    int romBankSelect;

    // 5-bit buffer:
    int regBuffer;
    int regBufferCounter;
public:
     void init(NES* nes);
     void mapperInternalStateLoad(ByteBuffer* buf);
     void mapperInternalStateSave(ByteBuffer* buf);
     void write(int address, short value);
     void setReg(int reg, int value);
     int getRegNumber(int address);
     void loadROM(ROM rom);
     void reset();
     void switchLowHighPrgRom(int oldSetting);
     void switch16to32();
     void switch32to16();
};

class MapperDefault : public IMemoryMapper {
     NES* nes;
     Memory cpuMem;
     Memory ppuMem;
     short[] cpuMemArray;
     ROM rom;
     CPU* cpu;
     PPU ppu;
     int cpuMemSize;
     int joy1StrobeState;
     int joy2StrobeState;
     int joypadLastWrite;
     bool mousePressed;
     bool gameGenieActive;
     int mouseX;
     int mouseY;
    int tmp;
public:
     void init(NES* nes);
     void stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     void mapperInternalStateLoad(ByteBuffer* buf);
     void mapperInternalStateSave(ByteBuffer* buf);
     void setGameGenieState(bool enable);
     bool getGameGenieState();
     void write(int address, short value);
     void writelow(int address, short value);
     short load(int address);
     short regLoad(int address);
     void regWrite(int address, short value);
     short joy1Read();
     short joy2Read();
     void loadROM(ROM rom);
    void loadPRGROM();
    void loadCHRROM();
     void loadBatteryRam();
    void loadRomBank(int bank, int address);
    void loadVromBank(int bank, int address);
    void load32kRomBank(int bank, int address);
    void load8kVromBank(int bank4kStart, int address);
    void load1kVromBank(int bank1k, int address);
    void load2kVromBank(int bank2k, int address);
    void load8kRomBank(int bank8k, int address);
     void clockIrqCounter();
     void latchAccess(int address);
     int syncV();
     int syncH(int scanline);
     void setMouseState(bool pressed, int x, int y);
     void reset();
     void destroy();
};

class Memory{
	 short[] mem;
	int memLength;
	NES* nes;
public:
	 Memory(NES* nes, int byteCount);
	 void stateLoad(ByteBuffer* buf);
	 void stateSave(ByteBuffer* buf);
	 void reset();
	 int getMemSize();
	 void write(int address, short value);
	 short load(int address);
	 void dump(string file);
	 void dump(string file, int offset, int length);
	 void write(int address, short[] array, int length);
	 void write(int address, short[] array, int arrayoffset, int length);
	 void destroy();
};

class Misc {
     static bool debug = Globals::debug;
     static float[] rnd = new float[100000];
     static int nextRnd = 0;
     static float rndret;

public:
    static {
        for (int i = 0; i < rnd.length; i++) {
            rnd[i] = (float) Math.random();
        }
    }

     static string hex8(int i);
     static string hex16(int i);
     static string binN(int num, int N);
     static string bin8(int num);
     static string bin16(int num);
     static string binStr(long value, int bitcount);
     static int[] resizeArray(int[] array, int newSize);
     static string pad(string str, string padStr, int length);
     static float random();
};

class NameTable {
    string name;
    short[] tile;
    short[] attrib;
    int width;
    int height;
public:
     NameTable(int width, int height, string name);
     short getTileIndex(int x, int y);
     short getAttrib(int x, int y);
     void writeTileIndex(int index, int value);
     void writeAttrib(int index, int value);
     void stateSave(ByteBuffer* buf);
     void stateLoad(ByteBuffer* buf);
};

class NES {
     AppletUI* gui;
     CPU* cpu;
     PPU ppu;
     PAPU* papu;
     Memory cpuMem;
     Memory ppuMem;
     Memory sprMem;
     IMemoryMapper* memMapper;
     PaletteTable palTable;
     ROM rom;
    int cc;
     string romFile;
    bool isRunning = false;
public:
     NES(AppletUI* gui);
     bool stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     bool isRunning();
     void startEmulation();
     void stopEmulation();
     void reloadRom();
     void clearCPUMemory();
     void setGameGenieState(bool enable);
     CPU getCpu();
     PPU getPpu();
     PAPU* getPapu();
     Memory getCpuMemory();
     Memory getPpuMemory();
     Memory getSprMemory();
     ROM getRom();
     UI getGui();
     IMemoryMapper* getMemoryMapper();
     bool loadRom(string file);
     void reset();
     void enableSound(bool enable);
     void setFramerate(int rate);
     void destroy();
};

class PaletteTable {
     static int[] curTable = new int[64];
     static int[] origTable = new int[64];
     static int[][] emphTable = new int[8][64];
    int currentEmph = -1;
    int currentHue, currentSaturation, currentLightness, currentContrast;
public:
     bool loadNTSCPalette();
     bool loadPALPalette();
     bool loadPalette(string file);
     void makeTables();
     void setEmphasis(int emph);
     int getEntry(int yiq);
     int RGBtoHSL(int r, int g, int b);
     int RGBtoHSL(int rgb);
     int HSLtoRGB(int h, int s, int l);
     int HSLtoRGB(int hsl);
     int getHue(int hsl);
     int getSaturation(int hsl);
     int getLightness(int hsl);
     int getRed(int rgb);
     int getGreen(int rgb);
     int getBlue(int rgb);
     int getRgb(int r, int g, int b);
     void updatePalette();
     void updatePalette(int hueAdd, int saturationAdd, int lightnessAdd, int contrastAdd);
     void loadDefaultPalette();
     void reset();
};

 class PAPU {
    NES* nes;
    Memory cpuMem;
    Mixer mixer;
    SourceDataLine line;
    ChannelSquare* square1;
    ChannelSquare* square2;
    ChannelTriangle* triangle;
    ChannelNoise* noise;
    ChannelDM* dmc;
    int[] lengthLookup;
    int[] dmcFreqLookup;
    int[] noiseWavelengthLookup;
    int[] square_table;
    int[] tnd_table;
    int[] ismpbuffer;
    int8_t* sampleBuffer;
    int frameIrqCounter;
    int frameIrqCounterMax;
    int initCounter;
    short channelEnableValue;
    int8_t b1, b2, b3, b4;
    int bufferSize = 2048;
    int bufferIndex;
    int sampleRate = 44100;
    bool frameIrqEnabled;
    bool frameIrqActive;
    bool frameClockNow;
    bool startedPlaying = false;
    bool recordOutput = false;
    bool stereo = true;
    bool initingHardware = false;
     bool userEnableSquare1 = true;
     bool userEnableSquare2 = true;
     bool userEnableTriangle = true;
     bool userEnableNoise = true;
     bool userEnableDmc = true;
    int masterFrameCounter;
    int derivedFrameCounter;
    int countSequence;
    int sampleTimer;
    int frameTime;
    int sampleTimerMax;
    int sampleCount;
    int sampleValueL, sampleValueR;
    int triValue = 0;
    int smpSquare1, smpSquare2, smpTriangle, smpNoise, smpDmc;
    int accCount;
    int sq_index, tnd_index;

    // DC removal vars:
    int prevSampleL = 0, prevSampleR = 0;
    int smpAccumL = 0, smpAccumR = 0;
    int smpDiffL = 0, smpDiffR = 0;

    // DAC range:
    int dacRange = 0;
    int dcValue = 0;

    // Master volume:
    int masterVolume;

    // Panning:
    int[] panning;

    // Stereo positioning:
    int stereoPosLSquare1;
    int stereoPosLSquare2;
    int stereoPosLTriangle;
    int stereoPosLNoise;
    int stereoPosLDMC;
    int stereoPosRSquare1;
    int stereoPosRSquare2;
    int stereoPosRTriangle;
    int stereoPosRNoise;
    int stereoPosRDMC;
    int extraCycles;
    int maxCycles;
public:
     PAPU(NES* nes);
     void stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     synchronized void start();
     NES* getNes();
     short readReg(int address);
     void writeReg(int address, short value);
     void resetCounter();
     void updateChannelEnable(int value);
     void clockFrameCounter(int nCycles);
     void accSample(int cycles);
     void frameCounterTick();
     void sample();
     void writeBuffer();
     void stop();
     int getSampleRate();
     void reset();
     int getLengthMax(int value);
     int getDmcFrequency(int value);
     int getNoiseWaveLength(int value);
     synchronized void setSampleRate(int rate, bool restart);
     synchronized void setStereo(bool s, bool restart);
     int getPapuBufferSize();
     void setChannelEnabled(int channel, bool value);
     void setPanning(int[] pos);
     void setMasterVolume(int value);
     void updateStereoPos();
     SourceDataLine getLine();
     bool isRunning();
     int getMillisToAvailableAbove(int target_avail);
     int getBufferPos();
     void initLengthLookup();
     void initDmcFrequencyLookup();
     void initNoiseWavelengthLookup();
     void initDACtables();
     void destroy();
};

class PPU {
     NES* nes;
     HiResTimer* timer;
     Memory ppuMem;
     Memory sprMem;
    // Rendering Options:
    bool showSpr0Hit = false;
    bool showSoundBuffer = false;
    bool clipTVcolumn = true;
    bool clipTVrow = false;
    // Control Flags Register 1:
     int f_nmiOnVblank;    // NMI on VBlank. 0=disable, 1=enable
     int f_spriteSize;     // Sprite size. 0=8x8, 1=8x16
     int f_bgPatternTable; // Background Pattern Table address. 0=0x0000,1=0x1000
     int f_spPatternTable; // Sprite Pattern Table address. 0=0x0000,1=0x1000
     int f_addrInc;        // PPU Address Increment. 0=1,1=32
     int f_nTblAddress;    // Name Table Address. 0=0x2000,1=0x2400,2=0x2800,3=0x2C00
    // Control Flags Register 2:
     int f_color;	   	 	 // Background color. 0=black, 1=blue, 2=green, 4=red
     int f_spVisibility;   // Sprite visibility. 0=not displayed,1=displayed
     int f_bgVisibility;   // Background visibility. 0=Not Displayed,1=displayed
     int f_spClipping;     // Sprite clipping. 0=Sprites invisible in left 8-pixel column,1=No clipping
     int f_bgClipping;     // Background clipping. 0=BG invisible in left 8-pixel column, 1=No clipping
     int f_dispType;       // Display type. 0=color, 1=monochrome
    // Status flags:
     int STATUS_VRAMWRITE = 4;
     int STATUS_SLSPRITECOUNT = 5;
     int STATUS_SPRITE0HIT = 6;
     int STATUS_VBLANK = 7;
    // VRAM I/O:
    int vramAddress;
    int vramTmpAddress;
    short vramBufferedReadValue;
    bool firstWrite = true; 		// VRAM/Scroll Hi/Lo latch
    int[] vramMirrorTable; 			// Mirroring Lookup Table.
    int i;

    // SPR-RAM I/O:
    short sramAddress; // 8-bit only.

    // Counters:
    int cntFV;
    int cntV;
    int cntH;
    int cntVT;
    int cntHT;

    // Registers:
    int regFV;
    int regV;
    int regH;
    int regVT;
    int regHT;
    int regFH;
    int regS;

    // VBlank extension for PAL emulation:
    int vblankAdd = 0;
     int curX;
     int scanline;
     int lastRenderedScanline;
     int mapperIrqCounter;
    // Sprite data:
     int[] sprX;				// X coordinate
     int[] sprY;				// Y coordinate
     int[] sprTile;			// Tile Index (into pattern table)
     int[] sprCol;			// Upper two bits of color
     bool[] vertFlip;		// Vertical Flip
     bool[] horiFlip;		// Horizontal Flip
     bool[] bgPriority;	// Background priority
     int spr0HitX;	// Sprite #0 hit X coordinate
     int spr0HitY;	// Sprite #0 hit Y coordinate
    bool hitSpr0;

    // Tiles:
     Tile[] ptTile;
    // Name table data:
    int[] ntable1 = new int[4];
    NameTable[] nameTable;
    int currentMirroring = -1;

    // Palette data:
    int[] sprPalette = new int[16];
    int[] imgPalette = new int[16];
    // Misc:
    bool scanlineAlreadyRendered;
    bool requestEndFrame;
    bool nmiOk;
    int nmiCounter;
    short tmp;
    bool dummyCycleToggle;

    // Vars used when updating regs/address:
    int address, b1, b2;
    // Variables used when rendering:
    int[] attrib = new int[32];
    int[] bgbuffer = new int[256 * 240];
    int[] pixrendered = new int[256 * 240];
    int[] spr0dummybuffer = new int[256 * 240];
    int[] dummyPixPriTable = new int[256 * 240];
    int[] oldFrame = new int[256 * 240];
    int[] buffer;
    int[] tpix;
    bool[] scanlineChanged = new bool[240];
    bool requestRenderAll = false;
    bool validTileData;
    int att;
    Tile[] scantile = new Tile[32];
    Tile t;
    // These are temporary variables used in rendering and sound procedures.
    // Their states outside of those procedures can be ignored.
    int curNt;
    int destIndex;
    int x, y, sx;
    int si, ei;
    int tile;
    int col;
    int baseTile;
    int tscanoffset;
    int srcy1, srcy2;
    int bufferSize, available, scale;
     int cycles = 0;
public:
     PPU(NES* nes);
     void init();
     void setMirroring(int mirroring);
     void defineMirrorRegion(int fromStart, int toStart, int size);
     void emulateCycles();
     void startVBlank();
     void endScanline();
     void startFrame();
     void endFrame();
     void updateControlReg1(int value);
     void updateControlReg2(int value);
     void setStatusFlag(int flag, bool value);
     short readStatusRegister();
     void writeSRAMAddress(short address);
     short sramLoad();
     void sramWrite(short value);
     void scrollWrite(short value);
     void writeVRAMAddress(int address);
     short vramLoad();
     void vramWrite(short value);
     void sramDMA(short value);
     void regsFromAddress();
     void cntsFromAddress();
     void regsToAddress();
     void cntsToAddress();
     void incTileCounter(int count);
     short mirroredLoad(int address);
     void mirroredWrite(int address, short value);
     void triggerRendering();
     void renderFramePartially(int[] buffer, int startScan, int scanCount);
     void renderBgScanline(int[] buffer, int scan);
     void renderSpritesPartially(int startscan, int scancount, bool bgPri);
     bool checkSprite0(int scan);
     void renderPattern();
     void renderNameTables();
     void renderPalettes();
     void writeMem(int address, short value);
     void updatePalettes();
     void patternWrite(int address, short value);
     void patternWrite(int address, short[] value, int offset, int length);
     void invalidateFrameCache();
     void nameTableWrite(int index, int address, short value);
     void attribTableWrite(int index, int address, short value);
     void spriteRamWriteUpdate(int address, short value);
     void doNMI();
     int statusRegsToInt();
     void statusRegsFromInt(int n);
     void stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     void reset();
     void destroy();
};

class Raster {
     int[] data;
     int width;
     int height;
public:
     Raster(int[] data, int w, int h);
     Raster(int w, int h);
     void drawTile(Raster srcRaster, int srcx, int srcy, int dstx, int dsty, int w, int h);
};

class ROM {
    // Mirroring types:
     static const int VERTICAL_MIRRORING = 0;
     static const int HORIZONTAL_MIRRORING = 1;
     static const int FOURSCREEN_MIRRORING = 2;
     static const int SINGLESCREEN_MIRRORING = 3;
     static const int SINGLESCREEN_MIRRORING2 = 4;
     static const int SINGLESCREEN_MIRRORING3 = 5;
     static const int SINGLESCREEN_MIRRORING4 = 6;
     static const int CHRROM_MIRRORING = 7;
    bool failedSaveFile = false;
    bool saveRamUpToDate = true;
    short[] header;
    short[][] rom;
    short[][] vrom;
    short[] saveRam;
    Tile[][] vromTile;
    NES* nes;
    int romCount;
    int vromCount;
    int mirroring;
    bool batteryRam;
    bool trainer;
    bool fourScreen;
    int mapperType;
    string fileName;
    RandomAccessFile raFile;
    bool enableSave = true;
    bool valid;
    static string[] mapperName;
    static bool[] mapperSupported;
public:
    static {

        mapperName = new string[255];
        mapperSupported = new bool[255];
        for (int i = 0; i < 255; i++) {
            mapperName[i] = "Unknown Mapper";
        }

        mapperName[ 0] = "NROM";
        mapperName[ 1] = "Nintendo MMC1";
        mapperName[ 2] = "UxROM";
        mapperName[ 3] = "CNROM";
        mapperName[ 4] = "Nintendo MMC3";
        mapperName[ 5] = "Nintendo MMC5";
        mapperName[ 6] = "FFE F4xxx";
        mapperName[ 7] = "AxROM";
        mapperName[ 8] = "FFE F3xxx";
        mapperName[ 9] = "Nintendo MMC2";
        mapperName[10] = "Nintendo MMC4";
        mapperName[11] = "Color Dreams";
        mapperName[12] = "FFE F6xxx";
        mapperName[13] = "CPROM";
        mapperName[15] = "iNES Mapper #015";
        mapperName[16] = "Bandai";
        mapperName[17] = "FFE F8xxx";
        mapperName[18] = "Jaleco SS8806";
        mapperName[19] = "Namcot 106";
        mapperName[20] = "(Hardware) Famicom Disk System";
        mapperName[21] = "Konami VRC4a, VRC4c";
        mapperName[22] = "Konami VRC2a";
        mapperName[23] = "Konami VRC2b, VRC4e, VRC4f";
        mapperName[24] = "Konami VRC6a";
        mapperName[25] = "Konami VRC4b, VRC4d";
        mapperName[26] = "Konami VRC6b";
        mapperName[32] = "Irem G-101";
        mapperName[33] = "Taito TC0190, TC0350";
        mapperName[34] = "BxROM, NINA-001";
        mapperName[41] = "Caltron 6-in-1";
        mapperName[46] = "Rumblestation 15-in-1";
        mapperName[47] = "Nintendo MMC3 Multicart (Super Spike V'Ball + Nintendo World Cup)";
        mapperName[48] = "iNES Mapper #048";
        mapperName[64] = "Tengen RAMBO-1";
        mapperName[65] = "Irem H-3001";
        mapperName[66] = "GxROM";
        mapperName[67] = "Sunsoft 3";
        mapperName[68] = "Sunsoft 4";
        mapperName[69] = "Sunsoft FME-7";
        mapperName[70] = "iNES Mapper #070";
        mapperName[71] = "Camerica";
        mapperName[72] = "iNES Mapper #072";
        mapperName[73] = "Konami VRC3";
        mapperName[75] = "Konami VRC1";
        mapperName[76] = "iNES Mapper #076 (Digital Devil Monogatari - Megami Tensei)";
        mapperName[77] = "iNES Mapper #077 (Napoleon Senki)";
        mapperName[78] = "Irem 74HC161/32";
        mapperName[79] = "American Game Cartridges";
        mapperName[80] = "iNES Mapper #080";
        mapperName[82] = "iNES Mapper #082";
        mapperName[85] = "Konami VRC7a, VRC7b";
        mapperName[86] = "iNES Mapper #086 (Moero!! Pro Yakyuu)";
        mapperName[87] = "iNES Mapper #087";
        mapperName[88] = "iNES Mapper #088";
        mapperName[89] = "iNES Mapper #087 (Mito Koumon)";
        mapperName[92] = "iNES Mapper #092";
        mapperName[93] = "iNES Mapper #093 (Fantasy Zone)";
        mapperName[94] = "iNES Mapper #094 (Senjou no Ookami)";
        mapperName[95] = "iNES Mapper #095 (Dragon Buster) [MMC3 Derived]";
        mapperName[96] = "(Hardware) Oeka Kids Tablet";
        mapperName[97] = "iNES Mapper #097 (Kaiketsu Yanchamaru)";
        mapperName[105] = "NES-EVENT [MMC1 Derived]";
        mapperName[113] = "iNES Mapper #113";
        mapperName[115] = "iNES Mapper #115 (Yuu Yuu Hakusho Final) [MMC3 Derived]";
        mapperName[118] = "iNES Mapper #118 [MMC3 Derived]";
        mapperName[119] = "TQROM";
        mapperName[140] = "iNES Mapper #140 (Bio Senshi Dan)";
        mapperName[152] = "iNES Mapper #152";
        mapperName[154] = "iNES Mapper #152 (Devil Man)";
        mapperName[159] = "Bandai (Alternate of #016)";
        mapperName[180] = "(Hardware) Crazy Climber Controller";
        mapperName[182] = "iNES Mapper #182";
        mapperName[184] = "iNES Mapper #184";
        mapperName[185] = "iNES Mapper #185";
        mapperName[207] = "iNES Mapper #185 (Fudou Myouou Den)";
        mapperName[228] = "Active Enterprises";
        mapperName[232] = "Camerica (Quattro series)";

        // The mappers supported:
        mapperSupported[ 0] = true; // No Mapper
        mapperSupported[ 1] = true; // MMC1
        mapperSupported[ 2] = true; // UNROM
        mapperSupported[ 3] = true; // CNROM
        mapperSupported[ 4] = true; // MMC3
        mapperSupported[ 7] = true; // AOROM
        mapperSupported[ 9] = true; // MMC2
        mapperSupported[10] = true; // MMC4
        mapperSupported[11] = true; // ColorDreams
        mapperSupported[15] = true;
        mapperSupported[18] = true;
        mapperSupported[21] = true;
        mapperSupported[22] = true;
        mapperSupported[23] = true;
        mapperSupported[32] = true;
        mapperSupported[33] = true;
        mapperSupported[34] = true; // BxROM
        mapperSupported[48] = true;
        mapperSupported[64] = true;
        mapperSupported[66] = true; // GNROM
        mapperSupported[68] = true; // SunSoft4 chip
        mapperSupported[71] = true; // Camerica
        mapperSupported[72] = true;
        mapperSupported[75] = true;
        mapperSupported[78] = true;
        mapperSupported[79] = true;
        mapperSupported[87] = true;
        mapperSupported[94] = true;
        mapperSupported[105] = true;
        mapperSupported[140] = true;
        mapperSupported[182] = true;
        mapperSupported[232] = true; // Camerica /Quattro
    }

     ROM(NES* nes);
     void load(string fileName);
     bool isValid();
     int getRomBankCount();
     int getVromBankCount();
     short[] getHeader();
     short[] getRomBank(int bank);
     short[] getVromBank(int bank);
     Tile[] getVromBankTiles(int bank);
     int getMirroringType();
     int getMapperType();
     string getMapperName();
     bool hasBatteryRam();
     bool hasTrainer();
     string getFileName();
     bool mapperSupported();
     IMemoryMapper* createMapper();
     void setSaveState(bool enableSave);
     short[] getBatteryRam();
     void loadBatteryRam();
     void writeBatteryRam(int address, short value);
     void closeRom();
     void destroy();
};

class Scale {
public:
     static int brightenShift;
     static int brightenShiftMask;
     static int brightenCutoffMask;
     static int darkenShift;
     static int darkenShiftMask;
     static int si,  di,  di2,  val,  x,  y;

     static void setFilterParams(int darkenDepth, int brightenDepth);
     static const void doScanlineScaling(int[] src, int[] dest, bool[] changed);
     static const void doRasterScaling(int[] src, int[] dest, bool[] changed);
     static const void doNormalScaling(int[] src, int[] dest, bool[] changed);
};

class MyMouseAdapter : public MouseAdapter {
    long lastClickTime = 0;
public:
    void mouseClicked(MouseEvent me);
    void mousePressed(MouseEvent me);
};

class ScreenView : public BufferView {
     MyMouseAdapter mouse;
     bool notifyImageReady;
public:
     ScreenView(NES* nes, int width, int height);
     void init();
     void mouseReleased(MouseEvent me);
     void setNotifyImageReady(bool value);
     void imageReady(bool skipFrame);
};

class Tile {
    // Tile data:
    int[] pix;
    int fbIndex;
    int tIndex;
    int x, y;
    int w, h;
    int incX, incY;
    int palIndex;
    int tpri;
    int c;
     bool initialized = false;
     bool[] opaque = new bool[8];
public:
     Tile();
     void setBuffer(short[] scanline);
     void setScanline(int sline, short b1, short b2);
     void renderSimple(int dx, int dy, int[] fBuffer, int palAdd, int[] palette);
     void renderSmall(int dx, int dy, int[] buffer, int palAdd, int[] palette);
     void render(int srcx1, int srcy1, int srcx2, int srcy2, int dx, int dy, int[] fBuffer, int palAdd, int[] palette, bool flipHorizontal, bool flipVertical, int pri, int[] priTable);
     bool isTransparent(int x, int y);
     void dumpData(string file);
     void stateSave(ByteBuffer* buf);
     void stateLoad(ByteBuffer* buf);
};

class vNES : public thread {
    bool scale;
    bool scanlines;
    bool sound;
    bool fps;
    bool stereo;
    bool timeemulation;
    bool showsoundbuffer;
    int samplerate;
    int romSize;
    int progress;
    AppletUI* gui;
    NES* nes;
    ScreenView* panelScreen;
    string rom;
    Font* progressFont;
    Color* bgColor;
    bool started;
public:
    void init();
    void addScreenView();
    void start();
    void run();
    void stop();
    void destroy();
    void showLoadProgress(int percentComplete);
    void paint(Graphics* g);
    void update(Graphics* g);
    void readParams();
    void initKeyCodes();
};

#endif

