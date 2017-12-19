// hojunes.lee@lge.com
#ifndef _EV_CHARGE_TIMER_H_
#define _EV_CHARGE_TIMER_H_

#include "Timer.h"
#include "EvChargeApp.h"

class EvChargeApp;
// Timer Class
class EvcTimer : public TimerTimeoutHandler
{
    public:
        EvcTimer(EvChargeApp& s):timer(s) {}
        virtual ~EvcTimer() {}

    public:
        virtual void handlerFunction( int );

    private:
        EvChargeApp& timer;
}; /* Timer Class.*/

#endif // _EV_CHARGE_TIMER_H_