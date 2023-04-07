#pragma once

#include "bitmap.hpp"

class CMonochromaticBitmap : public CBitmapInterface
{
public:
    CMonochromaticBitmap(const size_t dimensionX, const size_t dimensionY, uint8_t* const buffer)
        : CBitmapInterface(dimensionX, dimensionY, buffer)
    {
        ;
    }
    void SetPixelAt(const size_t x, const size_t y, bool val);
    bool GetPixelAt(const size_t x, const size_t y) const;
    void ClearAllPixels();
    void SetAllPixels();
private:
    virtual size_t GetBitsPerPixel() const override {return 1;}
};