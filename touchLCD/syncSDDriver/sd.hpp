#pragma once

#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <utility>

/*
    no MMC support 
    block size fixed to val 512
*/
class CSD
{
    using pinType = unsigned int;
public:
    enum class ESDType
    {
        UNKNOWN = 0,
        SD1 = 1,
        SD2_BYTE_ADDR = 2,
        SD2_BLOCK_ADDR = 3,
    };
    CSD(spi_inst_t* spi, const pinType mosi, const pinType miso, const pinType sck, const pinType cs);
    ~CSD() = default;
    bool ReadBlock(const size_t blockNum, uint8_t* const dataPtr) const;
    bool WriteBlock(const size_t blockNum, const uint8_t* const dataPtr) const;
    ESDType GetSDType() const
    {
        return m_sdType;
    }
private:
    ESDType Init() const;
    /* 
        args - 4 bytes always 
        returns NCR and busyTime measured in bytes
        pair<NCR, busy>>
    */
    std::pair<size_t, size_t> SendCommand(const uint8_t command, const uint8_t* args, const uint8_t crc,
        const size_t expectedResponseSize, uint8_t* response, bool handleCS = true) const;
    /*
        returns busyTime measured in bytes
    */
    size_t WaitForBusyEnd(bool handleCS) const;
    /* buffer must be big enough */
    uint8_t* AddressToCommandArgs(const size_t addr, uint8_t* const buffer) const;
    void HandleCS(bool desiredState) const;

    spi_inst_t* const m_spi;
    const pinType m_cs;
    bool m_byteAddress = false;
    mutable ESDType m_sdType = ESDType::UNKNOWN;

    static constexpr uint8_t s_dummyCrc = 0xff;
    static constexpr size_t s_initDummyBytesCount = 10;
    static constexpr unsigned int s_targetSpiBaudrate = 20'000'000;
};