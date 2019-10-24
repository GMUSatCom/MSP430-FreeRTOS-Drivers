//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  Texas Instruments, Inc
//  July 2013
//***************************************************************************************

#include <msp430.h>
#include <FreeRTOS.h> // required for task.h
#include <task.h> // xTaskCreateStatic
#include <eUSCI/UART.h> // UART class

uint8_t TxBufferInternal[64]; // internal memory for the Tx RingBuffer
uint8_t RxBufferInternal[64]; // internal memory for the Rx RingBuffer

SatLib::RingBuffer<uint8_t> TxBuffer(TxBufferInternal, (size_t)64); // Internal Tx Data strucutre for Serial
SatLib::RingBuffer<uint8_t> RxBuffer(RxBufferInternal, (size_t)64); // Internal Rx Data structure for Serial

SatLib::UART Serial(0, &TxBuffer, &RxBuffer); // Serial object, controls eUSCI_A 0 and uses TxBuffer and RxBuffer for memory

TaskHandle_t echoTask; // task handle for echo task, used to control the echo task
StaticTask_t echoTaskControlBuffer; // memory for task control
StackType_t echoTaskStack[150]; // stack for echoTask

void echoTaskCode(void* pvparams); // echo task function

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    CSCTL0_H = CSKEY_H; // unlock clock control registers
    CSCTL1 = DCOFSEL_3 /*| DCORSEL*/; // DCO to 4 MHz (max without waitstates for FRAM is 8 MHz. Table 5-6. DCO)
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // set ACLK to VLOCLK, and SMCLK and MCLK to DCOCLK. According to Table 3-6. CSCTL2 Register description (Family User's Guide)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // set all to divide by 1
    CSCTL0_H = 0; // re-lock the clock controls.

    P1DIR |= BIT0 | BIT1; // set P1.0 and P1.1 to outputs
    P1OUT &= ~(BIT0 | BIT1);
    PM5CTL0 &= ~LOCKLPM5;

    echoTask = xTaskCreateStatic(echoTaskCode, // function pointer
                                 "echoTask", // name (useful for debugging, but can be left blank or null)
                                 150, // 150 words of stack space (300 bytes or so). This is very much overkill
                                 (void*)0, // arguments to send to the task function. Ignored so we just cast 0 to a void pointer
                                 tskIDLE_PRIORITY + 2, // priority of the task relative to the idle task priority
                                 echoTaskStack, // stack reference
                                 &echoTaskControlBuffer); // used by our code and the OS to control this task


    vTaskStartScheduler(); // start running tasks and task scheduler.
}

uint8_t receivedBytes[64]; // bytes received over UART

void echoTaskCode(void* pvparams)
{
    if(Serial.begin(9600, UCSSEL__SMCLK, 4000000) == SatLib::UART::UART_ERROR::NO_ERR) // begin the Serial port and check output to ensure success
    {
        for(;;)
        {
            P1OUT |= BIT1; // set green LED solid.
            if(Serial.waitOnRx() == SatLib::UART::UART_ERROR::NO_ERR) // wait indefinitely for UART input.
            {
               size_t bytesRead = Serial.read(receivedBytes, 64); // read bytes received from UART
               Serial.write(receivedBytes, bytesRead); // echo them back!
               Serial.waitOnTx(); // optionally, block until the write is complete.
            }else{
                // This fails only if there is another task waiting for input.
                // If a timeout was specified, it may also fail when the timeout fails.
                P1OUT ^= BIT1; // blink green LED in case of failure here.
                vTaskDelay(pdMS_TO_TICKS(1000)); // block for one second
            }
        }
    }else{
        // May block if settings are invalid (i.e., baudrate is too high) or if there
        P1OUT ^= BIT0; // blink RED LED
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// eUSCI_A0 vector must call the appropriate IRQ Handler function
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    Serial.IRQHandler(); // call the IRQHandler to enable sending and receiving.
}
