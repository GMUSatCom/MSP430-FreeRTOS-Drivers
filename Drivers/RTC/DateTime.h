#pragma once
#include <stdint.h>

#define SUNDAY 0
#define MONDAY 1
#define TUESDAY 2
#define WEDNESDAY 3
#define THURSDAY 4
#define FRIDAY 5
#define SATURDAY 6

#define JANUARY 1
#define FEBRUARY 2
#define MARCH 3
#define APRIL 4
#define MAY 5
#define JUNE 6
#define JULY 7
#define AUGUST 8
#define SEPTEMBER 9
#define OCTOBER 10
#define NOVEMBER 11
#define DECEMBER 12

namespace SatLib
{
    class DateTime
    {
    private:
        int8_t hours;
        int8_t minutes;
        int8_t seconds;
        int8_t dayOfTheWeek;
        int8_t day;
        int8_t month;
        int16_t year;

        // used for calculating the day of the week.
        const int8_t monthCodes[12] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

    public:

        /**
         * Can be used to add two times together.
         */
        inline DateTime operator+(DateTime const &timestamp)
        {
            DateTime result;
            result.seconds = this->seconds + timestamp.seconds;
            result.minutes = this->minutes + timestamp.minutes;
            result.hours = this->hours + timestamp.hours;
            result.day = this->day + timestamp.day;
            result.month = this->month + timestamp.month;
            result.year = this->year + timestamp.year;

            while(result.seconds > 59)
            {
                result.seconds -= 60;
                result.minutes++;
            }

            while(result.minutes > 59)
            {
                result.minutes -= 60;
                result.hours++;
            }

            while(result.hours > 23)
            {
                result.hours -= 24;
                result.day++;
            }

            uint8_t currentMonthMaxDays = 0;

            do
            {
                switch(result.month)
                {
                case FEBRUARY:
                    if(result.year % 4 == 0) // if this is a leap year
                        currentMonthMaxDays = 29;
                    else
                        currentMonthMaxDays = 28;
                    break;
                case JANUARY:
                case MARCH:
                case MAY:
                case JULY:
                case AUGUST:
                case OCTOBER:
                case DECEMBER:
                    currentMonthMaxDays = 31;
                    break;
                case APRIL:
                case JUNE:
                case SEPTEMBER:
                case NOVEMBER:
                    currentMonthMaxDays = 30;
                    break;
                }

                if(result.day > currentMonthMaxDays)
                {
                    result.day -= currentMonthMaxDays;
                    result.month++;
                    if(result.month > DECEMBER)
                    {
                        result.year++;
                        result.month = JANUARY;
                    }
                }

            }while(result.day > currentMonthMaxDays);

            result.dayOfTheWeek = getDayOfTheWeek(result);

            return result;
        }

        /**
         * Can be used to add seconds to the current time.
         */
        inline DateTime operator+(uint32_t seconds)
        {
            DateTime result = *this;
            while(seconds > 0)
            {
                if(seconds > 60)
                {
                    result.seconds += 60;
                    seconds -= 60;
                }else{
                    result.seconds += seconds;
                    seconds -= seconds;
                }

                // ensure the timestamp is valid.
                while(result.seconds > 59)
                {
                    result.seconds -= 60;
                    result.minutes++;
                }

                while(result.minutes > 59)
                {
                    result.minutes -= 60;
                    result.hours++;
                }

                while(result.hours > 23)
                {
                    result.hours -= 24;
                    result.day++;
                }

                uint8_t currentMonthMaxDays = 0;

                do
                {
                    switch(result.month)
                    {
                    case FEBRUARY:
                        if(result.year % 4 == 0) // if this is a leap year
                            currentMonthMaxDays = 29;
                        else
                            currentMonthMaxDays = 28;
                        break;
                    case JANUARY:
                    case MARCH:
                    case MAY:
                    case JULY:
                    case AUGUST:
                    case OCTOBER:
                    case DECEMBER:
                        currentMonthMaxDays = 31;
                        break;
                    case APRIL:
                    case JUNE:
                    case SEPTEMBER:
                    case NOVEMBER:
                        currentMonthMaxDays = 30;
                        break;
                    }

                    if(result.day > currentMonthMaxDays)
                    {
                        result.day -= currentMonthMaxDays;
                        result.month++;
                        if(result.month > DECEMBER)
                        {
                            result.year++;
                            result.month = JANUARY;
                        }
                    }

                }while(result.day > currentMonthMaxDays);
            }

            result.dayOfTheWeek = getDayOfTheWeek(result);
            return result;
        }

        /**
         * Assignment operator.
         */
        inline void operator=(DateTime const& timestamp)
        {
            this->seconds = timestamp.seconds;
            this->minutes = timestamp.minutes;
            this->hours = timestamp.hours;
            this->day = timestamp.day;
            this->month = timestamp.month;
            this->year = timestamp.year;
        }

        /**
         * Equality of timestamps.
         */
        inline bool operator==(DateTime const& timestamp)
        {
            return (this->seconds == timestamp.seconds) &&
                   (this->minutes == timestamp.minutes) &&
                   (this->hours == timestamp.hours) &&
                   (this->day == timestamp.day) &&
                   (this->month == timestamp.month) &&
                   (this->year == timestamp.year) &&
                   (this->dayOfTheWeek == timestamp.dayOfTheWeek);
        }

        /**
         * Is this date before timestamp?
         */
        inline bool operator<(DateTime const& timestamp)
        {
            if(this->year > timestamp.year)
                return false;

            if(this->month > timestamp.month)
                return false;

            if(this->day > timestamp.day)
                return false;

            if(this->hours > timestamp.day)
                return false;

            if(this->minutes > timestamp.minutes)
                return false;

            if(this->seconds >= timestamp.seconds)
                return false;

            return true;
        }

        /**
         * Is this date after the timestamp?
         */
        inline bool operator>(DateTime const& timestamp)
        {
            if(this->year < timestamp.year)
                return false;

            if(this->month < timestamp.month)
                return false;

            if(this->day < timestamp.day)
                return false;

            if(this->hours < timestamp.day)
                return false;

            if(this->minutes < timestamp.minutes)
                return false;

            if(this->seconds <= timestamp.seconds)
                return false;

            return true;
        }

        inline bool operator<=(DateTime const& timestamp)
        {
            if(this->year > timestamp.year)
                return false;

            if(this->month > timestamp.month)
                return false;

            if(this->day > timestamp.day)
                return false;

            if(this->hours > timestamp.day)
                return false;

            if(this->minutes > timestamp.minutes)
                return false;

            if(this->seconds > timestamp.seconds)
                return false;

            return true;
        }

        inline bool operator>=(DateTime const& timestamp)
        {
            if(this->year < timestamp.year)
                return false;

            if(this->month < timestamp.month)
                return false;

            if(this->day < timestamp.day)
                return false;

            if(this->hours < timestamp.day)
                return false;

            if(this->minutes < timestamp.minutes)
                return false;

            if(this->seconds < timestamp.seconds)
                return false;

            return true;
        }

        inline int8_t getDayOfTheWeek(DateTime date)
        {
            if(date.month < 1 || date.month > 12)
                return -1; // return instead of accessing an invalid memory address

            date.year -= date.month < 3; // reduce the year by one if the month code is less than 3
            return (date.year + date.year/4 - date.year/100 + date.year/400 + monthCodes[date.month-1] + date.day) % 7;
        }
    };
}
