


#include "Globals.h"

using namespace std;


SDL_Event sdl_event;

void nes_CheckForQuitting() {
	while(SDL_PollEvent(&sdl_event) == 1) {
		//if(sdl_event.type == SDL_QUIT)
		//	self->isQuitting = true;
	}
}

int main(int argc, char* argv[]) {
	/* Initialize SDL, exit if there is an error. */
	if( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Could not initialize SDL: %s\n", 
			SDL_GetError());
		return -1;
	}
	
	/* When the program is through executing, call SDL_Quit */
	atexit(SDL_Quit);
	
	/* Grab a surface on the screen */
	Globals::sdl_screen = SDL_SetVideoMode(256, 240, 32, SDL_SWSURFACE|SDL_ANYFORMAT);
	if( !Globals::sdl_screen ) {
		fprintf(stderr, "Couldn't create a surface: %s\n",
			SDL_GetError());
		return -1;
	}
	
	vNES* vnes = new vNES();
	vnes->init();
	vnes->run();
	return 0;
}

