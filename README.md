

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
Because NaCl is dead, this project has moved to WebAssembly:
https://github.com/workhorsy/SaltyNES
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


SaltyNES is a browser based NES emulator derived from vNES. Because it is
written in C++, it can be run as a stand-alone SDL application, or 
in a web browser using Google Portable Native Client.

For SaltyNES see:
https://github.com/workhorsylegacy/SaltyNESLegacy

For vNES see:
https://github.com/workhorsylegacy/vNES



TO BUILD SDL VERSION:
```
	# Build the program
	./raise desktop

	# Run a NES ROM file
	./saltynes roms/example.nes
```


TO BUILD PNACL VERSION:
```
	# Make sure NaCl version 35 SDK is installed,
	# and the NACL_SDK_ROOT environmental variable is set.
	export NACL_SDK_ROOT=~/nacl_sdk/pepper_35

	# Build the program
	./raise nacl

	# Run the web server
	python httpd.py

	# View the website in a web browser
	http://localhost:5103/
```


* SaltyNES is licenced as GPLv3 or later.
* sha256sum.cc & sha256sum.h are from ulib and licensed under MIT.
* base64.cc and base64.h are licensed under a MIT style license.
* Color.cc from java.awt.Color is licenced as GPLv2.
* NACL httpd.py, and Makefile are licensed under a BSD style license.
* Jquery is dual licensed under MIT and GPLv2.
* CryptoJS is licensed under a BSD style license.
