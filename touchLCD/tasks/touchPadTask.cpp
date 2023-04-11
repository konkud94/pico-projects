#include "tasks.hpp"
#include <algorithm>
#include <inttypes.h>
#include <utility>
#include "../spiDmaDriver/spiDmaDriver.hpp"
#include "../pinDefinitions/pinDefinitions.hpp"
#include "../touchController/touchController.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
/*
	important: max LCD refresh rate is around 55 times per second!
*/
namespace FreeRtosTasks
{
    void TouchPadTask(void* args)
    {
        CTouchPadTaskArgs* const argsAsStructure = (CTouchPadTaskArgs*)args;
        CTouchController* const touchController = new CTouchController(argsAsStructure->SpiPacketQueue,
            CPinDefinitions::ChipSelectTouchPadPin);
        while(true)
        {
            static constexpr uint32_t refreshRateMS = 100;
            static constexpr uint32_t refreshRateTicks = refreshRateMS / portTICK_PERIOD_MS;
            const TickType_t startTicks = xTaskGetTickCount();
            [[maybe_unused]]const auto pair = touchController->GetRawAdcXY();
            const TickType_t elapsedTicks = xTaskGetTickCount() - startTicks;
            const TickType_t sleepForTicks = (TickType_t)std::max((int32_t)refreshRateTicks - (int32_t)elapsedTicks, (int32_t)0);
            vTaskDelay(sleepForTicks);
        }
    }
}