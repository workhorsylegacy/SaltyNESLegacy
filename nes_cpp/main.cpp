


#include "Globals.h"
#include <signal.h>

using namespace std;


vNES* vnes = NULL;



int main(int argc, char* argv[]) {
	printf("%s\n", "");
	printf("%s\n", "C++NES 2.14 \u00A9 2012 Matthew Brennan Jones <mattjones@workhorsy.org>");
	printf("%s\n", "A Port of vNES from Java to C++");
	printf("%s\n", "vNES 2.14 \u00A9 2006-2011 Jamie Sanders");
	printf("%s\n", "For vNES, see www.thatsanderskid.com");
	printf("%s\n", "Use of this program subject to GNU GPL, Version 3.");

	// Initialize SDL
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Could not initialize SDL: %s\n", 
			SDL_GetError());
		return -1;
	}
	
	// Grab a surface on the screen
	Globals::sdl_screen = SDL_SetVideoMode(256, 240, 32, SDL_SWSURFACE|SDL_ANYFORMAT);
	if( !Globals::sdl_screen ) {
		fprintf(stderr, "Couldn't create a surface: %s\n",
			SDL_GetError());
		return -1;
	}
	
	vnes = new vNES();
	vnes->init();
	vnes->run();

	delete_n_null(vnes);
	return 0;
}

