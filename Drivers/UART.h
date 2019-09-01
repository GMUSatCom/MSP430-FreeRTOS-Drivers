/*
 * UART.h
 *
 *  Created on: Aug 24, 2019
 *      Author: David Fiumano
 */

#pragma once

#include <msp430.h>
#include <stdint.h>
#include <cstring>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include "DriverConfig.h"
#include "eUSCI_A.h"

namespace SatLib
{
    class UART
    {
    public:
        enum MODE : uint8_t {NONE, SPI_MASTER, SPI_SLAVE, UART_MODE};
        enum eUSCI_ERROR : int8_t {NO_ERR = 0, BUF_FULL_ERR = -1, BUF_NULL_ERR = -2, eUSCI_NOT_SET_ERR = -3, eUSCI_UART_CONFIG_ERR = -4, UART_TX_MUTEX_NOT_FREE = -5};

    private:
        BaseType_t xHigherPriorityTaskWokenByRx = NULL;

        MODE mode = NONE;

        unsigned int srcClkSelect;
        uint32_t sourceClkHz = 0;

        uint32_t baudrate = 0;

        // references to the registers //
        volatile unsigned char * PxSEL0;
        volatile unsigned char * PxSEL1;
        unsigned int PxSEL0BITS;
        unsigned int PxSEL1BITS;

        volatile unsigned int * UCAxTXBUF;
        volatile unsigned int * UCAxRXBUF;
        volatile unsigned int * UCAxMCTLW;
        volatile unsigned int * UCAxCTLW0;
        volatile unsigned int * UCAxIE;
        volatile unsigned int * UCAxBRW;
        volatile unsigned int * UCAxIFG;
        volatile unsigned int * UCAxIV;

        QueueHandle_t TxBufferHandle;
        QueueHandle_t RxBufferHandle;

        StaticQueue_t TxBufferTCB;
        StaticQueue_t RxBufferTCB;

        uint8_t TxBuf[eUSCI_DEFAULT_BUF_SIZE];
        uint8_t RxBuf[eUSCI_DEFAULT_BUF_SIZE];

        int16_t UCBRS_mask;

        unsigned int UCAxBRW_Val;
        unsigned char UCBRF_Val;
        double lookup;

        eUSCI_ERROR initUART(uint32_t baud);

    public:

        UART(uint8_t module, unsigned int srcClkSelect = UCSSEL__SMCLK, uint32_t srcClkHz = 8000000);

        inline void IRQHandler() __attribute__((hot, flatten))
        {
            switch(__even_in_range((*UCAxIV), USCI_UART_UCTXCPTIFG))
            {
                case USCI_NONE: break; // no interrupt source
                case USCI_UART_UCRXIFG: // receive buffer full, read in the character so the next one can be written in.
                    xQueueSendToBackFromISR(this->RxBufferHandle, (void*)UCAxRXBUF, &xHigherPriorityTaskWokenByRx);
                    break;
                case USCI_UART_UCTXIFG:
                    if(xQueueIsQueueEmptyFromISR(this->TxBufferHandle) == pdFALSE)
                        xQueueReceiveFromISR(this->TxBufferHandle, (void*)UCAxTXBUF, NULL);
                    break; // transmit buffer full, write new character to buffer so the next one can get in
                case USCI_UART_UCSTTIFG: break; // start bit recieved
                case USCI_UART_UCTXCPTIFG: break; // transmit complete
                default: break;
            }
        }

        inline size_t available() // returns the number of bytes in the buffer
        {
            return (size_t)uxQueueMessagesWaitingFromISR(this->RxBufferHandle);
        }

        inline size_t txBytesWaiting()
        {
            return (size_t)uxQueueMessagesWaitingFromISR(this->TxBufferHandle);
        }

        eUSCI_ERROR write(uint8_t * buf, size_t size, bool block = true, int16_t msToWait = -1); // write to buffer

        eUSCI_ERROR write(uint8_t c); // write a single character

        uint8_t read(); // read a single character from the buffer. Returns 0 if there isn't one

        size_t read(uint8_t * buf, size_t size);

        /**
         * Wait on a single character to enter the queue for msToWait. If msToWait is negative, the delay is infinite.
         */
        bool waitOnRx(int32_t msToWait = -1);

        /**
         * Configured the pins and the eUSCI registers for transmission and reception.
         */
        virtual eUSCI_ERROR begin(uint32_t baud);
    };
}
