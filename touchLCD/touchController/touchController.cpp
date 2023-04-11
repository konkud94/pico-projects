#include "touchController.hpp"
#include <functional>
#include "hardware/gpio.h"

CTouchController::CTouchController(QueueHandle_t spiDriverQueue, pinType csPin)
    : m_spiDriverQueue(spiDriverQueue),
    m_csPin(csPin)
{
    gpio_init(csPin);
    gpio_set_dir(csPin, GPIO_OUT);
    gpio_put(csPin, true);

    m_transferMutex = xSemaphoreCreateMutex();
    assert(m_transferMutex != nullptr);
}
std::pair<uint16_t, uint16_t> CTouchController::GetRawAdcXY()
{
    static constexpr size_t transferLengthBytes = 6;
    const uint8_t payload[transferLengthBytes] = {s_commandX, s_dummySpiVal, s_dummySpiVal, s_commandY, s_dummySpiVal, s_dummySpiVal};
    uint8_t receivedData[transferLengthBytes];
    CTransferPacket transferPacket(ETransferType::READnWRITE, s_spiBaudrate, transferLengthBytes, payload, 
        receivedData, nullptr, nullptr, nullptr, nullptr);
    transferPacket.BeforeCallbackArg = nullptr;
    transferPacket.AfterCallbackArg = nullptr;
    transferPacket.BeforeTransferCallback = [this](void* arg){
        (void)arg;
        gpio_put(m_csPin, false);
    };
    transferPacket.AfterTransferCallback = [this](void* arg){
        (void)arg;
        gpio_put(m_csPin, true);
        xSemaphoreGive(m_transferMutex);
    };
    {
        [[maybe_unused]] const bool ret = xSemaphoreTake(m_transferMutex, portMAX_DELAY);
        assert(ret == pdTrue);
    }
    {
        [[maybe_unused]] const bool ret = xQueueSend(m_spiDriverQueue, &transferPacket, portMAX_DELAY);
        assert(ret == pdTrue);
    }
    /* block until transaction is finished */
    {
        [[maybe_unused]] const bool ret = xSemaphoreTake(m_transferMutex, portMAX_DELAY);
        assert(ret == pdTrue);
        xSemaphoreGive(m_transferMutex);
    }
    uint16_t x, y;
    /* x */
    {
        uint16_t rawAdcVal;
        rawAdcVal = (uint16_t)receivedData[1];
        rawAdcVal <<= 8;
        rawAdcVal |= (uint16_t)receivedData[2];
        rawAdcVal >>= 3;
        x = rawAdcVal;
    }
    /* y */
    {
        uint16_t rawAdcVal;
        rawAdcVal = (uint16_t)receivedData[4];
        rawAdcVal <<= 8;
        rawAdcVal |= (uint16_t)receivedData[5];
        rawAdcVal >>= 3;
        y = rawAdcVal;
    }
    return std::make_pair(x, y);
}
