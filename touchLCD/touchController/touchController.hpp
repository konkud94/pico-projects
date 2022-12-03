#pragma once

#include <utility>
#include <inttypes.h>

/*sort of forward declaration, seems like in C it is called opaque type*/
typedef struct spi_inst spi_inst_t;

class CTouchController
{
public:
    /*x, y*/
    static std::pair<uint16_t, uint16_t> GetRawAdcXY(const unsigned int chipSelectPin, spi_inst_t* const spi);
private:
    static uint16_t GetRawAdcVal(const uint8_t command, spi_inst_t* const spi);
};