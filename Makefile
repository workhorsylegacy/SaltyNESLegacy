# Copyright (c) 2013 Matthew Brennan Jones <mattjones@workhorsy.org>
# Copyright (c) 2013 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

#
# GNU Make based build file.  For details on GNU Make see:
#   http://www.gnu.org/software/make/manual/make.html
#

#
# Get pepper directory for toolchain and includes.
#
# If NACL_SDK_ROOT is not set, then assume it can be found a two directories up,
# from the default example directory location.
#
THIS_MAKEFILE := $(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT ?= $(abspath $(dir $(THIS_MAKEFILE))../..)

#
# Alias for standard commands
#
CP := python $(NACL_SDK_ROOT)/tools/oshelpers.py cp
MKDIR := python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir
MV := python $(NACL_SDK_ROOT)/tools/oshelpers.py mv
RM := python $(NACL_SDK_ROOT)/tools/oshelpers.py rm

#
# Compute path to requested NaCl Toolchain
#
OSNAME := $(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH := $(abspath $(NACL_SDK_ROOT)/toolchain)


#
# Defaults for TOOLS
#
NEWLIB_CC64 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-gcc -c #i686-nacl-gcc -c
NEWLIB_CXX64 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-g++ -c #i686-nacl-g++ -c
NEWLIB_LINK64 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-g++
NEWLIB_DUMP64 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-objdump #x86_64-nacl/bin/objdump
NEWLIB_STRIP64 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-strip #x86_64-nacl-strip

NEWLIB_CC32 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-gcc -c
NEWLIB_CXX32 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -c
NEWLIB_LINK32 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++
NEWLIB_DUMP32 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-objdump
NEWLIB_STRIP32 ?= $(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-strip

NMF := python $(NACL_SDK_ROOT)/tools/create_nmf.py

#
# Defaults
#
NACL_WARNINGS := -Wno-long-long -Wall -Wswitch-enum -Werror -pedantic
NACL_DEFINES := -g -DNDEBUG -DNACL=true -DTCNAME=newlib
NACL_CXXFLAGS := -MMD -O2 -pthread -fPIC -std=c++98 $(NACL_DEFINES) $(NACL_WARNINGS)
NACL_INCLUDES := -I$(NACL_SDK_ROOT)/include -I$(NACL_SDK_ROOT)/include/newlib
NACL_LDFLAGS := -lppapi_cpp -lppapi



all: clean
	# Make the dirs
	$(MKDIR) newlib
	$(MKDIR) newlib/src

	# Build all the object files
	$(NEWLIB_CXX64) -c -o newlib/src/base64_x86_64.o src/base64.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ByteBuffer_x86_64.o src/ByteBuffer.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/CPU_x86_64.o src/CPU.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ChannelDM_x86_64.o src/ChannelDM.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ChannelNoise_x86_64.o src/ChannelNoise.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ChannelSquare_x86_64.o src/ChannelSquare.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ChannelTriangle_x86_64.o src/ChannelTriangle.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Color_x86_64.o src/Color.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/CpuInfo_x86_64.o src/CpuInfo.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Globals_x86_64.o src/Globals.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/InputHandler_x86_64.o src/InputHandler.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper001_x86_64.o src/Mapper001.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper002_x86_64.o src/Mapper002.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper003_x86_64.o src/Mapper003.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper004_x86_64.o src/Mapper004.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper007_x86_64.o src/Mapper007.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Mapper009_x86_64.o src/Mapper009.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/MapperDefault_x86_64.o src/MapperDefault.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Memory_x86_64.o src/Memory.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Misc_x86_64.o src/Misc.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/NES_x86_64.o src/NES.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/NameTable_x86_64.o src/NameTable.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/PAPU_x86_64.o src/PAPU.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/PPU_x86_64.o src/PPU.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/PaletteTable_x86_64.o src/PaletteTable.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Parameters_x86_64.o src/Parameters.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/ROM_x86_64.o src/ROM.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Raster_x86_64.o src/Raster.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/SaltyNES_x86_64.o src/SaltyNES.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/sha256sum_x86_64.o src/sha256sum.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/Tile_x86_64.o src/Tile.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/main_sdl_x86_64.o src/main_sdl.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/main_nacl_x86_64.o src/main_nacl.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(NEWLIB_CXX64) -c -o newlib/src/vNES_x86_64.o src/vNES.cc -m64 $(NACL_CXXFLAGS) $(NACL_INCLUDES)

	# Build the object into a nexe
	$(NEWLIB_LINK64) -o newlib/salty_nes_unstripped_x86_64.nexe \
	newlib/src/base64_x86_64.o \
	newlib/src/ByteBuffer_x86_64.o \
	newlib/src/CPU_x86_64.o \
	newlib/src/ChannelDM_x86_64.o \
	newlib/src/ChannelNoise_x86_64.o \
	newlib/src/ChannelSquare_x86_64.o \
	newlib/src/ChannelTriangle_x86_64.o \
	newlib/src/Color_x86_64.o \
	newlib/src/CpuInfo_x86_64.o \
	newlib/src/Globals_x86_64.o \
	newlib/src/InputHandler_x86_64.o \
	newlib/src/Mapper001_x86_64.o \
	newlib/src/Mapper002_x86_64.o \
	newlib/src/Mapper003_x86_64.o \
	newlib/src/Mapper004_x86_64.o \
	newlib/src/Mapper007_x86_64.o \
	newlib/src/Mapper009_x86_64.o \
	newlib/src/MapperDefault_x86_64.o \
	newlib/src/Memory_x86_64.o \
	newlib/src/Misc_x86_64.o \
	newlib/src/NES_x86_64.o \
	newlib/src/NameTable_x86_64.o \
	newlib/src/PAPU_x86_64.o \
	newlib/src/PPU_x86_64.o \
	newlib/src/PaletteTable_x86_64.o \
	newlib/src/Parameters_x86_64.o \
	newlib/src/ROM_x86_64.o \
	newlib/src/Raster_x86_64.o \
	newlib/src/SaltyNES_x86_64.o \
	newlib/src/sha256sum_x86_64.o \
	newlib/src/Tile_x86_64.o \
	newlib/src/main_sdl_x86_64.o \
	newlib/src/main_nacl_x86_64.o \
	newlib/src/vNES_x86_64.o \
	-Wl,-as-needed -Wl,-Map,./newlib/salty_nes_x86_64.map \
	-L$(NACL_SDK_ROOT)/lib/newlib_x86_64/Release $(NACL_LDFLAGS)

	# Strip the nexe to be smaller
	$(NEWLIB_STRIP64) -o newlib/salty_nes_x86_64.nexe newlib/salty_nes_unstripped_x86_64.nexe

	# Make a NaCl manifest file
	$(NMF) -o newlib/salty_nes.nmf newlib/salty_nes_x86_64.nexe -s ./newlib

	# Remove the log files
	$(RM) -f ~/Desktop/nacl_stdout.log
	$(RM) -f ~/Desktop/nacl_stderr.log

clean:
	$(RM) -f 'saltynes'
	$(RM) -rf -f 'build'
	$(RM) -rf -f 'src/bin'
	$(RM) -rf -f 'newlib'


