#pragma once
#include <inttypes.h>
#include <stdio.h>
#include <array>

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

class CMonochromaticBitmap;
class CBitmapUtils
{
public:
    static void FillBitmapWithTriangle(CMonochromaticBitmap& bitmap);
};

class CBankOfColors
{
public:
    enum class EColorName
    {
        RED = 0,
        GREEN,
        BLUE,
        BLACK,
        WHITE,

        /* keep last*/
        METADATA_COUNT,
    };
    class CColorDescriptor
    {
	public:
		CColorDescriptor(EColorName name, uint8_t red, uint8_t green, uint8_t blue)
		:	Name(name), Red(red), Green(green), Blue(blue)
		{
			;
		}
		const EColorName Name; 
		const uint8_t Red;
		const uint8_t Green;
		const uint8_t Blue;
	};
    static const CColorDescriptor& GetColor(const EColorName name);
    static const std::array<CColorDescriptor, (size_t)EColorName::METADATA_COUNT> colors;
};