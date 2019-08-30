/*
 * UART.cpp
 *
 *  Created on: Aug 10, 2019
 *      Author: Mynam
 */
#include "../UART.h"

using namespace SatLib;

//Baud Rate calculation:
//SourceClk/(16*desiredBaud)
//8000000/(16*115200) = 4.34
//Fractional Part = 0.083 so UCBRSx = 0x4 according to Table 30-4. USBRSx Settings for Fractional Portion of N=fBRCLK/Baud Rate (Family User's Guide)
//UCAxBRW = 52
//UCBRFx = int( (52.083-52)*16) = 5
// The steps here are from Section 30.3.10 of the Family User's Guide

struct
{
    double val;
    uint8_t UCBRSx;
}typedef UCBRSx_TableEntry;

#pragma PERSISTENT
static UCBRSx_TableEntry UCBRSx_Vals[] = {
                                             {0.0, 0x00}, {0.0529, 0x01}, {0.0715, 0x02}, {0.0835, 0x04},
                                             {0.1001, 0x08}, {0.1252, 0x10}, {0.1430, 0x20}, {0.1670, 0x11},
                                             {0.2147, 0x21}, {0.2224, 0x22}, {0.2503, 0x44}, {0.3000, 0x25},
                                             {0.3335, 0x49}, {0.3575, 0x4A}, {0.3753, 0x52}, {0.4003, 0x92},
                                             {0.4286, 0x53}, {0.4378, 0x55}, {0.5002, 0xAA}, {0.5715, 0x6B},
                                             {0.6003, 0xAD}, {0.6254, 0xB5}, {0.6432, 0xB6}, {0.6667, 0xD6},
                                             {0.7001, 0xB7}, {0.7147, 0xBB}, {0.7503, 0xDD}, {0.7861, 0xED},
                                             {0.8004, 0xEE}, {0.8333, 0xBF}, {0.8464, 0xDF}, {0.8572, 0xEF},
                                             {0.8751, 0xF7}, {0.9004, 0xFB}, {0.9170, 0xFD}, {0.9288, 0xFE}
                                          };

UART::UART(uint8_t module, unsigned int srcClkSelect, uint32_t srcClkHz)
{
     this->TxBufferHandle = xQueueCreateStatic(eUSCI_DEFAULT_BUF_SIZE,
                                                  sizeof(uint8_t),
                                                  this->TxBuf,
                                                  &this->TxBufferTCB);

     this->RxBufferHandle = xQueueCreateStatic(eUSCI_DEFAULT_BUF_SIZE,
                                                  sizeof(uint8_t),
                                                  this->RxBuf,
                                                  &this->RxBufferTCB);

     eUSCI_A_Type eUSCI_A_module = eUSCI_A_Modules[module];

     this->PxSEL0 = eUSCI_A_module.PxSEL0;
     this->PxSEL1 = eUSCI_A_module.PxSEL1;
     this->PxSEL0BITS = eUSCI_A_module.PxSEL0BITS;
     this->PxSEL1BITS = eUSCI_A_module.PxSEL1BITS;

     this->UCAxTXBUF = eUSCI_A_module.UCAxTXBUF;
     this->UCAxRXBUF = eUSCI_A_module.UCAxRXBUF;
     this->UCAxMCTLW = eUSCI_A_module.UCAxMCTLW;
     this->UCAxCTLW0 = eUSCI_A_module.UCAxCTLW0;
     this->UCAxIE = eUSCI_A_module.UCAxIE;
     this->UCAxBRW = eUSCI_A_module.UCAxBRW;
     this->UCAxIFG = eUSCI_A_module.UCAxIFG;
     this->UCAxIV = eUSCI_A_module.UCAxIV;
     this->srcClkSelect = srcClkSelect;

     this->sourceClkHz = srcClkHz;
}

UART::eUSCI_ERROR UART::begin(uint32_t baud)
{
    *PxSEL0 &= ~(PxSEL0BITS); // clear PxSEL0
    *PxSEL0 |= PxSEL0BITS; // set the PxSEL0BITS

    *PxSEL1 &= ~(PxSEL1BITS); // clear PxSEL1
    *PxSEL1 |= PxSEL1BITS; // set the PxSEL1BITS

    PM5CTL0 &= ~LOCKLPM5; // ensure GPIO changes take effect by disabling power-on default high-impedance mode

    this->baudrate = baud;

    return initUART(baud);
}

UART::eUSCI_ERROR SatLib::UART::initUART(uint32_t baud)
{
    UCAxBRW_Val = sourceClkHz/(16*baud);
    UCBRF_Val = int((((double)sourceClkHz/(16.0*((double)baud))) - ((double)UCAxBRW_Val)) * 16);

    switch(UCBRF_Val)
    {
    case 0:
        UCBRF_Val = UCBRF_0;
        break;
    case 1:
        UCBRF_Val = UCBRF_1;
        break;
    case 2:
        UCBRF_Val = UCBRF_2;
        break;
    case 3:
        UCBRF_Val = UCBRF_3;
        break;
    case 4:
        UCBRF_Val = UCBRF_4;
        break;
    case 5:
        UCBRF_Val = UCBRF_5;
        break;
    case 6:
        UCBRF_Val = UCBRF_6;
        break;
    case 7:
        UCBRF_Val = UCBRF_7;
        break;
    case 8:
        UCBRF_Val = UCBRF_8;
        break;
    case 9:
        UCBRF_Val = UCBRF_9;
        break;
    case 0xA:
        UCBRF_Val = UCBRF_10;
        break;
    case 0xB:
        UCBRF_Val = UCBRF_11;
        break;
    case 0xC:
        UCBRF_Val = UCBRF_12;
        break;
    case 0xD:
        UCBRF_Val = UCBRF_13;
        break;
    case 0xE:
        UCBRF_Val = UCBRF_14;
        break;
    case 0xF:
        UCBRF_Val = UCBRF_15;
        break;
    default:
        this->mode = NONE;
        return eUSCI_UART_CONFIG_ERR;
    }

    lookup = (((double)sourceClkHz)/((double)baudrate));
    lookup = lookup - ((int)lookup);
    UCBRS_mask = -1;

    uint8_t i = 0;
    for(i = 0; i < 36; i++)
    {
        if(lookup > UCBRSx_Vals[i].val)
            UCBRS_mask = UCBRSx_Vals[i].UCBRSx; // set the mask appropriately
    }

    if(UCBRS_mask == -1)
        UCBRS_mask = 0xFE;

    *UCAxCTLW0 = UCSWRST;
    *UCAxCTLW0 |= srcClkSelect; // select the source clock.

    *UCAxBRW = UCAxBRW_Val; // set baudrate calculation
    *UCAxMCTLW = UCOS16 | UCBRF_Val | (UCBRS_mask << 8);
    *UCAxCTLW0 &= ~(UCSWRST);

    *UCAxIE = UCRXIE | UCTXIE;

    return NO_ERR;
}

UART::eUSCI_ERROR UART::write(uint8_t * buf, size_t size, bool block, int16_t msToWait)
{
    BaseType_t err = pdPASS;
    size_t it = 0;
    for(it = 0; it < size && (err == pdPASS); it++)
       err = xQueueSendToBack(this->TxBufferHandle, (void*)&buf[it], 1); // if the queue is full, it may be appropriate to wait a tick

    if(uxQueueMessagesWaiting(this->TxBufferHandle) != size) // if there is not already a write going on
        xQueueReceive(this->TxBufferHandle, (void*)UCAxTXBUF, 0); // write the first character to enable sending.

    return err == pdPASS ? NO_ERR : BUF_FULL_ERR;
}

UART::eUSCI_ERROR UART::write(uint8_t c)
{
    if(xQueueSendToBack(this->TxBufferHandle, (void*)&c, 0) != pdPASS)
        return BUF_FULL_ERR;

    if(!((*UCAxIFG) & UCTXIFG)) // if there is not a character already being sent
    {
        xQueueReceiveFromISR(this->TxBufferHandle, (void*)UCAxTXBUF, NULL); // send the character.
    }

    return NO_ERR;
}

uint8_t UART::read()
{
    uint8_t ret = 0;
    if(xQueueReceiveFromISR(this->RxBufferHandle, (void*)&ret, NULL) != pdPASS)
        return 0;

    return ret;
}

size_t UART::read(uint8_t * buf, size_t size)
{
    BaseType_t err = pdPASS;
    size_t it = 0;
    for(it = 0; it < size && (err == pdPASS); it++)
        err = xQueueReceiveFromISR(this->RxBufferHandle, (void*)&buf[it], NULL);

    return it;
}

bool UART::waitOnRx(int32_t msToWait)
{
    uint8_t junk;
    if(msToWait < 0)
        return xQueuePeek(this->RxBufferHandle, (void*)&junk, portMAX_DELAY) == pdPASS;
    else
        return xQueuePeek(this->RxBufferHandle, (void*)&junk, pdMS_TO_TICKS(msToWait)) == pdPASS;
}
