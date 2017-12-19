#include "EvClimateAppHandler.h"

#define LOG_TAG "[EVT]EvClimateAppHandler"
#include "Log.h"

#define DELAYED_NGPT_ALERT 0xFE

// Handler for Asyn Signal Call aback pettern.
void EvClimateAppHandler::handleMessage(const sp<sl::Message>& msg)
{
    switch(msg->what)
    {
        case EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT:
        {
            NGTP_PRECONDITON_UPDATE_ALERT(msg->arg1);
            break;
        }

        case DELAYED_NGPT_ALERT:
        {
            delayed_NGTP_PRECONDITON_UPDATE_ALERT();
            break;
        }

        default:
        {
            break;
        }
    }
}

void EvClimateAppHandler::NGTP_PRECONDITON_UPDATE_ALERT(uint16_t alert_Reason)
{
    /* Deleted by Yogesh's comments.
        From : "Shinde, Yogesh" <yshinde1@jaguarlandrover.com>
        To : 이호준 주임연구원(hojunes.lee)
        Cc : Dvijika Patel <dpatel12@jaguarlandrover.com>, Manns, Marc <mmanns1@jaguarlandrover.com>, Mishra, Poojan <pmishra2@jaguarlandrover.com>, [GROUP]JLR-SW-PL, 최승우 주임연구원(paulwj.choi)
        Date : 2017/05/29 16:44:47
        Subject : Re: [EV][ECC] Question about SRD9 spec.
    */
    // if(alert_Reason != svc::AlertData::preconditioningStarted_chosen)
    // {
    //     send_NGTP_PRECONDITON_UPDATE_ALERT(alert_Reason);
    //     return;
    // }

    if(hasMessages(DELAYED_NGPT_ALERT) == true)
    {
        mAlertReason = alert_Reason;
        return;
    }
    else
    {
        send_NGTP_PRECONDITON_UPDATE_ALERT(alert_Reason);
        trigger_delayed_NGTP_PRECONDITON_UPDATE_ALERT();
    }
}

void EvClimateAppHandler::delayed_NGTP_PRECONDITON_UPDATE_ALERT()
{
    if(mAlertReason == UNKNOWN)
    {
        return;
    }
    send_NGTP_PRECONDITON_UPDATE_ALERT(mAlertReason);
    trigger_delayed_NGTP_PRECONDITON_UPDATE_ALERT();
}

void EvClimateAppHandler::send_NGTP_PRECONDITON_UPDATE_ALERT(uint16_t alert_Reason)
{
    //update data
    EvtEVPreconditionStatus::GetInstance()->setPreconditionOperatingStatusField(mEvClimateApp.mEvVif->queryCanData(Signal_PreClimateStatus_RX));
    mEvClimateApp.mPreconditionSettings->setCurPreconditionSettingSignal(mEvClimateApp.mEvVif->queryCanData(Signal_FFHPreClimSetting_RX),
                                                                         mEvClimateApp.mEvVif->queryCanData(Signal_PreClimRangeComfort_RX));
    // request NGTP msg to EvtNgtp Service.
    mEvClimateApp.mEvNgtp->send_NgtpMessage(EvtNgtpMsgManager::PRECONDITON_UPDATE_ALERT, alert_Reason);
    // initialize alertreason.
    mAlertReason = UNKNOWN;
}

void EvClimateAppHandler::trigger_delayed_NGTP_PRECONDITON_UPDATE_ALERT()
{
    int32_t delayTime = mEvClimateApp.mEvConfig->getConfigData("ECC_CAN_STATUS_ALERT_TIME");
    if(delayTime == -1)
    {
        LOGE("[WARNNING] ECC_CAN_STATUS_ALERT_TIME is not defined in 'In Conotrol Config parameter'!! --> Setting defualt Value as 5sec");
        delayTime = 5;
    }
    else
    {
        LOGE("ECC_CAN_STATUS_ALERT_TIME : %d sec", delayTime);
    }

    //20170609 LCF provides as second unit but handler parameter should be set as milisecond
    delayTime = delayTime*1000;
    this->sendMessageDelayed(/*Message*/this->obtainMessage(/*What*/DELAYED_NGPT_ALERT), /*DelayTime MiliSec*/delayTime);
}