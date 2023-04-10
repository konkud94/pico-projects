#pragma once

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

namespace FreeRtosTasks
{
    struct CSpi1DmaTaskArgs
    {
        QueueHandle_t SpiPacketQueue;
    };
    struct CLcdTaskArgs
    {
        QueueHandle_t SpiPacketQueue;
    };
    void Spi1DmaTask(void* args);
    void LcdTask(void* args);
}