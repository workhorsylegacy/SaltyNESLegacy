



#include "Globals.h"
#include <iostream>

using namespace std;

int main (int argc, char *argv[]) {
	vNES* vnes = new vNES();
	vnes->init();
	vnes->run();
	
	return 0;
}

