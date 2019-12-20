/*
 * SatCLKS.h
 *
 *  Created on: Nov 18, 2019
 *  Updated on: Dec 5, 2019
 *      Author: Matthew Middleton
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <msp430.h>

class CLKS
{

    private:
        /*-----------------------------------------------------------------------------------------------------------------*/
        /*CLK registers*/
        volatile unsigned char *CSCTL0_H_;   /*Unlocks/locks the CS registers for manipulation. CSKEY_H to unlock, 0 to lock*/
        volatile unsigned int *CSCTL1_;      /*Sets the frequency for the DCO*/
        volatile unsigned int *CSCTL2_;      /*Sets the CLK to the desired oscillator*/
        volatile unsigned int *CSCTL3_;      /*Sets the CLK divider*/
        volatile unsigned int *CSCTL4_;      /*Sets/adusts the drive strength or turns the specified CLKS off*/
        volatile unsigned int *CSCTL5_;      /*Sets the fault flags for the specified oscillators*/
        volatile unsigned int *CSCTL6_;      /*Sets the given CLKS to be available for conditional module requests*/
        volatile unsigned int *FRCTL0_;      /*For configuring the wait states needed for high frequency operation*/
        /*-----------------------------------------------------------------------------------------------------------------*/

        /*Returns the value needed for calculating the delays needed based on the frequency operation
         */
        unsigned int lookup_frequency(unsigned int dco_range, unsigned int freq);

        /*Determines and sets the wait states for high frequency operation
         */
        void config_wait_states(unsigned int dco_range, unsigned int freq);

    public:
        /*Constructor to assign the registers to pointers
         */
        CLKS();

        /*Configures a clock to the desired clock, clk, the dco range to use, dco_range,
         * the frequency to set the specified clock to, freq, and the amount to divide the specified clock, div.
         * Configuring more than one clock at once for different frequencies may caused undesired/unintended
         * effects, so configure one clock at a time.
         * Currently there are no checks for invalid conditions besides setting wait states for FRAM.
         */
        void configClks(unsigned int clk, unsigned int dco_range, unsigned int freq, unsigned int osc_src);

        /*Configures the options associated with CSCTL4-6
         */
        void config_opts(unsigned int csctl4_opts, unsigned int counter_and_flags_opts, unsigned int clk_requests);
};
