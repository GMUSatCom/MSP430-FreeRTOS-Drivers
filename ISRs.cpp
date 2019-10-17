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
extern SatLib::UART GPSSerial;
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
        GPSSerial.IRQHandler();
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

    /*-----------------------------------------------------------*/

    /* The MSP430X port uses this callback function to configure its tick interrupt.
    This allows the application to choose the tick interrupt source.
    configTICK_VECTOR must also be set in FreeRTOSConfig.h to the correct
    interrupt vector for the chosen tick interrupt source.  This implementation of
    vApplicationSetupTimerInterrupt() generates the tick from timer A0, so in this
    case configTICK_VECTOR is set to TIMER0_A0_VECTOR. */
    void vApplicationSetupTimerInterrupt( void )
    {
    const unsigned short usACLK_Frequency_Hz = 32768;

        /* Ensure the timer is stopped. */
        TA0CTL = 0;

        /* Run the timer from the ACLK. */
        TA0CTL = TASSEL_1;

        /* Clear everything to start with. */
        TA0CTL |= TACLR;

        /* Set the compare match value according to the tick rate we want. */
        TA0CCR0 = usACLK_Frequency_Hz / configTICK_RATE_HZ;

        /* Enable the interrupts. */
        TA0CCTL0 = CCIE;

        /* Start up clean. */
        TA0CTL |= TACLR;

        /* Up mode. */
        TA0CTL |= MC_1;
    }

}
