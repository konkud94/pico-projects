#include <stdio.h>
#include <initializer_list>
#include <utility>
#include <array>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include "hardware/spi.h"
#include "pinDefinitions/pinDefinitions.hpp"
#include "touchController/touchController.hpp"
#include "lcdController/lcdDriver.hpp"
#include "graphics/bitmap/bitmap12.hpp"
#include "graphics/utils/utils.hpp"
#include "graphics/bitmap/monochromaticBitmap.hpp"
#include "graphics/layoutController/experimentalLayoutController.hpp"


/*
	important: max refresh rate is around 55 times per second!
*/
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
	sleep_ms(5*1000);
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

	CLcdDriver* const lcdDriver = new CLcdDriver(spi1, CPinDefinitions::ChipSelectLcdPin,
		CPinDefinitions::LcdRstPin, CPinDefinitions::LcdDcPin);
	static constexpr size_t lcdBufferSize = lcdDriver->GetBitsPerPixel() * lcdDriver->GetPixelsAlongX() * lcdDriver->GetPixelsAlongY() / (size_t)8 ;
	uint8_t* const buffer = new uint8_t[lcdBufferSize];
	if(buffer == nullptr)
	{
		assert(false);
	}
	CColorBitmapInterface* const bitmap12 = new CBitmap12(240, 320, buffer);
	CExperimentalLayoutController* const layoutCtrl = new CExperimentalLayoutController(bitmap12);

	while(true)
	{
		const uint32_t sleepMS = 20;
		layoutCtrl->MoveTriangle();
		const uint32_t status = save_and_disable_interrupts();
		lcdDriver->FlushData(layoutCtrl->GetBuffer(), lcdBufferSize);
		restore_interrupts(status);
		sleep_ms(sleepMS);
	}

	return 0;
}
