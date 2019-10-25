/*
 * This example takes input from the user over the backchannel UART on the MSP430 and dumps it over SPI.
 * It also
 */

#include <msp430.h> // P1 registers
#include <FreeRTOS.h> // needed for task.h
#include <task.h> // xTaskCreateStatic()
#include <SatLib.h> // init for the library
#include <eUSCI/SPI.h> // for SPI Class
#include <eUSCI/UART.h> // for UART class

uint8_t SPIRxBuffer[64];
uint8_t SPITxBuffer[64];

uint8_t SerialTxBuffer[64];
uint8_t SerialRxBuffer[64];

SatLib::RingBuffer<uint8_t> SerialTxStructure(SerialTxBuffer, 64);
SatLib::RingBuffer<uint8_t> SerialRxStructure(SerialRxBuffer, 64);

SatLib::SPI SPI(SatLib::SPI::ModuleType::A, 3); // create SPI port on eUSCI_A 3

SatLib::UART Serial(0, &SerialTxStructure, &SerialRxStructure); // create UART port on eUSCI_A 0

TaskHandle_t SPITaskHandle; // SPI Task handle (used for controlling tasks)
StaticTask_t SPITaskControlBuffer; // SPI TCB, used internally by the OS
StackType_t SPITaskStack[150]; // SPI Stack

SatLib::SPI::SPI_Settings settings;

void SPITask(void * pvparams);

void main(void) {
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    CSCTL0_H = CSKEY_H; // unlock clock control registers
    CSCTL1 = DCOFSEL_3 /*| DCORSEL*/; // DCO to 4 MHz (max without waitstates for FRAM is 8 MHz. Table 5-6. DCO)
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK; // set ACLK to VLOCLK, and SMCLK and MCLK to DCOCLK. According to Table 3-6. CSCTL2 Register description (Family User's Guide)
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1; // set all to divide by 1
    CSCTL0_H = 0; // re-lock the clock controls.

    P1DIR |= BIT0 | BIT1 | BIT3; // set P1.0, P1.1 & P1.3 to outputs
    P1OUT &= ~(BIT0 | BIT1 | BIT3); // make sure they aren't high when this starts
    PM5CTL0 &= ~LOCKLPM5;

    P1OUT |= BIT3; // set P1.3 high (chip select line)

    SPITaskHandle = xTaskCreateStatic(SPITask,
                                "SPITask",
                                150,
                                (void*)0,
                                tskIDLE_PRIORITY + 2,
                                SPITaskStack,
                                &SPITaskControlBuffer);

    SatLib::initSatLib(); // always call this before the scheduler is started

    vTaskStartScheduler();
}

uint8_t buffer[64];

void SPITask(void* pvparams)
{
    Serial.begin(9600, UCSSEL__SMCLK, 4000000);
    for(;;)
    {
        if(Serial.waitOnRx() == SatLib::UART::UART_ERROR::NO_ERR)
        {
            P1OUT ^= BIT0 | BIT1; // set the LEDs high

            size_t bytesRead = Serial.read(buffer, 64);
          /*settings struct example:
            settings.clockSelect = UCSSEL__SMCLK; // use SMCLK as the clock for SPI
            settings.ClockDivisor = 0x2; // divide SMCLK by two
            settings.DMARxChannel = 0; // pick lowest DMA channel for DMARx
            settings.DMATxChannel = 0; // pick second lowest DMA channel for DMATx
            settings.chSize = SatLib::SPI::CharacterSize::BITS8; // 8 bit character size
            settings.order = SatLib::SPI::BitOrder::MSB; // send byte most significant bit first
            settings.phase = SatLib::SPI::ClockPhase::CPHA_0; // set clock phase to CPHA_0
            settings.polarity = SatLib::SPI::ClockPolarity::LOW; // set clock polarity low
            settings.role = SatLib::SPI::Role::MASTER; // set as master
            SPI.beginTransaction(SPITxBuffer, 64, SPIRxBuffer, 64, settings);
            */

            // OR, you can use the following prebuilt structs (SPI_MODE0, SPI_MODE1 ... SPI_MODE3)
            SPI.beginTransaction(SPITxBuffer, 64, SPIRxBuffer, 64, SPI_MODE0); // initialize in mode 0 at half the baudrate of the source clock, MSB first, 8 bit character size master
            SPI.write(buffer, bytesRead);
            P1OUT &= ~BIT3; // pull chip select low
            SPI.transfer();
            P1OUT |= BIT3; // pull chip select high again
            size_t bytesReceived = SPI.read(buffer, 64);
            SPI.endTransaction();

            Serial.write(buffer, bytesReceived); // write the bytes received from DMA.

            P1OUT ^= BIT0 | BIT1; // set the LEDs low again
        }
    }
}

// eUSCI_A0 vector must call the appropriate IRQ Handler function
#pragma vector=EUSCI_A0_VECTOR
__interrupt void EUSCI_A0_ISR(void)
{
    Serial.IRQHandler(); // call the IRQHandler to enable sending and receiving.
}

#pragma vector=DMA_VECTOR
__interrupt void DMA_ISR(void) // required for SPI to work properly
{
    SatLib::DMA_IRQHandler();
}
