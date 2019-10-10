/*
 * SPI.cpp
 *
 *  Created on: Sep 20, 2019
 *      Author: David Fiumano
 */

#include "../eUSCI/SPI.h"
#include "../Memory/DMA.h"

extern SatLib::eUSCI_A_Type eUSCI_A_Modules[];
extern SatLib::DMA_Channel_type DMA_Channels[];

SatLib::SPI::SPI(ModuleType type, uint8_t moduleNumber)
    : registersType(type)
{
    if(registersType == A)
        this->registers = (eUSCI_A_Type*)&eUSCI_A_Modules[moduleNumber];
}

SatLib::SPI::TransactionError SatLib::SPI::acquireRxChannel(SPI_Settings settings, TickType_t delay)
{
    switch(settings.DMARxChannel)
    {
    case 0:
        if(registers->DMAChannel0 < 0)
            return UNSUPPORTED_DMA_SELECTION_RX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel0].mutex, delay) == pdPASS)
        {
            RxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel0];
            Rx_DMAxTSEL = registers->DMAChannel0RxVal;
            break;
        }else{
            return RX_DMA_NOT_FREE;
        }
    case 1:
        if(registers->DMAChannel1 < 0)
            return UNSUPPORTED_DMA_SELECTION_RX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel1].mutex, delay) == pdPASS)
        {
            RxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel1];
            Rx_DMAxTSEL = registers->DMAChannel1RxVal;
            break;
        }else{
            return RX_DMA_NOT_FREE;
        }
    case 2:
        if(registers->DMAChannel2 < 0)
            return UNSUPPORTED_DMA_SELECTION_RX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel2].mutex, delay) == pdPASS)
        {
            RxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel2];
            Rx_DMAxTSEL = registers->DMAChannel2RxVal;
            break;
        }else{
            return RX_DMA_NOT_FREE;
        }
    case 3:
        if(registers->DMAChannel3 < 0)
            return UNSUPPORTED_DMA_SELECTION_RX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel3].mutex, delay) == pdPASS)
        {
            RxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel3];
            Rx_DMAxTSEL = registers->DMAChannel3RxVal;
            break;
        }else{
            return RX_DMA_NOT_FREE;
        }
    default:
        return UNSUPPORTED_DMA_SELECTION_RX;
    }
    return NO_ERR;
}

SatLib::SPI::TransactionError SatLib::SPI::acquireTxChannel(SPI_Settings settings, TickType_t delay)
{
    switch(settings.DMATxChannel)
    {
    case 0:
        if(registers->DMAChannel0 < 0)
            return UNSUPPORTED_DMA_SELECTION_TX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel0].mutex, delay) == pdPASS)
        {
            TxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel0];
            Tx_DMAxTSEL = registers->DMAChannel0TxVal;
            break;
        }else{
            return TX_DMA_NOT_FREE;
        }
    case 1:
        if(registers->DMAChannel1 < 0)
            return UNSUPPORTED_DMA_SELECTION_TX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel1].mutex, delay) == pdPASS)
        {
            TxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel1];
            Tx_DMAxTSEL = registers->DMAChannel1TxVal;
            break;
        }else{
            return TX_DMA_NOT_FREE;
        }
    case 2:
        if(registers->DMAChannel2 < 0)
            return UNSUPPORTED_DMA_SELECTION_TX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel2].mutex, delay) == pdPASS)
        {
            TxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel2];
            Tx_DMAxTSEL = registers->DMAChannel2TxVal;
            break;
        }else{
            return TX_DMA_NOT_FREE;
        }
    case 3:
        if(registers->DMAChannel3 < 0)
            return UNSUPPORTED_DMA_SELECTION_TX;

        if(xSemaphoreTake(DMA_Channels[registers->DMAChannel3].mutex, delay) == pdPASS)
        {
            TxChannel = (DMA_Channel_type*)&DMA_Channels[registers->DMAChannel3];
            Tx_DMAxTSEL = registers->DMAChannel3TxVal;
            break;
        }else{
            return TX_DMA_NOT_FREE;
        }
    default:
        return UNSUPPORTED_DMA_SELECTION_TX;
    }

    return NO_ERR;
}

void SatLib::SPI::configurePins()
{
    *(registers->PxSEL0) &= ~(registers->PxSEL0BITS_SPI);
    *(registers->PxSEL0) |= (registers->PxSEL0BITS_SPI);

    *(registers->PxSEL1) &= ~(registers->PxSEL1BITS_SPI);
    *(registers->PxSEL1) |= (registers->PxSEL1BITS_SPI);

    PM5CTL0 &= ~(LOCKLPM5);
}

SatLib::SPI::TransactionError SatLib::SPI::beginTransaction(uint8_t* TxBuffer, size_t TxSize, uint8_t *RxBuffer, size_t RxSize, SPI_Settings settings, int16_t msToWait)
{
    TickType_t delay = msToWait < 0 ? portMAX_DELAY : pdMS_TO_TICKS(msToWait);

    if(settings.ClockDivisor < 0x2)
        return FREQ_TOO_HIGH;

    TransactionError check = this->acquireRxChannel(settings, delay);
    if(check != NO_ERR)
        return check;

    check = this->acquireTxChannel(settings, delay);
    if(check != NO_ERR)
        return check;

    configurePins();

    if(registersType == A)
    {
        *(registers->UCAxCTLW0) = UCSWRST;
        *(registers->UCAxCTLW0) |= settings.role | settings.order | settings.polarity | settings.phase | settings.clockSelect | settings.chSize | UCSYNC;
        *(registers->UCAxBRW) = settings.ClockDivisor;
        *(registers->UCAxCTLW0) &= ~(UCSWRST);
    }else{
        return UNSUPPORTED_FEATURE;
    }

    // store buffer sizes, DMAxSZ will always be <= the buffer sizes.
    this->TxBufferSize = TxSize;
    this->RxBufferSize = RxSize;

    this->TxBuffer = TxBuffer;
    this->RxBuffer = RxBuffer;

    return NO_ERR;
}

bool SatLib::SPI::write(uint8_t byte)
{
    if(TxBufferIterator + 1 >= TxBufferSize)
        return false;

    TxBuffer[TxBufferIterator++] = byte;
    TransactionByteSize++;
    return true;
}

size_t SatLib::SPI::write(uint8_t * bytes, size_t size)
{
    size_t ret = 0;
    while(ret < size)
    {
       //while (!(*(registers->UCAxIFG) & UCTXIFG));
       //*(registers->UCAxTXBUF) = bytes[ret];
       this->TxBuffer[this->TransactionByteSize + ret] = bytes[ret];
       ret++;
    }

    this->TransactionByteSize += ret;

    return ret;
}

uint8_t SatLib::SPI::read()
{
    return RxBuffer[RxBufferIterator++];
}

size_t SatLib::SPI::read(uint8_t * bytes, size_t bytesToRead)
{
    size_t ret = 0;
    while(RxBufferIterator < RxBufferSize)
    {
        bytes[ret] = RxBuffer[RxBufferIterator++];
        ret++;
    }
    return ret;
}

void SatLib::SPI::transfer()
{

    *(registers->UCAxIFG) &= ~UCTXIFG; // clear this bit if has been set.

    // set DMA source addresses
    __data20_write_long((uintptr_t)(TxChannel->DMAxSA), (uintptr_t)TxBuffer);
    __data20_write_long((uintptr_t)(RxChannel->DMAxSA), (uintptr_t)(registers->UCAxRXBUF));

    // set DMA destination addresses
    __data20_write_long((uintptr_t)(TxChannel->DMAxDA), (uintptr_t)(registers->UCAxTXBUF));
    __data20_write_long((uintptr_t)(RxChannel->DMAxDA), (uintptr_t)RxBuffer);

    *(TxChannel->DMAxSZ) = this->TransactionByteSize;
    *(RxChannel->DMAxSZ) = this->TransactionByteSize;

    *(TxChannel->DMACTLx) |= Tx_DMAxTSEL;
    *(RxChannel->DMACTLx) |= Rx_DMAxTSEL;

    // configure DMA channels. Tx channel increments only the source address, Rx channel increments only the destination address
    *(TxChannel->DMAxCTL) = DMADT_0 | DMASRCINCR_3 | DMADSTINCR_0 | DMADSTBYTE | DMASRCBYTE | DMALEVEL | DMAEN | DMAIE;
    *(RxChannel->DMAxCTL) = DMADT_0 | DMASRCINCR_0 | DMADSTINCR_3 | DMADSTBYTE | DMASRCBYTE | DMALEVEL | DMAEN | DMAIE;

    xSemaphoreTake(TxChannel->finished, 0); // Guaranteed to be taken so no need to wait.
    xSemaphoreTake(RxChannel->finished, 0); // Guaranteed to be taken so no need to wait.

    *(registers->UCAxIFG) |= UCTXIFG;

    this->TransactionByteSize = 0; // reset this to zero for the next transfer.
}

void SatLib::SPI::endTransaction()
{
    // TODO check if the transaction is finished. If not, wait.
    xSemaphoreTake(this->TxChannel->finished, portMAX_DELAY); // should work as soon as the DMA transaction finishes.
    xSemaphoreTake(this->RxChannel->finished, portMAX_DELAY);
    if((*(registers->UCAxCTLW0) & UCMST) == UCMST) // if configured as master
    {
        // set size to zero (prevents erroneous DMA transfers while the other writes occur)
        *(TxChannel->DMAxSZ) = 0;
        *(RxChannel->DMAxSZ) = 0;

        DMACTL2 &= ~(Tx_DMAxTSEL | Rx_DMAxTSEL); // remove the triggers

        // set DMAxCTL to default values
        *(TxChannel->DMAxCTL) = 0;
        *(RxChannel->DMAxCTL) = 0;

        // reset registers
        *(registers->UCAxCTLW0) |= UCSWRST;
        *(registers->UCAxCTLW0) = 0;
        *(registers->UCAxBRW) = 0;

        // return DMA channels to the available pool
        xSemaphoreGive(TxChannel->mutex);
        xSemaphoreGive(RxChannel->mutex);

        // if this is running, the Tx and Rx Channels have finished sending.
        xSemaphoreGive(this->TxChannel->finished);
        xSemaphoreGive(this->RxChannel->finished);

        // unconfigure pins
        *(registers->PxSEL0) &= ~(registers->PxSEL0BITS_SPI);
        *(registers->PxSEL1) &= ~(registers->PxSEL1BITS_SPI);

        // reset channel references
        RxChannel = NULL;
        TxChannel = NULL;

        // reset buffers
        TxBuffer = NULL;
        RxBuffer = NULL;

        // reset helper variables
        TxBufferSize = 0;
        RxBufferSize = 0;

        // reset the transaction size
        TransactionByteSize = 0;
    }
}
