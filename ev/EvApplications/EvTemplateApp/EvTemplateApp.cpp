#include "EvTemplateApp.h"

void EvTemplateApp::onStart()
{
    //TODO:
}

void EvTemplateApp::setTimer(uint8_t timerName, uint16_t expireTime)
{
    // Timer Setting.
    if(expireTime == 0)
    {
        LOGE("Error Timer[%d] expireTime has zero!!", timerName);
        return;
    }

    EvrTimer* t_timer = new EvrTimer(*this);
    Timer* m_Timer = new Timer( t_timer, timerName );
    m_Timer->setDuration(expireTime, 0);
    m_Timer->start();
}

void EvTemplateApp::EvrTimer::handlerFunction(int timer_id )
{
    // switch(timer_id)
    // {
    //     case XXX_TIMER:
    //     {
    //         LOGV("XXX has expired!");
    //         timer.XXX();
    //         break;
    //     }

    //     default:
    //         break;
    // }
}

void EvTemplateApp::toStringState(unsigned char state, unsigned char newState)
{
    // std::string EvAppState[EVC::_END_ENUM_];
    // EvAppState[EVC::OFF]                = "OFF";
    // EvAppState[EVC::IDLE]               = "IDLE";
    // EvAppState[EVC::WAIT_WakeUpTimer]   = "WAIT_WakeUpTimer";
    // EvAppState[EVC::WAIT_IMCTimer]      = "WAIT_IMCTimer";
    // EvAppState[EVC::WAIT_CANsignal]     = "WAIT_CANsignal";

    // if(state < 0 && state >= EVC::_END_ENUM_)
    // {
    //     return ;
    // }

    // if(newState < 0 && newState >= EVC::_END_ENUM_)
    // {
    //     return ;
    // }

    // LOGE("EvChargeApp state is changed : [%s] => [%s]", EvAppState[state].c_str(), EvAppState[newState].c_str());
}
