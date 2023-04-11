#pragma once

#include <inttypes.h>
#include <utility>
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "../spiDmaDriver/spiDmaDriver.hpp"

class CTouchController
{
    using pinType = unsigned int;
    using CTransferPacket = CSpiDmaDriver::CTransferPacket;
    using ETransferType = CSpiDmaDriver::ETransferType;
public:
    CTouchController(QueueHandle_t spiDriverQueue, pinType csPin);
    ~CTouchController() = default;
    std::pair<uint16_t, uint16_t> GetRawAdcXY();
private:
    const QueueHandle_t m_spiDriverQueue;
    const pinType m_csPin;
    SemaphoreHandle_t m_transferMutex;

    static constexpr unsigned int s_spiBaudrate = 2'000'000;
    static constexpr uint8_t s_dummySpiVal = 0x00;
    static constexpr uint8_t s_commandX = 0xD0;
    static constexpr uint8_t s_commandY = 0x90;

};