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

#ifndef CPP_NES_GLOBALS
#define CPP_NES_GLOBALS

#include <assert.h>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <SDL/SDL.h>

#include "Color.h"

using namespace std;



// Temp stub classes
class Point {
public:
	int x;
	int y;
	
	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}
};
class Graphics {};
class BufferedImage {};
class VolatileImage {};
class Font {
public:
	static const int BOLD = 0;
	Font(string name, int style, int size) {
		
	}
};
class KeyListener {};
class KeyEvent {
public:
	static const int VK_F5 = 0; // FIXME
	static const int VK_F10 = 1; // FIXME
	static const int VK_F12 = 2; // FIXME
	int getKeyCode() {
		return 0;
	}
};
class MouseAdapter {
public:
	void setFocusable(bool derp) {
	}
	void requestFocus() {
	}
};
class MouseEvent {
public:
	int getX() {
		return 0;
	}
	int getY() {
		return 0;
	}
};
class Mixer {};
class SourceDataLine {
public:
	size_t getBufferSize() { return 0; };
	bool isActive() { return false; }
	int available() { return 0; }
};
class Graphics2D {};

// Forward declarations
//class IMemoryMapper;
class IPapuChannel;

class ByteBuffer;
class ChannelDM;
class ChannelNoise;
class ChannelSquare;
class ChannelTriangle;
class CPU;
class CpuInfo;
class FileLoader;
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
class Tile;
class vNES;

// Interfaces
/*
class IMemoryMapper {
public:
     virtual void init(NES* nes) = 0;
     virtual void loadROM(ROM* rom) = 0;
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
*/
class IPapuChannel {
     virtual void writeReg(int address, int value) = 0;
     virtual void setEnabled(bool value) = 0;
     virtual bool isEnabled() = 0;
     virtual void reset() = 0;
     virtual int getLengthStatus() = 0;
};

class Parameters {
public:
	static const string rom;
	static size_t romsize;
	static bool scale;
	static bool sound;
	static bool stereo;
	static bool scanlines;
	static bool fps;
	static bool timeemulation;
	static bool showsoundbuffer;
	static string p1_up;
	static string p1_down;
	static string p1_left;
	static string p1_right;
	static string p1_a;
	static string p1_b;
	static string p1_start;
	static string p1_select;
	
	static string p2_up;
	static string p2_down;
	static string p2_left;
	static string p2_right;
	static string p2_a;
	static string p2_b;
	static string p2_start;
	static string p2_select;
};

// Class Prototypes
class Globals {
public:
    static SDL_Surface* sdl_screen;
    static NES* nes;
    static double CPU_FREQ_NTSC;
    static double CPU_FREQ_PAL;
    static int preferredFrameRate;
    
    // Microseconds per frame:
    static const double NS_PER_FRAME;
    // What value to flush memory with on power-up:
    static short memoryFlushValue;

    static const bool debug;
    static const bool fsdebug;

    static bool appletMode;
    static bool disableSprites;
    static bool timeEmulation;
    static bool palEmulation;
    static bool enableSound;
    static bool focused;

    static std::map<string, uint32_t> keycodes; //Java key codes
    static std::map<string, string> controls; //vNES controls codes

    static void println(string s);
};

class ByteBuffer {
public:
	 static const bool debug = false;
	 static const int BO_BIG_ENDIAN = 0;
     static const int BO_LITTLE_ENDIAN = 1;
     
     vector<short>* buf;
     int size;
     int byteOrder;
     size_t curPos;
     bool hasBeenErrors;
     bool expandable;
     int expandBy;

    ByteBuffer(int size, int byteOrdering);
    ByteBuffer(vector<int8_t>* content, int byteOrdering);
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
    bool putByteArray(vector<short>* arr);
    bool readByteArray(vector<short>* arr);
    bool putShortArray(vector<short>* arr);
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
    vector<short>* expandShortArray(vector<short>* array, int size);
    void crop();
    static ByteBuffer* asciiEncode(ByteBuffer* buf);
    static ByteBuffer* asciiDecode(ByteBuffer* buf);
    //static void saveToZipFile(File f, ByteBuffer* buf);
    //static ByteBuffer* readFromZipFile(File f);
};

class ChannelDM : public IPapuChannel {
public:
    static const int MODE_NORMAL = 0;
    static const int MODE_LOOP = 1;
    static const int MODE_IRQ = 2;
    
    PAPU* papu;
    bool _isEnabled;
    bool hasSample;
    bool irqGenerated;
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
public:
    PAPU* papu;
     bool _isEnabled;
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
     long accValue;
     long accCount;
     int tmp;

     ChannelNoise(PAPU* papu);
     void clockLengthCounter();
     void clockEnvDecay();
     void updateSampleValue();
     void writeReg(int address, int value);
     void setEnabled(bool value);
     bool isEnabled();
     int getLengthStatus();
     void reset();
     void destroy();
};

class ChannelSquare : public IPapuChannel {
public:
    static const int dutyLookup[32];
    static const int impLookup[32];

    PAPU* papu;
    bool sqr1;
    bool _isEnabled;
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
public:
    PAPU* papu;
    bool _isEnabled;
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

class CPU {
public:
	// IRQ Types:
	static const int IRQ_NORMAL = 0;
	static const int IRQ_NMI    = 1;
	static const int IRQ_RESET  = 2;
	 
	// References to other parts of NES :
	 NES* nes;
	 MapperDefault* mmap;
	 vector<short>* mem;

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

	// Interrupt notification:
	 bool irqRequested;
	 int irqType;

	// Op/Inst Data:
	 vector<int>* opdata;

	// Misc vars:
	 int cyclesToHalt;
	 bool stopRunning;
	 bool crash;

	 CPU(NES* nes);
	 void init();
	 void stateLoad(ByteBuffer* buf);
	 void stateSave(ByteBuffer* buf);
	 void reset();
	 void run();
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
	 void setMapper(MapperDefault* mapper);
	 void destroy();
};

class CpuInfo {
public:
    // Opdata array:
     static vector<int>* opdata;
    // Instruction names:
     static vector<string>* instname;
    // Address mode descriptions:
     static vector<string>* addrDesc;
     static const int cycTable[256];
    // Instruction types:
    // -------------------------------- //
     static const int INS_ADC;
     static const int INS_AND;
     static const int INS_ASL;
     static const int INS_BCC;
     static const int INS_BCS;
     static const int INS_BEQ;
     static const int INS_BIT;
     static const int INS_BMI;
     static const int INS_BNE;
     static const int INS_BPL;
     static const int INS_BRK;
     static const int INS_BVC;
     static const int INS_BVS;
     static const int INS_CLC;
     static const int INS_CLD;
     static const int INS_CLI;
     static const int INS_CLV;
     static const int INS_CMP;
     static const int INS_CPX;
     static const int INS_CPY;
     static const int INS_DEC;
     static const int INS_DEX;
     static const int INS_DEY;
     static const int INS_EOR;
     static const int INS_INC;
     static const int INS_INX;
     static const int INS_INY;
     static const int INS_JMP;
     static const int INS_JSR;
     static const int INS_LDA;
     static const int INS_LDX;
     static const int INS_LDY;
     static const int INS_LSR;
     static const int INS_NOP;
     static const int INS_ORA;
     static const int INS_PHA;
     static const int INS_PHP;
     static const int INS_PLA;
     static const int INS_PLP;
     static const int INS_ROL;
     static const int INS_ROR;
     static const int INS_RTI;
     static const int INS_RTS;
     static const int INS_SBC;
     static const int INS_SEC;
     static const int INS_SED;
     static const int INS_SEI;
     static const int INS_STA;
     static const int INS_STX;
     static const int INS_STY;
     static const int INS_TAX;
     static const int INS_TAY;
     static const int INS_TSX;
     static const int INS_TXA;
     static const int INS_TXS;
     static const int INS_TYA;
     static const int INS_DUMMY; // dummy instruction used for 'halting' the processor some cycles
    // -------------------------------- //
    // Addressing modes:
     static const int ADDR_ZP;
     static const int ADDR_REL;
     static const int ADDR_IMP;
     static const int ADDR_ABS;
     static const int ADDR_ACC;
     static const int ADDR_IMM;
     static const int ADDR_ZPX;
     static const int ADDR_ZPY;
     static const int ADDR_ABSX;
     static const int ADDR_ABSY;
     static const int ADDR_PREIDXIND;
     static const int ADDR_POSTIDXIND;
     static const int ADDR_INDABS;

     static vector<int>* getOpData();
     static vector<string>* getInstNames();
     static string getInstName(size_t inst);
     static vector<string>* getAddressModeNames();
     static string getAddressModeName(int addrMode);
     static void initOpData();
     static void setOp(int inst, int op, int addr, int size, int cycles);
     static void initInstNames();
     static void initAddrDesc();
};

class FileLoader {
public:
     uint8_t* loadFile(string fileName, size_t& length);
};

class KbInputHandler : public KeyListener {
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

    NES* nes;
    int id;
    vector<bool>* allKeysState;
    vector<int>* keyMapping;

     KbInputHandler(NES* nes, int id);
     short getKeyState(int padKey);
     void mapKey(int padKey, int kbKeycode);
     void poll_for_key_events();
     void keyTyped(KeyEvent* ke);
     void reset();
     void update();
     void destroy();
};

class Memory {
public:
	NES* nes;
	vector<short>* mem;
	int memLength;

	 Memory(NES* nes, int byteCount);
	 void stateLoad(ByteBuffer* buf);
	 void stateSave(ByteBuffer* buf);
	 void reset();
	 int getMemSize();
	 void write(int address, short value);
	 short load(int address);
	 void dump(string file);
	 void dump(string file, int offset, int length);
	 void write(int address, vector<short>* array, int length);
	 void write(int address, vector<short>* array, int arrayoffset, int length);
	 void destroy();
};

class MapperDefault {
public:
     NES* nes;
     Memory* cpuMem;
     Memory* ppuMem;
     vector<short>* cpuMemArray;
     ROM* rom;
     CPU* cpu;
     PPU* ppu;
     int cpuMemSize;
     int joy1StrobeState;
     int joy2StrobeState;
     int joypadLastWrite;
     bool mousePressed;
     bool gameGenieActive;
     int mouseX;
     int mouseY;
    int tmp;

     MapperDefault();
     virtual void write(int address, short value);
     virtual void init(NES* nes);
     void base_init(NES* nes);     
     void stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     void mapperInternalStateLoad(ByteBuffer* buf);
     void mapperInternalStateSave(ByteBuffer* buf);
     void setGameGenieState(bool enable);
     bool getGameGenieState();
     void base_write(int address, short value);
     void writelow(int address, short value);
     short load(int address);
     short regLoad(int address);
     void regWrite(int address, short value);
     short joy1Read();
     short joy2Read();
     void loadROM(ROM* rom);
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

class Mapper001 : public MapperDefault {
public:
    // Register flags:

    // Register 0:
    int mirroring;
    int oneScreenMirroring;
    int prgSwitchingArea;
    int prgSwitchingSize;
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

     Mapper001();
     void init(NES* nes);
     void mapperInternalStateLoad(ByteBuffer* buf);
     void mapperInternalStateSave(ByteBuffer* buf);
     void write(int address, short value);
     void setReg(int reg, int value);
     int getRegNumber(int address);
     void loadROM(ROM* rom);
     void reset();
     void switchLowHighPrgRom(int oldSetting);
     void switch16to32();
     void switch32to16();
};

class Misc {
public:
     static bool debug;
     static vector<float>* _rnd;
     static int nextRnd;
     static float rndret;

     static vector<float>* rnd();
     static string hex8(int i);
     static string hex16(int i);
     static string binN(int num, int N);
     static string bin8(int num);
     static string bin16(int num);
     static string binStr(long value, int bitcount);
     static vector<int>* resizeArray(vector<int>* array, int newSize);
     static string pad(string str, string padStr, int length);
     static float random();
};

class NameTable {
public:
    string name;
    vector<short>* tile;
    vector<short>* attrib;
    int width;
    int height;

     NameTable(int width, int height, string name);
     short getTileIndex(int x, int y);
     short getAttrib(int x, int y);
     void writeTileIndex(int index, int value);
     void writeAttrib(int index, int value);
     void stateSave(ByteBuffer* buf);
     void stateLoad(ByteBuffer* buf);
};

class NES {
public:
     CPU* cpu;
     PPU* ppu;
     PAPU* papu;
     KbInputHandler* _joy1;
     KbInputHandler* _joy2;
     Memory* cpuMem;
     Memory* ppuMem;
     Memory* sprMem;
     MapperDefault* memMapper;
     PaletteTable* palTable;
     ROM* rom;
     int cc;
     string romFile;
    bool _isRunning;

     NES();
     bool stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     bool isRunning();
     void startEmulation();
     void stopEmulation();
     void reloadRom();
     void clearCPUMemory();
     void dumpRomMemory();
     void dumpCPUMemory();
     void setGameGenieState(bool enable);
     CPU* getCpu();
     PPU* getPpu();
     PAPU* getPapu();
     Memory* getCpuMemory();
     Memory* getPpuMemory();
     Memory* getSprMemory();
     ROM* getRom();
     MapperDefault* getMemoryMapper();
     bool loadRom(string file);
     void reset();
     void enableSound(bool enable);
//     void setFramerate(int rate);
     void destroy();
};

class PaletteTable {
public:
     static int curTable[64];
     static int origTable[64];
     static int emphTable[8][64];
     
    int currentEmph;
    int currentHue, currentSaturation, currentLightness, currentContrast;

	 PaletteTable();
     bool loadNTSCPalette();
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
	 static void setRed(int& rgb, int r);
	 static void setGreen(int& rgb, int g);
	 static void setBlue(int& rgb, int b);
     int getRgb(int r, int g, int b);
     void updatePalette();
     void updatePalette(int hueAdd, int saturationAdd, int lightnessAdd, int contrastAdd);
     void loadDefaultPalette();
     void reset();
};

 class PAPU {
 public:
    // Panning:
    static const int panning[];
    static const int lengthLookup[];

    mutable pthread_mutex_t _mutex;
    NES* nes;
    Memory* cpuMem;
    Mixer* mixer;
    ChannelSquare* square1;
    ChannelSquare* square2;
    ChannelTriangle* triangle;
    ChannelNoise* noise;
    ChannelDM* dmc;
    int* dmcFreqLookup;
    int* noiseWavelengthLookup;
    vector<int>* square_table;
    vector<int>* tnd_table;
    vector<int>* ismpbuffer;
    vector<int8_t>* sampleBuffer;
    int frameIrqCounter;
    int frameIrqCounterMax;
    int initCounter;
    short channelEnableValue;
    int8_t b1, b2, b3, b4;
    int bufferSize;
    int sampleRate;
    bool frameIrqEnabled;
    bool frameIrqActive;
    bool frameClockNow;
    bool startedPlaying;
    bool recordOutput;
    bool stereo;
    bool initingHardware;
     bool userEnableSquare1;
     bool userEnableSquare2;
     bool userEnableTriangle;
     bool userEnableNoise;
     bool userEnableDmc;
    int masterFrameCounter;
    int derivedFrameCounter;
    int countSequence;
    int sampleTimer;
    int frameTime;
    int sampleTimerMax;
    int sampleCount;
    int sampleValueL, sampleValueR;
    int triValue;
    int smpSquare1, smpSquare2, smpTriangle, smpNoise, smpDmc;
    int accCount;
    int sq_index, tnd_index;

    // DC removal vars:
    int prevSampleL, prevSampleR;
    int smpAccumL, smpAccumR;
    int smpDiffL, smpDiffR;

    // DAC range:
    int dacRange;
    int dcValue;

    // Master volume:
    int masterVolume;

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

    SourceDataLine* line;
    int bufferIndex;

     void lock_mutex();
     void unlock_mutex();
     PAPU(NES* nes);
     void stateLoad(ByteBuffer* buf);
     void stateSave(ByteBuffer* buf);
     void synchronized_start();
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
     void synchronized_setSampleRate(int rate, bool restart);
     void synchronized_setStereo(bool s, bool restart);
     int getPapuBufferSize();
     void setChannelEnabled(int channel, bool value);
     void setMasterVolume(int value);
     void updateStereoPos();
     SourceDataLine* getLine();
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
public:
     NES* nes;
     struct timespec _frame_start;
     struct timespec _frame_end;
     double _ticks_since_second;
     int32_t frameCounter;
     Memory* ppuMem;
     Memory* sprMem;
    // Rendering Options:
    bool showSpr0Hit;
    bool showSoundBuffer;
    bool clipTVcolumn;
    bool clipTVrow;
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
     int STATUS_VRAMWRITE;
     int STATUS_SLSPRITECOUNT;
     int STATUS_SPRITE0HIT;
     int STATUS_VBLANK;
    // VRAM I/O:
    int vramAddress;
    int vramTmpAddress;
    short vramBufferedReadValue;
    bool firstWrite; 		// VRAM/Scroll Hi/Lo latch
    vector<int>* vramMirrorTable; 			// Mirroring Lookup Table.
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
    int vblankAdd;
     int curX;
     int scanline;
     int lastRenderedScanline;
     int mapperIrqCounter;
    // Sprite data:
     int sprX[64];				// X coordinate
     int sprY[64];				// Y coordinate
     int sprTile[64];			// Tile Index (into pattern table)
     int sprCol[64];			// Upper two bits of color
     bool vertFlip[64];		// Vertical Flip
     bool horiFlip[64];		// Horizontal Flip
     bool bgPriority[64];	// Background priority
     int spr0HitX;	// Sprite #0 hit X coordinate
     int spr0HitY;	// Sprite #0 hit Y coordinate
    bool hitSpr0;

    // Tiles:
     vector<Tile*>* ptTile;
    // Name table data:
    int ntable1[4];
    vector<NameTable*>* nameTable;
    int currentMirroring;

    // Palette data:
    int sprPalette[16];
    int imgPalette[16];
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
    vector<int>* attrib;
    vector<int>* bgbuffer;
    vector<int>* pixrendered;
    vector<int>* spr0dummybuffer;
    vector<int>* dummyPixPriTable;
    vector<int>* oldFrame;
    int* tpix;
    bool scanlineChanged[240];
    bool requestRenderAll;
    bool validTileData;
    int att;
    vector<Tile*>* scantile;
    Tile* t;
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
    int cycles;
    vector<int>* _screen_buffer;

     vector<int>* get_screen_buffer();
     vector<int>* get_pattern_buffer();
     vector<int>* get_name_buffer();
     vector<int>* get_img_palette_buffer();
     vector<int>* get_spr_palette_buffer();
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
     void renderFramePartially(vector<int>* buffer, int startScan, int scanCount);
     void renderBgScanline(vector<int>* buffer, int scan);
     void renderSpritesPartially(int startscan, int scancount, bool bgPri);
     bool checkSprite0(int scan);
     void renderPattern();
     void renderNameTables();
     void renderPalettes();
     void writeMem(int address, short value);
     void updatePalettes();
     void patternWrite(int address, short value);
     void patternWrite(int address, vector<short>* value, int offset, int length);
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
public:
     vector<int>* data;
     int width;
     int height;

     Raster(vector<int>* data, int w, int h);
     Raster(int w, int h);
     void drawTile(Raster* srcRaster, int srcx, int srcy, int dstx, int dsty, int w, int h);
};

class ROM {
public:
    // Mirroring types:
     static const int VERTICAL_MIRRORING = 0;
     static const int HORIZONTAL_MIRRORING = 1;
     static const int FOURSCREEN_MIRRORING = 2;
     static const int SINGLESCREEN_MIRRORING = 3;
     static const int SINGLESCREEN_MIRRORING2 = 4;
     static const int SINGLESCREEN_MIRRORING3 = 5;
     static const int SINGLESCREEN_MIRRORING4 = 6;
     static const int CHRROM_MIRRORING = 7;
    bool failedSaveFile;
    bool saveRamUpToDate;
    short* header;
    vector<vector<short>*>* rom;
    vector<vector<short>*>* vrom;
    vector<short>* saveRam;
    vector<vector<Tile*>*>* vromTile;
    NES* nes;
    size_t romCount;
    size_t vromCount;
    int mirroring;
    bool batteryRam;
    bool trainer;
    bool fourScreen;
    int mapperType;
    string fileName;
    bool enableSave;
    bool valid;
    static vector<string>* _mapperName;
    static vector<bool>* _mapperSupported;

     ROM(NES* nes);
     string getmapperName();
     static vector<bool>* getmapperSupported();
     void load(string fileName);
     bool isValid();
     int getRomBankCount();
     int getVromBankCount();
     short* getHeader();
     vector<short>* getRomBank(int bank);
     vector<short>* getVromBank(int bank);
     vector<Tile*>* getVromBankTiles(int bank);
     int getMirroringType();
     int getMapperType();
     string getMapperName();
     bool hasBatteryRam();
     bool hasTrainer();
     string getFileName();
     bool mapperSupported();
     MapperDefault* createMapper();
     void setSaveState(bool enableSave);
     vector<short>* getBatteryRam();
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
     static const void doScanlineScaling(vector<int>* src, vector<int>* dest, bool* changed);
     static const void doRasterScaling(vector<int>* src, vector<int>* dest, bool* changed);
     static const void doNormalScaling(vector<int>* src, vector<int>* dest, bool* changed);
};

class MyMouseAdapter : public MouseAdapter {
    long lastClickTime;
public:
	MyMouseAdapter();
    void mouseClicked(MouseEvent* me);
    void mousePressed(MouseEvent* me);
    void mouseReleased(MouseEvent* me);
};

class Tile {
public:
    // Tile data:
    int pix[64];
    int fbIndex;
    int tIndex;
    int x, y;
    int w, h;
    int incX, incY;
    int palIndex;
    int tpri;
    int c;
     bool initialized;
     bool opaque[8];

     Tile();
     void setBuffer(vector<short>* scanline);
     void setScanline(int sline, short b1, short b2);
     void renderSimple(int dx, int dy, vector<int>* fBuffer, int palAdd, int* palette);
     void renderSmall(int dx, int dy, vector<int>* buffer, int palAdd, int* palette);
     void render(int srcx1, int srcy1, int srcx2, int srcy2, int dx, int dy, vector<int>* fBuffer, int palAdd, int* palette, bool flipHorizontal, bool flipVertical, int pri, vector<int>* priTable);
     bool isTransparent(int x, int y);
     void dumpData(string file);
     void stateSave(ByteBuffer* buf);
     void stateLoad(ByteBuffer* buf);
};

class vNES {
public:
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
    NES* nes;
    string rom;
    Font* progressFont;

    //Color* bgColor;
    bool started;

    void init();
    void run();
    void stop();
    void destroy();
    void showLoadProgress(int percentComplete);
    void paint(Graphics* g);
    void update(Graphics* g);
    void readParams();
    void initKeyCodes();
    int getWidth();
    int getHeight();
};

inline void arraycopy_short(vector<short>* src, int srcPos, vector<short>* dest, int destPos, int length) {
	assert(srcPos >= 0);
	assert(destPos >= 0);
	assert(length >= 0);
	assert(((size_t)srcPos)+length <= src->size());
	assert(((size_t)destPos)+length <= dest->size());
	
	for(int i=0; i<length; i++) {
		(*dest)[destPos + i] = (*src)[srcPos + i];
	}
}

inline void arraycopy_Tile(vector<Tile*>* src, int srcPos, vector<Tile*>* dest, int destPos, int length) {
	assert(srcPos >= 0);
	assert(destPos >= 0);
	assert(length >= 0);
	assert(((size_t)srcPos)+length <= src->size());
	assert(((size_t)destPos)+length <= dest->size());
	
	for(int i=0; i<length; i++) {
		(*dest)[destPos + i] = (*src)[srcPos + i];
	}
}

inline void arraycopy_int(vector<int>* src, int srcPos, vector<int>* dest, int destPos, int length) {
	assert(srcPos >= 0);
	assert(destPos >= 0);
	assert(length >= 0);
	assert(((size_t)srcPos)+length <= src->size());
	assert(((size_t)destPos)+length <= dest->size());
	
	for(int i=0; i<length; i++) {
		(*dest)[destPos + i] = (*src)[srcPos + i];
	}
}

inline string intToHexString(int i) {
    stringstream ss;
    ss << std::hex << std::showbase << i;
    return ss.str();
}


inline string toUpperCase(string s) {
	std::transform(s.begin(), s.end(), s.begin(), (int (*)(int))std::toupper);
	return s;
}

inline string toLowerCase(string s) {
	std::transform(s.begin(), s.end(), s.begin(), (int (*)(int))std::tolower);
	return s;
}

inline bool endsWith(string str, string key) {
	size_t keylen = key.length();
	size_t strlen = str.length();
	
	if(keylen == 0 && strlen == 0) {
		return true;
	} else if(keylen == 0 || strlen == 0) {
		return false;
	}

	if(keylen <= strlen && str.substr(strlen - keylen, keylen) == key) {
		return true;
	} else {
		return false;
	}
}

inline bool startsWith(string str, string key) {
	size_t keylen = key.length();
	size_t strlen = str.length();

	if(keylen == 0 && strlen == 0) {
		return true;
	} else if(keylen == 0 || strlen == 0) {
		return false;
	}

	if(keylen <= strlen && str.substr(0, keylen) == key) {
		return true;
	} else {
		return false;
	}
}

inline float rand_float() {
	return ((float) rand() / (RAND_MAX));
}

template <typename T>
inline T hexStringTo(string str) {
	T x;   
	std::stringstream ss;
	ss << std::hex << str;
	ss >> x;
	return x;
}

#endif

