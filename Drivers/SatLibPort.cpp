/*
 * port.cpp
 *
 *  Created on: Aug 24, 2019
 *      Author: David Fiumano
 */

#include <string.h> // for NULL
#include "eUSCI/eUSCI_A.h"
#include "Memory/DMA.h"
#include "GPIO/GPIO.h"

namespace SatLib
{
    #pragma PERSISTENT
    eUSCI_A_Type eUSCI_A_Modules[] =
    {
     {&P2SEL0, &P2SEL1, 0, (BIT0 | BIT8), 0, (BIT0 | BIT8 | BIT2), &UCA0TXBUF, &UCA0RXBUF, &UCA0MCTLW, &UCA0CTLW0, &UCA0BRW, &UCA0IE, &UCA0IFG, &UCA0IV, 0, 1, 2, -1, DMA0TSEL_15, DMA0TSEL_14, DMA1TSEL_15, DMA1TSEL_14, DMA2TSEL_15, DMA2TSEL_14, 0, 0},
     {&P2SEL0, &P2SEL1, 0, (BIT5 | BIT6), 0, (BIT4 | BIT5 | BIT6), &UCA1TXBUF, &UCA1RXBUF, &UCA1MCTLW, &UCA1CTLW0, &UCA1BRW, &UCA1IE, &UCA1IFG, &UCA1IV, 0, 1, 2, -1, DMA0TSEL_17, DMA0TSEL_16, DMA1TSEL_17, DMA1TSEL_16, DMA2TSEL_17, DMA2TSEL_16, 0, 0},
     {&P5SEL0, &P5SEL1, (BIT4 | BIT5), 0, (BIT4 | BIT5 | BIT6), 0, &UCA2TXBUF, &UCA2RXBUF, &UCA2MCTLW, &UCA2CTLW0, &UCA2BRW, &UCA2IE, &UCA2IFG, &UCA2IV, 3, 4, 5, -1, DMA3TSEL_15, DMA3TSEL_14, DMA4TSEL_15, DMA4TSEL_14, DMA5TSEL_15, DMA5TSEL_14, 0, 0},
     {&P6SEL0, &P6SEL1, (BIT0 | BIT8), 0, (BIT0 | BIT8 | BIT2), 0, &UCA3TXBUF, &UCA3RXBUF, &UCA3MCTLW, &UCA3CTLW0, &UCA3BRW, &UCA3IE, &UCA3IFG, &UCA3IV, 3, 4, 5, -1, DMA3TSEL_17, DMA3TSEL_16, DMA4TSEL_17, DMA4TSEL_16, DMA5TSEL_17, DMA5TSEL_16, 0, 0}
    };

    // Contains all of the addresses of the GPIO registers for each port
    GPIO_Type GPIO_Ports[] =
    {
     {&P1IN, &P1OUT, &P1DIR, &P1REN, &P1SEL0, &P1SEL1, &P1SELC, &P1IES, &P1IE, &P1IFG},
     {&P2IN, &P2OUT, &P2DIR, &P2REN, &P2SEL0, &P2SEL1, &P2SELC, &P2IES, &P2IE, &P2IFG},
     {&P3IN, &P3OUT, &P3DIR, &P3REN, &P3SEL0, &P3SEL1, &P3SELC, &P3IES, &P3IE, &P3IFG},
     {&P4IN, &P4OUT, &P4DIR, &P4REN, &P4SEL0, &P4SEL1, &P4SELC, &P4IES, &P4IE, &P4IFG},
     {&P5IN, &P5OUT, &P5DIR, &P5REN, &P5SEL0, &P5SEL1, &P5SELC, &P5IES, &P5IE, &P5IFG},
     {&P6IN, &P6OUT, &P6DIR, &P6REN, &P6SEL0, &P6SEL1, &P6SELC, &P6IES, &P6IE, &P6IFG},
     {&P7IN, &P7OUT, &P7DIR, &P7REN, &P7SEL0, &P7SEL1, &P7SELC, &P7IES, &P7IE, &P7IFG},
     {&P8IN, &P8OUT, &P8DIR, &P8REN, &P8SEL0, &P8SEL1, &P8SELC, &P8IES, &P8IE, &P8IFG}
     //{&PJIN_L, &PJOUT_L, &PJDIR_L, &PJREN_L, &PJSEL0_L, &PJSEL1_L, &PJSELC_L},
     //{&PJIN_H, &PJOUT_H, &PJDIR_H, &PJREN_H, &PJSEL0_H, &PJSEL1_H, &PJSELC_H}
    };
    // only lower port of PJ ports working since available MSP430s for testing do not
    // use the upper 8 PJBITS currently only lower 8 bits &PJIN_L, &PJOUT_L ... are
    // confirmed to work

    // variable created so single number can be used for selecting pin in GPIO functions
    uint8_t GPIO_BITS[] = {BIT0,BIT1,BIT2,BIT3,BIT4,BIT5,BIT6,BIT7};

    StaticSemaphore_t DMA0MutexCB;
    StaticSemaphore_t DMA1MutexCB;
    StaticSemaphore_t DMA2MutexCB;
    StaticSemaphore_t DMA3MutexCB;
    StaticSemaphore_t DMA4MutexCB;
    StaticSemaphore_t DMA5MutexCB;

    StaticSemaphore_t DMA0FinishedCB;
    StaticSemaphore_t DMA1FinishedCB;
    StaticSemaphore_t DMA2FinishedCB;
    StaticSemaphore_t DMA3FinishedCB;
    StaticSemaphore_t DMA4FinishedCB;
    StaticSemaphore_t DMA5FinishedCB;

    #pragma PERSISTENT
    DMA_Channel_type DMA_Channels[] =
    {
     {&DMA0CTL, &DMA0SZ, &DMACTL0, &DMA0SA, &DMA0DA, &DMA0MutexCB, &DMA0FinishedCB, NULL, NULL},
     {&DMA1CTL, &DMA1SZ, &DMACTL0, &DMA1SA, &DMA1DA, &DMA1MutexCB, &DMA1FinishedCB, NULL, NULL},
     {&DMA2CTL, &DMA2SZ, &DMACTL1, &DMA2SA, &DMA2DA, &DMA2MutexCB, &DMA2FinishedCB, NULL, NULL},
     {&DMA3CTL, &DMA3SZ, &DMACTL1, &DMA3SA, &DMA3DA, &DMA3MutexCB, &DMA3FinishedCB, NULL, NULL},
     {&DMA4CTL, &DMA4SZ, &DMACTL2, &DMA4SA, &DMA4DA, &DMA4MutexCB, &DMA4FinishedCB, NULL, NULL},
     {&DMA5CTL, &DMA5SZ, &DMACTL2, &DMA5SA, &DMA5DA, &DMA5MutexCB, &DMA5FinishedCB, NULL, NULL}
    };
}
