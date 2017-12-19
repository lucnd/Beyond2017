#ifndef _EV_NEXT_DEPARTURE_TIMER_H_
#define _EV_NEXT_DEPARTURE_TIMER_H_
#include <vector>
#include "EvtSingleton.h"
#include "EvtElectricVehicleInformation.h"

class EvtNextDepartureTimer : public EvtSingleton<EvtNextDepartureTimer>
{
   public :
       EvtNextDepartureTimer();
       ~EvtNextDepartureTimer(){};
       void resetAll();
       void printData();

       void setAllValues(uint32_t _year, uint32_t _month, uint32_t _day, uint32_t _hour, uint32_t _minute);

       uint32_t getCreationTimeStamp() const;
       void setCreationTimeStamp(uint32_t _creationTimeStamp);

       bool isDateDayFieldUpdated() const;
       void setDateDayFieldUpdated(bool dateDayFieldUpdated);

       bool isDateHourFieldUpdated() const;
       void setDateHourFieldUpdated(bool dateHourFieldUpdated);

       bool isDateMinuteFieldUpdated() const;
       void setDateMinuteFieldUpdated(bool dateMinuteFieldUpdated);

       bool isDateMonthFieldUpdated() const;
       void setDateMonthFieldUpdated(bool dateMonthFieldUpdated);

       bool isDateYearFieldUpdated() const;
       void setDateYearFieldUpdated(bool dateYearFieldUpdated);

       uint32_t getDateDayField() const;
       void setDateDayField(uint32_t dateDayField);

       uint32_t getDateHourField() const;
       void setDateHourField(uint32_t dateHourField);

       uint32_t getDateMinuteField() const;
       void setDateMinuteField(uint32_t dateMinuteField);

       uint32_t getDateMonthField() const;
       void setDateMonthField(uint32_t dateMonthField);

       uint32_t getDateYearField() const;
       void setDateYearField(uint32_t dateYearField);

       int32_t getCanDateYearField() const;
       void setCanDateYearField(int32_t _canDateYearField);

       int32_t getCanDateMonthField() const;
       void setCanDateMonthField(int32_t _candDateMonthField);

       int32_t getCanDateDayField() const;
       void setCanDateDayField(int32_t _canDateDayField);

       int32_t getCanDateHourField() const;
       void setCanDateHourField(int32_t _canDateHourField);

       int32_t getCanDateMinField() const;
       void setCanDateMinField(int32_t _canDateMinField);

       bool isNextDepartureTimerChanged(int32_t _canYear, int32_t _canMonth, int32_t _canDay, int32_t _canHour, int32_t _canMinute);

   private :
       uint32_t creationTimeStamp;

       bool bDateYear_field_updated;
       bool bDateMonth_field_updated;
       bool bDateDay_field_updated;
       bool bDateHour_field_updated;
       bool bDateMinute_field_updated;

       uint32_t dateYear_field;
       uint32_t dateMonth_field;
       uint32_t dateDay_field;
       uint32_t dateHour_field;
       uint32_t dateMinute_field;

       int32_t canDateYear_field;
       int32_t canDateMonth_field;
       int32_t canDateDay_field;
       int32_t canDateHour_field;
       int32_t canDateMin_field;
};
#endif // _EV_NEXT_DEPARTURE_TIMER_H_
