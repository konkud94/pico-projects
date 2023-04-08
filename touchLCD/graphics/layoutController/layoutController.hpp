#pragma once

#include "../bitmap/colorBitmap.hpp"

class CLayoutControllerInterface
{
public:
    CLayoutControllerInterface(CColorBitmapInterface* const mainBitMap)
        :m_mainBitMap(mainBitMap)
    {
        ;
    }
    virtual ~CLayoutControllerInterface() = default;
    uint8_t* GetBuffer() const
    {
        return m_mainBitMap->GetBuffer();
    }
protected:
    CColorBitmapInterface* const m_mainBitMap;
};