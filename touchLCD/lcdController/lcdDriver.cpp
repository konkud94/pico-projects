#include <limits>
#include <assert.h>
#include "lcdDriver.hpp"
#include "hardware/spi.h"

CLcdDriver::CLcdDriver(spi_inst_t* spi, pinType csPin, pinType lcdRstPin, pinType lcdDcPin)
    : m_spi(spi), m_csPin(csPin), m_lcdRstPin(lcdRstPin), m_lcdDcPin(lcdDcPin)
{
    LcdResetBlocking();
    m_lcdId = (int16_t)ReadLcdId();
    SetUpRegisters();
	sleep_ms(200);
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
    const uint initialBaudRate = spi_get_baudrate(m_spi);
    {
        [[maybe_unused]] static constexpr uint debugSpeedHz = 100'000; 
        /* should settle around 62.5 MHz */
        spi_set_baudrate(m_spi, std::numeric_limits<uint>::max());
    }
    {
        /* set frame pointers to x = 0, y = 0*/
        static constexpr uint8_t command = 0x2c;
        HandleSpiTransfer([this](){
            spi_write_blocking(m_spi, &command, 1);
        }, ESpiTransferType::COMMAND, ESpiTransferWidth::ONE_BYTE);
    }
    HandleSpiTransfer([this, &len, data](){
        len = 2 * spi_write16_blocking(m_spi, (const uint16_t*)data, len / 2);
    }, ESpiTransferType::PARAMETER, ESpiTransferWidth::TWO_BYTES);

    spi_set_baudrate(m_spi, initialBaudRate);
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
void CLcdDriver::SetUpRegisters() const
{
    uint8_t valueToSend = 0;
    const std::function<void()> spiTransferFunction = [this, &valueToSend](){
        spi_write_blocking(m_spi, &valueToSend, 1);
    };
    valueToSend = 0x11;
    HandleSpiTransfer(spiTransferFunction, ESpiTransferType::COMMAND, ESpiTransferWidth::ONE_BYTE);
    sleep_ms(100);
    for(const auto& pair : s_initDeviceArray)
    {
        valueToSend = pair.second;
        HandleSpiTransfer(spiTransferFunction, pair.first, ESpiTransferWidth::ONE_BYTE);
    }
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

const std::array<CLcdDriver::InitEntryType, CLcdDriver::s_initDeviceArrayCount> CLcdDriver::s_initDeviceArray{
    std::make_pair(ESpiTransferType::COMMAND, 0x36),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::COMMAND, 0x3a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x55),
    std::make_pair(ESpiTransferType::COMMAND, 0xb2),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x33),
    std::make_pair(ESpiTransferType::PARAMETER, 0x33),
    std::make_pair(ESpiTransferType::COMMAND, 0xb7),
    std::make_pair(ESpiTransferType::PARAMETER, 0x35),
    std::make_pair(ESpiTransferType::COMMAND, 0xbb),
    std::make_pair(ESpiTransferType::PARAMETER, 0x28),
    std::make_pair(ESpiTransferType::COMMAND, 0xc0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3c),
    std::make_pair(ESpiTransferType::COMMAND, 0xc2),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::COMMAND, 0xc3),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::COMMAND, 0xc4),
    std::make_pair(ESpiTransferType::PARAMETER, 0x20),
    std::make_pair(ESpiTransferType::COMMAND, 0xc6),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0f),
    std::make_pair(ESpiTransferType::COMMAND, 0xD0),
    std::make_pair(ESpiTransferType::PARAMETER, 0xa4),
    std::make_pair(ESpiTransferType::PARAMETER, 0xa1),
    std::make_pair(ESpiTransferType::COMMAND, 0xe0),
    std::make_pair(ESpiTransferType::PARAMETER, 0xd0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::PARAMETER, 0x08),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0f),
    std::make_pair(ESpiTransferType::PARAMETER, 0x11),
    std::make_pair(ESpiTransferType::PARAMETER, 0x2a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x36),
    std::make_pair(ESpiTransferType::PARAMETER, 0x55),
    std::make_pair(ESpiTransferType::PARAMETER, 0x44),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x06),
    std::make_pair(ESpiTransferType::PARAMETER, 0x11),
    std::make_pair(ESpiTransferType::PARAMETER, 0x20),
    std::make_pair(ESpiTransferType::COMMAND, 0xe1),
    std::make_pair(ESpiTransferType::PARAMETER, 0xd0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x02),
    std::make_pair(ESpiTransferType::PARAMETER, 0x07),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x18),
    std::make_pair(ESpiTransferType::PARAMETER, 0x34),
    std::make_pair(ESpiTransferType::PARAMETER, 0x43),
    std::make_pair(ESpiTransferType::PARAMETER, 0x4a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x2b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x22),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1f),
    std::make_pair(ESpiTransferType::COMMAND, 0x55),
    std::make_pair(ESpiTransferType::PARAMETER, 0xB0),
    std::make_pair(ESpiTransferType::COMMAND, 0x29),
    std::make_pair(ESpiTransferType::COMMAND, 0x2a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0xef),
    std::make_pair(ESpiTransferType::COMMAND, 0x2b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3f),
};