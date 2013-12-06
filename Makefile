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
PNACL_CC ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-clang -c
PNACL_CXX ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-clang++ -c
PNACL_LINK ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-clang++
PNACL_DUMP ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-objdump
PNACL_STRIP ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-strip
PNACL_FINALIZE ?= $(TC_PATH)/$(OSNAME)_pnacl/bin/pnacl-finalize

NMF := python $(NACL_SDK_ROOT)/tools/create_nmf.py

#
# Defaults
#
NACL_WARNINGS := -Wno-long-long -Wall -Wswitch-enum -Werror -pedantic
NACL_DEFINES := -g -DNDEBUG -DNACL=true -DTCNAME=pnacl
NACL_CXXFLAGS := -MMD -O2 -pthread -fPIC -std=c++98 $(NACL_DEFINES) $(NACL_WARNINGS)
NACL_INCLUDES := -I$(NACL_SDK_ROOT)/include -I$(NACL_SDK_ROOT)/include/pnacl
NACL_LDFLAGS := -lppapi_cpp -lppapi



all: clean
	# Make the dirs
	$(MKDIR) pnacl
	$(MKDIR) pnacl/src

	# Build all the object files
	$(PNACL_CXX) -c -o pnacl/src/base64_pnacl.o src/base64.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ByteBuffer_pnacl.o src/ByteBuffer.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/CPU_pnacl.o src/CPU.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ChannelDM_pnacl.o src/ChannelDM.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ChannelNoise_pnacl.o src/ChannelNoise.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ChannelSquare_pnacl.o src/ChannelSquare.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ChannelTriangle_pnacl.o src/ChannelTriangle.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Color_pnacl.o src/Color.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/CpuInfo_pnacl.o src/CpuInfo.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Globals_pnacl.o src/Globals.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/InputHandler_pnacl.o src/InputHandler.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper001_pnacl.o src/Mapper001.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper002_pnacl.o src/Mapper002.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper003_pnacl.o src/Mapper003.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper004_pnacl.o src/Mapper004.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper007_pnacl.o src/Mapper007.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Mapper009_pnacl.o src/Mapper009.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/MapperDefault_pnacl.o src/MapperDefault.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Memory_pnacl.o src/Memory.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Misc_pnacl.o src/Misc.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/NES_pnacl.o src/NES.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/NameTable_pnacl.o src/NameTable.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/PAPU_pnacl.o src/PAPU.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/PPU_pnacl.o src/PPU.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/PaletteTable_pnacl.o src/PaletteTable.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Parameters_pnacl.o src/Parameters.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/ROM_pnacl.o src/ROM.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Raster_pnacl.o src/Raster.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/SaltyNES_pnacl.o src/SaltyNES.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/sha256sum_pnacl.o src/sha256sum.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/Tile_pnacl.o src/Tile.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/main_sdl_pnacl.o src/main_sdl.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/main_nacl_pnacl.o src/main_nacl.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)
	$(PNACL_CXX) -c -o pnacl/src/vNES_pnacl.o src/vNES.cc $(NACL_CXXFLAGS) $(NACL_INCLUDES)

	# Convert the objects into byte code
	$(PNACL_LINK) -o pnacl/salty_nes_unstripped.bc \
	./pnacl/src/base64_pnacl.o \
	./pnacl/src/ByteBuffer_pnacl.o \
	./pnacl/src/CPU_pnacl.o \
	./pnacl/src/ChannelDM_pnacl.o \
	./pnacl/src/ChannelNoise_pnacl.o \
	./pnacl/src/ChannelSquare_pnacl.o \
	./pnacl/src/ChannelTriangle_pnacl.o \
	./pnacl/src/Color_pnacl.o \
	./pnacl/src/CpuInfo_pnacl.o \
	./pnacl/src/Globals_pnacl.o \
	./pnacl/src/InputHandler_pnacl.o \
	./pnacl/src/Mapper001_pnacl.o \
	./pnacl/src/Mapper002_pnacl.o \
	./pnacl/src/Mapper003_pnacl.o \
	./pnacl/src/Mapper004_pnacl.o \
	./pnacl/src/Mapper007_pnacl.o \
	./pnacl/src/Mapper009_pnacl.o \
	./pnacl/src/MapperDefault_pnacl.o \
	./pnacl/src/Memory_pnacl.o \
	./pnacl/src/Misc_pnacl.o \
	./pnacl/src/NES_pnacl.o \
	./pnacl/src/NameTable_pnacl.o \
	./pnacl/src/PAPU_pnacl.o \
	./pnacl/src/PPU_pnacl.o \
	./pnacl/src/PaletteTable_pnacl.o \
	./pnacl/src/Parameters_pnacl.o \
	./pnacl/src/ROM_pnacl.o \
	./pnacl/src/Raster_pnacl.o \
	./pnacl/src/SaltyNES_pnacl.o \
	./pnacl/src/sha256sum_pnacl.o \
	./pnacl/src/Tile_pnacl.o \
	./pnacl/src/main_sdl_pnacl.o \
	./pnacl/src/main_nacl_pnacl.o \
	./pnacl/src/vNES_pnacl.o \
	-L$(NACL_SDK_ROOT)/lib/pnacl/Release $(NACL_LDFLAGS)

	# Conver the byte code into a pexe
	$(PNACL_FINALIZE) -o pnacl/salty_nes_unstripped.pexe pnacl/salty_nes_unstripped.bc

	# Strip the pexe to be smaller
	$(PNACL_STRIP) -o pnacl/salty_nes.pexe pnacl/salty_nes_unstripped.pexe

	# Generate the Nacl Manifest
	$(NMF) -o pnacl/salty_nes.nmf pnacl/salty_nes.pexe -s ./pnacl/Release

	# Remove the log files
	$(RM) -f ~/Desktop/nacl_stdout.log
	$(RM) -f ~/Desktop/nacl_stderr.log

clean:
	$(RM) -f 'saltynes'
	$(RM) -rf -f 'build'
	$(RM) -rf -f 'src/bin'
	$(RM) -rf -f 'pnacl'


