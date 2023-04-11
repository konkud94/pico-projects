#include "spiDmaDriver.hpp"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

CSpiDmaDriver::CSpiDmaDriver(spi_inst_t* spi, const pinType mosi, const pinType miso, const pinType sck, const unsigned int dmaChannelTx,
    const unsigned int dmaChannelRx)
    :m_spi(spi), m_dmaChannelTx(dmaChannelTx), m_dmaChannelRx(dmaChannelRx)
{
    spi_init(m_spi, 4000000);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_set_function(sck, GPIO_FUNC_SPI);
}
bool CSpiDmaDriver::PerformTransferBlocking(const CTransferPacket& packet, const unsigned int sleepMS) const
{
    const bool faultyPacket = packet.TransferLengthBytes == 0 || packet.Source == nullptr || 
        (packet.TransferType == ETransferType::READnWRITE && packet.Destination == nullptr);
    if(faultyPacket)
    {
        /* 
            perform callback even though there is no spi transaction
                to release mutexes in calling threads
        */
        if(packet.BeforeTransferCallback)
        {
            packet.BeforeTransferCallback(packet.BeforeCallbackArg);        
        }
        if(packet.AfterTransferCallback)
        {
            packet.AfterTransferCallback(packet.AfterCallbackArg);        
        }
        return false;
    }
    spi_set_baudrate(m_spi, packet.SpiBaudrate);
    if(packet.BeforeTransferCallback)
    {
        packet.BeforeTransferCallback(packet.BeforeCallbackArg);        
    }
    {
        /* write channel */
        dma_channel_config config = dma_channel_get_default_config(m_dmaChannelTx);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, spi_get_dreq(m_spi, true));
        dma_channel_configure(m_dmaChannelTx, &config,
                            &spi_get_hw(m_spi)->dr,
                            packet.Source,
                            packet.TransferLengthBytes,
                            false);
    }
    {
        /*
            read channel
            even if transaction is of type 'Write' only
                launch m_dmaChannelRx to drain spi fifo from garbage data
        */
        const bool writeOnlyTransfer = packet.TransferType == ETransferType::WRITE;
        const bool writeAddressIncrement = !writeOnlyTransfer;
        uint8_t dummyDestination;
        uint8_t* writeToAddress = writeOnlyTransfer? &dummyDestination : packet.Destination;
        dma_channel_config config = dma_channel_get_default_config(m_dmaChannelRx);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, spi_get_dreq(m_spi, false));
        channel_config_set_read_increment(&config, false);
        channel_config_set_write_increment(&config, writeAddressIncrement);
        dma_channel_configure(m_dmaChannelRx, &config,
                    writeToAddress,
                    &spi_get_hw(m_spi)->dr,
                    packet.TransferLengthBytes,
                    false);
    }
    const uint32_t dmaChannelMask = (1u << m_dmaChannelTx) | (1u << m_dmaChannelRx);
    dma_start_channel_mask(dmaChannelMask);
    while(dma_channel_is_busy(m_dmaChannelRx) || dma_channel_is_busy(m_dmaChannelTx))
    {
        vTaskDelay(sleepMS / portTICK_PERIOD_MS);
    }
    __compiler_memory_barrier();
    /* spi will continue to consume data supplied by dma */
    while(spi_is_busy(m_spi))
    {
        vTaskDelay(0);        
    }
    if(packet.AfterTransferCallback)
    {
        packet.AfterTransferCallback(packet.AfterCallbackArg);  
    }
    return true;
}
