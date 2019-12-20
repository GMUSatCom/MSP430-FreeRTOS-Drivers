/*
 * SatCLKS.cpp
 *
 *  Created on: Nov 18, 2019
 *  Updated on: Dec 19, 2019
 *      Author: Matthew Middleton
 */

#include "../Clocks/CLKS.h"

CLKS::CLKS()
{
    /*Assigns each of the CS registers to a pointer within the class*/
    this->CSCTL0_H_ = (volatile unsigned char *) &CSCTL0_H;
    this->CSCTL1_ = (volatile unsigned int *) &CSCTL1;
    this->CSCTL2_ = (volatile unsigned int *) &CSCTL2;
    this->CSCTL3_ = (volatile unsigned int *) &CSCTL3;
    this->CSCTL4_ = (volatile unsigned int *) &CSCTL4;
    this->CSCTL5_ = (volatile unsigned int *) &CSCTL5;
    this->CSCTL6_ = (volatile unsigned int *) &CSCTL6;
    this->FRCTL0_ = (volatile unsigned int *) &FRCTL0;
}

/*Configures a clock to the desired clock, clk, the dco range to use, dco_range,
 * the frequency to set the specified clock to, freq, and the amount to divide the specified clock, div.
 * Configuring more than one clock at once for different frequencies may caused undesired/unintended
 * effects, so configure one clock at a time.
 * Currently there are no checks for invalid conditions besides setting wait states for FRAM.
*/
void CLKS::configClks(unsigned int clk, unsigned int dco_range, unsigned int freq, unsigned int osc_src)
{
    volatile unsigned int cycles = 0;

    //calculation for cycle delays. k cycles = 20 cycles buffer + (10us /(1/(n MHz) ))
    cycles = 20 + (10 * lookup_frequency(dco_range, freq));
    config_wait_states(dco_range, freq);

    *CSCTL0_H_ = CSKEY_H;           //Unlocks the CS registers

    *CSCTL1_ = dco_range | freq;   //Sets the DCO to the given frequency
    *CSCTL2_ = clk;                //Sets the clock to DCO
    *CSCTL3_ = osc_src;            //Sets the divider according to the clocks provided

    //Delay by ~10us to let DCO settle, per device errata.
    //may be delaying too long
    //simulates NOP sled
    while((cycles--)>0)
    {
        __delay_cycles(1);
    }

    *CSCTL0_H_ &= ~CSKEY_H;         //Locks the CS registers
}

/*Looks up the frequency needed to process the cycle delays based off the dco_range, upper or lower (DCORSEL), and freq, the frequency (DCOFSEL)
 */
unsigned int CLKS::lookup_frequency(unsigned int dco_range, unsigned int freq)
{
    unsigned int lookup = 1;
    if(!(dco_range&BIT6))//lower frequency range; DCORSEL value = 0
    {
        switch(freq & (BIT3|BIT2|BIT1))//DCOFSEL value passed in
        {
        //values rounded up per CSCTL1 register DCOFSEL values
            case 0:         //1MHz
                lookup = 1;
                break;
            case 2:         //2.67MHz
                lookup = 3;
                break;
            case 4:         //3.5MHz
            case 6:         //4MHz
                lookup = 4;
                break;
            case 8:         //5.33MHz
                lookup = 6;
                break;
            case 10:         //7MHz
                lookup = 7;
                break;
            case 12:         //8MHz
                lookup = 8;
                break;
            default:        //1MHz
                lookup = 1;
                break;
        }
    }
    else//upper frequency range; DCORSEL value = 1
    {
        switch(freq & (BIT3|BIT2|BIT1))//DCORSEL value passed in
        {
            case 0:         //1MHz
                lookup = 1;
                break;
            case 2:         //5.33MHz
                lookup = 6;
                break;
            case 4:         //7MHz
                lookup = 7;
                break;
            case 6:         //8MHz
                lookup = 8;
                break;
            case 8:         //16MHz
                lookup = 16;
                break;
            case 10:         //21MHz
                lookup = 21;
                break;
            case 12:         //24MHz
                lookup = 24;
                break;
            default:        //1MHz
                lookup = 1;
                break;
        }
    }
    return lookup;
}

/*Determines and sets the wait states for high frequency operation
*/
void CLKS::config_wait_states(unsigned int dco_range, unsigned int freq)
{
//#ifdef FRCTL0
    if(dco_range&DCORSEL)   //The DCO range may exceed 8MHz.
    {
        /*Determine how many waitstates are required for the fram
         * switches based on the bit pattern(see family guide) specified by freq
         */
        switch(freq & (BIT3|BIT2|BIT1))
        {
        //If the frequency is 8 MHz: 8MHz/8MHz = 1
        case 6:
            *FRCTL0_ = FRCTLPW | NWAITS1;
            break;
        /*If the frequency is 16 MHz: 16MHz / 8MHz = 2
         *or the frequency is 21 MHz: 21MHz / 8MHz = 2.625
         *2 wait states are needed
         */
        case 8:
        case 10:
            *FRCTL0_ = FRCTLPW | NWAITS2;
            break;
        //If the frequency is 24 MHz: 24MHz / 8MHz = 3 wait states are needed
        case 12:
            *FRCTL0_ = FRCTLPW | NWAITS3;
            break;
        //assumed to be less than 8MHz operational range
        default:
            break;
        }
    }
    /*DCORSEL is 0 and DCOFSEL is at 8MHz
     */
    else if((freq & (BIT3|BIT2)) && !(freq & BIT1))
    {
        *FRCTL0_ = FRCTLPW | NWAITS1;
    }
//#endif
}

void CLKS::config_opts(unsigned int osc_clk_and_drive_opts, unsigned int counter_and_flags_opts, unsigned int clk_requests)
{
    *CSCTL0_H_ = CSKEY_H;               //unlock CS registers
    *CSCTL4_ = osc_clk_and_drive_opts; //sets the drive for the non-DCO oscillators,
                                        //as well as the control for turning oscillators or clocks off
    *CSCTL5_ = counter_and_flags_opts; //sets the counter and/or flag options
    *CSCTL6_ = clk_requests;           //enables clock requests, if any
    *CSCTL0_H_ &= ~CSKEY_H;             //locks the CS registers
}

