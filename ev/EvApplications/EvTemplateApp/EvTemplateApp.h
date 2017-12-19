#ifndef _EV_RANGE_APP_H_
#define _EV_RANGE_APP_H_

#include <utils/RefBase.h>

#include "EvtServiceManager/EvServiceManager.h"
#include "EvtServiceManager/EvtTimer.h"
#include "EvtData/EvtSingleton.h"
#include "EvtUtil/EvState.h"

class EvTemplateApp : public EvtSingleton<EvTemplateApp>, EvState
{
public:
    EvTemplateApp(){};
    ~EvTemplateApp(){};

    void onStart();

private:
    void setTimer(uint8_t timerName, uint16_t expireTime);
    virtual void toStringState(unsigned char state, unsigned char newState);

    // Timer Class
    class EvrTimer : public TimerTimeoutHandler
    {
        public:
            EvrTimer(EvTemplateApp& s):timer(s) {}
            virtual ~EvrTimer() {}
            virtual void handlerFunction( int );

        private:
            EvTemplateApp& timer;
    }; /* Timer Class.*/

};


#endif //_EV_RANGE_APP_H_