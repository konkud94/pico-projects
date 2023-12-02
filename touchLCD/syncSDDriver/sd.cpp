#include "sd.hpp"
#include "commandList.hpp"
#include "pico/stdlib.h"
#include "pico/stdio.h"
#include <assert.h>
#include <stdio.h>
/*
    TODO: add some timeout so that software dont stuck (wait for busy or NCR)
        when there is no SD card onboard
*/

CSD::CSD(spi_inst_t* spi, const pinType mosi, const pinType miso, const pinType sck, const pinType cs)
    :m_spi(spi), m_cs(cs)
{
    spi_init(spi, 100'000);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_init(m_cs);
    gpio_set_dir(m_cs, GPIO_OUT);
    gpio_put(m_cs, true);
    [[maybe_unused]]const auto sdType = Init();
    spi_set_baudrate(m_spi, s_targetSpiBaudrate);
}
CSD::ESDType CSD::Init() const
{
    /* 1 ms required */
    sleep_ms(10);
    for(size_t idx = 0; idx < s_initDummyBytesCount; idx++)
    {
        static constexpr uint8_t initVal = 0xff;
        spi_write_blocking(m_spi, &initVal, 1);
    }
    uint8_t commandsArray[4] = {0, 0, 0, 0};
    auto argsToArray = [commandsArray](uint8_t arg0, uint8_t arg1, uint8_t arg2, uint8_t arg3)mutable -> uint8_t*{
        commandsArray[0] = arg0;
        commandsArray[1] = arg1;
        commandsArray[2] = arg2;
        commandsArray[3] = arg3;
        return commandsArray;
    };
    {
        uint8_t r1Response;
        SendCommand(CCommandList::s_cmd0, argsToArray(0x00, 0x00, 0x00, 0x00), 0x95, 1, &r1Response);
        if(r1Response != 0x01)
        {
            return CSD::ESDType::UNKNOWN;
        }
    }
    enum class EGeneralSDType
    {
        SDV1 = 1, 
        SDV2 = 2,
    };
    EGeneralSDType expectedType = EGeneralSDType::SDV1;
    {
        uint8_t r7Response[5];
        SendCommand(CCommandList::s_cmd8, argsToArray(0x00, 0x00, 0x01, 0xaa), 0x87, 5, r7Response);
        const uint8_t r1Response = r7Response[0];
        if(r1Response != 0x01 && r1Response != 0x05)
        {
            return CSD::ESDType::UNKNOWN;
        }
        bool responseMatch = true;
        {
            if(r7Response[4] != 0xaa)
            {
                responseMatch = false;
            }
            const uint8_t maskedVal = r7Response[3] & 0x0f;
            if(maskedVal != 0x01)
            {
                responseMatch = false;
            }
        }
        if(r1Response == 0x01 && !responseMatch)
        {
            return CSD::ESDType::UNKNOWN;
        }
        if(r1Response == 0x01)
        {
            expectedType = EGeneralSDType::SDV2;
        }
        else
        {
            /* 0x05 */
            expectedType = EGeneralSDType::SDV1;
        }
    }
    if(expectedType == EGeneralSDType::SDV1)
    {
        uint8_t r1Response;
        do
        {
            SendCommand(CCommandList::s_cmd55, argsToArray(0x00, 0x00, 0x00, 0x00), s_dummyCrc, 1, &r1Response);
            assert(r1Response == 0x01);
            SendCommand(CCommandList::s_cmd41, argsToArray(0x00, 0x00, 0x00, 0x00), s_dummyCrc, 1, &r1Response);
            const bool correctResponse = (r1Response == 0x01 || r1Response == 0x00);
            if(!correctResponse)
            {
                // MMC
                return CSD::ESDType::UNKNOWN;
            }
        }
        while(r1Response == 0x01);
        SendCommand(CCommandList::s_cmd16, argsToArray(0x00, 0x00, 0x02, 0x00), s_dummyCrc, 1, &r1Response);
        if(r1Response != 0x00)
        {
            return CSD::ESDType::UNKNOWN;
        }
        m_sdType = CSD::ESDType::SD1;
        return CSD::ESDType::SD1;
    }
    else
    {
        uint8_t r1Response;
        do
        {
            SendCommand(CCommandList::s_cmd55, argsToArray(0x00, 0x00, 0x00, 0x00), s_dummyCrc, 1, &r1Response);
            assert(r1Response == 0x01);
            SendCommand(CCommandList::s_cmd41, argsToArray(0x40, 0x00, 0x00, 0x00), s_dummyCrc, 1, &r1Response);
            const bool correctResponse = (r1Response == 0x01 || r1Response == 0x00);
            if(!correctResponse)
            {
                // MMC
                return CSD::ESDType::UNKNOWN;
            }
        }
        while(r1Response == 0x01);
        uint8_t r3Response[5];
        SendCommand(CCommandList::s_cmd58, argsToArray(0x00, 0x00, 0x00, 0x00), s_dummyCrc, 5, r3Response);
        r1Response = r3Response[0];
        if(r1Response != 0x00)
        {
            return CSD::ESDType::UNKNOWN;
        }
        static constexpr uint8_t ccsBitMask = 0x40;
        if(r3Response[1] & ccsBitMask)
        {
            m_sdType = CSD::ESDType::SD2_BLOCK_ADDR;
            return CSD::ESDType::SD2_BLOCK_ADDR;
        }
        SendCommand(CCommandList::s_cmd16, argsToArray(0x00, 0x00, 0x02, 0x00), s_dummyCrc, 1, &r1Response);
        if(r1Response != 0x00)
        {
            return CSD::ESDType::UNKNOWN;
        }
        m_sdType = CSD::ESDType::SD2_BYTE_ADDR;
        return CSD::ESDType::SD2_BYTE_ADDR;
    }
}
bool CSD::ReadBlock(const size_t blockNum, uint8_t* const dataPtr) const
{
    if(m_sdType == ESDType::UNKNOWN)
    {
        return false;
    }
    const size_t readAddres = (m_sdType == ESDType::SD2_BLOCK_ADDR)? blockNum : blockNum * 512;
    printf("CSD::ReadBlock; readAddres = %u\n", readAddres);
    HandleCS(false);
    uint8_t argsBuffer[sizeof(readAddres)];
    uint8_t r1Response;
    const auto pair = SendCommand(CCommandList::s_cmd17, AddressToCommandArgs(readAddres, argsBuffer), s_dummyCrc, 1, &r1Response, false);
    printf("(cmd17) NCR = %u\n", pair.first);
    if(r1Response != 0x00)
    {
        printf("CSD::ReadBlock Failed \n");
        HandleCS(true);
        return false;
    }
    const uint8_t dataToken = 0xfe;
    static constexpr uint8_t dummySpiSendVal = 0xff;
    {
        uint8_t lastReceivedByte;
        size_t loopCounter = 0;
        static constexpr size_t loopLimit = 1000'000;
        do
        {   
            spi_write_read_blocking(m_spi, &dummySpiSendVal, &lastReceivedByte, 1);
            loopCounter++;
        }
        while(lastReceivedByte == 0xff && loopCounter < loopLimit);
        if(loopCounter >= loopLimit)
        {
            printf("CSD::ReadBlock Failed as loopCounter >= loopLimit; lastReceivedByte = %u\n", lastReceivedByte);
            HandleCS(true);
            return false;
        }
        if(lastReceivedByte != dataToken)
        {
            printf("CSD::ReadBlock Failed as error token was received = 0x%x at loopCounter = %u \n", lastReceivedByte, loopCounter);
            HandleCS(true);
            return false;
        }
        printf("(waitForDataToken) NCR = %u\n", loopCounter);
    }
    spi_read_blocking(m_spi, dummySpiSendVal, dataPtr, 512);
    /* read unused crc */
    uint8_t dummySpiSendValArray[2] = {dummySpiSendVal, dummySpiSendVal};
    spi_write_blocking(m_spi, dummySpiSendValArray, 2);
    HandleCS(true);
    return true;
}
bool CSD::WriteBlock(const size_t blockNum, const uint8_t* const dataPtr) const
{
    if(m_sdType == ESDType::UNKNOWN)
    {
        return false;
    }
    const size_t writeAddres = (m_sdType == ESDType::SD2_BLOCK_ADDR)? blockNum : blockNum * 512;
    printf("CSD::WriteBlock; writeAddres = %u\n", writeAddres);
    HandleCS(false);
    uint8_t argsBuffer[sizeof(writeAddres)];
    uint8_t r1Response;
    const auto pair = SendCommand(CCommandList::s_cmd24, AddressToCommandArgs(writeAddres, argsBuffer), s_dummyCrc, 1, &r1Response, false);
    printf("(cmd24) NCR = %u\n", pair.first);
    if(r1Response != 0x00)
    {
        printf("CSD::WriteBlock Failed \n");
        HandleCS(true);
        return false;
    }
    /* TODO: make sure one byte is enough */
    {
        static constexpr uint8_t additionalByte = 0xff;
        spi_write_blocking(m_spi, &additionalByte, 1);
    }
    {
        /* send data packet: 
            token, data, 2 x dummyCrc 
        */
       static constexpr uint8_t token = 0xfe;
       static constexpr uint8_t writeDummyCrc[2] = {s_dummyCrc, s_dummyCrc};
       spi_write_blocking(m_spi, &token, 1);
       spi_write_blocking(m_spi, dataPtr, 512);
       spi_write_blocking(m_spi, writeDummyCrc, 2);
    }
    {
        /* get data response */
        static constexpr uint8_t dummyByte = 0xff;
        uint8_t response;
        spi_write_read_blocking(m_spi, &dummyByte, &response, 1);
        response = response >> 1;
        response = response & 0b00000111;
        const bool failed = response != 0b00000010;
        if(failed)
        {
            const char* failReason = (response == 0b00000101? "crc error" : "write error");
            printf("CSD::WriteBlock Failed; reason = %s\n", failReason);
            HandleCS(true);
            return false;
        }
    }
    HandleCS(true);
    return true;
}

std::pair<size_t, size_t> CSD::SendCommand(const uint8_t command, const uint8_t* args, const uint8_t crc,
    const size_t expectedResponseSize, uint8_t* response, bool handleCS) const
{
    const size_t busyCount = WaitForBusyEnd(handleCS);
    uint8_t buffer[6] = {command, args[0], args[1], args[2], args[3], crc};
    if(handleCS)
    {
        HandleCS(false);
    }
    spi_write_blocking(m_spi, buffer, 6);
    /* wait NCR */
    size_t ncrWaitCount = 0;
    while(true)
    {
        static constexpr uint8_t sendVal = 0xff;
        uint8_t receiveVal;
        spi_write_read_blocking(m_spi, &sendVal, &receiveVal, 1);
        if(receiveVal != 0xff)
        {
            response[0] = receiveVal;
            break;
        }
        ncrWaitCount++;
    }
    for(size_t idx = 1; idx < expectedResponseSize; idx++)
    {
        static constexpr uint8_t sendVal = 0xff;
        spi_write_read_blocking(m_spi, &sendVal, &(response[idx]), 1);
    }
    if(handleCS)
    {
        HandleCS(true);
    }
    return std::make_pair(ncrWaitCount, busyCount);
}
size_t CSD::WaitForBusyEnd(bool handleCS) const
{
    if(handleCS)
    {
        HandleCS(false);
    }
    size_t bytesCount = 0;
    bool busy;
    do
    {
        static constexpr uint8_t sendVal = 0xff;
        uint8_t receiveVal;
        spi_write_read_blocking(m_spi, &sendVal, &receiveVal, 1);
        busy = (receiveVal == 0x00);
        bytesCount++;
    } while (busy);
    if(handleCS)
    {
        HandleCS(true);
    }
    return bytesCount;
}
uint8_t* CSD::AddressToCommandArgs(const size_t addr, uint8_t* const buffer) const
{
    static constexpr size_t addrSizeBytes = sizeof(addr);
    for(size_t idx = 0; idx < addrSizeBytes; idx++)
    {
        /* rp2040 is little endian */
        const uint8_t* ptr = (const uint8_t*)&addr;
        const size_t bufferIdx = addrSizeBytes - 1 - idx;
        buffer[bufferIdx] = ptr[idx];
    }
    return buffer;
}
void CSD::HandleCS(bool desiredState) const
{
    gpio_put(m_cs, desiredState);
    /* provide extra clocks for some reason when releasing spi */
    if(desiredState)
    {
        static constexpr size_t spiReleaseExtraBytesCount = 1;
        static constexpr uint8_t extraByte = 0xff;
        for(size_t idx  = 0 ; idx < spiReleaseExtraBytesCount; idx++)
        {
            spi_write_blocking(m_spi, &extraByte, 1);
        }
    }
}