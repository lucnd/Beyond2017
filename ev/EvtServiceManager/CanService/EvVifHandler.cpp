#include "EvVifHandler.h"
#include "EvtUtil/EvtUtil.h"
#include "EvConfigData.h"

#define LOG_TAG "[EVT]EvVifHandler"
#include "Log.h"

#include "vifManagerService/LGE_CANSignalFrameID.h"
using namespace SignalFrame_PMZ_e;

#include "EvCanFrame.h"

// Handler for Asyn Signal Call aback pettern.
void EvVifHandler::handleMessage(const sp<sl::Message>& msg)
{
    switch(msg->what)
    {
        case CAN_TX:
        {
            break;
        }
        case CAN_ECHO:
        {
            sendCanEchoData(msg->arg1);
            break;
        }
        case CAN_FRAME_ECHO:
        {
            sendEchoFrameData(msg->obj);
            break;
        }
        default:
        {
            break;
        }
    }
}

#define CAN_QUERY(ID) mEvVifMgr.queryCanData(ID)
#define CAN_TX_SINGLE(ID, VALUE) do{\
                                    if(ID == -1) \
                                        {break;}\
                                    else\
                                        {mEvVifMgr.sendCanSignal(ID, VALUE);}\
                                }while(0)
#define STORE_ECHO_CAN(ID) EvConfigData::GetInstance()->setPropertyData(std::to_string(ID), std::to_string(CAN_QUERY(ID)))

void EvVifHandler::sendCanEchoData(uint32_t sig_Id)
{
    //###### PCR040 #########################
    //This code is depend on PCR041.
    if(sig_Id == Signal_PreClimateStatus_RX)
    {
        if(CAN_QUERY(sig_Id) != 0)
            return;
    }
    //#######################################

    if((sig_Id == Signal_BulkSoCTargetDisp_RX) ||(sig_Id == Signal_BulkSoCSingleUseDisp_RX))
    {
        uint32_t val = (uint32_t)mEvVifMgr.queryCanData(sig_Id);

        uint8_t tmpPermdata[8] = {0,};
        tmpPermdata[1] = val & 0xFF;
        tmpPermdata[0] = (val & 0xFF00) >> 8;
        mEvVifMgr.sendCanArraySignal(getEchoCanId(sig_Id), tmpPermdata);
    }
    else
    {
        CAN_TX_SINGLE(getEchoCanId(sig_Id), CAN_QUERY(sig_Id));
    }

    if(getEchoCanId(sig_Id) != -1) // If the sig_Id is valid for Echo CAN signal,
    {
        STORE_ECHO_CAN(sig_Id);
    }
}

int32_t EvVifHandler::getEchoCanId(uint32_t sig_Id)
{
    switch(sig_Id)
    {
        case Signal_PreClimateStatus_RX:
            return Signal_ParkClimateReqTCU_TX;
        /*
            Described on the CAN rx <-> CAN tx pair
        */
        case Signal_ChargeNowRequestExt_RX:  // ChargeSettings-ChargeNowSetting
            return Signal_ChargeNowReqExtUpdte2_TX;

        case Signal_ChargingModeChoice_RX:  // ChargeSettings-ChargingModeChoice
            return Signal_ChrgModeChoiceExtUpdte_TX;

        case Signal_BulkSoCTargetDisp_RX:  //ChargeSettings - PermanentMaxSOC
            return Signal_BulkSoCSetExtUpdate_TX;

        case Signal_BulkSoCSingleUseDisp_RX:  //ChargeSettings - OneOffMaxSoc
            return Signal_BulkSoCSingleUseExtUpd_TX;
        default:
            return -1;
    }
}

int32_t EvVifHandler::sendEchoFrameData(void* obj)
{
    EvCanFrame *tEvCanFrame = (EvCanFrame *)obj;
    uint16_t frame_Id = tEvCanFrame->mFrameID;

    switch(frame_Id)
    {
        case SignalFrame_GWM_PMZ_AD_Hybrid_RX:
        {
            send_Echo_GWM_PMZ_AD_Hybrid_Frame(tEvCanFrame);
            break;
        }
        case SignalFrame_GWM_PMZ_R_Hybrid_RX:
        {
            send_Echo_GWM_PMZ_R_Hybrid_Frame(tEvCanFrame);
            break;
        }
        case SignalFrame_GWM_PMZ_S_Hybrid_RX:
        {
            send_Echo_GWM_PMZ_S_Hybrid_Frame(tEvCanFrame);
            break;
        }
        case SignalFrame_GWM_PMZ_T_Hybrid_RX:
        {
            send_Echo_GWM_PMZ_T_Hybrid_Frame(tEvCanFrame);
            break;
        }
        case SignalFrame_GWM_PMZ_U_Hybrid_RX:
        {
            send_Echo_GWM_PMZ_U_Hybrid_Frame(tEvCanFrame);
            break;
        }
        case SignalFrame_PCM_PMZ_Q_Hybrid_RX:
        {
            /* call the Echo function. */
            break;
        }
        case SignalFrame_PCM_PMZ_S_Hybrid_RX:
        {
            /* call the Echo function. */
            break;
        }

        default:
            LOGE("[WARN] undefined CAN Frame ID[%d]", frame_Id);
            break;
    }

    delete tEvCanFrame; // this pointer had been created by EvVifManager.cpp

    return frame_Id;
}

#define CAN_PUSH_INIT() sp<vifCANContainer> _canContainer = new vifCANContainer(); int cnt = 0
#define CAN_PUSH(ID, DATA) mEvVifMgr.pushSingleData(_canContainer, ID, DATA)
#define ECHO_GWM_PMZ_CAN_PUSH(SIG) do{\
                                    CAN_PUSH(Signal_##SIG##ExtUpd_TX, tFrame->search(Signal_##SIG##_RX));\
                                    STORE_ECHO_CAN(Signal_##SIG##_RX);\
                                    cnt++;\
                                }while(0)
#define ECHO_GWM_PMZ_TARIFF_CAN_PUSH(IDX, TSIG, RSIG) do{\
                                    CAN_PUSH(Signal_TmdChrg##IDX##TSIG##ExtUpd_TX, tFrame->search(Signal_TimedCharge##IDX##RSIG##_RX));\
                                    STORE_ECHO_CAN(Signal_TimedCharge##IDX##RSIG##_RX);\
                                    cnt++;\
                                }while(0)
#define CAN_PUSH_END() mEvVifMgr.sendCanMultiSignal(cnt, _canContainer)

void EvVifHandler::send_Echo_GWM_PMZ_AD_Hybrid_Frame(EvCanFrame* tFrame)
{
    LOGV("[send_Echo_GWM_PMZ_AD_Hybrid_Frame]");
    CAN_PUSH_INIT();

    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActFri);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActive);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActMon);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActSat);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActSun);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActThu);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActTue);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeActWed);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeDate);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeHour);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeIndex);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeMins);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeMnth);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeType);
    ECHO_GWM_PMZ_CAN_PUSH(PreDepTimeYear);

    CAN_PUSH_END();
}

void EvVifHandler::send_Echo_GWM_PMZ_R_Hybrid_Frame(EvCanFrame* tFrame)
{
    LOGV("[send_Echo_GWM_PMZ_R_Hybrid_Frame]");
    CAN_PUSH_INIT();

    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, Actv, Active);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvFri, ActiveFri);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvMon, ActiveMon);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvSat, ActiveSat);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvSun, ActiveSun);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvThur, ActiveThur);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvTues, ActiveTues);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ActvWed, ActiveWed);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, EndTimeA, EndTimeA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, EndTimeB, EndTimeB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, EndTimeC, EndTimeC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, EndTimeD, EndTimeD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ZoneA, ZoneA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ZoneB, ZoneB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ZoneC, ZoneC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ZoneD, ZoneD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(1, ZoneE, ZoneE);

    CAN_PUSH_END();
}

void EvVifHandler::send_Echo_GWM_PMZ_S_Hybrid_Frame(EvCanFrame* tFrame)
{
    LOGV("[send_Echo_GWM_PMZ_S_Hybrid_Frame]");
    CAN_PUSH_INIT();

    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, Actv, Active);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvFri, ActiveFri);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvMon, ActiveMon);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvSat, ActiveSat);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvSun, ActiveSun);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvThur, ActiveThur);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvTues, ActiveTues);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ActvWed, ActiveWed);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, EndTimeA, EndTimeA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, EndTimeB, EndTimeB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, EndTimeC, EndTimeC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, EndTimeD, EndTimeD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ZoneA, ZoneA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ZoneB, ZoneB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ZoneC, ZoneC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ZoneD, ZoneD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(2, ZoneE, ZoneE);

    CAN_PUSH_END();
}

void EvVifHandler::send_Echo_GWM_PMZ_T_Hybrid_Frame(EvCanFrame* tFrame)
{
    LOGV("[send_Echo_GWM_PMZ_T_Hybrid_Frame]");
    CAN_PUSH_INIT();

    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, Actv, Active);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvFri, ActiveFri);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvMon, ActiveMon);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvSat, ActiveSat);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvSun, ActiveSun);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvThur, ActiveThur);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvTues, ActiveTues);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ActvWed, ActiveWed);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, EndTimeA, EndTimeA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, EndTimeB, EndTimeB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, EndTimeC, EndTimeC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, EndTimeD, EndTimeD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ZoneA, ZoneA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ZoneB, ZoneB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ZoneC, ZoneC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ZoneD, ZoneD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(3, ZoneE, ZoneE);

    CAN_PUSH_END();
}

void EvVifHandler::send_Echo_GWM_PMZ_U_Hybrid_Frame(EvCanFrame* tFrame)
{
    LOGV("[send_Echo_GWM_PMZ_U_Hybrid_Frame]");
    CAN_PUSH_INIT();

    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, Actv, Active);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvFri, ActiveFri);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvMon, ActiveMon);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvSat, ActiveSat);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvSun, ActiveSun);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvThur, ActiveThur);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvTues, ActiveTues);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ActvWed, ActiveWed);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, EndTimeA, EndTimeA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, EndTimeB, EndTimeB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, EndTimeC, EndTimeC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, EndTimeD, EndTimeD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ZoneA, ZoneA);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ZoneB, ZoneB);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ZoneC, ZoneC);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ZoneD, ZoneD);
    ECHO_GWM_PMZ_TARIFF_CAN_PUSH(4, ZoneE, ZoneE);

    CAN_PUSH_END();
}
