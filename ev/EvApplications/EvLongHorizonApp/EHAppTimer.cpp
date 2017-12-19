#include "EHAppTimer.h"

#define LOG_TAG "[EVT]EHAppTimer"
#include "Log.h"

void EHAppTimer::handlerFunction(int timer_id )
{
    // timer.resetTimer((uint8_t)timer_id);

    switch(timer_id)
    {
        // case XXXXX:
        // {
        //     XXXXXXX();
        //     break;
        // }

        default:
        {
            LOGV("[ERROR] UNKNOWN TIMER has expired!");
            break;
        }
    }
}