/*
 * ISRs.c
 *
 *  Created on: Aug 4, 2019
 *      Author: Mynam
 */

#include <msp430.h>
#include "Drivers/eUSCI/UART.h"
#include "Drivers/Memory/DMA.h"

extern SatLib::UART backchannelUART;
extern SatLib::DMA_Channel_type SatLib::DMA_Channels[];

extern "C"
{
    #pragma vector=EUSCI_A0_VECTOR
    __interrupt void USCI_A0_ISR(void)
    {
        backchannelUART.IRQHandler();
    }

    uint8_t i = 0xAA;
    #pragma vector=EUSCI_A1_VECTOR
    __interrupt void USCI_A1_ISR(void)
    {

    }

    #pragma vector=EUSCI_A3_VECTOR
    __interrupt void USCI_A3_ISR(void)
    {

    }

    BaseType_t xHigherPriorityTaskWokenByDMA;

    #pragma vector=DMA_VECTOR
    __interrupt void DMA_ISR(void)
    {
        switch(__even_in_range((DMAIV), DMAIV__DMA7IFG))
        {
        case DMAIV__DMA0IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[0].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA1IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[1].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA2IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[2].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA3IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[3].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA4IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[4].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA5IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[5].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA6IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[6].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV__DMA7IFG:
            xSemaphoreGiveFromISR(SatLib::DMA_Channels[7].finished, &xHigherPriorityTaskWokenByDMA);
            break;
        case DMAIV_NONE:
            __no_operation(); // will probably never run, but put this here for the debugger in case it does.
            break;
        default:
            __no_operation();
            break;
        }
        portYIELD_FROM_ISR(xHigherPriorityTaskWokenByDMA);
    }
}
