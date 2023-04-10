#include "tasks.hpp"
#include "../spiDmaDriver/spiDmaDriver.hpp"
#include "../pinDefinitions/pinDefinitions.hpp"
#include "../lcdController/lcdDriver.hpp"
#include "../graphics/bitmap/bitmap12.hpp"
#include "../graphics/layoutController/experimentalLayoutController.hpp"
#include "hardware/spi.h"
#include "tasks.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace FreeRtosTasks
{
    void LcdTask(void* args)
    {
        CLcdTaskArgs* const argsAsStructure = (CLcdTaskArgs*)args;
        CLcdDriver* const lcdDriver = new CLcdDriver(argsAsStructure->SpiPacketQueue, CPinDefinitions::ChipSelectLcdPin,
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
            static constexpr uint32_t sleepMS = 20;
            layoutCtrl->MoveTriangle();
            lcdDriver->FlushData(layoutCtrl->GetBuffer(), lcdBufferSize);
            vTaskDelay(sleepMS / portTICK_PERIOD_MS);
        }

    }
}