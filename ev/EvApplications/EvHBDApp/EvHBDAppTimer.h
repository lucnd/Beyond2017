#ifndef _EV_HBD_TIMER_H_
#define _EV_HBD_TIMER_H_

#include "EvHBDApp.h"

// Timer Class
class EvHBDTimer : public TimerTimeoutHandler
{
    public:
        EvHBDTimer(EvHBDApp& s):timer(s) {}
        virtual ~EvHBDTimer() {}

    public:
        virtual void handlerFunction( int );

    private:
        EvHBDApp& timer;
}; /* Timer Class.*/

#endif // _EV_HBD_TIMER_H_