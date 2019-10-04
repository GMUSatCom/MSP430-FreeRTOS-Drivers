/*
 * DMA.h
 *
 *  Created on: Sep 19, 2019
 *      Author: Mynam
 */

#pragma once

#include <msp430.h>
#include <FreeRTOS.h>
#include <semphr.h>

namespace SatLib
{
    typedef struct
    {
        volatile unsigned int * DMAxCTL;
        volatile unsigned int * DMAxSZ;
        volatile __SFR_FARPTR * DMAxSA;
        volatile __SFR_FARPTR * DMAxDA;
        StaticSemaphore_t * pxMutexBuffer;
        SemaphoreHandle_t mutex;
    }DMA_Channel_type;

    extern DMA_Channel_type DMA_Channels[];
}
