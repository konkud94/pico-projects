#include <stdio.h>
#include <initializer_list>
#include <utility>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
#include "touchController/touchController.hpp"
#include "lcdController/lcdDriver.hpp"

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

	printf("spi baudrate is = %d \n", spi_get_baudrate(spi1));
	while(1)
	{
		const auto xyPair = CTouchController::GetRawAdcXY(CPinDefinitions::ChipSelectTouchPadPin, spi1);
		printf("x = %u, y = %u \n", xyPair.first, xyPair.second);
		sleep_ms(1000);
	}
	return 0;
}
