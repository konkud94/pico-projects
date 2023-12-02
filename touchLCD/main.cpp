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

#include "syncSDDriver/sd.hpp"

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

const char* GetSDTypeAsString(const CSD::ESDType& type)
{
	switch (type)
	{
	case CSD::ESDType::SD1:
		return "SD1";
		break;
	case CSD::ESDType::SD2_BLOCK_ADDR:
		return "SD2_BLOCK_ADDR";
		break;
	case CSD::ESDType::SD2_BYTE_ADDR:
		return "SD2_BYTE_ADDR";
		break;
	default:
		return "UNKNOWN";
		break;
	}
}
int main() 
{
	stdio_init_all();
	sleep_ms(5*1000);
	for(const auto pin : {CPinDefinitions::ChipSelectLcdPin, CPinDefinitions::LcdBklPin, (unsigned int)PICO_DEFAULT_LED_PIN, CPinDefinitions::ChipSelectTouchPadPin})
	{
		gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
	}


	CSD* const sd = new CSD(spi1, CPinDefinitions::SpiMosiPin, CPinDefinitions::SpiMisoPin,
		CPinDefinitions::SpiClkPin, CPinDefinitions::ChipSelectSDPin);
	printf("\nSD type is: %s \n", GetSDTypeAsString(sd->GetSDType()));


	uint8_t* const sector = new uint8_t[512];
	uint8_t* const sectorCopy = new uint8_t[512];
	for(size_t idx = 0; idx < 512; idx++)
	{
		sector[idx] = idx;
		sectorCopy[idx] = 0;
	}
	for(size_t blockNum = 0; blockNum < 10'000; blockNum++)
	{
		const bool writeRet = sd->WriteBlock(blockNum, sector);
		printf("writeRet = %u\n", (unsigned int)writeRet);
		const bool readRet = sd->ReadBlock(blockNum, sectorCopy);
		printf("readRet = %u\n",(unsigned int)readRet);
		if(writeRet && readRet)
		{
			printf("\n=====SUCCESS======\n");
			while(true)
			{
				;
			}
		}
	}
	//static constexpr size_t blockNum = 1000;


	// for(size_t idx = 0; idx < 512; idx++)
	// {
	// 	if(sector[idx] != sectorCopy[idx])
	// 	{
	// 		printf("Mismatch at idx = %u, sector = %u, sectorCopy = %u\n", idx, sector[idx], sectorCopy[idx]);
	// 	}
	// }

	while(true)
	{
		;
	}


	QueueHandle_t spiPacketQueue = xQueueCreate(10, sizeof(CSpiDmaDriver::CTransferPacket));
	assert(spiPacketQueue != nullptr);
	FreeRtosTasks::CSpi1DmaTaskArgs spi1DmaTaskArgs {spiPacketQueue};
	FreeRtosTasks::CLcdTaskArgs lcdTaskArgs {spiPacketQueue};
	FreeRtosTasks::CTouchPadTaskArgs touchPadTaskArgs {spiPacketQueue};
	xTaskCreate(FreeRtosTasks::Spi1DmaTask, "Spi1DmaTask", 1000, &spi1DmaTaskArgs, 1, NULL);
	xTaskCreate(FreeRtosTasks::LcdTask, "LCDTask", 1000, &lcdTaskArgs, 1, NULL);
	xTaskCreate(FreeRtosTasks::TouchPadTask, "TouchPadTask", 350, &touchPadTaskArgs, 1, NULL);
	xTaskCreate(HeartBeatTask, "HeartBeatTask", 300, NULL, 1, NULL);
	vTaskStartScheduler();
	while(true)
	{
		;
	}

	return 0;
}
