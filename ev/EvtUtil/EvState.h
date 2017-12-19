// hojunes.lee@lge.com
#ifndef _EV_STATE_H_
#define _EV_STATE_H_

class EvState
{
public:
    unsigned char getState();
    virtual void toStringState(unsigned char state, unsigned char newState);

protected:
    void setState(unsigned char newState);
    unsigned char state;
};

#endif // _EV_STATE_H_