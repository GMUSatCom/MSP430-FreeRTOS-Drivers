#pragma once

#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>

#include "DateTime.h"

namespace SatLib
{
    class Alarm
    {
    private:
        void (*alarmHandler)(void);
        
        int16_t counter = 0; // the number of times that this alarm will fire.
        
        /**
         * Time for next alarm. Negative one indicates that the value should be the same as the current time.
         * For example, if today is 10:15:00 12/21/2019 and alarmHours = 10, alarmMinutes = 30 and everything else is -1,
         * this alarm will be configured to fire at 10:30:00 12/21/2019.
         */
        DateTime alarmTime;

    public:

        enum ALARM_ERROR : int8_t {
          NO_ERR,
          INVALID_TIME,
          INVALID_SECONDS,
          INVALID_MINUTES,
          INVALID_HOURS,
          INVALID_DAYS,
          INVALID_MONTHS,
          INVALID_YEARS,
          INVALID_DayOfTheWeek,
          INVALID_Units
        };

        inline void configure(DateTime * alarmTime, void (*alarmHandler)(void))
        {
            this->alarmHandler = alarmHandler;

            this->alarmTime = *alarmTime;
        }

        inline DateTime getAlarm()
        {
            return this->alarmTime;
        }

        inline bool operator==(Alarm const& alarm)
        {
            return this->alarmTime == alarm.alarmTime;
        }

        inline bool operator<(Alarm const& alarm)
        {
            return this->alarmTime < alarm.alarmTime;
        }

        inline bool operator>(Alarm const& alarm)
        {
            return this->alarmTime > alarm.alarmTime;
        }

        inline bool operator<=(Alarm const& alarm)
        {
            return this->alarmTime <= alarm.alarmTime;
        }

        inline bool operator>=(Alarm const& alarm)
        {
            return this->alarmTime >= alarm.alarmTime;
        }
    };
}
