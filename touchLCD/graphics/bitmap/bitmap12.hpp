#pragma once
#include <inttypes.h>
#include <utility>
#include <cstddef>
#include "colorBitmap.hpp"
#include "../utils/utils.hpp"

class CBitmap12 : public CColorBitmapInterface
{
public:
    CBitmap12(const size_t dimensionX, const size_t diemensionY, uint8_t* const buffer);
    virtual void SetPixelAt(const size_t x, const size_t y, uint16_t pixel, const EPixelType pixelType) override;
    virtual uint16_t GetPixelAt(const size_t x, const size_t y, const EPixelType pixelType) const override;
    virtual void SetWholeBufferToColor(uint16_t pixel, const EPixelType pixelType) override;
    virtual void PutColorBitmapAt(const size_t x, const size_t y, const CColorBitmapInterface& bitmap) override;
    virtual void PutMonoBitmapAt(const size_t x, const size_t y, const CMonochromaticBitmap& bitmap, uint16_t pixelColor,
        uint16_t backgroundColor, const EPixelType pixelType) override;
private:
    virtual size_t GetBitsPerPixel() const override {return 12;}
};