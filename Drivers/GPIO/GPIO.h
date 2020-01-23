/*
 * HPIO.h
 *
 *  Created on: Nov 1, 2019
 *      Author: Kyle Shiflett
 */

#pragma once

#include <msp430.h>
#include <stdint.h>

// used with GPIO functions to index addresses of registers
#define P1  0
#define P2  1
#define P3  2
#define P4  3
#define P5  4
#define P6  5
#define P7  6
#define P8  7
#define PJL 8
#define PJH 9

namespace SatLib
{
    typedef struct
          {
        //address of GPIO pin configuration registers
        volatile unsigned char * PxIN;    //holds current status of GPIO pin
        volatile unsigned char * PxOUT;   //set pin to either HIGH PxOUT = 1 or LOW = 0
        volatile unsigned char * PxDIR;   //set direction 0 input 1 output
        volatile unsigned char * PxREN;   //enable pullup resistor if PxOUT = 1 pullup if 0 pulldown
        volatile unsigned char * PxSEL0;  //change mode of GPIO pin refer to data sheet for specific mode
        volatile unsigned char * PxSEL1;  //configuration
        volatile unsigned char * PxSELC;  //changes the status of PxSEL0 and PxSEL1 to its respective
                                         //compliment (used when both PxSEL need to be changed at the same time)
        volatile unsigned char * PxIES;   //configure rising (PxIES = 0) or falling (PxIES = 1) edge to trigger interrupt
        volatile unsigned char * PxIE;    //enable interrupts on specific pin
        volatile unsigned char * PxIFG;   //holds flags (1's) corresponding an interrupt that happened on that pin

          } GPIO_Type;

    extern GPIO_Type GPIO_Ports[];
    extern uint8_t GPIO_BITS[];
    class GPIO
    {

    public:
     void GPIO_Write(char PinGroup, char Pin, bool high);

     bool GPIO_Read(char PinGroup, char Pin);

     void GPIO_Direction(char PinGroup, char Pin, bool out);

     void GPIO_PUD(char PinGroup, char Pin, bool enable);

     void GPIO_Select_Bits_0(char PinGroup, char Pin, bool high);

     void GPIO_Select_Bits_1(char PinGroup, char Pin, bool high);

     void GPIO_Select_Bits_Compliment(char PinGroup, char Pin);

     void GPIO_Interrupt_Edge_Select(char PinGroup, char Pin, bool fallingedge);

     void GPIO_Interrupt_Enable(char PinGroup, char Pin, bool enable);

     void GPIO_Interrupt_Flag(char PinGroup, char Pin, bool set);


   };

}
