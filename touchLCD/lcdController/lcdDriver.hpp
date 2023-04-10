#pragma once

#include <inttypes.h>
#include <functional>
#include <array>
#include <utility>
#include "pico/stdlib.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "../spiDmaDriver/spiDmaDriver.hpp"
class CLcdDriver
{
    using pinType = unsigned int;
    enum class ESpiTransferType
    {
        COMMAND = 0,
        PARAMETER = 1,
    };
    using CTransferPacket = CSpiDmaDriver::CTransferPacket;
public:
    CLcdDriver(QueueHandle_t spiDriverQueue, pinType csPin, pinType lcdRstPin, pinType lcdDcPin);
    ~CLcdDriver() = default;
    /* return: bytes flushed */
    size_t FlushData(const uint8_t* data, size_t len) const;
    int16_t GetLcdId() const;
    static constexpr size_t GetBitsPerPixel()
    {
        return s_bitsPerPixel;
    }
    static constexpr size_t GetPixelsAlongX()
    {
        return s_pixelsAlongX;
    }
    static constexpr size_t GetPixelsAlongY()
    {
        return s_pixelsAlongY;
    }
private:
    void LcdResetBlocking() const;
    void HandleSpiTransfer(CTransferPacket& transferPacket, const ESpiTransferType type) const;
    void SetUpRegisters() const;
    uint8_t ReadLcdId() const;

    const QueueHandle_t m_spiDriverQueue;
    const pinType m_csPin;
    const pinType m_lcdRstPin;
    const pinType m_lcdDcPin;
    int16_t m_lcdId = -1;
    SemaphoreHandle_t m_transferMutex;

    static constexpr size_t s_bitsPerPixel = 12;
    static constexpr size_t s_pixelsAlongX = 240;
    static constexpr size_t s_pixelsAlongY = 320;
    static constexpr uint s_spiMaxReadSpeedHz = 6U * 1000U *  1000U;
    static constexpr uint8_t s_dummySpiWriteVal = 0x00;

    static constexpr size_t s_initDeviceArrayCount = 70;
    /* transactionType, value */
    using InitEntryType = std::pair<ESpiTransferType, uint8_t>;
    static const std::array<InitEntryType, s_initDeviceArrayCount> s_initDeviceArray;

};