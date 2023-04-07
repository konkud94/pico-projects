#include "bitmap12.hpp"
#include <stdio.h>
#include "monochromaticBitmap.hpp"

CBitmap12::CBitmap12(const size_t dimensionX, const size_t dimensionY, uint8_t* const buffer)
    :CColorBitmapInterface(dimensionX, dimensionY, buffer)
{
    ;
}
void CBitmap12::SetPixelAt(const size_t x, const size_t y, uint16_t pixel, const EPixelType pixelType)
{
    const bool coordinatesOk = x < m_x && y < m_y;
    if(!coordinatesOk)
    {
        return;
    }
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
    const bool coordinatesOk = x < m_x && y < m_y;
    if(!coordinatesOk)
    {
        /* black */
        return 0;
    }
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
void CBitmap12::PutColorBitmapAt(const size_t x, const size_t y, const CColorBitmapInterface& bitmap)
{
    const size_t xb = bitmap.GetDimensions().first;
    const size_t yb = bitmap.GetDimensions().second;
    size_t destinationY = y;
    size_t sourceY = 0;
    for(; destinationY < m_y && sourceY < yb; destinationY++, sourceY++)
    {
        size_t destinationX = x;
        size_t sourceX = 0;
        for(; destinationX < m_x && sourceX < xb; destinationX++, sourceX++)
        {
            const uint16_t pixel12 = bitmap.GetPixelAt(sourceX, sourceY, EPixelType::BitsPerPixel12);
            SetPixelAt(destinationX, destinationY, pixel12, EPixelType::BitsPerPixel12);
        }
    }
}
void CBitmap12::PutMonoBitmapAt(const size_t x, const size_t y, const CMonochromaticBitmap& bitmap, uint16_t pixelColor,
        uint16_t backgroundColor, const EPixelType pixelType)
{
    if(pixelType == EPixelType::BitsPerPixel16)
    {
        pixelColor = CGraphicsUtils::Pixel16ToPixel12(pixelColor);
        backgroundColor = CGraphicsUtils::Pixel16ToPixel12(backgroundColor);
    }
    const size_t xb = bitmap.GetDimensions().first;
    const size_t yb = bitmap.GetDimensions().second;
    size_t destinationY = y;
    size_t sourceY = 0;
    for(; destinationY < m_y && sourceY < yb; destinationY++, sourceY++)
    {
        size_t destinationX = x;
        size_t sourceX = 0;
        for(; destinationX < m_x && sourceX < xb; destinationX++, sourceX++)
        {
            const bool pixelMono = bitmap.GetPixelAt(sourceX, sourceY);
            const uint16_t pixel12 = pixelMono? pixelColor : backgroundColor;
            SetPixelAt(destinationX, destinationY, pixel12, EPixelType::BitsPerPixel12);
        }
    }
}