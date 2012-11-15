/*
SaltyNES Copyright (c) 2012 Matthew Brennan Jones <mattjones@workhorsy.org>
vNES Copyright (c) 2006-2011 Jamie Sanders

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

#include "SaltyNES.h"

     vector<float>* Misc::_rnd = NULL;
     int Misc::nextRnd = 0;
     float Misc::rndret = 0;

	vector<float>* Misc::rnd() {
		if(_rnd == NULL) {
			_rnd = new vector<float>(100000);
			for (size_t i = 0; i < _rnd->size(); i++) {
	            (*_rnd)[i] = rand_float();

	        }
		}
        
        return _rnd;
	}

     string Misc::hex8(int i) {
        string s = intToHexString(i);
        while (s.length() < 2) {
            s = "0" + s;
        }
        return toUpperCase(s);
    }

     string Misc::hex16(int i) {
        string s = intToHexString(i);
        while (s.length() < 4) {
            s = "0" + s;
        }
        return toUpperCase(s);
    }

     string Misc::binN(int num, int N) {
        char* c = new char[N];
        for (int i = 0; i < N; i++) {
            c[N - i - 1] = (num & 0x1) == 1 ? '1' : '0';
            num >>= 1;
        }
        return string(c);
    }

     string Misc::bin8(int num) {
        return binN(num, 8);
    }

     string Misc::bin16(int num) {
        return binN(num, 16);
    }

     string Misc::binStr(long value, int bitcount) {
        string ret = "";
        for (int i = 0; i < bitcount; i++) {
            ret = ((value & (1 << i)) != 0 ? "1" : "0") + ret;
        }
        return ret;
    }

     vector<int>* Misc::resizeArray(vector<int>* array, int newSize) {

        vector<int>* newArr = new vector<int>(newSize);
        arraycopy_int(array, 0, newArr, 0, min(newSize, (int)array->size()));
        return newArr;

    }

     string Misc::pad(string str, string padStr, int length) {
        while ((int)str.length() < length) {
            str += padStr;
        }
        return str;
    }

     float Misc::random() {
        rndret = (*rnd())[nextRnd];
        nextRnd++;
        if (nextRnd >= (int) rnd()->size()) {
            nextRnd = (int) (rand_float() * (rnd()->size() - 1));
        }
        return rndret;
    }
