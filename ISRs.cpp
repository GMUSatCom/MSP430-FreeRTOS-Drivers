/*
 * ISRs.c
 *
 *  Created on: Aug 4, 2019
 *      Author: Mynam
 */

#include <msp430.h>
#include "Drivers/UART.h"

extern SatLib::UART backchannelUART;
extern SatLib::UART Serial1;
extern SatLib::UART Serial2;

extern "C"
{
    #pragma vector=EUSCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void)
    {
        backchannelUART.IRQHandler();
    }

    #pragma vector=EUSCI_A1_VECTOR
    __interrupt void USCI_A1_ISR(void)
    {
        Serial1.IRQHandler();
    }

    #pragma vector=EUSCI_A3_VECTOR
    __interrupt void USCI_A3_ISR(void)
    {
        Serial2.IRQHandler();
    }
}
