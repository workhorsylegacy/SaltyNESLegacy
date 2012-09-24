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

    void BlipBuffer::init(int bufferSize, int windowSize, int samplePeriod, int sincPeriods) {

        win_size = windowSize;
        smp_period = samplePeriod;
        sinc_periods = sincPeriods;
        vector<double> buf = vector<double>(smp_period * win_size);


        // Sample sinc:
        double si_p = sinc_periods;
        for (int i = 0; i < buf.size(); i++) {
            buf[i] = sinc(-si_p * M_PI  + (si_p * 2.0 * ((double) i) * M_PI ) / ((double) buf.size()));
        }

        // Fill into impulse buffer:
        imp = new vector<vector<int>*>(smp_period);
        for(int i=0; i<win_size; i++)
        	(*imp)[i] = new vector<int>(win_size);
        for (int off = 0; off < smp_period; off++) {
            double sum = 0;
            for (int i = 0; i < win_size; i++) {
                sum += 32768.0 * buf[i * smp_period + off];
                (*(*imp)[smp_period - 1 - off])[i] = (int) sum;
            }
        }

        // Create difference buffer:
        diff = new vector<int>(bufferSize);
        lastChanged = 0;
        prevSum = 0;
        dc_prev = 0;
        dc_diff = 0;
        dc_acc = 0;

    }

    void BlipBuffer::impulse(int smpPos, int smpOffset, int magnitude) {

        // Add into difference buffer:
        //if(smpPos+win_size < diff.length){
        for (int i = lastChanged; i < smpPos + win_size; i++) {
            (*diff)[i] = prevSum;
        }
        for (int i = 0; i < win_size; i++) {
            (*diff)[smpPos + i] += ((*(*imp)[smpOffset])[i] * magnitude) >> 8;
        }
        lastChanged = smpPos + win_size;
        prevSum = (*diff)[smpPos + win_size - 1];
    //}

    }

    int BlipBuffer::integrate() {

        int sum = prevSum;
        for (int i = 0; i < diff->size(); i++) {

            sum += (*diff)[i];

            // Remove DC:
            dc_diff = sum - dc_prev;
            dc_prev += dc_diff;
            dc_acc += dc_diff - (dc_acc >> 10);
            (*diff)[i] = dc_acc;

        }
        prevSum = sum;
        return lastChanged;

    }

    void BlipBuffer::clear() {

        for (int i = 0; i < diff->size(); i++) {
            (*diff)[i] = 0;
        }
        lastChanged = 0;

    }

    double BlipBuffer::sinc(double x) {
        if (x == 0.0) {
            return 1.0;
        }
        return sin(x) / x;
    }