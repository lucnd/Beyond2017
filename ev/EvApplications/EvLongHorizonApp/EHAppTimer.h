#ifndef _EV_LONG_HORIZON_TIMER_H_
#define _EV_LONG_HORIZON_TIMER_H_

#include "EHApp.h"

// Timer Class
class EHAppTimer : public TimerTimeoutHandler
{
    public:
        EHAppTimer(EHAppTimer& s):timer(s) {}
        virtual ~EHAppTimer() {}
        virtual void handlerFunction( int );

    private:
        EHAppTimer& timer;
}; /* Timer Class.*/

#endif //_EV_LONG_HORIZON_TIMER_H_
