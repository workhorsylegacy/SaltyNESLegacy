
#include "Globals.h"

    SDL_Surface* Globals::sdl_screen = NULL;

    double Globals::CPU_FREQ_NTSC = 1789772.5d;
    double Globals::CPU_FREQ_PAL = 1773447.4d;
    int Globals::preferredFrameRate = 60;
    
    // Microseconds per frame:
    int Globals::frameTime = 1000000 / preferredFrameRate;
    // What value to flush memory with on power-up:
    short Globals::memoryFlushValue = 0xFF;

    const bool Globals::debug = true;
    const bool Globals::fsdebug = false;

    bool Globals::appletMode = true;
    bool Globals::disableSprites = false;
    bool Globals::timeEmulation = true;
    bool Globals::palEmulation;
    bool Globals::enableSound = true;
    bool Globals::focused = false;

    std::map<string, uint32_t> Globals::keycodes; //Java key codes
    std::map<string, string> Globals::controls; //vNES controls codes

    NES* Globals::nes;

    void Globals::println(string s) {
        printf("%s\n", s.c_str());
    }

