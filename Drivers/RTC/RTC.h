#pragma once

#include <stdint.h>
#include <msp430.h>

#include "Alarm.h"

namespace SatLib
{
    /**
     * This class is responsible for managing the RTC. It's only capable of setting and getting the date/time. See the Alarm class for information on setting alarms.
     */
    class RTC
    {
    private:

    public:
        // this class only has static members, so there is no reasons to allow construction of the object
        RTC() = delete;
        ~RTC() = delete;
        RTC(RTC const&) = delete;
        RTC& operator=(RTC const&) = delete;

        /**
         * Returns the current seconds.
         */
        static inline uint8_t getSeconds()
        {
            return RTCSEC;
        }

        /**
         * Return the current minutes.
         */
        static inline uint8_t getMinutes()
        {
            return RTCMIN;
        }

        /**
         * Return the current hours in military time (ex: this function would return 13 at 1 pm).
         */
        static inline uint8_t getHours()
        {
            return RTCHOUR;
        }

        /**
         * Return the current day.
         */
        static inline uint8_t getDay()
        {
            return RTCDAY;
        }

        /**
         * Returns the current month. In January, this returns 1, in February it returns 2, etc.
         */
        static inline uint8_t getMonth()
        {
            return RTCMON;
        }

        /**
         * Returns the current year. In 2020, this will return the number 2020.
         */
        static inline uint16_t getYear()
        {
            return RTCYEAR;
        }

        /**
         * Sets the current seconds.
         */
        static void setSeconds(uint8_t seconds)
        {
            RTCSEC = seconds;
        }

        /**
         * Sets the current minutes.
         */
        static void setMinutes(uint8_t minutes)
        {
            RTCMIN = minutes;
        }

        /**
         * Sets the current hours.
         */
        static void setHours(uint8_t hours)
        {
            RTCHOUR = hours;
        }

        /**
         * Sets the day of the week.
         */
        static void setDayOfTheWeek(uint8_t dayOfTheWeek)
        {
            RTCDOW = dayOfTheWeek;
        }

        /**
         * Sets the current day.
         */
        static void setDay(uint8_t day)
        {
            RTCDAY = day;
        }

        /**
         * Set the current month.
         */
        static void setMonth(uint8_t month)
        {
            RTCMON = month;
        }

        /**
         * Set the current year.
         */
        static void setYear(uint16_t year)
        {
            RTCYEAR = year;
        }

        /**
         * Set the current time (all at once).
         */
        static void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds)
        {
            RTCHOUR = hours;
            RTCMIN = minutes;
            RTCSEC = seconds;
        }

        /**
         * Set the current date (all at once).
         */
        static void setDate(uint8_t dayOfTheWeek, uint8_t day, uint8_t month, uint16_t year)
        {
            RTCMON = month;
            RTCDAY = day;
            RTCYEAR = year;
            RTCDOW = dayOfTheWeek;
        }

        /**
         * Initializes the LFXT and the RTC. The RTC requires a 32768 Hz external oscillator on LFXT.
         */
        static void initRTC()
        {
            PJSEL0 |= BIT4 | BIT5; // configure LFXT pins

            PM5CTL0 &= ~LOCKLPM5; // ensure the above pin settings take effect.

            CSCTL0_H = CSKEY_H;
            CSCTL4 &= ~LFXTOFF;
            do // clear the fault flag until the oscillator is working
            {
                CSCTL5 &= ~LFXTOFF;
                SFRIFG1 &= ~OFIFG;
            }while(SFRIFG1 & OFIFG);
            CSCTL0_H = 0; // re-lock the clock controls.

            RTCCTL0 = RTCKEY;
            RTCCTL13 |= RTCMODE | RTCHOLD; // calendar mode
        }

        /**
         * Starts and locks the RTC.
         */
        static void startClock()
        {
            RTCCTL13 &= ~(RTCHOLD);
            RTCCTL0 &= ~(RTCKEY);
        }

        /**
         * Stops and unlocks the RTC.
         */
        static void stopClock()
        {
            RTCCTL0 |= RTCKEY;
            RTCCTL0 |= RTCHOLD;
        }

        /**
         * Adds the specified alarm to the list of alarms.
         * Requires that user provide memory for the alarm by providing an instance of the alarm class.
         */
        static void setAlarm(Alarm* alarm, DateTime * timestamp, void(*alarmHandler)(void))
        {

        }

        /**
         * Finds and removes the specified alarm from the list of alarms.
         */
        static void removeAlarm(DateTime alarm)
        {

        }

        /**
         * Initialize alarm values. Any negative numbers are replaced with today's time and date.
         */
        static void initializeAlarm(DateTime * alarm)
        {

        }
    };
}
