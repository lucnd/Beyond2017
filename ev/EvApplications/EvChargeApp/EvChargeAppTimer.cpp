#define LOG_TAG "[EVT]EvChargeAppTimer"
#include "Log.h"

#include "EvChargeAppTimer.h"

void EvcTimer::handlerFunction(int timer_id )
{
    EvChargeApp::GetInstance()->resetTimer(timer_id);

    switch(timer_id)
    {
        case EvChargeApp::EVC_GWM_BUSOPEN_TIMER:
        {
            LOGV("EVC_GWM_BUSOPEN_TIMER has expired.");
            timer.timeout_EVC_GWM_BUSOPEN_TIMER();
            break;
        }

        case EvChargeApp::EVC_IMC_IGNR_TIMER:
        {
            LOGV("EVC_IMC_IGNR_TIMER has expired.");
            timer.timeout_EVC_IMC_IGNR_TIMER();
            break;
        }

        case EvChargeApp::EVC_IMC_WKUP_TIMER:
        {
            LOGV("EVC_IMC_WKUP_TIMER has expired.");
            timer.timeout_EVC_IMC_WKUP_TIMER();
            break;
        }

        case EvChargeApp::EVC_EVI_PUSH_TIMER:
        {
            LOGV("EVC_EVI_PUSH_TIMER has expired.");
            timer.timeout_EVC_EVI_PUSH_TIMER();
            break;
        }

        default:
            break;
    }
}