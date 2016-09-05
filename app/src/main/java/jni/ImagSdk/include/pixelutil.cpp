#include "ibmp.h"
#include "ipoint.h"

void pixel2rgba(int ipixel,PIXELS& opixel) {
    RGBQUAD rgba;
    rgba.rgbReserved = (ipixel & 0xFF000000) >> 24;
    rgba.rgbRed      = (ipixel & 0x00FF0000) >> 16;;
    rgba.rgbGreen    = (ipixel & 0x0000FF00) >> 8;;
    rgba.rgbBlue     = (ipixel & 0x000000FF);;
    opixel.setRGB(rgba);
}

int rgba2pixel(PIXELS& pixel) {
    int x = pixel.getX();
    int y = pixel.getY();
    int R = pixel.getRed();
    int G = pixel.getGreen();
    int B = pixel.getBlue();
    int A = 255;
    return ((A << 24) | (R << 16) | (G << 8) | B);
}

