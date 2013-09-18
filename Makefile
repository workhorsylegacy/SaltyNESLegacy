# Copyright (c) 2013 Matthew Brennan Jones <mattjones@workhorsy.org>
# Copyright (c) 2012 The Native Client Authors. All rights reserved .
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
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)

#
# Defaults
#
NACL_CXXFLAGS:= -O2 -g -pthread -std=gnu++98 -Wno-long-long -Wall -Wswitch-enum -Werror -pedantic
NACL_LDFLAGS:=-g -pthread -lppapi_cpp -lppapi 

#
# Alias for standard commands
#
CP:=python $(NACL_SDK_ROOT)/tools/oshelpers.py cp
MKDIR:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mkdir
MV:=python $(NACL_SDK_ROOT)/tools/oshelpers.py mv
RM:=python $(NACL_SDK_ROOT)/tools/oshelpers.py rm
#
# Compute path to requested NaCl Toolchain
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain)


#
# Defaults for TOOLS
#

NEWLIB_CC?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-gcc -c
NEWLIB_CXX?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -c
NEWLIB_LINK?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-g++ -Wl,-as-needed
NEWLIB_DUMP?=$(TC_PATH)/$(OSNAME)_x86_newlib/x86_64-nacl/bin/objdump
NEWLIB_STRIP32?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/i686-nacl-strip
NEWLIB_STRIP64?=$(TC_PATH)/$(OSNAME)_x86_newlib/bin/x86_64-nacl-strip


#
# NMF Manifiest generation
#
# Use the python script create_nmf to scan the binaries for dependencies using
# objdump.  Pass in the (-L) paths to the default library toolchains so that we
# can find those libraries and have it automatically copy the files (-s) to
# the target directory for us.
NMF:=python $(NACL_SDK_ROOT)/tools/create_nmf.py

SALTY_NES_CXX:= \
src/base64.cc \
src/ByteBuffer.cc \
src/CPU.cc \
src/ChannelDM.cc \
src/ChannelNoise.cc \
src/ChannelSquare.cc \
src/ChannelTriangle.cc \
src/Color.cc \
src/CpuInfo.cc \
src/Globals.cc \
src/InputHandler.cc \
src/Mapper001.cc \
src/Mapper002.cc \
src/Mapper003.cc \
src/Mapper004.cc \
src/Mapper007.cc \
src/Mapper009.cc \
src/MapperDefault.cc \
src/Memory.cc \
src/Misc.cc \
src/NES.cc \
src/NameTable.cc \
src/PAPU.cc \
src/PPU.cc \
src/PaletteTable.cc \
src/Parameters.cc \
src/ROM.cc \
src/Raster.cc \
src/SaltyNES.cc \
src/sha256sum.cc \
src/Tile.cc \
src/main_sdl.cc \
src/main_nacl.cc \
src/vNES.cc

#
# Default target
#
all: newlib/salty_nes.nmf
	$(NEWLIB_STRIP32) newlib/salty_nes_x86_32.nexe
	$(NEWLIB_STRIP64) newlib/salty_nes_x86_64.nexe
	$(RM) -f ~/Desktop/nacl_stdout.log
	$(RM) -f ~/Desktop/nacl_stderr.log


#
# Rules for newlib toolchain
#
newlib:
	$(MKDIR) newlib
	$(MKDIR) newlib/src

NEWLIB_SALTY_NES_x86_32_CXX_O:=$(patsubst %.cc, newlib/%_x86_32.o,$(SALTY_NES_CXX))
$(NEWLIB_SALTY_NES_x86_32_CXX_O) : newlib/%_x86_32.o : %.cc $(THIS_MAKE) | newlib
	$(NEWLIB_CXX) -o $@ $< -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include

newlib/salty_nes_x86_32.nexe : $(NEWLIB_SALTY_NES_x86_32_CXX_O)
	$(NEWLIB_LINK) -o $@ $^ -m32 $(NACL_LDFLAGS)
NEWLIB_NMF+=newlib/salty_nes_x86_32.nexe 

NEWLIB_SALTY_NES_x86_64_CXX_O:=$(patsubst %.cc, newlib/%_x86_64.o,$(SALTY_NES_CXX))
$(NEWLIB_SALTY_NES_x86_64_CXX_O) : newlib/%_x86_64.o : %.cc $(THIS_MAKE) | newlib
	$(NEWLIB_CXX) -o $@ $< -m64 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include

newlib/salty_nes_x86_64.nexe : $(NEWLIB_SALTY_NES_x86_64_CXX_O)
	$(NEWLIB_LINK) -o $@ $^ -m64 $(NACL_LDFLAGS)
NEWLIB_NMF+=newlib/salty_nes_x86_64.nexe 

newlib/salty_nes.nmf : $(NEWLIB_NMF)
	$(NMF) -D $(NEWLIB_DUMP) -o $@ $^ -t newlib -s newlib




RUN: all
	python ../httpd.py

clean:
	$(RM) -f 'saltynes'
	$(RM) -rf -f 'build'
	$(RM) -rf -f 'src/bin'
	$(RM) -rf -f 'newlib'


derp:
	$(MKDIR) newlib
	$(MKDIR) newlib/src

	$(NEWLIB_CXX) -c -o newlib/src/base64_x86_32.o src/base64.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ByteBuffer_x86_32.o src/ByteBuffer.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/CPU_x86_32.o src/CPU.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ChannelDM_x86_32.o src/ChannelDM.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ChannelNoise_x86_32.o src/ChannelNoise.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ChannelSquare_x86_32.o src/ChannelSquare.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ChannelTriangle_x86_32.o src/ChannelTriangle.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Color_x86_32.o src/Color.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/CpuInfo_x86_32.o src/CpuInfo.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Globals_x86_32.o src/Globals.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/InputHandler_x86_32.o src/InputHandler.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper001_x86_32.o src/Mapper001.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper002_x86_32.o src/Mapper002.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper003_x86_32.o src/Mapper003.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper004_x86_32.o src/Mapper004.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper007_x86_32.o src/Mapper007.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Mapper009_x86_32.o src/Mapper009.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/MapperDefault_x86_32.o src/MapperDefault.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Memory_x86_32.o src/Memory.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Misc_x86_32.o src/Misc.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/NES_x86_32.o src/NES.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/NameTable_x86_32.o src/NameTable.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/PAPU_x86_32.o src/PAPU.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/PPU_x86_32.o src/PPU.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/PaletteTable_x86_32.o src/PaletteTable.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Parameters_x86_32.o src/Parameters.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/ROM_x86_32.o src/ROM.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Raster_x86_32.o src/Raster.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/SaltyNES_x86_32.o src/SaltyNES.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/sha256sum_x86_32.o src/sha256sum.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/Tile_x86_32.o src/Tile.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/main_sdl_x86_32.o src/main_sdl.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/main_nacl_x86_32.o src/main_nacl.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include
	$(NEWLIB_CXX) -c -o newlib/src/vNES_x86_32.o src/vNES.cc -m32 $(NACL_CXXFLAGS) -DTCNAME=newlib -DNACL=true -I$(NACL_SDK_ROOT)/include

	$(NEWLIB_LINK) -o newlib/salty_nes_x86_32.nexe \
	newlib/src/base64_x86_32.o \
	newlib/src/ByteBuffer_x86_32.o \
	newlib/src/CPU_x86_32.o \
	newlib/src/ChannelDM_x86_32.o \
	newlib/src/ChannelNoise_x86_32.o \
	newlib/src/ChannelSquare_x86_32.o \
	newlib/src/ChannelTriangle_x86_32.o \
	newlib/src/Color_x86_32.o \
	newlib/src/CpuInfo_x86_32.o \
	newlib/src/Globals_x86_32.o \
	newlib/src/InputHandler_x86_32.o \
	newlib/src/Mapper001_x86_32.o \
	newlib/src/Mapper002_x86_32.o \
	newlib/src/Mapper003_x86_32.o \
	newlib/src/Mapper004_x86_32.o \
	newlib/src/Mapper007_x86_32.o \
	newlib/src/Mapper009_x86_32.o \
	newlib/src/MapperDefault_x86_32.o \
	newlib/src/Memory_x86_32.o \
	newlib/src/Misc_x86_32.o \
	newlib/src/NES_x86_32.o \
	newlib/src/NameTable_x86_32.o \
	newlib/src/PAPU_x86_32.o \
	newlib/src/PPU_x86_32.o \
	newlib/src/PaletteTable_x86_32.o \
	newlib/src/Parameters_x86_32.o \
	newlib/src/ROM_x86_32.o \
	newlib/src/Raster_x86_32.o \
	newlib/src/SaltyNES_x86_32.o \
	newlib/src/sha256sum_x86_32.o \
	newlib/src/Tile_x86_32.o \
	newlib/src/main_sdl_x86_32.o \
	newlib/src/main_nacl_x86_32.o \
	newlib/src/vNES_x86_32.o \
	-m32 $(NACL_LDFLAGS)

	$(NMF) -D $(NEWLIB_DUMP) -o newlib/salty_nes.nmf newlib/salty_nes_x86_32.nexe -t newlib -s newlib
	# newlib/salty_nes_x86_32.nexe newlib/salty_nes_x86_64.nexe

	$(NEWLIB_STRIP32) newlib/salty_nes_x86_32.nexe
	#$(NEWLIB_STRIP64) newlib/salty_nes_x86_64.nexe
	$(RM) -f ~/Desktop/nacl_stdout.log
	$(RM) -f ~/Desktop/nacl_stderr.log




