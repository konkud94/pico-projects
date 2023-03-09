#pragma once

#include <inttypes.h>
#include <functional>
#include "pico/stdlib.h"
#include "stdio.h"

 /* sort of forward declaration, seems like in C it is called opaque type */
typedef struct spi_inst spi_inst_t;

class CLcdDriver
{
    using pinType = unsigned int;
    enum class ESpiTransferType
    {
        COMMAND = 0,
        PARAMETER = 1,
    };
    enum class ESpiTransferWidth
    {
        ONE_BYTE = 0,
        TWO_BYTES = 1, 
    };
public:

    CLcdDriver(spi_inst_t* spi, pinType csPin, pinType lcdRstPin, pinType lcdDcPin);
    ~CLcdDriver() = default;
    /* return: bytes flushed */
    size_t FlushData(const uint8_t* data, size_t len) const;
    int16_t GetLcdId() const;
    static constexpr size_t GetBitsPerPixel()
    {
        return s_bitsPerPixel;
    }
    static constexpr size_t GetPixelsAlongX()
    {
        return s_pixelsAlongX;
    }
    static constexpr size_t GetPixelsAlongY()
    {
        return s_pixelsAlongY;
    }
private:
    void LcdResetBlocking() const;
    void HandleSpiTransfer(const std::function<void()>& spiTransferFunction, const ESpiTransferType type, const ESpiTransferWidth width) const;
    uint8_t ReadLcdId() const;

    spi_inst_t* const m_spi;
    const pinType m_csPin;
    const pinType m_lcdRstPin;
    const pinType m_lcdDcPin;
    int16_t m_lcdId = -1;

    static constexpr size_t s_bitsPerPixel = 12; /*TODO: confgure*/
    static constexpr size_t s_pixelsAlongX = 240;
    static constexpr size_t s_pixelsAlongY = 240;
    static constexpr uint s_spiMaxReadSpeedHz = 6U * 1000U *  1000U;
    static constexpr uint8_t s_dummySpiWriteVal = 0x00;


};