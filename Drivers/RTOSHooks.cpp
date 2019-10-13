/*
 * RTOSHooks.cpp
 *
 *  Created on: Oct 13, 2019
 *      Author: David Fiumano
 */

#include "DriverConfig.h"
#include <msp430.h>
#include <FreeRTOS.h>
#include <task.h>

extern "C"
{
    void vApplicationMallocFailedHook( void );

    void vApplicationIdleHook( void );

    void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );

    void vApplicationTickHook( void );
}

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected.
    See http://www.freertos.org/Stacks-and-stack-overflow-checking.html */

}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    __bis_SR_register( LPM_IDLE_BITS + GIE );
    __no_operation();
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{

}
