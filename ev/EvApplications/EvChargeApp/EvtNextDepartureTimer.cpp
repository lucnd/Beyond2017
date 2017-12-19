#include "EvtNextDepartureTimer.h"
#include "NGTPManagerService/NGTPUtil.h"

#define LOG_TAG "[EVT]NextDepartureTimer"
#include "Log.h"

EvtNextDepartureTimer::EvtNextDepartureTimer()
{
    resetAll();
};

void EvtNextDepartureTimer::resetAll()
{
    // Initialize variables
    setCreationTimeStamp(0);

    dateYear_field   = 0;
    dateMonth_field  = 0;
    dateDay_field    = 0;
    dateHour_field   = 0;
    dateMinute_field = 0;

    setDateDayFieldUpdated(false);
    setDateHourFieldUpdated(false);
    setDateMinuteFieldUpdated(false);
    setDateMonthFieldUpdated(false);
    setDateYearFieldUpdated(false);
}

void EvtNextDepartureTimer::printData()
{

}

void EvtNextDepartureTimer::setAllValues(uint32_t _year, uint32_t _month, uint32_t _day, uint32_t _hour, uint32_t _minute)
{
    setDateYearField(_year);
    setDateMonthField(_month);
    setDateDayField(_day);
    setDateHourField(_hour);
    setDateMinuteField(_minute);
}
void EvtNextDepartureTimer::setCreationTimeStamp(uint32_t _creationTimeStamp){
    creationTimeStamp = _creationTimeStamp;
}

uint32_t EvtNextDepartureTimer::getCreationTimeStamp() const{
    return creationTimeStamp;
}

bool EvtNextDepartureTimer::isDateDayFieldUpdated() const {
    return bDateDay_field_updated;
}

void EvtNextDepartureTimer::setDateDayFieldUpdated(bool dateDayFieldUpdated) {
    bDateDay_field_updated = dateDayFieldUpdated;
}

bool EvtNextDepartureTimer::isDateHourFieldUpdated() const {
    return bDateHour_field_updated;
}

void EvtNextDepartureTimer::setDateHourFieldUpdated(bool dateHourFieldUpdated) {
    bDateHour_field_updated = dateHourFieldUpdated;
}

bool EvtNextDepartureTimer::isDateMinuteFieldUpdated() const {
    return bDateMinute_field_updated;
}

void EvtNextDepartureTimer::setDateMinuteFieldUpdated(bool dateMinuteFieldUpdated) {
    bDateMinute_field_updated = dateMinuteFieldUpdated;
}

bool EvtNextDepartureTimer::isDateMonthFieldUpdated() const {
    return bDateMonth_field_updated;
}

void EvtNextDepartureTimer::setDateMonthFieldUpdated(bool dateMonthFieldUpdated) {
    bDateMonth_field_updated = dateMonthFieldUpdated;
}

bool EvtNextDepartureTimer::isDateYearFieldUpdated() const {
    return bDateYear_field_updated;
}

void EvtNextDepartureTimer::setDateYearFieldUpdated(bool dateYearFieldUpdated) {
    bDateYear_field_updated = dateYearFieldUpdated;
}

uint32_t EvtNextDepartureTimer::getDateDayField() const {
    return dateDay_field;
}

void EvtNextDepartureTimer::setDateDayField(uint32_t dateDayField) {
    dateDay_field = dateDayField;
    setDateDayFieldUpdated(true);
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated TimedChrgStartDateDisp[%u],[%u]", getDateDayField(), getCreationTimeStamp());
}

uint32_t EvtNextDepartureTimer::getDateHourField() const {
    return dateHour_field;
}

void EvtNextDepartureTimer::setDateHourField(uint32_t dateHourField) {
    dateHour_field = dateHourField;
    setDateHourFieldUpdated(true);
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated TimedChrgStartHourDisp[%u],[%u]", getDateHourField(), getCreationTimeStamp());
}

uint32_t EvtNextDepartureTimer::getDateMinuteField() const {
    return dateMinute_field;
}

void EvtNextDepartureTimer::setDateMinuteField(uint32_t dateMinuteField) {
    dateMinute_field = dateMinuteField;
    setDateMinuteFieldUpdated(true);
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated TimedChrgStartMinsDisp[%u],[%u]", getDateMinuteField(), getCreationTimeStamp());
}

uint32_t EvtNextDepartureTimer::getDateMonthField() const {
    return dateMonth_field;
}

void EvtNextDepartureTimer::setDateMonthField(uint32_t dateMonthField) {
    dateMonth_field = dateMonthField;
    setDateMonthFieldUpdated(true);
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated TimedChrgStartMnthDisp[%u],[%u]", getDateMonthField(), getCreationTimeStamp());
}

uint32_t EvtNextDepartureTimer::getDateYearField() const {
    return dateYear_field;
}

void EvtNextDepartureTimer::setDateYearField(uint32_t dateYearField) {
    dateYear_field = dateYearField;
    setDateYearFieldUpdated(true);
    setCreationTimeStamp(UTIL::get_NGTP_time());
    LOGV("Updated TimedChrgStartYearDisp[%u],[%u]", getDateYearField(), getCreationTimeStamp());
}

int32_t EvtNextDepartureTimer::getCanDateYearField() const
{
    return canDateYear_field;
}

void EvtNextDepartureTimer::setCanDateYearField(int32_t _canDateYearField)
{
    canDateYear_field = _canDateYearField;
}

int32_t EvtNextDepartureTimer::getCanDateMonthField() const
{
    return canDateMonth_field;
}

void EvtNextDepartureTimer::setCanDateMonthField(int32_t _candDateMonthField)
{
    canDateMonth_field = _candDateMonthField;
}

int32_t EvtNextDepartureTimer::getCanDateDayField() const
{
    return canDateDay_field;
}

void EvtNextDepartureTimer::setCanDateDayField(int32_t _canDateDayField)
{
    canDateDay_field = _canDateDayField;
}

int32_t EvtNextDepartureTimer::getCanDateHourField() const
{
    return canDateHour_field;
}

void EvtNextDepartureTimer::setCanDateHourField(int32_t _canDateHourField)
{
    canDateHour_field = _canDateHourField;
}

int32_t EvtNextDepartureTimer::getCanDateMinField() const
{
    return canDateMin_field;
}

void EvtNextDepartureTimer::setCanDateMinField(int32_t _canDateMinField)
{
    canDateMin_field = _canDateMinField;
}

bool EvtNextDepartureTimer::isNextDepartureTimerChanged(int32_t _canYear, int32_t _canMonth, int32_t _canDay, int32_t _canHour, int32_t _canMinute)
{
    bool bChanged = false;

    int32_t tcuYear = getCanDateYearField();
    int32_t tcuMonth = getCanDateMonthField();
    int32_t tcuDay = getCanDateDayField();
    int32_t tcuHour = getCanDateHourField();
    int32_t tcuMin = getCanDateMinField();
    //TODO Implement comparison
    if( (_canYear       != tcuYear)
        || ( _canMonth  != tcuMonth)
        || ( _canDay    != tcuDay)
        || ( _canHour   != tcuHour)
        || ( _canMinute  != tcuMin))
    {
        LOGV("[Comp NextDept] CHANGED [C_T] Y[%d_%d] M[%d_%d] D[%d_%d] H[%d_%d] MIN[%d_%d]"
            , _canYear, tcuYear
            , _canMonth, tcuMonth
            , _canDay, tcuDay
            , _canHour, tcuHour
            , _canMinute, tcuMin);
        bChanged = true;
    }
    else
    {
        LOGV("[Comp NextDept] Not CHANGED [C_T] Y[%d_%d] M[%d_%d] D[%d_%d] H[%d_%d] MIN[%d_%d]"
            , _canYear, tcuYear
            , _canMonth, tcuMonth
            , _canDay, tcuDay
            , _canHour, tcuHour
            , _canMinute, tcuMin);
    }
    return bChanged;
}
