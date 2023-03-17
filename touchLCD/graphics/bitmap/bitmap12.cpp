#include "bitmap12.hpp"
#include <stdio.h>

CBitmap12::CBitmap12(const size_t dimensionX, const size_t dimensionY, uint8_t* const buffer)
    :m_x(dimensionX), m_y(dimensionY), m_buffer(buffer)
{
    ;
}
/* TODO: check if x and y are within boundaries */
void CBitmap12::SetPixelAt(const size_t x, const size_t y, uint16_t pixel, const EPixelType pixelType)
{
    if(pixelType == EPixelType::BitsPerPixel16)
    {
        pixel = CGraphicsUtils::Pixel16ToPixel12(pixel);
    }
    const size_t nibbleIdx = y * 3 * m_x + x * 3;
    const size_t byteIdx = nibbleIdx / 2;
    const bool byteAligned = nibbleIdx % 2 == 0;
    if(byteAligned)
    {
        const uint8_t rg = (uint8_t)(pixel >> 8);
        const uint8_t b =  (uint8_t)(pixel & 0xf0);
        m_buffer[byteIdx] = rg;
        uint8_t nextByte = m_buffer[byteIdx + 1];
        nextByte = nextByte & 0x0f;
        nextByte |= b;
        m_buffer[byteIdx + 1] = nextByte;
    }
    else
    {
        const uint8_t r = (uint8_t)(pixel >> 12);
        uint8_t currentByte = m_buffer[byteIdx];
        currentByte = currentByte & 0xf0;
        currentByte |= r;
        m_buffer[byteIdx] = currentByte;
        const uint8_t gb = (uint8_t)(pixel >> 4);
        m_buffer[byteIdx + 1] = gb;
    }

}
uint16_t CBitmap12::GetPixelAt(const size_t x, const size_t y, const EPixelType pixelType) const
{
    uint16_t pixel12;
    const size_t nibbleIdx = y * 3 * m_x + x * 3;
    const size_t byteIdx = nibbleIdx / 2;
    const bool byteAligned = nibbleIdx % 2 == 0;
    if(byteAligned)
    {
        const uint8_t rg = m_buffer[byteIdx];
        const uint8_t bx = m_buffer[byteIdx + 1];
        const uint8_t b = bx & 0xf0;
        pixel12 = (((uint16_t)rg) << 8 )| (uint16_t)b;
    }
    else
    {
        const uint8_t xr = m_buffer[byteIdx];
        const uint8_t gb = m_buffer[byteIdx + 1];
        pixel12 = (((uint16_t)xr) << 12 )| (((uint16_t)gb) << 4);
    }
    if(pixelType == EPixelType::BitsPerPixel16)
    {
        return CGraphicsUtils::Pixel12ToPixel16(pixel12);
    }
    return pixel12;
}
void CBitmap12::SetWholeBufferToColor(uint16_t pixel, const EPixelType pixelType)
{
    if(pixelType == EPixelType::BitsPerPixel16)
    {
        pixel = CGraphicsUtils::Pixel16ToPixel12(pixel);
    }
    const size_t presumedBufferSize = GetRequiredBufferSizeBytes(m_x, m_y);
    const uint8_t rg = (uint8_t)(pixel >> 8);
	const uint8_t gb = (uint8_t)(pixel >> 4);
	const uint16_t gbrg = (((uint16_t)gb) << 8) | (uint16_t)rg;
	const uint8_t br = (uint8_t)(gbrg >> 4);
	/* buffer will go like: rg br gb rg br gb ...*/
	const size_t patternSize = 3;
	const uint8_t pattern[patternSize] = {rg, br, gb};
	for(size_t idx = 0; idx < presumedBufferSize; idx++)
	{
		const size_t patternIdx = idx % patternSize;
		m_buffer[idx] = pattern[patternIdx];
	}

}