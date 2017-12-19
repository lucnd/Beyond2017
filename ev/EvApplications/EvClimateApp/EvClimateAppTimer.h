#ifndef _EV_CLIMATE_CONTROL_TIMER_H_
#define _EV_CLIMATE_CONTROL_TIMER_H_

#include "EvClimateApp.h"

// Timer Class
class EccTimer : public TimerTimeoutHandler
{
    public:
        EccTimer(EvClimateApp& s):timer(s) {}
        virtual ~EccTimer() {}
        virtual void handlerFunction( int );

    private:
        EvClimateApp& timer;
}; /* Timer Class.*/

#endif //_EV_CLIMATE_CONTROL_TIMER_H_