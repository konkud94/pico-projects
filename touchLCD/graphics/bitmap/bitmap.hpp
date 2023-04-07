#pragma once

#include <inttypes.h>
#include <utility>
#include <cstddef>

class CBitmapInterface
{
public:
    /* buffer must be huge enough for all pixels to fit */
    CBitmapInterface(const size_t dimensionX, const size_t dimensionY, uint8_t* const buffer)
        :m_x(dimensionX), m_y(dimensionY), m_buffer(buffer)
    {
        ;
    }
    virtual ~CBitmapInterface() {};
    CBitmapInterface(const CBitmapInterface&) = delete;
    CBitmapInterface& operator=(const CBitmapInterface&) = delete;
    /* x, y */
    std::pair<size_t, size_t> GetDimensions() const
    {
        return std::make_pair(m_x, m_y);
    }  
    uint8_t* GetBuffer() const
    {
        return m_buffer;
    }
    size_t GetRequiredBufferSizeBytes(const size_t dimensionX, const size_t dimensionY)
    {
        size_t requiredSizeBits = dimensionX * dimensionY * GetBitsPerPixel();
        const size_t modulo8 = requiredSizeBits % 8;
        if(modulo8 == 0)
        {
            return requiredSizeBits / 8;
        }
        requiredSizeBits += 8 - modulo8;
        return requiredSizeBits / 8;
    }
protected:
    const size_t m_x;
    const size_t m_y;
    uint8_t* const m_buffer;
private:
    virtual size_t GetBitsPerPixel() const = 0;
};
