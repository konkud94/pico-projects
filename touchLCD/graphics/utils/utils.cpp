#include "utils.hpp"
uint16_t CGraphicsUtils::Pixel16ToPixel12(uint16_t pixel16)
{
    static constexpr uint16_t maskRed = 0b1111000000000000;
    static constexpr uint16_t maskGreen = 0b0000111100000000;
    static constexpr uint16_t maskBlue = 0b0000000011110000;
    const uint16_t red =  (pixel16 << 0) & maskRed;
    const uint16_t green = (pixel16 << 1) & maskGreen;
    const uint16_t blue = (pixel16 << 3) & maskBlue;
    const uint16_t newPixel = red | green | blue;
    return newPixel;
}
uint16_t CGraphicsUtils::Pixel12ToPixel16(uint16_t pixel12)
{
    static constexpr uint16_t maskRed = 0b1111000000000000;
    static constexpr uint16_t maskGreen = 0b0000111100000000;
    static constexpr uint16_t maskBlue = 0b0000000011110000;
    const uint16_t red = pixel12 & maskRed;
    const uint16_t green = pixel12 & maskGreen;
    const uint16_t blue = pixel12 & maskBlue;
    const uint16_t newPixel = red | (green >> 1) | (blue >> 3);
    return newPixel;
}
uint16_t CGraphicsUtils::RGBToPixel12Bit(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t _red = red >> 4;
	uint16_t _green = green >> 4;
	uint16_t _blue = blue >> 4;
	const uint16_t pixel12Bit = _red << 12 | _green << 8 | _blue << 4;
	return pixel12Bit;
}
uint16_t CGraphicsUtils::RGBToPixel16Bit(uint8_t red, uint8_t green, uint8_t blue)
{
    const uint16_t pixel12Bit = RGBToPixel12Bit(red, green, blue);
    return Pixel12ToPixel16(pixel12Bit);
}
size_t CGraphicsUtils::GetRequiredBufferSizeBytes(const size_t dimensionX, const size_t dimensionY, const size_t bitsPerPixel)
{
    size_t requiredSizeBits = dimensionX * dimensionY * bitsPerPixel;
    const size_t modulo8 = requiredSizeBits % 8;
    if(modulo8 == 0)
    {
        return requiredSizeBits / 8;
    }
    requiredSizeBits += 8 - modulo8;
    return requiredSizeBits / 8;
}