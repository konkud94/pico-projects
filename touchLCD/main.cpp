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

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	(void)xTask;
	printf("vApplicationStackOverflowHook: pcTaskName = %s \n", pcTaskName);
}
void HeartBeatTask(void* arg)
{
	(void)arg;
	while(1)
	{
		const bool pinState = gpio_get_out_level(PICO_DEFAULT_LED_PIN);
		gpio_put(PICO_DEFAULT_LED_PIN, !pinState);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
int main() 
{
	stdio_init_all();
	//sleep_ms(5*1000);
	for(const auto pin : {CPinDefinitions::ChipSelectTouchPadPin, CPinDefinitions::ChipSelectSDPin, CPinDefinitions::LcdBklPin, (unsigned int)PICO_DEFAULT_LED_PIN})
	{
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
	}
	QueueHandle_t spiPacketQueue = xQueueCreate(10, sizeof(CSpiDmaDriver::CTransferPacket));
	assert(spiPacketQueue != nullptr);
	FreeRtosTasks::CSpi1DmaTaskArgs spi1DmaTaskArgs {spiPacketQueue};
	FreeRtosTasks::CLcdTaskArgs lcdTaskArgs {spiPacketQueue};
	xTaskCreate(FreeRtosTasks::Spi1DmaTask, "Spi1DmaTask", 1000, &spi1DmaTaskArgs, 1, NULL);
	xTaskCreate(FreeRtosTasks::LcdTask, "LCDTask", 1000, &lcdTaskArgs, 1, NULL);
	xTaskCreate(HeartBeatTask, "HeartBeatTask", 300, NULL, 2, NULL);
	vTaskStartScheduler();

	while(true)
	{
		;
	}

	return 0;
}
