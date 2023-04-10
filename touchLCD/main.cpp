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
#include "tasks./tasks.hpp"
#include "queue.h"

/*
	important: max LCD refresh rate is around 55 times per second!
*/

int main() 
{
	stdio_init_all();
	sleep_ms(5*1000);
	for(const auto pin : {CPinDefinitions::ChipSelectTouchPadPin, CPinDefinitions::ChipSelectSDPin, CPinDefinitions::LcdBklPin})
	{
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
	}
	QueueHandle_t spiPacketQueue = xQueueCreate(10, sizeof(CSpiDmaDriver::CTransferPacket));
	assert(spiPacketQueue != nullptr);

	FreeRtosTasks::CSpi1DmaTaskArgs spi1DmaTaskArgs {spiPacketQueue};
	FreeRtosTasks::CLcdTaskArgs lcdTaskArgs {spiPacketQueue};

	xTaskCreate(FreeRtosTasks::Spi1DmaTask, "Spi1DmaTask", 500, &spi1DmaTaskArgs, 10, NULL);
	xTaskCreate(FreeRtosTasks::LcdTask, "LCDTask", 500, &lcdTaskArgs, 1, NULL);
	vTaskStartScheduler();

	while(true)
	{
		;
	}

	return 0;
}
