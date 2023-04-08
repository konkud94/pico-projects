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

bool CSpiDmaDriver::PerformTransferBlocking(const CTransferPacket& packet) const
{
    const bool writeTransfer = (packet.TransferType == ETransferType::WRITE || 
        packet.TransferType == ETransferType::READnWRITE ) && packet.Source != nullptr;
    const bool readTransfer = (packet.TransferType == ETransferType::READ || 
        packet.TransferType == ETransferType::READnWRITE ) && packet.Destination != nullptr;
    spi_set_baudrate(m_spi, packet.SpiBaudrate);
    const bool faultyPacket = (!writeTransfer && !readTransfer) || packet.TransferLengthBytes == 0;
    if(faultyPacket)
    {
        return false;
    }
    if(packet.BeforeTransferCallback)
    {
        packet.BeforeTransferCallback(packet.BeforeCallbackArg);
    }
    uint32_t dmaChannelMask = 0;
    if(writeTransfer)
    {
        dma_channel_config config = dma_channel_get_default_config(m_dmaChannelTx);
        dma_channel_configure(m_dmaChannelTx, &config,
                            &spi_get_hw(m_spi)->dr,
                            packet.Source,
                            packet.TransferLengthBytes,
                            false);
        dmaChannelMask |= (1u << m_dmaChannelTx);
    }
    if(readTransfer)
    {
        dma_channel_config config = dma_channel_get_default_config(m_dmaChannelRx);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, spi_get_dreq(m_spi, false));
        channel_config_set_read_increment(&config, false);
        channel_config_set_write_increment(&config, true);
        channel_config_set_transfer_data_size(&config, DMA_SIZE_8);
        channel_config_set_dreq(&config, spi_get_dreq(m_spi, true));
        dma_channel_configure(m_dmaChannelRx, &config,
                    packet.Destination,
                    &spi_get_hw(m_spi)->dr,
                    packet.TransferLengthBytes,
                    false);
        dmaChannelMask |= (1u << m_dmaChannelRx);
    }
    dma_start_channel_mask(dmaChannelMask);
    while(dma_channel_is_busy(m_dmaChannelRx) || dma_channel_is_busy(m_dmaChannelTx))
    {
        static constexpr unsigned int sleepMS = 10;
        vTaskDelay(sleepMS / portTICK_PERIOD_MS);
    }
    if(packet.AfterTransferCallback)
    {
        packet.AfterTransferCallback(packet.AfterCallbackArg);   
    }
    return true;
}
