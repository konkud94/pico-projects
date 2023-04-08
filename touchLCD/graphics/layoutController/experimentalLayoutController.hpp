#pragma once

#include "layoutController.hpp"
#include "../utils/utils.hpp"

class CMonochromaticBitmap;
class CExperimentalLayoutController : public CLayoutControllerInterface
{
public:
    /* TODO: make non copyable etc */
    CExperimentalLayoutController(CColorBitmapInterface* const mainBitMap);
    void MoveTriangle();
private:
    CMonochromaticBitmap* const m_triangleBitmap;
    size_t m_trianglePosX = 0;
    size_t m_trianglePosY = 0;
    
    static constexpr CBankOfColors::EColorName m_backgroundColorName = CBankOfColors::EColorName::WHITE;
    static constexpr CBankOfColors::EColorName m_triangleColorName = CBankOfColors::EColorName::RED;
};