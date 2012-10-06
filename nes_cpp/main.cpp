


#include "Globals.h"
#include <signal.h>

using namespace std;


SDL_Event sdl_event;

void nes_CheckForQuitting() {
	while(SDL_PollEvent(&sdl_event) == 1) {
		//if(sdl_event.type == SDL_QUIT)
		//	self->isQuitting = true;
	}
}

void exit_handler(int signum) {
	exit(signum);
}

int main(int argc, char* argv[]) {
	printf("%s\n", "");
	printf("%s\n", "C++NES 2.14 \u00A9 2012 Matthew Brennan Jones <mattjones@workhorsy.org>");
	printf("%s\n", "A Port of vNES from Java to C++");
	printf("%s\n", "vNES 2.14 \u00A9 2006-2011 Jamie Sanders");
	printf("%s\n", "For vNES, see www.thatsanderskid.com");
	printf("%s\n", "Use of this program subject to GNU GPL, Version 3.");

	// Initialize SDL, exit if there is an error.
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
	
	// When the program is through executing, call SDL_Quit
	atexit(SDL_Quit);
	
	// Make ctrl+c exit too
	signal(SIGINT, exit_handler);
	
	vNES* vnes = new vNES();
	vnes->init();
	vnes->run();
	return 0;
}

