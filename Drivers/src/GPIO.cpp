/*
 * GPIO.cpp
 *
 *  Created on: Nov 1, 2019
 *      Author: Kyle Shiflett
 */
#include <Drivers/GPIO/GPIO.h>
#include <stdlib.h>
#include <string.h>

// Functions to control GPIO Pins
//-----------------------------------------------------------------------------------------------------
// used to read the value of a pin when function is called and returns the value on that pin as 1 for high or 0 for low
bool SatLib::GPIO::GPIO_Read(char PinGroup, char Pin){

    if(  *(GPIO_Ports[PinGroup].PxIN) & GPIO_BITS[Pin] )
        return 1;
    else
        return 0;
}
//-----------------------------------------------------------------------------------------------------
// used to change what a particular pin outputs
void SatLib::GPIO::GPIO_Write(char PinGroup, char Pin, bool high){
    if(high){
    *(GPIO_Ports[PinGroup].PxOUT) |= GPIO_BITS[Pin] ;
    }
    else
    *(GPIO_Ports[PinGroup].PxOUT) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used to change the pin output if out = 1 or input if out = 0
void SatLib::GPIO::GPIO_Direction(char PinGroup, char Pin, bool out){
    if(out)
    *(GPIO_Ports[PinGroup].PxDIR) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxDIR) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used to enable or disable the pull up/down resistors
void SatLib::GPIO::GPIO_PUD(char PinGroup, char Pin, bool enable){
    if(enable)
    *(GPIO_Ports[PinGroup].PxREN) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxREN) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used to set the values of the select bits
void SatLib::GPIO::GPIO_Select_Bits_0(char PinGroup, char Pin, bool high){

    if(high)
    *(GPIO_Ports[PinGroup].PxSEL0) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxSEL0) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used to set the values of the select bits
void SatLib::GPIO::GPIO_Select_Bits_1(char PinGroup, char Pin, bool high){
    if(high)
    *(GPIO_Ports[PinGroup].PxSEL1) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxSEL1) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used if both select bits need to be changed at the same time and not sequentially
void SatLib::GPIO::GPIO_Select_Bits_Compliment(char PinGroup, char Pin){

    *(GPIO_Ports[PinGroup].PxSELC) |= GPIO_BITS[Pin] ;

}
//-----------------------------------------------------------------------------------------------------
// used to determine if interrupts on pin occur on raising "fallingedge" = 0 or falling edge "fallingedge" = 1
void SatLib::GPIO::GPIO_Interrupt_Edge_Select(char PinGroup, char Pin, bool fallingedge){
    if(fallingedge)
    *(GPIO_Ports[PinGroup].PxIES) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxIES) &= ~(GPIO_BITS[Pin]) ;

}
//-----------------------------------------------------------------------------------------------------
// used to enable interrupts on a particular pin
void SatLib::GPIO::GPIO_Interrupt_Enable(char PinGroup, char Pin, bool enable){

    if(enable)
    *(GPIO_Ports[PinGroup].PxIE) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxIE) &= ~(GPIO_BITS[Pin]) ;
}
//-----------------------------------------------------------------------------------------------------
// used to set or clear an interrupt flag. "set" = 0 flag is cleared
void SatLib::GPIO::GPIO_Interrupt_Flag(char PinGroup, char Pin, bool set){
    if(set)
    *(GPIO_Ports[PinGroup].PxIFG) |= GPIO_BITS[Pin] ;

    else
    *(GPIO_Ports[PinGroup].PxIFG) &= ~(GPIO_BITS[Pin]) ;

}
