/*
 * SatLib.cpp
 *
 *  Created on: Sep 19, 2019
 *      Author: Mynam
 */

#include "SatLib.h"
#include "Memory/DMA.h"

void SatLib::initSatLib()
{
    // first, initialize DMA channels
    uint8_t channel = 0;
    for(channel = 0; channel < __MSP430_HAS_DMA__; channel++)
    {
        DMA_Channels[channel].mutex = xSemaphoreCreateMutexStatic(DMA_Channels[channel].pxMutexBuffer);
    }

    DMACTL4 = DMARMWDIS; // make DMA wait for current CPU instruction to finish for safety. Do not interrupt. Prevents read/writes from being interrupted by DMA.
}
