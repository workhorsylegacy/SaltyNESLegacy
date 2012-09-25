

#include "Globals.h"
#include <limits.h>

int Color::getRGB() {
	return 0;
}

// Copied from
// http://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb
typedef struct {
    double r;       // percent
    double g;       // percent
    double b;       // percent
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // percent
    double v;       // percent
} hsv;

float* Color::RGBtoHSV(int b, int g, int r, float* hsbvals) {
    hsv         out;
    double      min, max, delta;

    min = r < g ? r : g;
    min = min  < b ? min  : b;

    max = r > g ? r : g;
    max = max  > b ? max  : b;

    out.v = max;                                // v
    delta = max - min;
    if( max > 0.0 ) {
        out.s = (delta / max);                  // s
    } else {
        // r = g = b = 0                        // s = 0, v is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
		hsbvals[0] = out.h;
		hsbvals[1] = out.s;
		hsbvals[2] = out.v;
         return hsbvals;
    }
    if( r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( g - b ) / delta;        // between yellow & magenta
    else
    if( g >= max )
        out.h = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

	hsbvals[0] = out.h;
	hsbvals[1] = out.s;
	hsbvals[2] = out.v;
    return hsbvals;
}


int Color::HSVtoRGB(float h, float s, float v) {
	hsv in;
	in.h = h;
	in.s = s;
	in.v = v;
	int retval;
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        if(isnan(in.h)) {   // in.h == NAN
            out.r = in.v;
            out.g = in.v;
            out.b = in.v;
			PaletteTable::setRed(retval, out.r);
			PaletteTable::setGreen(retval, out.g);
			PaletteTable::setBlue(retval, out.b);
            return retval;
        }
        // error - should never happen
        out.r = 0.0;
        out.g = 0.0;
        out.b = 0.0;
		PaletteTable::setRed(retval, out.r);
		PaletteTable::setGreen(retval, out.g);
		PaletteTable::setBlue(retval, out.b);
        return retval;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
   	PaletteTable::setRed(retval, out.r);
	PaletteTable::setGreen(retval, out.g);
	PaletteTable::setBlue(retval, out.b);
    return retval;     
}

