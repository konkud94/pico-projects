#pragma once
#include <inttypes.h>
#include <stdio.h>

/*
    pixel 16: RGB 5 6 5 
    pixel 12: RGB 4 4 4
*/
class CGraphicsUtils
{
public:
    static uint16_t Pixel16ToPixel12(uint16_t pixel16);
    static uint16_t Pixel12ToPixel16(uint16_t pixel12);
    static uint16_t RGBToPixel12Bit(uint8_t red, uint8_t green, uint8_t blue);
    static uint16_t RGBToPixel16Bit(uint8_t red, uint8_t green, uint8_t blue);
    static size_t GetRequiredBufferSizeBytes(const size_t dimensionX, const size_t dimensionY, const size_t bitsPerPixel);
};