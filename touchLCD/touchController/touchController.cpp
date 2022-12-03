#include "touchController.hpp"
#include "hardware/spi.h"
#include "pico/stdlib.h"

/*x, y*/
std::pair<uint16_t, uint16_t> CTouchController::GetRawAdcXY(const unsigned int chipSelectPin, spi_inst_t* const spi)
{
    gpio_put(chipSelectPin, false);
    const uint16_t x = GetRawAdcVal(0xD0, spi);
    const uint16_t y = GetRawAdcVal(0x90, spi);
    gpio_put(chipSelectPin, true);
    return std::make_pair(x, y);
}
uint16_t CTouchController::GetRawAdcVal(const uint8_t command, spi_inst_t* const spi)
{
    {
        uint8_t dummy;
        [[maybe_unused]] auto ret = spi_write_read_blocking(spi, &command, &dummy, 1);
    }
    /*TODO: verify real busy interval*/
    constexpr uint16_t busyTimeUS = 1;
    sleep_us(busyTimeUS);
    uint16_t rawAdcVal;
    static constexpr uint8_t dummyWriteVal = 0x00;
    /*TODO: instead perform one 16bit transaction?*/
    {
        uint8_t readVal;
        [[maybe_unused]] auto ret = spi_write_read_blocking(spi, &dummyWriteVal, &readVal, 1);
        rawAdcVal = (uint16_t)readVal;
        rawAdcVal <<= 8;
    }
    {
        uint8_t readVal;
        [[maybe_unused]] auto ret = spi_write_read_blocking(spi, &dummyWriteVal, &readVal, 1);
        rawAdcVal |= (uint16_t)readVal;
        rawAdcVal >>= 3;
    }
    return rawAdcVal;
}
