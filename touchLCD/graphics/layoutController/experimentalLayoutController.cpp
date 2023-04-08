#include "experimentalLayoutController.hpp"
#include "../bitmap/monochromaticBitmap.hpp"

CExperimentalLayoutController::CExperimentalLayoutController(CColorBitmapInterface* const mainBitMap)
    : CLayoutControllerInterface(mainBitMap), m_triangleBitmap(new CMonochromaticBitmap(60, 60, new uint8_t[CGraphicsUtils::GetRequiredBufferSizeBytes(60, 60, 2)]))
{
    CBitmapUtils::FillBitmapWithTriangle(*m_triangleBitmap);
}

void CExperimentalLayoutController::MoveTriangle()
{
    const auto& backgroundColorDescr = CBankOfColors::GetColor(m_backgroundColorName);
    const auto& triangleColorDescr = CBankOfColors::GetColor(m_triangleColorName);
    const uint16_t backgroundColor12 = CGraphicsUtils::RGBToPixel12Bit(backgroundColorDescr.Red, 
        backgroundColorDescr.Green, backgroundColorDescr.Blue);
    const uint16_t triangleColor12 = CGraphicsUtils::RGBToPixel12Bit(triangleColorDescr.Red, 
        triangleColorDescr.Green, triangleColorDescr.Blue);
    m_mainBitMap->SetWholeBufferToColor(backgroundColor12, CColorBitmapInterface::EPixelType::BitsPerPixel12);
    m_mainBitMap->PutMonoBitmapAt(m_trianglePosX, m_trianglePosY, *m_triangleBitmap, triangleColor12,
        backgroundColor12, CColorBitmapInterface::EPixelType::BitsPerPixel12);

    static constexpr size_t advanceRate = 1;
    m_trianglePosX += advanceRate;
    m_trianglePosY += advanceRate;
    if(m_trianglePosX > m_mainBitMap->GetDimensions().first - 1 || m_trianglePosY > m_mainBitMap->GetDimensions().second - 1)
    {
        m_trianglePosX = 0;
        m_trianglePosY = 0;
    }
}