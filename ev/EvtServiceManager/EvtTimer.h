//EvtTimer
#ifndef _EV_TIMER_H_
#define _EV_TIMER_H_

#include "Timer.h"

//Caution :: DO NOT USE this template class, It occurs run-time issue. 03-03-2016!!

// Timer template Class
template<class T>
class EvtTimer : public TimerTimeoutHandler
{
    public:
        EvtTimer(T* s):timer(s) {};
        virtual ~EvtTimer() {};
        virtual void handlerFunction( int );

    private:
        T* timer;
}; /* Timer Class.*/

#endif //_EV_TIMER_H_