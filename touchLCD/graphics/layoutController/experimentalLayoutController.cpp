#include "experimentalLayoutController.hpp"
#include "../bitmap/monochromaticBitmap.hpp"
CExperimentalLayoutController::CExperimentalLayoutController(CColorBitmapInterface* const mainBitMap)
    : CLayoutControllerInterface(mainBitMap),
    m_triangleBitmap(new CMonochromaticBitmap(m_triangleBitMapSizeX, m_triangleBitMapSizeY,
        new uint8_t[CGraphicsUtils::GetRequiredBufferSizeBytes(m_triangleBitMapSizeX, m_triangleBitMapSizeY, 2)]))
{
    CBitmapUtils::FillBitmapWithTriangle(*m_triangleBitmap);
    const auto& backgroundColorDescr = CBankOfColors::GetColor(m_backgroundColorName);
    const auto& triangleColorDescr = CBankOfColors::GetColor(m_triangleColorName);
    m_backgroundColor12 = CGraphicsUtils::RGBToPixel12Bit(backgroundColorDescr.Red, 
        backgroundColorDescr.Green, backgroundColorDescr.Blue);
    m_triangleColor12 = CGraphicsUtils::RGBToPixel12Bit(triangleColorDescr.Red, 
        triangleColorDescr.Green, triangleColorDescr.Blue);
    m_mainBitMap->SetWholeBufferToColor(m_backgroundColor12, CColorBitmapInterface::EPixelType::BitsPerPixel12);
}

void CExperimentalLayoutController::MoveTriangle()
{
    /* clear previous */
    m_mainBitMap->PutMonoBitmapAt(m_prevTrianglePosX, m_prevTrianglePosY, *m_triangleBitmap, m_backgroundColor12,
        m_backgroundColor12, CColorBitmapInterface::EPixelType::BitsPerPixel12);
    /* put new */
    m_mainBitMap->PutMonoBitmapAt(m_trianglePosX, m_trianglePosY, *m_triangleBitmap, m_triangleColor12,
        m_backgroundColor12, CColorBitmapInterface::EPixelType::BitsPerPixel12);
    m_prevTrianglePosX = m_trianglePosX;
    m_prevTrianglePosY = m_trianglePosY;
    static constexpr size_t advanceRate = 1;
    m_trianglePosX += advanceRate;
    m_trianglePosY += advanceRate;
    if(m_trianglePosX > m_mainBitMap->GetDimensions().first - 1 || m_trianglePosY > m_mainBitMap->GetDimensions().second - 1)
    {
        m_trianglePosX = 0;
        m_trianglePosY = 0;
    }
}