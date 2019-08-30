/*
 * UART.h
 *
 *  Created on: Aug 10, 2019
 *      Author: Mynam
 */

#pragma once

#include <msp430.h>

namespace SatLib
{
    typedef struct
    {
        volatile unsigned char * PxSEL0;
        volatile unsigned char * PxSEL1;
        unsigned int PxSEL0BITS;
        unsigned int PxSEL1BITS;

        volatile unsigned int * UCAxTXBUF;
        volatile unsigned int * UCAxRXBUF;
        volatile unsigned int * UCAxMCTLW;
        volatile unsigned int * UCAxCTLW0;
        volatile unsigned int * UCAxBRW;
        volatile unsigned int * UCAxIE;
        volatile unsigned int * UCAxIFG;
        volatile unsigned int * UCAxIV;
    }eUSCI_A_Type;

    extern const eUSCI_A_Type eUSCI_A_Modules[];
}
