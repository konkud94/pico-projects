#include <stdio.h>
#include <initializer_list>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
int main() 
{
	stdio_init_all();
	spi_init(spi1, 4000000);
	for(const auto pin : {CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::ChipSelectTouchPadPin, CPinDefinitions::ChipSelectSDPin})
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
	
	while(1)
	{
		printf("Hello World \n");
		sleep_ms(1000);
	}
	return 0;
}
