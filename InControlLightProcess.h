#ifndef INCONTROLLIGHTPROCESS_H
#define INCONTROLLIGHTPROCESS_H

#include "SpecialModeBaseProcess.h"

class InControlLightProcess : public SpecialModeBaseProcess
{
public:
    InControlLightProcess();
    virtual ~InControlLightProcess();
    virtual void doSpecialModeHandler(int32_t what, const sp<sl::Message>& message);
    virtual void handleTimerEvent(int timerId);
    virtual void handleEvent(uint32_t ev);

protected:
    virtual void initializeProcess();
};

#endif // INCONTROLLIGHTPROCESS_H
