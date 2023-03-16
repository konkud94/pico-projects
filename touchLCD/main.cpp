#include <stdio.h>
#include <initializer_list>
#include <utility>
#include <array>
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
#include "touchController/touchController.hpp"
#include "lcdController/lcdDriver.hpp"

/*
	important: max refresh rate is around 55 times per second!
*/
uint16_t RGBToPixel12Bit(uint8_t red, uint8_t green, uint8_t blue)
{
	uint16_t _red = red >> 4;
	uint16_t _green = green >> 4;
	uint16_t _blue = blue >> 4;

	const uint16_t pixel12Bit = _red << 12 | _green << 8 | _blue << 4;
	return pixel12Bit;
}
/*
	color_12Bit (RGB 4-4-4): rrrr gggg bbbb xxxx
	where x - may be anything, preferably 0
*/
void fillBufferWithColor_12Bit(uint8_t* const buffer, size_t bufferSizebytes, const uint16_t color_12Bit)
{
	const uint8_t rg = (uint8_t)(color_12Bit >> 8);
	const uint8_t gb = (uint8_t)(color_12Bit >> 4);
	const uint16_t gbrg = (((uint16_t)gb) << 8) | (uint16_t)rg;
	const uint8_t br = (uint8_t)(gbrg >> 4);
	/* buffer will go like: rg br gb rg br gb ...*/
	const size_t patternSize = 3;
	const uint8_t pattern[patternSize] = {rg, br, gb};
	for(size_t idx = 0; idx < bufferSizebytes; idx++)
	{
		const size_t patternIdx = idx % patternSize;
		buffer[idx] = pattern[patternIdx];
	}
}
uint32_t GetTimeMSSinceBoot()
{
	const auto absoluteTime = get_absolute_time();
	const uint32_t msSinceBoot = to_ms_since_boot(absoluteTime);
	return msSinceBoot;
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

	const char* red = "RED";
	const char* green = "GREEN";
	const char* blue = "BLUE";
	const char* black = "BLACK"; 
	const char* white = "WHITE";

	class CColorDescriptor {
	public:
		CColorDescriptor(const char* name, uint8_t red, uint8_t green, uint8_t blue)
		:	Name(name), Red(red), Green(green), Blue(blue)
		{
			;
		}
		const char* Name; 
		const uint8_t Red;
		const uint8_t Green;
		const uint8_t Blue;
	};
	std::array<CColorDescriptor, 5> colors = {
		CColorDescriptor(red, 255, 0, 0),
		CColorDescriptor(green, 0, 255, 0),
		CColorDescriptor(blue, 0, 0, 255),
		CColorDescriptor(black, 0, 0, 0),
		CColorDescriptor(white, 255, 255, 255),
	};
	while(true)
	{
		for(size_t idx = 0; idx < 5; idx++)
		{
			const auto& color = colors[idx];
			const uint16_t pixel12Bit = RGBToPixel12Bit(color.Red, color.Green, color.Blue);
			fillBufferWithColor_12Bit(buffer, lcdBufferSize, pixel12Bit);
			const uint32_t startMS = GetTimeMSSinceBoot();
			const uint32_t status = save_and_disable_interrupts();
			const size_t bytesFlushed = lcdDriver->FlushData(buffer, lcdBufferSize);
			restore_interrupts(status);
			const uint32_t tookMS = GetTimeMSSinceBoot() - startMS;
			printf("FlushData took %u [ms]\n", tookMS);
			if(lcdBufferSize != bytesFlushed)
			{
				printf("Error: lcdBufferSize = %u, bytesFlushed = %u, color = %s", lcdBufferSize, bytesFlushed, color.Name);
				while(true)
				{
					;
				}
			}
			printf("color = %s \n", color.Name);
			sleep_ms(500);
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
