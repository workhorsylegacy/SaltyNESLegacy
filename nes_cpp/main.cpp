



#include "Globals.h"

#include <iostream>
using namespace std;

int main (int argc, char *argv[]) {
	cout << "Hello world!" << endl;
	
	vNES* vnes = new vNES();
	vnes->init();
	return 0;
}

