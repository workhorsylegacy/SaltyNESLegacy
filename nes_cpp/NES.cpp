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

    // Creates the NES system.
     NES::NES(AppletUI* gui) {
		this->_isRunning = false;
        Globals::nes = this;
        this->gui = gui;

        // Create memory:
        cpuMem = new Memory(this, 0x10000);	// Main memory (internal to CPU)
        ppuMem = new Memory(this, 0x8000);	// VRAM memory (internal to PPU)
        sprMem = new Memory(this, 0x100);	// Sprite RAM  (internal to PPU)


        // Create system units:
        cpu = new CPU(this);
        palTable = new PaletteTable();
        ppu = new PPU(this);
        papu = new PAPU(this);

        // Init sound registers:
        for (int i = 0; i < 0x14; i++) {
            if (i == 0x10) {
                papu->writeReg(0x4010, (short) 0x10);
            } else {
                papu->writeReg(0x4000 + i, (short) 0);
            }
        }

        // Load NTSC palette:
        if (!palTable->loadNTSCPalette()) {
            //System.out.println("Unable to load palette file. Using default.");
            palTable->loadDefaultPalette();
        }

        // Initialize units:
        cpu->init();
        ppu->init();

        // Enable sound:
        enableSound(true);

        // Clear CPU memory:
        clearCPUMemory();

    }

     bool NES::stateLoad(ByteBuffer* buf) {

        bool continueEmulation = false;
        bool success;

        // Pause emulation:
        if (cpu->isRunning()) {
            continueEmulation = true;
            stopEmulation();
        }

        // Check version:
        if (buf->readByte() == 1) {

            // Let units load their state from the buffer:
            cpuMem->stateLoad(buf);
            ppuMem->stateLoad(buf);
            sprMem->stateLoad(buf);
            cpu->stateLoad(buf);
            memMapper->stateLoad(buf);
            ppu->stateLoad(buf);
            success = true;

        } else {

            //System.out.println("State file has wrong format. version="+buf->readByte(0));
            success = false;

        }

        // Continue emulation:
        if (continueEmulation) {
            startEmulation();
        }

        return success;

    }

     void NES::stateSave(ByteBuffer* buf) {

        bool continueEmulation = isRunning();
        stopEmulation();

        // Version:
        buf->putByte((short) 1);

        // Let units save their state:
        cpuMem->stateSave(buf);
        ppuMem->stateSave(buf);
        sprMem->stateSave(buf);
        cpu->stateSave(buf);
        memMapper->stateSave(buf);
        ppu->stateSave(buf);

        // Continue emulation:
        if (continueEmulation) {
            startEmulation();
        }

    }

     bool NES::isRunning() {

        return _isRunning;

    }

     void NES::startEmulation() {

        if (Globals::enableSound && !papu->isRunning()) {
            papu->start();
        }
        {
            if (rom != NULL && rom->isValid() && !cpu->isRunning()) {
                cpu->beginExecution();
                _isRunning = true;
            }
        }
    }

     void NES::stopEmulation() {
        if (cpu->isRunning()) {
            cpu->endExecution();
            _isRunning = false;
        }

        if (Globals::enableSound && papu->isRunning()) {
            papu->stop();
        }
    }

     void NES::reloadRom() {

        if (romFile.empty()) {
            loadRom(romFile);
        }

    }

     void NES::clearCPUMemory() {

        short flushval = Globals::memoryFlushValue;
        for (int i = 0; i < 0x2000; i++) {
            (*cpuMem->mem)[i] = flushval;
        }
        for (int p = 0; p < 4; p++) {
            int i = p * 0x800;
            (*cpuMem->mem)[i + 0x008] = 0xF7;
            (*cpuMem->mem)[i + 0x009] = 0xEF;
            (*cpuMem->mem)[i + 0x00A] = 0xDF;
            (*cpuMem->mem)[i + 0x00F] = 0xBF;
        }

    }

     void NES::setGameGenieState(bool enable) {
        if (memMapper != NULL) {
            memMapper->setGameGenieState(enable);
        }
    }

    // Returns CPU object.
     CPU* NES::getCpu() {
        return cpu;
    }

    // Returns PPU object.
     PPU* NES::getPpu() {
        return ppu;
    }

    // Returns pAPU object.
     PAPU* NES::getPapu() {
        return papu;
    }

    // Returns CPU Memory.
     Memory* NES::getCpuMemory() {
        return cpuMem;
    }

    // Returns PPU Memory.
     Memory* NES::getPpuMemory() {
        return ppuMem;
    }

    // Returns Sprite Memory.
     Memory* NES::getSprMemory() {
        return sprMem;
    }

    // Returns the currently loaded ROM.
     ROM* NES::getRom() {
        return rom;
    }

    // Returns the GUI.
     AppletUI* NES::getGui() {
        return gui;
    }

    // Returns the memory mapper.
     IMemoryMapper* NES::getMemoryMapper() {
        return memMapper;
    }

    // Loads a ROM file into the CPU and PPU.
    // The ROM file is validated first.
     bool NES::loadRom(string file) {

        // Can't load ROM while still running.
        if (_isRunning) {
            stopEmulation();
        }

        {
            // Load ROM file:

            rom = new ROM(this);
            rom->load(file);
            if (rom->isValid()) {

                // The CPU will load
                // the ROM into the CPU
                // and PPU memory.

                reset();

                memMapper = rom->createMapper();
                memMapper->init(this);
                cpu->setMapper(memMapper);
                memMapper->loadROM(rom);
                ppu->setMirroring(rom->getMirroringType());

                this->romFile = file;

            }
            return rom->isValid();
        }

    }

    // Resets the system.
     void NES::reset() {

        if (rom != NULL) {
            rom->closeRom();
        }
        if (memMapper != NULL) {
            memMapper->reset();
        }

        cpuMem->reset();
        ppuMem->reset();
        sprMem->reset();

        clearCPUMemory();

        cpu->reset();
        cpu->init();
        ppu->reset();
        palTable->reset();
        papu->reset();

        KbInputHandler* joy1 = gui->getJoy1();
        if (joy1 != NULL) {
            joy1->reset();
        }

    }

    // Enable or disable sound playback.
     void NES::enableSound(bool enable) {

        bool wasRunning = isRunning();
        if (wasRunning) {
            stopEmulation();
        }

        if (enable) {
            papu->start();
        } else {
            papu->stop();
        }

        //System.out.println("** SOUND ENABLE = "+enable+" **");
        Globals::enableSound = enable;

        if (wasRunning) {
            startEmulation();
        }

    }

     void NES::setFramerate(int rate) {

        Globals::preferredFrameRate = rate;
        Globals::frameTime = 1000000 / rate;
        papu->setSampleRate(papu->getSampleRate(), false);

    }

     void NES::destroy() {

        if (cpu != NULL) {
            cpu->destroy();
        }
        if (ppu != NULL) {
            ppu->destroy();
        }
        if (papu != NULL) {
            papu->destroy();
        }
        if (cpuMem != NULL) {
            cpuMem->destroy();
        }
        if (ppuMem != NULL) {
            ppuMem->destroy();
        }
        if (sprMem != NULL) {
            sprMem->destroy();
        }
        if (memMapper != NULL) {
            memMapper->destroy();
        }
        if (rom != NULL) {
            rom->destroy();
        }

        gui = NULL;
        cpu = NULL;
        ppu = NULL;
        papu = NULL;
        cpuMem = NULL;
        ppuMem = NULL;
        sprMem = NULL;
        memMapper = NULL;
        rom = NULL;
        palTable = NULL;

    }
