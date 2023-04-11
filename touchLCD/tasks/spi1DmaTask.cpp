#include "../spiDmaDriver/spiDmaDriver.hpp"
#include "../pinDefinitions/pinDefinitions.hpp"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "tasks.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace FreeRtosTasks
{
    void Spi1DmaTask(void* args)
    {
        CSpi1DmaTaskArgs* const argsAsStructure = (CSpi1DmaTaskArgs*)args;
        QueueHandle_t spiPacketQueue = argsAsStructure->SpiPacketQueue;
        const unsigned int dmaTx = dma_claim_unused_channel(true);
        const unsigned int dmaRx = dma_claim_unused_channel(true);
        CSpiDmaDriver* const spiDmaDriver = new CSpiDmaDriver(spi1, CPinDefinitions::SpiMosiPin, CPinDefinitions::SpiMisoPin,
            CPinDefinitions::SpiClkPin, dmaTx, dmaRx);
        CSpiDmaDriver::CTransferPacket transferPacketPlaceHolder(CSpiDmaDriver::ETransferType::WRITE, 0, 0, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr);
        while(true)
        {
            if(xQueueReceive(spiPacketQueue, (void*)&transferPacketPlaceHolder, portMAX_DELAY) == pdTRUE)
            {
                [[maybe_unused]] const bool ret = spiDmaDriver->PerformTransferBlocking(transferPacketPlaceHolder, 5);
                assert(ret);
            }
            else
            {
                ; //TODO: panic?
            }
        }
    }
}