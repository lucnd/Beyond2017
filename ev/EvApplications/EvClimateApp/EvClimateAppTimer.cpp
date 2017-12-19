#include "EvClimateAppTimer.h"

#define LOG_TAG "[EVT]EvClimateAppTimer"
#include "Log.h"

void EccTimer::handlerFunction(int timer_id )
{
    if(timer.resetTimer((uint8_t)timer_id) == false)
    {
        return;
    }
    switch(timer_id)
    {
        case EvClimateApp::ECC_CAN_STATUS_IGNORE_TIMER:
        {
            LOGV("ECC_CAN_STATUS_IGNORE_TIMER has expired!");
            timer.timeout_ECC_CAN_STATUS_IGNORE_TIMER();
            break;
        }
        case EvClimateApp::ECC_HVAC_NO_RESP_TIMEOUT_TIMER:
        {
            LOGV("ECC_HVAC_NO_RESP_TIMEOUT_TIMER has expired!");
            timer.timeout_ECC_HVAC_NO_RESP_TIMEOUT_TIMER();
            break;
        }
        case EvClimateApp::ECC_DIRECT_PRECONDITION_TIMER:
        {
            LOGV("ECC_DIRECT_PRECONDITION_TIMER has expired!");
            timer.timeout_ECC_DIRECT_PRECONDITION_TIMER();
            break;
        }
        case EvClimateApp::ECC_CAN_SETTING_IGNORE_TIMER:
        {
            LOGV("ECC_CAN_SETTING_IGNORE_TIMER has expired!");
            timer.timeout_ECC_CAN_SETTING_IGNORE_TIMER();
            break;
        }
        case EvClimateApp::ECC_SETTING_TRIGGER_TIMER:
        {
            LOGV("ECC_SETTING_TRIGGER_TIMER has expired!");
            timer.timeout_ECC_SETTING_TRIGGER_TIMER();
            break;
        }
        case EvClimateApp::ECC_NM_TIMEOUT_TIMER:
        {
            LOGV("ECC_NM_TIMEOUT_TIMER has expired!");
            timer.timeout_ECC_NM_TIMEOUT_TIMER();
            break;
        }

        case EvClimateApp::ECC_PRESET_NM_TIMEOUT_TIMER:
        {
            LOGV("ECC_PRESET_NM_TIMEOUT_TIMER has expired!");
            timer.timeout_ECC_PRESET_NM_TIMEOUT_TIMER();
            break;
        }

        default:
        {
            LOGV("[ERROR] UNKNOWN TIMER has expired!");
            break;
        }
    }
}