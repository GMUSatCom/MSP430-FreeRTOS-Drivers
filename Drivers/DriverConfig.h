/*
 * DriverConfig.h
 *
 *  Created on: Aug 10, 2019
 *      Author: Mynam
 */

#pragma once

#include <msp430.h>

/**
 * Configuration option for what low power mode the MSP430 drops into during it's idle loop.
 * Lower power modes reduce power consumption but may cause problems with I/O since clocks take a while to start up.
 * LPM1 is the happy middle between power consumption and reliability for most applications it seems.
 * Set to Zero if you don't want the idle loop to set the chip into a low power mode.
 */
#define LPM_IDLE_BITS LPM1_bits
