#define LOG_TAG "[EVT]EvHBDAppTimer"
#include "Log.h"

#include "EvHBDAppTimer.h"

void EvHBDTimer::handlerFunction(int timer_id )
{
    EvHBDApp::GetInstance()->resetTimer(timer_id);
    switch(timer_id)
    {
        case EvHBDApp::HBD_CAN_READ_TIMER:
            LOGV("HBD_CAN_READ_TIMER has expired.");
            timer.timeout_HBD_CAN_READ_TIMER();
            break;
        case EvHBDApp::HBD_CAN_BUFFER_TIMER:
            LOGV("HBD_CAN_BUFFER_TIMER has expired.");
            timer.timeout_HBD_CAN_BUFFER_TIMER();
            break;
        case EvHBDApp::HBD_BUFFER_MAX_TIMER:
            LOGV("HBD_BUFFER_MAX_TIMER has expired.");
            timer.timeout_HBD_BUFFER_MAX_TIMER();
            break;
        case EvHBDApp::HBD_TCU_PUSH_TIMER:
            LOGV("HBD_TCU_PUSH_TIMER has expired.");
            timer.timeout_HBD_TCU_PUSH_TIMER();
        default:
            break;
    }
}