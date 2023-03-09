#pragma once

class CPinDefinitions
{
    using pinType = unsigned int;
public:
    static const pinType ChipSelectLcdPin;
    static const pinType ChipSelectTouchPadPin;
    static const pinType ChipSelectSDPin;
    static const pinType SpiMosiPin;
    static const pinType SpiMisoPin;
    static const pinType SpiClkPin;
    static const pinType LcdRstPin;
    static const pinType LcdDcPin;
    static const pinType LcdBklPin;
};