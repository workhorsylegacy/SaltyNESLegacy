
#include "nes_cpp.h"

using namespace std;

#ifdef SDL

int main(/*int argc, char* argv[]*/) {
	printf("%s\n", "");
	printf("%s\n", "C++NES: A Port of vNES from Java to C++");
	printf("%s\n", "C++NES 2.14 (C) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>");
	printf("%s\n", "vNES 2.14 (C) 2006-2011 Jamie Sanders thatsanderskid.com");
	printf("%s\n", "Use of this program subject to GNU GPL, Version 3.");

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
	vnes.init("example.nes");
	vnes.run();

	// Clanup the SDL resources then exit
	SDL_Quit();
	return 0;
}

#endif
