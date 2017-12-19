//hojunes.lee@lge.com
#include "EvState.h"
#include "Log.h"

unsigned char EvState::getState()
{
    return state;
}

void EvState::setState(unsigned char newState)
{
    if(state == newState)
    {
        return ;
    }
    toStringState(state, newState);
    state = newState;
}

void EvState::toStringState(unsigned char state, unsigned char newState)
{
    return ;
}