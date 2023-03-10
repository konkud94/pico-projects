#include <stdio.h>
#include <initializer_list>
#include <utility>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
#include "touchController/touchController.hpp"
#include "lcdController/lcdDriver.hpp"

void setColorToBuffer(uint8_t* const buffer, const size_t bufferLenBytes, const uint16_t color)
{
	if(bufferLenBytes % 2 != 0)
	{
		printf("error: bufferLenBytes % 2 != 0");
		return;
	}
	uint16_t* _buffer = (uint16_t*)buffer;
	size_t _len = bufferLenBytes / 2;
	for(size_t idx = 0; idx < _len; idx++)
	{
		_buffer[idx] = color;
	}
}

int main() 
{
	stdio_init_all();
	spi_init(spi1, 4000000);
	sleep_ms(1000 * 5);
	for(const auto pin : {CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::ChipSelectTouchPadPin, CPinDefinitions::ChipSelectSDPin, CPinDefinitions::LcdBklPin,
		CPinDefinitions::LcdDcPin, CPinDefinitions::LcdRstPin})
	{
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
	}
	for(const auto pin : {CPinDefinitions::SpiMosiPin,
		CPinDefinitions::SpiMisoPin, CPinDefinitions::SpiClkPin})
	{
		gpio_set_function(pin, GPIO_FUNC_SPI);
	}
	printf("spi intial baudrate is = %d \n", spi_get_baudrate(spi1));

	CLcdDriver* const lcdDriver = new CLcdDriver(spi1, CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::LcdRstPin, CPinDefinitions::LcdDcPin);
	const int16_t lcdId  = lcdDriver->GetLcdId();
	printf("lcd id is = %d \n", lcdId);

	static constexpr size_t lcdBufferSize = lcdDriver->GetBitsPerPixel() * lcdDriver->GetPixelsAlongX() * lcdDriver->GetPixelsAlongY() / (size_t)8 ;
	uint8_t* const buffer = new uint8_t[lcdBufferSize];
	if(buffer != nullptr)
	{
		printf("successfuly allocated %d bytes for lcdBuffer! \n", lcdBufferSize);
	}
	else
	{
		printf("failed to  allocate memory for lcdBuffer! \n");
		while(true)
		{
			;
		}
	}

	uint16_t colors[] =
	{
		0xFFFF, 
		0x0000, 
		0x001F, 
		0xF81F, 
		0xFFE0, 
		0x07FF, 
		0xF800, 
		0xF81F, 
		0x07E0, 
		0x7FFF, 
		0xFFE0, 
		0xBC40, 
		0xFC07, 
		0x8430
	};
	while(true)
	{
		for(size_t idx = 0; idx < sizeof(colors) / sizeof(colors[0]); idx++)
		{
			setColorToBuffer(buffer, lcdBufferSize, colors[idx]);
			const size_t bytesFlushed = lcdDriver->FlushData(buffer, lcdBufferSize);
			if(lcdBufferSize != bytesFlushed)
			{
				printf("Error: lcdBufferSize = %u, bytesFlushed = %u, color = %u", lcdBufferSize, bytesFlushed, colors[idx]);
				while(true)
				{
					;
				}
			}
			sleep_ms(1000 * 1);
			printf("Line Ended\n");
		}
	}

	printf("spi baudrate is = %d \n", spi_get_baudrate(spi1));
	while(1)
	{
		const auto xyPair = CTouchController::GetRawAdcXY(CPinDefinitions::ChipSelectTouchPadPin, spi1);
		printf("x = %u, y = %u \n", xyPair.first, xyPair.second);
		sleep_ms(1000);
	}
	return 0;
}
