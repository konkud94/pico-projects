#pragma once
#include <inttypes.h>
#include <utility>
#include <cstddef>
#include "bitmap.hpp"
#include "../utils/utils.hpp"
class CBitmap12
{
    /*buffer of pixels which are of type BitsPerPixel12*/
public:
    /* buffer must be huge enough for all pixels to fit */
    CBitmap12(const size_t dimensionX, const size_t diemensionY, uint8_t* const buffer);
    /* TODO: 
    add method for setting background color, that is setting all (or maybe some?) pixels 
    add method for adding filled rectangle
    */
    ~CBitmap12() = default;
    CBitmap12(const CBitmap12&) = delete;
    CBitmap12& operator=(const CBitmap12&) = delete;

    void SetPixelAt(const size_t x, const size_t y, uint16_t pixel, const EPixelType pixelType = EPixelType::BitsPerPixel12);
    uint16_t GetPixelAt(const size_t x, const size_t y, const EPixelType pixelType = EPixelType::BitsPerPixel12) const;
    void SetWholeBufferToColor(uint16_t pixel, const EPixelType pixelType = EPixelType::BitsPerPixel12);
    /* x, y */
    std::pair<size_t, size_t> GetDimensions() const
    {
        return std::make_pair(m_x, m_y);
    }  
    uint8_t* GetBuffer() const
    {
        return m_buffer;
    }
    static size_t GetRequiredBufferSizeBytes(const size_t dimensionX, const size_t dimensionY)
    {
        size_t requiredSizeBits = dimensionX * dimensionY * 12;
        const size_t modulo8 = requiredSizeBits % 8;
        if(modulo8 == 0)
        {
            return requiredSizeBits / 8;
        }
        requiredSizeBits += 8 - modulo8;
        return requiredSizeBits / 8;
    }
private:
    const size_t m_x;
    const size_t m_y;
    uint8_t* const m_buffer;
};