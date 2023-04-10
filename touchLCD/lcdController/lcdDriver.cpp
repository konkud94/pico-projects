#include <limits>
#include <assert.h>
#include "lcdDriver.hpp"
#include "hardware/gpio.h"


CLcdDriver::CLcdDriver(QueueHandle_t spiDriverQueue, pinType csPin, pinType lcdRstPin, pinType lcdDcPin)
    : m_spiDriverQueue(spiDriverQueue), m_csPin(csPin), m_lcdRstPin(lcdRstPin), m_lcdDcPin(lcdDcPin)
{
    for(const auto pin : {csPin, lcdRstPin, lcdDcPin})
    {
        gpio_init(pin);
		gpio_set_dir(pin, GPIO_OUT);
		gpio_put(pin, true);
    }
    m_transferMutex = xSemaphoreCreateMutex();
    assert(m_transferMutex != nullptr);
    LcdResetBlocking();
    m_lcdId = (int16_t)ReadLcdId();
    SetUpRegisters();
	sleep_ms(200);
}

size_t CLcdDriver::FlushData(const uint8_t* data, size_t len) const
{
    static constexpr size_t lcdBitsCapacity = s_pixelsAlongX * s_pixelsAlongY * s_bitsPerPixel;
    static constexpr size_t lcdBytesCapacity = lcdBitsCapacity / 8;
    len = std::min(len, lcdBytesCapacity);
    if(len < 2)
    {
        assert(false);
        return 0;
    }
    {
        /* set frame pointers to x = 0, y = 0*/
        static constexpr uint8_t command = 0x2c;
        CTransferPacket transferPacket(CSpiDmaDriver::ETransferType::WRITE, std::numeric_limits<uint>::max(), 1, 
            &command, nullptr, nullptr, nullptr, nullptr, nullptr);
        HandleSpiTransfer(transferPacket, ESpiTransferType::COMMAND);
    }
    {
        CTransferPacket transferPacket(CSpiDmaDriver::ETransferType::WRITE, std::numeric_limits<uint>::max(), len, 
            data, nullptr, nullptr, nullptr, nullptr, nullptr);
        HandleSpiTransfer(transferPacket, ESpiTransferType::PARAMETER);
    }
    return len;
}
int16_t CLcdDriver::GetLcdId() const
{
    return m_lcdId;
}
void CLcdDriver::LcdResetBlocking() const
{
    const bool pinState = gpio_get(m_lcdRstPin);
    if(pinState)
    {
        gpio_put(m_lcdRstPin, false);
        sleep_ms(500);
    }
    gpio_put(m_lcdRstPin, true);
}
void CLcdDriver::HandleSpiTransfer(CTransferPacket& transferPacket, const ESpiTransferType type) const
{
    const bool dcPinDesiredState = type == ESpiTransferType::PARAMETER;
    const bool dcPinInitialState = gpio_get(m_lcdDcPin);
    transferPacket.BeforeCallbackArg = nullptr;
    transferPacket.AfterCallbackArg = nullptr;
    transferPacket.BeforeTransferCallback = [this, &dcPinDesiredState](void* arg){
        (void)arg;
        gpio_put(m_csPin, false);
        gpio_put(m_lcdDcPin, dcPinDesiredState);
    };
    transferPacket.AfterTransferCallback = [this, &dcPinInitialState](void* arg){
        (void)arg;
        gpio_put(m_lcdDcPin, dcPinInitialState);
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
}
void CLcdDriver::SetUpRegisters() const
{
    const size_t spiBaudrate = 4'000'000;
    {
        const uint8_t value = 0x11;
        CTransferPacket transferPacket(CSpiDmaDriver::ETransferType::WRITE, spiBaudrate, 1, 
            &value, nullptr, nullptr, nullptr, nullptr, nullptr);
        HandleSpiTransfer(transferPacket, ESpiTransferType::COMMAND);
    }
    static constexpr unsigned int sleepMS = 100;
    vTaskDelay(sleepMS / portTICK_PERIOD_MS);

    for(const auto& pair : s_initDeviceArray)
    {
        const auto transferType = pair.first;
        const uint8_t valueToSend = pair.second;
        CTransferPacket transferPacket(CSpiDmaDriver::ETransferType::WRITE, spiBaudrate, 1, 
            &valueToSend, nullptr, nullptr, nullptr, nullptr, nullptr);
        HandleSpiTransfer(transferPacket, transferType);
    }
}
uint8_t CLcdDriver::ReadLcdId() const
{
    static constexpr uint8_t reg = 0xdc;
    const uint8_t payload[2] = {reg, s_dummySpiWriteVal};
    uint8_t receivedData[2];
    CTransferPacket transferPacket(CSpiDmaDriver::ETransferType::READnWRITE, s_spiMaxReadSpeedHz, 2, 
            payload, receivedData, nullptr, nullptr, nullptr, nullptr);
    HandleSpiTransfer(transferPacket, ESpiTransferType::COMMAND);
    const uint8_t idVal = receivedData[1];
    return idVal;
}

const std::array<CLcdDriver::InitEntryType, CLcdDriver::s_initDeviceArrayCount> CLcdDriver::s_initDeviceArray{
    std::make_pair(ESpiTransferType::COMMAND, 0x36),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::COMMAND, 0x3a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x03),
    std::make_pair(ESpiTransferType::COMMAND, 0xb2),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x33),
    std::make_pair(ESpiTransferType::PARAMETER, 0x33),
    std::make_pair(ESpiTransferType::COMMAND, 0xb7),
    std::make_pair(ESpiTransferType::PARAMETER, 0x35),
    std::make_pair(ESpiTransferType::COMMAND, 0xbb),
    std::make_pair(ESpiTransferType::PARAMETER, 0x28),
    std::make_pair(ESpiTransferType::COMMAND, 0xc0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3c),
    std::make_pair(ESpiTransferType::COMMAND, 0xc2),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::COMMAND, 0xc3),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::COMMAND, 0xc4),
    std::make_pair(ESpiTransferType::PARAMETER, 0x20),
    std::make_pair(ESpiTransferType::COMMAND, 0xc6),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0f),
    std::make_pair(ESpiTransferType::COMMAND, 0xD0),
    std::make_pair(ESpiTransferType::PARAMETER, 0xa4),
    std::make_pair(ESpiTransferType::PARAMETER, 0xa1),
    std::make_pair(ESpiTransferType::COMMAND, 0xe0),
    std::make_pair(ESpiTransferType::PARAMETER, 0xd0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::PARAMETER, 0x08),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0f),
    std::make_pair(ESpiTransferType::PARAMETER, 0x11),
    std::make_pair(ESpiTransferType::PARAMETER, 0x2a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x36),
    std::make_pair(ESpiTransferType::PARAMETER, 0x55),
    std::make_pair(ESpiTransferType::PARAMETER, 0x44),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x06),
    std::make_pair(ESpiTransferType::PARAMETER, 0x11),
    std::make_pair(ESpiTransferType::PARAMETER, 0x20),
    std::make_pair(ESpiTransferType::COMMAND, 0xe1),
    std::make_pair(ESpiTransferType::PARAMETER, 0xd0),
    std::make_pair(ESpiTransferType::PARAMETER, 0x02),
    std::make_pair(ESpiTransferType::PARAMETER, 0x07),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x0b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x18),
    std::make_pair(ESpiTransferType::PARAMETER, 0x34),
    std::make_pair(ESpiTransferType::PARAMETER, 0x43),
    std::make_pair(ESpiTransferType::PARAMETER, 0x4a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x2b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1c),
    std::make_pair(ESpiTransferType::PARAMETER, 0x22),
    std::make_pair(ESpiTransferType::PARAMETER, 0x1f),
    std::make_pair(ESpiTransferType::COMMAND, 0x55),
    std::make_pair(ESpiTransferType::PARAMETER, 0xB0),
    std::make_pair(ESpiTransferType::COMMAND, 0x29),
    std::make_pair(ESpiTransferType::COMMAND, 0x2a),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0xef),
    std::make_pair(ESpiTransferType::COMMAND, 0x2b),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x00),
    std::make_pair(ESpiTransferType::PARAMETER, 0x01),
    std::make_pair(ESpiTransferType::PARAMETER, 0x3f),
};