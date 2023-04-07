#pragma once

#include "bitmap.hpp"

/* TODO: 
add method for setting background color, that is setting all (or maybe some?) pixels 
add method for adding filled rectangle
*/
class CMonochromaticBitmap;

class CColorBitmapInterface : public CBitmapInterface
{
public:
    enum class EPixelType
    {
        BitsPerPixel12 = 0,
        BitsPerPixel16 = 1,
    };
    /* buffer must be huge enough for all pixels to fit */
    CColorBitmapInterface(const size_t dimensionX, const size_t dimensionY, uint8_t* const buffer)
        : CBitmapInterface(dimensionX, dimensionY, buffer)
    {
        ;
    }
    virtual void SetPixelAt(const size_t x, const size_t y, uint16_t pixel, const EPixelType pixelType) = 0;
    virtual uint16_t GetPixelAt(const size_t x, const size_t y, const EPixelType pixelType) const = 0;
    virtual void SetWholeBufferToColor(uint16_t pixel, const EPixelType pixelType) = 0;
    virtual void PutColorBitmapAt(const size_t x, const size_t y, const CColorBitmapInterface& bitmap) = 0;
    virtual void PutMonoBitmapAt(const size_t x, const size_t y, const CMonochromaticBitmap& bitmap, uint16_t pixelColor,
        uint16_t backgroundColor, const EPixelType pixelType) = 0;
};