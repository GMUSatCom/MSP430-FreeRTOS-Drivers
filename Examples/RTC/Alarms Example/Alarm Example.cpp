/*
 * This example sets several Alarms which all do different things.
 */

#include <msp430.h>
#include <FreeRTOS.h>
#include <task.h>

#include <eUSCI/UART.h>
#include <RTC/RTC.h>

#include <stdio.h>
#include <stdlib.h> // itoa

uint8_t TxBufferInternal[64]; // internal memory for the Tx RingBuffer
uint8_t RxBufferInternal[64]; // internal memory for the Rx RingBuffer

SatLib::RingBuffer<uint8_t> TxBuffer(TxBufferInternal, (size_t)64); // Internal Tx Data strucutre for Serial
SatLib::RingBuffer<uint8_t> RxBuffer(RxBufferInternal, (size_t)64); // Internal Rx Data structure for Serial

SatLib::UART Serial(0, &TxBuffer, &RxBuffer); // Serial object, controls eUSCI_A 0 and uses TxBuffer and RxBuffer for memory

TaskHandle_t clockTaskHandle;
StaticTask_t clockTCB;
StackType_t clockStack[300];

void clockTask(void* pvparams);

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    PJSEL0 |= BIT4 | BIT5; // configure LFXT pins

    PM5CTL0 &= ~LOCKLPM5; // ensure the above pin settings take effect.

    CSCTL0_H = CSKEY_H; // unlock clock control registers
    CSCTL1 = DCOFSEL_3 /*| DCORSEL*/; // DCO to 4 MHz (max without waitstates for FRAM is 8 MHz. Table 5-6. DCO)
    CSCTL2 = SELA__LFXTCLK | SELS__DCOCLK | SELM__DCOCLK; // set ACLK to VLOCLK, and SMCLK and MCLK to DCOCLK. According to Table 3-6. CSCTL2 Register description (Family User's Guide)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // set all to divide by 1
    CSCTL4 &= ~LFXTOFF; // enable the LFXT oscillator for the clock

    do // clear the fault flag until the oscillator is working
    {
        CSCTL5 &= ~LFXTOFF;
        SFRIFG1 &= ~OFIFG;
    }while(SFRIFG1 & OFIFG);
    CSCTL0_H = 0; // re-lock the clock controls.

    clockTaskHandle = xTaskCreateStatic(clockTask,
                                          "clock",
                                          300,
                                          (void*)1,
                                          tskIDLE_PRIORITY + 2,
                                          clockStack,
                                          &clockTCB);

    vTaskStartScheduler();
}

uint8_t DateTimeStr[30] = {0};

void clockTask(void* pvparams)
{
    __no_operation();
    SatLib::RTC::initRTC(); // initialize the RTC hardware. All of RTC's member functions are static, no object needs to be created.
    SatLib::RTC::setDate(TUESDAY, 31, 12, 2019); // set the date to October 31st, 2019
    SatLib::RTC::setTime(23, 59, 30); // set the time to 6:30 pm
    SatLib::RTC::startClock();

    Serial.begin(9600, UCSSEL__SMCLK, 4000000);

    for(;;)
    {
        uint8_t day = SatLib::RTC::getDay();
        uint8_t month = SatLib::RTC::getMonth();
        uint16_t year = SatLib::RTC::getYear();

        uint8_t hours = SatLib::RTC::getHours();
        uint8_t minutes = SatLib::RTC::getMinutes();
        uint8_t seconds = SatLib::RTC::getSeconds();

        snprintf((char*)DateTimeStr, 29, "%d/%d/%d %d:%d:%d\n", month, day, year, hours, minutes, seconds);

        Serial.write(DateTimeStr, strlen((char*)DateTimeStr));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// eUSCI_A0 vector must call the appropriate IRQ Handler function
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    Serial.IRQHandler(); // call the IRQHandler to enable sending and receiving.
}
