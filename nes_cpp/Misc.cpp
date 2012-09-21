/*
vNES
Copyright © 2006-2011 Jamie Sanders

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

#include "Globals.h"

     static bool Misc::debug = Globals::debug;
     static float[] Misc::_rnd = NULL;
     static int Misc::nextRnd = 0;
     static float Misc::rndret = 0;

	static float* Misc::rnd() {
		if(_rnd == NULL) {
			_rnd = new float[100000]();
			for (int i = 0; i < _rnd.length; i++) {
	            _rnd[i] = (float) Math.random();
	        }
		}
        
        return _rnd;
	}

     static string Misc::hex8(int i) {
        string s = Integer.toHexString(i);
        while (s.length() < 2) {
            s = "0" + s;
        }
        return s.toUpperCase();
    }

     static string Misc::hex16(int i) {
        string s = Integer.toHexString(i);
        while (s.length() < 4) {
            s = "0" + s;
        }
        return s.toUpperCase();
    }

     static string Misc::binN(int num, int N) {
        char[] c = new char[N];
        for (int i = 0; i < N; i++) {
            c[N - i - 1] = (num & 0x1) == 1 ? '1' : '0';
            num >>= 1;
        }
        return new string(c);
    }

     static string Misc::bin8(int num) {
        return binN(num, 8);
    }

     static string Misc::bin16(int num) {
        return binN(num, 16);
    }

     static string Misc::binStr(long value, int bitcount) {
        string ret = "";
        for (int i = 0; i < bitcount; i++) {
            ret = ((value & (1 << i)) != 0 ? "1" : "0") + ret;
        }
        return ret;
    }

     static int* Misc::resizeArray(int* array, int newSize) {

        int* newArr = new int[newSize];
        System.arraycopy(array, 0, newArr, 0, Math.min(newSize, array.length));
        return newArr;

    }

     static string Misc::pad(string str, string padStr, int length) {
        while (str.length() < length) {
            str += padStr;
        }
        return str;
    }

     static float Misc::random() {
        rndret = rnd()[nextRnd];
        nextRnd++;
        if (nextRnd >= rnd().length) {
            nextRnd = (int) (Math.random() * (rnd().length - 1));
        }
        return rndret;
    }
