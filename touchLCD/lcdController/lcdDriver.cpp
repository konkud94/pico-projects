#include <limits>
#include <assert.h>
#include "lcdDriver.hpp"
#include "hardware/spi.h"

CLcdDriver::CLcdDriver(spi_inst_t* spi, pinType csPin, pinType lcdRstPin, pinType lcdDcPin)
    : m_spi(spi), m_csPin(csPin), m_lcdRstPin(lcdRstPin), m_lcdDcPin(lcdDcPin)
{
    LcdResetBlocking();
    m_lcdId = (int16_t)ReadLcdId();
}

size_t CLcdDriver::FlushData(const uint8_t* data, size_t len) const
{
    static constexpr size_t lcdBitsCapacity = s_pixelsAlongX * s_pixelsAlongY * s_bitsPerPixel;
    static constexpr size_t lcdBytesCapacity = lcdBitsCapacity / 8;
    len = std::min(len, lcdBytesCapacity);
    /* since spi sends two bytes per transfer */
    if(len < 2)
    {
        assert(false);
        return 0;
    }
    if(len % 2 != 0)
    {
        assert(false);
        len--;
    }
    HandleSpiTransfer([this, &len, data](){
        const uint initialBaudRate = spi_get_baudrate(m_spi);
        /* should settle around 62.5 MHz */
        spi_set_baudrate(m_spi, std::numeric_limits<uint>::max());
        len = spi_write16_blocking(m_spi, (const uint16_t*)data, len / 2);
        spi_set_baudrate(m_spi, initialBaudRate);
    }, ESpiTransferType::PARAMETER, ESpiTransferWidth::TWO_BYTES);
    return len;
}
int16_t CLcdDriver::GetLcdId() const
{
    return m_lcdId;
}
void CLcdDriver::LcdResetBlocking() const
{
    const bool pinState = gpio_get(m_lcdRstPin);
    if(pinState)
    {
        gpio_put(m_lcdRstPin, false);
        sleep_ms(500);
    }
    gpio_put(m_lcdRstPin, true);
}
void CLcdDriver::HandleSpiTransfer(const std::function<void()>& spiTransferFunction, const ESpiTransferType type, const ESpiTransferWidth width) const
{
    gpio_put(m_csPin, false);
    /* assume 8 bit is initial and normal spi mode */
    const bool reconfigureSpi = width == ESpiTransferWidth::TWO_BYTES;
    const bool dcPinDesiredState = type == ESpiTransferType::PARAMETER;
    const bool dcPinInitialState = gpio_get(m_lcdDcPin);
    gpio_put(m_lcdDcPin, dcPinDesiredState);
    if(reconfigureSpi)
    {
        spi_set_format(m_spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    }
    spiTransferFunction();
    spi_set_format(m_spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_put(m_lcdDcPin, dcPinInitialState);
    gpio_put(m_csPin, true);
}
uint8_t CLcdDriver::ReadLcdId() const
{
    static constexpr uint8_t reg = 0xdc;
    uint8_t idVal;
    HandleSpiTransfer([this, &idVal](){
        uint8_t receivedData[2];
        const uint8_t payload[2] = {reg, s_dummySpiWriteVal};
        const uint initialBaudRate = spi_get_baudrate(m_spi);
        const uint desiredBaudrate = std::min(initialBaudRate, s_spiMaxReadSpeedHz);
        spi_set_baudrate(m_spi, desiredBaudrate);
        spi_write_read_blocking(m_spi, payload, receivedData, 2);
        spi_set_baudrate(m_spi, initialBaudRate);
        idVal = receivedData[1];
    }, ESpiTransferType::COMMAND, ESpiTransferWidth::ONE_BYTE);
    return idVal;
}