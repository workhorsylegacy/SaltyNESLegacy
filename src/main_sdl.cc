/*
C++NES Copyright (c) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>

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
#ifdef SDL

#include "nes_cpp.h"

using namespace std;

int main(int argc, char* argv[]) {
	printf("%s\n", "");
	printf("%s\n", "C++NES: A Port of vNES from Java to C++");
	printf("%s\n", "C++NES 2.14 (C) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>");
	printf("%s\n", "vNES 2.14 (C) 2006-2011 Jamie Sanders thatsanderskid.com");
	printf("%s\n", "Use of this program subject to GNU GPL, Version 3.");
	printf("%s\n", "");

	// Make sure there is a rom file name
	if(argc < 2) {
		fprintf(stderr, "No rom file argument provided. Exiting ...\n");
		return -1;
	}

	// Initialize SDL
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Could not initialize SDL: %s\n", 
			SDL_GetError());
		return -1;
	}

	// Grab a SDL surface from the screen
	Globals::sdl_screen = SDL_SetVideoMode(256, 240, 32, SDL_SWSURFACE|SDL_ANYFORMAT);
	if( !Globals::sdl_screen ) {
		fprintf(stderr, "Couldn't create a surface: %s\n",
			SDL_GetError());
		return -1;
	}

	// Run the emulator
	vNES vnes;
	vnes.init(argv[1]);
	vnes.pre_run_setup();
	vnes.run();

	// Clanup the SDL resources then exit
	SDL_Quit();
	return 0;
}

#endif
