#include "monochromaticBitmap.hpp"


void CMonochromaticBitmap::SetPixelAt(const size_t x, const size_t y, bool val)
{
    const bool coordinatesOk = x < m_x && y < m_y;
    if(!coordinatesOk)
    {
        return;
    }
    const size_t bitIdx = y * m_x + x;
    const size_t bufferIdx = bitIdx / 8;
    const size_t bitNum = bitIdx % 8;
    uint8_t& byte = m_buffer[bufferIdx];
    if(val)
    {
        byte |= (1 << bitNum);
    }
    else
    {
        byte &= ~(1 << bitNum);
    }
}
bool CMonochromaticBitmap::GetPixelAt(const size_t x, const size_t y) const
{
    const bool coordinatesOk = x < m_x && y < m_y;
    if(!coordinatesOk)
    {
        return false;
    }
    const size_t bitIdx = y * m_x + x;
    const size_t bufferIdx = bitIdx / 8;
    const size_t bitNum = bitIdx % 8;
    const uint8_t& byte = m_buffer[bufferIdx];
    return byte & (1 << bitNum);
}
void CMonochromaticBitmap::ClearAllPixels()
{
    for(size_t x = 0; x < m_x; x++)
    {
        for(size_t y = 0; y < m_y; y++)
        {
            SetPixelAt(x, y, false);
        }
    }
}
void CMonochromaticBitmap::SetAllPixels()
{
    for(size_t x = 0; x < m_x; x++)
    {
        for(size_t y = 0; y < m_y; y++)
        {
            SetPixelAt(x, y, true);
        }
    }
}