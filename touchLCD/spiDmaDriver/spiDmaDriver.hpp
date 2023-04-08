#pragma once

#include <limits>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
#include <functional>

 /* sort of forward declaration, seems like in C it is called opaque type */
typedef struct spi_inst spi_inst_t;

class CSpiDmaDriver
{
    using pinType = unsigned int;
public:
    enum class ETransferType
    {
        WRITE = 0,
        READ = 1,
        READnWRITE = 2,
    };
    struct CTransferPacket
    {
        using CallbackType = std::function<void(void*)>;
        CTransferPacket(ETransferType transferType, unsigned int spiBaudrate, size_t transferLengthBytes, const uint8_t* source, 
            uint8_t* destination, CallbackType beforeTransferCallback, void* beforeCallbackArg, CallbackType afterTransferCallback, void* afterCallbackArg)
            : TransferType(transferType), SpiBaudrate(spiBaudrate), TransferLengthBytes(transferLengthBytes), Source(source),
            Destination(destination), BeforeTransferCallback(beforeTransferCallback), BeforeCallbackArg(beforeCallbackArg),
            AfterTransferCallback(afterTransferCallback), AfterCallbackArg(afterCallbackArg)
        {
            ;
        }
        const ETransferType TransferType;
        const unsigned int SpiBaudrate;
        const size_t TransferLengthBytes;
        const uint8_t* const Source;
        uint8_t* const Destination;
        const CallbackType BeforeTransferCallback;
        void* const BeforeCallbackArg;
        const CallbackType AfterTransferCallback;
        void* const AfterCallbackArg;
    };

    CSpiDmaDriver(spi_inst_t* spi, const pinType mosi, const pinType miso, const pinType sck, const unsigned int dmaChannelTx, 
        const unsigned int dmaChannelRx);
    ~CSpiDmaDriver() = delete;
    CSpiDmaDriver(const CSpiDmaDriver&) = delete;
    CSpiDmaDriver& operator=(const CSpiDmaDriver&) = delete;

    bool PerformTransferBlocking(const CTransferPacket& packet) const;
private:
    spi_inst_t* const m_spi;
    const unsigned int m_dmaChannelTx;
    const unsigned int m_dmaChannelRx;
};