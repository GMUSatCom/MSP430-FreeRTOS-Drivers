/*
 * UART.cpp
 *
 *  Created on: Aug 10, 2019
 *      Author: Mynam
 */
#include "../eUSCI/UART.h"

//Baud Rate calculation:
//SourceClk/(16*desiredBaud)
//Example:
//8000000/(16*115200) = 4.34
//Fractional Part = 0.34 so UCBRSx = 0x4 according to Table 30-4. USBRSx Settings for Fractional Portion of N=fBRCLK/Baud Rate (Family User's Guide)
//UCAxBRW = 4
//UCBRFx = int( (4.34-4)*16) = 5
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

SatLib::UART::UART(uint8_t module, RingBuffer<uint8_t> * TxBuffer, RingBuffer<uint8_t> * RxBuffer)
{
     eUSCI_A_Type eUSCI_A_module = eUSCI_A_Modules[module];

     // ugly and memory inefficient but saves a good amount of processing time because we don't need to jump to parts of a struct
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

     this->TxBuffer = TxBuffer;
     this->RxBuffer = RxBuffer;
     this->TxBuffer->reset();
     this->RxBuffer->reset();
}

SatLib::UART::UART_ERROR SatLib::UART::begin(uint32_t baud, unsigned int srcClkSelect, uint32_t srcClkHz)
{
    *PxSEL0 &= ~(PxSEL0BITS); // clear PxSEL0
    *PxSEL0 |= PxSEL0BITS; // set the PxSEL0BITS

    *PxSEL1 &= ~(PxSEL1BITS); // clear PxSEL1
    *PxSEL1 |= PxSEL1BITS; // set the PxSEL1BITS

    PM5CTL0 &= ~LOCKLPM5; // ensure GPIO changes take effect by disabling power-on default high-impedance mode

    this->baudrate = baud;

    return initUART(baud, srcClkSelect, srcClkHz);
}

SatLib::UART::UART_ERROR SatLib::UART::initUART(uint32_t baud, unsigned int srcClkSelect, uint32_t srcClkHz)
{
    UCAxBRW_Val = srcClkHz/(16*baud);
    UCBRF_Val = int((((double)srcClkHz/(16.0*((double)baud))) - ((double)UCAxBRW_Val)) * 16);

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
        return eUSCI_UART_CONFIG_ERR;
    }

    lookup = (((double)srcClkHz)/((double)baud));
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

    *UCAxIE = UCRXIE | UCTXIE; // Rx ISR enabled so I can store characters in the right buffers, Tx ISR enabled so I can transmit characters from a buffer

    return NO_ERR;
}

size_t SatLib::UART::write(uint8_t * buf, size_t size)
{
    size_t it = 0;
    while(!TxBuffer->full() && it < size)
    {
        taskENTER_CRITICAL();
        TxBuffer->put(buf[it]);
        taskEXIT_CRITICAL();
        it++;
    }

    if(!((*UCAxIV) & UCTXIFG))
        *UCAxTXBUF = TxBuffer->get();

    return it;
}

SatLib::UART::UART_ERROR SatLib::UART::write(uint8_t c)
{
    if(TxBuffer->full())
        return BUF_FULL_ERR;

    if(!((*UCAxIV) & UCTXIFG)){
        *UCAxTXBUF = c;
    }else{
        taskENTER_CRITICAL();
        TxBuffer->put(c);
        taskEXIT_CRITICAL();
    }


    return NO_ERR;
}

uint8_t SatLib::UART::read()
{
    taskENTER_CRITICAL();
    uint8_t ret = RxBuffer->get();
    taskEXIT_CRITICAL();
    return ret;
}

size_t SatLib::UART::read(uint8_t * buf, size_t size)
{
    size_t it = 0;
    while(!RxBuffer->empty() && it < size)
    {
        taskENTER_CRITICAL();
        buf[it] = RxBuffer->get();
        taskEXIT_CRITICAL();
        it++;
    }

    return it;
}

SatLib::UART::UART_ERROR SatLib::UART::waitOnRx(int32_t msToWait)
{
    if(!RxBuffer->empty())
        return NO_ERR;

    if(waitingTaskRx != NULL) // multiple tasks can no longer wait for the output at the same time.
        return TASK_ALREADY_WAITING;       // prevents this from causing tasks to block forever

    this->waitingTaskRx = xTaskGetCurrentTaskHandle(); // set the waiting task so the ISR knows who to notify
    BaseType_t ret;

    if(msToWait > 0) // if the user specified how many milliseconds to wait
        ret = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(msToWait)); // wait for msToWait milliseconds
    else // otherwise
        ret = ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // block indefinitely and let the MCU do other things while we wait for UART

    this->waitingTaskRx = NULL; // reset the waiting task

    if(ret != pdFALSE) // if a notification was received before the timeout expired
        return NO_ERR; // return NO_ERROR
    else // otherwise
        return TIMEOUT_ERR; // the timer expired
}

SatLib::UART::UART_ERROR SatLib::UART::waitOnTx(int32_t msToWait)
{
    if(TxBuffer->empty())
        return NO_ERR;

    if(waitingTaskTx != NULL)
        return TASK_ALREADY_WAITING;

    this->waitingTaskTx = xTaskGetCurrentTaskHandle();
    BaseType_t ret;

    if(msToWait > 0)
        ret = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(msToWait));
    else
        ret = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    waitingTaskTx = NULL;

    if(ret != pdFALSE)
        return NO_ERR;
    else
        return TIMEOUT_ERR;
}

void SatLib::UART::end()
{
    while(waitingTaskTx != NULL) // wait for any pending writes to finish
        vTaskDelay(1); // give UART time to finish writing and give the writing task time to be notified.

    while(waitingTaskRx != NULL) // avoid a situation where you cause some task to block forever by calling end.
        vTaskDelay(1); // give the task time to get the stuff from UART.

    *UCAxIE = 0; // prevent erroneous reads or sends during this time.

    // unassign GPIO from the module.
    *PxSEL0 &= ~(PxSEL0BITS); // clear PxSEL0
    *PxSEL1 &= ~(PxSEL1BITS); // clear PxSEL1
    PM5CTL0 &= ~LOCKLPM5; // ensure GPIO changes take effect

    this->baudrate = 0; // reset this variable since it's no longer valid.

    *UCAxCTLW0 = UCSWRST;
    *UCAxBRW = 0;
    *UCAxMCTLW = 0;
    unsigned int dummy = *UCAxRXBUF; // read UCAxRXBUF to clear interrupt.
    *UCAxCTLW0 = 0;

    RxBuffer->reset();
    TxBuffer->reset();
}
