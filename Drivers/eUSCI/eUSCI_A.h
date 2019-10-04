/*
 * UART.h
 *
 *  Created on: Aug 10, 2019
 *      Author: Mynam
 */

#pragma once

#include <msp430.h>
#include <stdint.h>

namespace SatLib
{
    typedef struct
    {
        // pin select registers so we can mux pins to the appropriate port
        volatile unsigned char * PxSEL0;
        volatile unsigned char * PxSEL1;

        // values for those registers so they can be used by the module for UART
        unsigned int PxSEL0BITS;
        unsigned int PxSEL1BITS;

        // values for pin select registers so the pins can be used by the module for SPI
        unsigned int PxSEL0BITS_SPI;
        unsigned int PxSEL1BITS_SPI;

        // The eUSCI_A module registers
        volatile unsigned int * UCAxTXBUF;
        volatile unsigned int * UCAxRXBUF;
        volatile unsigned int * UCAxMCTLW;
        volatile unsigned int * UCAxCTLW0;
        volatile unsigned int * UCAxBRW;
        volatile unsigned int * UCAxIE;
        volatile unsigned int * UCAxIFG;
        volatile unsigned int * UCAxIV;

        // The DMA channels associated with the module
        int8_t DMAChannel0;
        int8_t DMAChannel1;
        int8_t DMAChannel2;
        int8_t DMAChannel3;

        // The DMA channels selection values. Found in Table 6-11 DMA Trigger Assignments in the datasheet
        unsigned int DMAChannel0TxVal;
        unsigned int DMAChannel0RxVal;

        unsigned int DMAChannel1TxVal;
        unsigned int DMAChannel1RxVal;

        unsigned int DMAChannel2TxVal;
        unsigned int DMAChannel2RxVal;

        unsigned int DMAChannel3TxVal;
        unsigned int DMAChannel3RxVal;
    }eUSCI_A_Type;

    extern eUSCI_A_Type eUSCI_A_Modules[];
}
