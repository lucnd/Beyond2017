#include <cmath>
#include "EvtVifManager.h"
#include "EvChargeApp.h"
#include "EvClimateApp.h"
#include "EvtUtil/EvtUtil.h"
#include "EvVifHandler.h"
#include "EvVifReceiverTx.h"
#include "vifManagerService/LGE_CANSignalFrameID.h"

using namespace SignalFrame_PMZ_e;

#define LOG_TAG "[EVT]EvtVifManager"
#include "Log.h"

inline void setCanName();
inline uint32_t getCanData(Buffer& buf);
static const char* pCanName[Signal_END_OF_DEFINED] = {0,};

EvtVifManager::EvtVifManager(){
    setCanName();
    m_Looper = SLLooper::myLooper();
    mHandler = new EvVifHandler(m_Looper, *this);

    mVifReceiverTx = new VifReceiverTx(*this);
    mVifSvcTx = SET_SERVICE(vifManagerServiceTX);
}

const char* EvtVifManager::getCanName(uint16_t canID)
{
    return pCanName[canID];
}

int32_t EvtVifManager::queryCanData(int sigID)
{
    sp<vifCANSigRequest> canSigReq = new vifCANSigRequest(sigID);
    if(m_vifMgrService->getCANDataFromVifmgr(1, canSigReq, 0x00) == E_OK)
    {
        if (canSigReq->getCANSigDataSize() > 0 && canSigReq->getCANSigDataPtr() != NULL)
        {
            Buffer _buf;
            _buf.setTo(canSigReq->getCANSigDataPtr(), canSigReq->getCANSigDataSize());
            return getCanData(_buf);
        }
        else
        {
            LOGE("getCurrCANData [%s][%d] sigdata size(%d) is 0 or sigdata ptr is NULL", getCanName(sigID), sigID, canSigReq->getCANSigDataSize());
            return -1;
        }
    }

    LOGE("getCurrCANData [%s][%d] size(%d) is 0 or sigdata ptr is NULL", getCanName(sigID), sigID, canSigReq->getCANSigDataSize());
    return -1;
}

error_t EvtVifManager::sendCanArraySignal(int sigID, const uint8_t data[])
{
    sp<vifCANContainer> indata = new vifCANContainer();

    // Temprary guard code for VIF Side code issue.
    // Please maintain HARD code (MAGIC NUMBER) until Fixing VIF side code!!!!! - 20161229
    if(sigID == Signal_BulkSoCSetExtUpdate_TX || sigID == Signal_BulkSoCSingleUseExtUpd_TX)
    {
        // HARD CODE.
        indata->setData(sigID, 2, data);
        LOGE("CAN[%s][%d] - HARD CODED GUARD CODE UNTIL Fixing VIF SIDE!! [2]",getCanName(sigID), sigID);
    }
    else
    {
        // ORIGINAL CODE
        indata->setData(sigID, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID), data);
        LOGE("CAN[%s] - GET_CAN_SIGNAL_VALUE_BYTE_SIZE(%d)=[%d]",getCanName(sigID), sigID, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID));
    }


    uint8_t result = m_vifMgrService->send_CANdataToMicom(indata->NumofSignals, indata);
    if(result != E_OK)
    {
        LOGE("[ERROR] CAN signal[%d] Tx has failed. please check VIF side error[%d]", sigID, result);
    }
    else
    {
        LOG_EV("CAN [%s] was sent [%02X %02X %02X %02X %02X %02X %02X %02X]", getCanName(sigID), data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]);
    }
    return result;
}

error_t EvtVifManager::sendCanSignal(int sigID, const uint8_t data)
{
    if(GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID) == 1)
    {
        LOGE("[WARN] This CAN[%d] would be missed partially.(Not side 1byte)");
    }
    uint8_t tmpdata[CAN_VALUES_MAX_BUF_SIZE] = {0,};
    sp<vifCANContainer> indata = new vifCANContainer();
    tmpdata[0] = data;
    indata->setData(sigID, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID), tmpdata);
    uint8_t result = m_vifMgrService->send_CANdataToMicom(indata->NumofSignals, indata);
    if(result != E_OK)
    {
        LOGE("[ERROR] CAN signal[%d] Tx has failed. please check VIF side error[%d]", sigID, result);
    }
    else
    {
        LOG_EV("CAN [%s] was sent [%d]",getCanName(sigID), data);
    }
    return result;
}

error_t EvtVifManager::cancelCanMultiSignal_EH(uint32_t frameIdentifier)
{
    sp<IvifManagerReceiverTX> emptyVifTxRcv; // VIF guide line code. 2017/09/14
    sp<vifCANContainer> emptySigData = new vifCANContainer();  // VIF New guid line. 2017/10/16
    emptySigData->numbering = frameIdentifier;
    uint8_t result = mVifSvcTx->send_CANdataToMicom(0, emptySigData, mAppId, emptyVifTxRcv);
    return result;
}

error_t EvtVifManager::sendCanMultiSignal_EH(uint32_t NumofSignals, sp<vifCANContainer>& sigdata)
{
    sp<IvifManagerReceiverTX> emptyVifTxRcv; // VIF guide line code.  2017/09/14
    uint8_t result = mVifSvcTx->send_CANdataToMicom(NumofSignals, sigdata, mAppId, emptyVifTxRcv);

    if(result != E_OK)
    {
        LOGE("[ERROR] Multiple CAN signals Tx (EH) have failed. please check VIF side error[%d]", result);
    }
    return result;
}

error_t EvtVifManager::sendCanMultiSignal(uint32_t NumofSignals, sp<vifCANContainer>& sigdata)
{
    //TODO: forward to EvtVifHandler for ansynic processing.
    uint8_t result = m_vifMgrService->send_CANdataToMicom(NumofSignals, sigdata);
    if(result != E_OK)
    {
        LOGE("[ERROR] Multiple CAN signals Tx have failed. please check VIF side error[%d]", result);
    }
    return result;
}

void EvtVifManager::pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint32_t data)
{
    uint8_t tmpdata[8] = {0,};
    setByteDataFromInterger(tmpdata, data, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID));
    sigdata->setData(sigID, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(sigID), tmpdata);
}

void EvtVifManager::setByteDataFromInterger(uint8_t _byteData[], uint32_t _intData, uint8_t size)
{
    switch(size)
    {
        case 1:
            _byteData[0] = _intData & 0xFF;
            break;
        case 2:
            _byteData[0] = (_intData & 0xFF00) >> 8;
            _byteData[1] = _intData & 0x00FF;
            break;
        default:
            break;
    }
}

void EvtVifManager::receiveCAN(int32_t Sigid, Buffer& buf)
{
    //Buffer *buf = new Buffer(buffer);
    if( buf.size() < 1 )
    {
        LOGE("CAN signal[0x%02X] received without data.", Sigid);
        return;
    }

    // PCR044
    bool bChargeSettingSignal = isChargeSettingSignal(Sigid);
    bool bStoreChargeSetting = isStoreChargeSetting();

    if((bChargeSettingSignal) && (!bStoreChargeSetting))
    {
        LOGE("Not received - [%s] bCSSignal[%s] bStoreCS[%s]"
                , getCanName(Sigid)
                , bChargeSettingSignal ? "T" : "F"
                , bStoreChargeSetting ? "T" : "F");
        return;
    }

    LOGI("received [%s][%d] = %d ", getCanName(Sigid), Sigid, getCanData(buf));

    // if(isEchoNecessary(Sigid))
    // {
    //     LOGI("Sending ECHO [%s]", getCanName(Sigid));
    #if defined(PCR041ENABLE)
    mHandler->obtainMessage(CAN_ECHO, Sigid)->sendToTarget();
    #endif
    // }

    EvChargeApp *mEvc = EvChargeApp::GetInstance();
    EvClimateApp *mEcc = EvClimateApp::GetInstance();
    bool bValueChanged = true;

    switch(Sigid){
        case Signal_EVRangeDisp_RX:
        case Signal_EVRangeMilesDisp_RX:
        case Signal_CombinedRangeDisp_RX:
        case Signal_CombinedRangeMilesDisp_RX:
        case Signal_TimeToFullSoC_RX:
        case Signal_TimeToBulkSoC_RX:
        case Signal_HVBattChrgeRateSOCDisp_RX:
        case Signal_HVBattChrgRateMileDisp_RX:
        case Signal_HVBattChargeRateKmDisp_RX:
        case Signal_kWhConsumedLastCharge_RX:
        case Signal_VSCInitialHVBattEnergy_RX:
        case Signal_VSCRegenEnergyAvailable_RX:
        case Signal_VSCRevisedHVBattEnergy_RX:
        case Signal_VSCHVEnergyAscent_RX:
        case Signal_VSCHVEnergyDescent_RX:
        case Signal_VSCHVEnergyTimePen_RX:
        case Signal_VSCRegenEnergyFactor_RX:
        case Signal_VSCVehAccelFactor_RX:
        case Signal_VSCRangeMapRefactrComf_RX:
        case Signal_VSCRangeMapRefactrEco_RX:
        case Signal_VSCRangeMapRefactrGMH_RX:
        case Signal_VSCHVBattConsumpSpd1_RX:
        case Signal_VSCHVBattConsumpSpd2_RX:
        case Signal_VSCHVBattConsumpSpd3_RX:
        case Signal_VSCHVBattConsumpSpd4_RX:
        case Signal_VSCHVBattConsumpSpd5_RX:
        case Signal_VSCHVBattConsumpSpd6_RX:
        case Signal_VSCHVBattConsumpSpd7_RX:
        case Signal_EVRangePredictStatus_RX:
        case Signal_EVRangeComfort_RX:
        case Signal_EVRangeEco_RX:
        case Signal_EVRangeGetMeHome_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            break;
        }
        case Signal_WlChargingStatusDisp_RX:
        {
            //ECW
            EvChargeApp::GetInstance()->triggerECWstate();

            int32_t convertedNgtpVal = EvtElectricVehicleInformation::GetInstance()->convertWLCSFromCanToNgtp(getCanData(buf));
            if(isValueChanged(Sigid, EvtElectricVehicleInformation::GetInstance()->bWlChargingStatus_is_present
                          , EvtElectricVehicleInformation::GetInstance()->getWlChargingStatusField()
                          , convertedNgtpVal))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->rcv_ChargingStatusDisp();
                mEvc->triggerUpdateAlertCondition(Sigid);
            }

            break;
        }
        case Signal_ChargingStatusDisp_RX:
        {
            //ECW
            EvChargeApp::GetInstance()->triggerECWstate();

            int32_t convertedNgtpVal = EvtElectricVehicleInformation::GetInstance()->convertCSFromCanToNgtp(getCanData(buf));
            if(isValueChanged(Sigid, EvtElectricVehicleInformation::GetInstance()->bChargingStatus_is_present
                          , EvtElectricVehicleInformation::GetInstance()->getChargingStatusField()
                          , convertedNgtpVal))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->rcv_ChargingStatusDisp();
                mEvc->triggerUpdateAlertCondition(Sigid);
            }

            break;
        }
        case Signal_ChargePwrInterruptDisp_RX:
        case Signal_ChrgNotReadyByDepDisp_RX:
        case Signal_ChargePlugDiscAlarmReq_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEvc->triggerUpdateAlertCondition(Sigid);
            break;
        }

        case Signal_HVBatteryUsableSOCDisp_RX:
        {
            if(isValueChanged(Sigid, EvtElectricVehicleInformation::GetInstance()->bBatteryStatus_is_present
                          , EvtElectricVehicleInformation::GetInstance()->getStateOfChargeField()
                          , getCanData(buf)))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->rcv_HVBatteryUsableSOCDisp(buf);
            }
            break;
        }
        case Signal_PreClimateStatus_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_PreClimateStatus();
            break;
        }
        case Signal_FFHOperatingStatus_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_FFHOperatingStatus(buf);
            break;
        }
        case Signal_HVBattHeatingStatus_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_HVBattHeatingStatus(buf);
            break;
        }
        case Signal_HVBattCoolingStatusExt_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_HVBattCoolingStatusExt(buf);
            break;
        }
        case Signal_PreClimateRequests_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_PreClimateRequests(buf);
            break;
        }
        case Signal_BulkSoCTargetDisp_RX:  // ChargeSettings - MaxSOC
        {
            int32_t tmpValue = (getCanData(buf) > 1000) ? -1 : getCanData(buf)/10;
            if(isValueChanged(Sigid
                            , EvtChargeSettings::GetInstance()->bPermanentMaxSoc_is_updated
                            , EvtChargeSettings::GetInstance()->getPermanentMaxSocField()
                            , tmpValue))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->triggerUpdateAlertCondition(Sigid);
                mEvc->send_ChargingSettingsUpdate();
            }
            break;
        }
        case Signal_BulkSoCSingleUseDisp_RX:  // ChargeSettings - MaxSOC
        {
            int32_t tmpValue = (getCanData(buf) > 1000) ? -1 : getCanData(buf)/10;
            if(isValueChanged(Sigid, EvtChargeSettings::GetInstance()->bOneOffMaxSoc_is_updated
                           , EvtChargeSettings::GetInstance()->getOneOffMaxSocField()
                           , tmpValue))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->triggerUpdateAlertCondition(Sigid);
                mEvc->send_ChargingSettingsUpdate();
            }
            break;
        }
        case Signal_ChargingModeChoice_RX:    // ChargeSettings - ChargeModeChoice
        {
            int32_t tmpOldVal = (int32_t)EvtChargeSettings::GetInstance()->getChargingModeChoiceField();
            int32_t tmpNewVal = (getCanData(buf) > 2) ? -1 : getCanData(buf) ;
            if(isValueChanged(Sigid, EvtChargeSettings::GetInstance()->bChargingModeChoice_is_present, tmpOldVal, tmpNewVal))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->send_ChargingSettingsUpdate();
            }
            break;
        }
        case Signal_ChargeNowRequestExt_RX:  // ChargeSettings - ChargeNowSetting
        {
            int32_t tmpOldVal = (int32_t)EvtChargeSettings::GetInstance()->getChargeNowSettingField();
            int32_t tmpNewVal = (getCanData(buf) > 3) ? -1 : getCanData(buf) ;
            if(isValueChanged(Sigid, EvtChargeSettings::GetInstance()->bChargeNowSetting_is_present, tmpOldVal, tmpNewVal))
            {
                storeElectricVehicleInformationFromCanData(Sigid, buf);
                mEvc->send_ChargingSettingsUpdate();
            }
            break;
        }
        case Signal_FFHPreClimSetting_RX:       // SRD_ReqECC0122_v1, Fix Harmony JLRTCU-1940
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_FFHPreClimSetting(buf);
            break;
        }
        case Signal_PreClimRangeComfort_RX:
        {
            storeElectricVehicleInformationFromCanData(Sigid, buf);
            mEcc->rcv_CAN_PreClimRangeComfort(buf);
            break;
        }
        default:
        {
            break;
        }
    }
}

void EvtVifManager::receiveCAN_Frame(EvCanFrame *pFrame)
{
    EvCanFrame *pEvCanFrame = new EvCanFrame(pFrame); // CAUTION :  this pointer should be deleted at EvtVifHandler.cpp
    uint16_t frameID = pEvCanFrame->mFrameID;

    for(auto tCanInfo : pEvCanFrame->mCanList)
    {
        LOG_EV("FrameID:[%d] Signal:[%d] Data:[%d]",pEvCanFrame->mFrameID, tCanInfo.mCanId, tCanInfo.mCanData[0]);
    }

    bool bChargeSettingFrame = isChargeSettingFrame(frameID);
    bool bStoreChargeSetting = isStoreChargeSetting();

    if((bChargeSettingFrame) && (!bStoreChargeSetting))
    {
        LOGE("[receiveCAN_Frame] Not Receiving - FRAME[%d] bCSFrame[%s] bStoreCS[%s]"
        , frameID
        , bChargeSettingFrame ? "T" : "F"
        , bStoreChargeSetting ? "T" : "F");
        return;
    }
    else
    {
        LOGV("[receiveCAN_Frame] Receiving - FRAME[%d] bCSFrame[%s] bStoreCS[%s]"
        , frameID
        , bChargeSettingFrame ? "T" : "F"
        , bStoreChargeSetting ? "T" : "F");
        EvChargeApp::GetInstance()->rcv_CAN_Frame(pEvCanFrame);
        #if defined(PCR041ENABLE)
            sp<sl::Message> message = mHandler->obtainMessage(CAN_FRAME_ECHO, (void*)pEvCanFrame);
            message->sendToTarget();
        #endif
    }
}

/* toString Example code.
sp<String> CellIdentityLte::toString() {
    StringWrapper s = StringWrapper("CellIdentityLte:{");
    s.appendFormatted(" mMcc=%d", mMcc);
    s.appendFormatted(" mMnc=%d", mMnc);
    s.appendFormatted(" mCi=%d", mCi);
    s.appendFormatted(" mPci=%d", mPci);
    s.appendFormatted(" mTac=%d", mTac);
    s.append("}");

    return new String(s.c_str());
}
*/

////////////////////Private Method//////////////////////////
uint32_t IsCorrectBitRange(uint32_t value, uint8_t bit)
{
    uint32_t rangeValue = std::pow(2, bit);
    if(rangeValue <= value)
    {
        LOGE("[Error] Received CAN value[%d] is over than [%d][%d bit]. return [%d]", value, rangeValue, bit, rangeValue-1);
        return rangeValue-1;
    }
    return value;
}

void EvtVifManager::storeElectricVehicleInformationFromCanData(uint16_t Sigid, Buffer& buf)
{
    uint32_t canData = getCanData(buf);
    switch(Sigid)
    {
        ////////////////////EvtElectricVehicleInformation////////////////////////
        case Signal_ChargingStatusDisp_RX:  //chargingStatus
        {
            EvtElectricVehicleInformation::GetInstance()->setChargingStatusFromCAN(IsCorrectBitRange(canData, 4));
            break;
        }
        case Signal_WlChargingStatusDisp_RX: // wireless ChargingStatus
        {
            // SRD_ReqEVC0047_v2 - WlChargeStatus is added to ElectricVehicleInformation
            EvtElectricVehicleInformation::GetInstance()->setWlChargingStatusFromCAN(IsCorrectBitRange(canData, 4));
            break;
        }
        case Signal_EVRangeDisp_RX:         //rangeOnBatteryKm
        {
            //EvtElectricVehicleInformation::GetInstance()->setRangeOnBatteryKmField(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setRangeOnBatteryKmField(IsCorrectBitRange(canData, 10));
            break;
        }
        case Signal_EVRangeMilesDisp_RX:    //rangeOnBatteryMiles
        {
            //EvtElectricVehicleInformation::GetInstance()->setRangeOnBatteryMilesFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setRangeOnBatteryMilesFromCAN(IsCorrectBitRange(canData,10));
            break;
        }
        case Signal_CombinedRangeDisp_RX:   //rangeCombinedKm
        {
            //EvtElectricVehicleInformation::GetInstance()->setRangeCombinedKmFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setRangeCombinedKmFromCAN(IsCorrectBitRange(canData,11));
            break;
        }
        case Signal_CombinedRangeMilesDisp_RX: //rangeCombinedMiles
        {
            //EvtElectricVehicleInformation::GetInstance()->setRangeCombinedMilesFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setRangeCombinedMilesFromCAN(IsCorrectBitRange(canData,11));
            break;
        }
        case Signal_HVBatteryUsableSOCDisp_RX: //stateOfCharge
        {
            uint32_t value = IsCorrectBitRange(canData,7) ;
            value = (value > 100) ? 100 : value;
            EvtElectricVehicleInformation::GetInstance()->setStateOfChargeFromCAN(value);
            break;
        }
        case Signal_TimeToFullSoC_RX:  //minutesToFullyCharged
        {
            IsCorrectBitRange(canData, 16);
            EvtElectricVehicleInformation::GetInstance()->setMinutesToFullyChargedFromCAN(buf.data()[0], buf.data()[1]);
            break;
        }
        case Signal_TimeToBulkSoC_RX:  //minutesToBulkCharged
        {
            IsCorrectBitRange(canData, 6);
            EvtElectricVehicleInformation::GetInstance()->setMinutesToBulkChargedFromCAN(buf.data()[0], buf.data()[1]);
            break;
        }
        case Signal_HVBattChrgeRateSOCDisp_RX: // chargingRateSocPerHour
        {
            //EvtElectricVehicleInformation::GetInstance()->setChargingRateSocPerHourFromCAN(*(uint32_t*)canBlock);
            //uint32_t value = IsCorrectBitRange(canData,11) / 10; // down scaling. ESIS_TCU_C spec.
            // SRD_ReqEVC0061_v1
            uint32_t value = IsCorrectBitRange(canData,11) ; // down scaling. ESIS_TCU_C spec but multiply x10. (NGTP Signal changed to chargingRateSocPerHourx10)
            value = (value > 2047) ? 2047 : value; //IF spec --> -- Amount of SOC being added per hour. -1=unknown chargingRateSocPerHourx10   INTEGER (-1..10000),
            EvtElectricVehicleInformation::GetInstance()->setChargingRateSocPerHourFromCAN(value);
            break;
        }
        case Signal_HVBattChrgRateMileDisp_RX: //chargingRateMilesPerHour
        {
            //EvtElectricVehicleInformation::GetInstance()->setChargingRateMilesPerHourFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setChargingRateMilesPerHourFromCAN(IsCorrectBitRange(canData,12));
            break;
        }
        case Signal_HVBattChargeRateKmDisp_RX: // chargingRateKmPerHour
        {
            //EvtElectricVehicleInformation::GetInstance()->setChargingRateKmPerHourFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setChargingRateKmPerHourFromCAN(IsCorrectBitRange(canData,12));
            break;
        }
        case Signal_kWhConsumedLastCharge_RX:   // energyConsumedLastChargekWh
        {
            //EvtElectricVehicleInformation::GetInstance()->setEnergyConsumedLastChargekWhFromCAN(*(uint32_t*)canBlock);
            EvtElectricVehicleInformation::GetInstance()->setEnergyConsumedLastChargekWhFromCAN(IsCorrectBitRange(canData,11));
            break;
        }
		//EvtChargeSettings
		case Signal_ChargeNowRequestExt_RX:  // ChargeNowReqExt
        {
            EvtChargeSettings::GetInstance()->setChargeNowSettingField(IsCorrectBitRange(canData,11));

            // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
            EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());
            break;
        }
		case Signal_ChargingModeChoice_RX:   // ChargingModeChoice
        {
            EvtChargeSettings::GetInstance()->setChargingModeChoiceField(IsCorrectBitRange(canData,1));

            // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
            EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());
            break;
        }
		case Signal_BulkSoCTargetDisp_RX:   // MaxStateOfCharge - permanentMaxSoc
        {
            // SRD_ReqEVC0065_v1
            LOGV("Signal_BulkSoCTargetDisp_RX Received");
            uint32_t value = IsCorrectBitRange(canData,10) ;
            value = value > 1000 ? -1 : value/10 ;
            EvtChargeSettings::GetInstance()->setPermanentMaxSocField(value);

            if(value == 0)
            {
                EvtChargeSettings::GetInstance()->setPermanentMaxSocStatus(2); // clear
            }
            else
            {
                // 20170816 Perm and Oneoff should not be shown together. So Incase of PermMaxSoc, oneOff should be noChange
                EvtChargeSettings::GetInstance()->setPermanentMaxSocStatus(3); // maxSOC
                EvtChargeSettings::GetInstance()->setOneOffMaxSocStatus(1); // noChange
            }

            // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
            EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());
            break;
        }
		case Signal_BulkSoCSingleUseDisp_RX:  // MaxStateOfCharge -oneOffMaxSoc
        {
            // SRD_ReqEVC0066_v1
            LOGV("Signal_BulkSoCSingleUseDisp_RX Received");
            uint32_t value = IsCorrectBitRange(canData,10) ;
            value = value > 1000 ? -1 : value/10 ;
            EvtChargeSettings::GetInstance()->setOneOffMaxSocField(value);

            if(value == 0)
            {
                EvtChargeSettings::GetInstance()->setOneOffMaxSocStatus(2); // clear
            }
            else
            {
                // 20170816 Perm and Oneoff should not be shown together. So Incase of oneOffMaxSOC, Perm should be noChange
                EvtChargeSettings::GetInstance()->setOneOffMaxSocStatus(3); // maxSOC
                EvtChargeSettings::GetInstance()->setPermanentMaxSocStatus(1); // noChange
            }

            // SRD_ReqEVC0035_v2 - setLatestTimestamp to Compare Upcoming TSP Message TimeStamp.
            EvtChargeSettings::GetInstance()->setLatestTimeStamp(UTIL::get_NGTP_time());
            break;
        }
        // TODO CSW Partially applied NGTP Ver 3.11 for EV 3.3. It will be fully applied soon.
        #if SVC_MINOR_VER >= 11 && SVC_MAJOR_VER == 3
        LOGE("[EVTVIF] NGTP MINOR VER > 11");
         //EVRangeMapInformation
        case Signal_VSCInitialHVBattEnergy_RX:
        {
            // 0.05(CAN Scale) * 100(NGTP Scale)
            EvtElectricVehicleInformation::GetInstance()->setVscInitialHvBattEnergyField((IsCorrectBitRange(canData,11))*5);
            break;
        }
        case Signal_VSCRegenEnergyAvailable_RX:
        {
            // 0.05(CAN Scale) * 100(NGTP Scale)
            EvtElectricVehicleInformation::GetInstance()->setVscRegenEnergyAvailableField((IsCorrectBitRange(canData,11))*5);
            break;
        }
        case Signal_VSCRevisedHVBattEnergy_RX:
        {
            // 0.05(CAN Scale) * 100(NGTP Scale)
            EvtElectricVehicleInformation::GetInstance()->setVscRevisedHvBattEnergyField(IsCorrectBitRange(canData,11)*5);
            break;
        }
        case Signal_VSCHVEnergyAscent_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyAscentField((IsCorrectBitRange(canData,7)));
    	  break;
    	}
        case Signal_VSCHVEnergyDescent_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyDescentField((IsCorrectBitRange(canData,7)));
    	  break;
    	}
        case Signal_VSCHVEnergyTimePen_RX:
    	{
          // 0.03(CAN Scale) * 100(NGTP Scale)
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyTimePenField((IsCorrectBitRange(canData,5))*3);
    	  break;
    	}
        case Signal_VSCRegenEnergyFactor_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRegenEnergyFactorField((IsCorrectBitRange(canData,4))*10);
    	  break;
    	}
    	case Signal_VSCVehAccelFactor_RX:
        {
          EvtElectricVehicleInformation::GetInstance()->setVscVehAccelFactorField((IsCorrectBitRange(canData,5))*10+100);
    	  break;
    	}
        case Signal_VSCRangeMapRefactrComf_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrComfField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCRangeMapRefactrEco_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrEcoField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCRangeMapRefactrGMH_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrGmhField(IsCorrectBitRange(canData,7));
    	  break;
    	}
    	case Signal_VSCHVBattConsumpSpd1_RX:
    	{
          // VSCHVBattConsumpSpd1 ~ 7 : CAN Scale : 6, Offset : 100
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd1Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd2_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd2Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd3_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd3Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd4_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd4Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd5_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd5Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd6_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd6Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd7_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd7Field((IsCorrectBitRange(canData,7))*6 + 100);
    	  break;
    	}
        #else
         //EVRangeMapInformation
         LOGE("[EVTVIF] NGTP MINOR VER : under 11");
        case Signal_VSCInitialHVBattEnergy_RX:
        {
            EvtElectricVehicleInformation::GetInstance()->setVscInitialHvBattEnergyField(IsCorrectBitRange(canData,11));
            break;
        }
        case Signal_VSCRevisedHVBattEnergy_RX:
        {
            EvtElectricVehicleInformation::GetInstance()->setVscRevisedHvBattEnergyField(IsCorrectBitRange(canData,11));
            break;
        }
        case Signal_VSCHVEnergyAscent_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyAscentField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVEnergyDescent_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyDescentField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVEnergyTimePen_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvEnergyTimePenField(IsCorrectBitRange(canData,5));
    	  break;
    	}
        case Signal_VSCRegenEnergyFactor_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRegenEnergyFactorField(IsCorrectBitRange(canData,4));
    	  break;
    	}
    	case Signal_VSCVehAccelFactor_RX:
        {
          EvtElectricVehicleInformation::GetInstance()->setVscVehAccelFactorField(IsCorrectBitRange(canData,5));
    	  break;
    	}
        case Signal_VSCRangeMapRefactrComf_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrComfField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCRangeMapRefactrEco_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrEcoField(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCRangeMapRefactrGMH_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVscRangeMapRefactrGmhField(IsCorrectBitRange(canData,7));
    	  break;
    	}
    	case Signal_VSCHVBattConsumpSpd1_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd1Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd2_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd2Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd3_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd3Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd4_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd4Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd5_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd5Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd6_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd6Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        case Signal_VSCHVBattConsumpSpd7_RX:
    	{
          EvtElectricVehicleInformation::GetInstance()->setVschvBattConsumpSpd7Field(IsCorrectBitRange(canData,7));
    	  break;
    	}
        #endif
        case Signal_EVRangePredictStatus_RX:
        {
           EvtElectricVehicleInformation::GetInstance()->setEvRangePredicStatusField(IsCorrectBitRange(canData,2));
           break;
        }
        case Signal_EVRangeComfort_RX:
        {
           EvtElectricVehicleInformation::GetInstance()->setEvRangeComfortField(IsCorrectBitRange(canData,14));
           break;
        }
        case Signal_EVRangeEco_RX:
        {
           EvtElectricVehicleInformation::GetInstance()->setEvRangeECOField(IsCorrectBitRange(canData,14));
           break;
        }
        case Signal_EVRangeGetMeHome_RX:
        {
           EvtElectricVehicleInformation::GetInstance()->setEvRangeGetMeHomeField(IsCorrectBitRange(canData,14));
           break;
        }
        /////////////////////////////////////////////////////////////////////////
        default:
        {
            break;
        }
    }
}

inline uint32_t getCanData(Buffer& buf)
{
    uint8_t buff_size = buf.size();
    uint32_t buff_data = 0;

    switch(buff_size)
    {
        case 1:
        {
            buff_data = buf.data()[0];
            break;
        }
        case 2:
        {
            buff_data = ((buf.data()[1]) | (buf.data()[0] << 8));
            break;
        }
        case 3:
        {
            buff_data = (buf.data()[2]) | (buf.data()[1] << 8) | (buf.data()[0] << 16);
            break;
        }
        case 4:
        {
            buff_data = ((buf.data()[3]) | (buf.data()[2] << 8) | (buf.data()[1] << 16) | (buf.data()[0] << 24));
            break;
        }
        default:
            break;
    }

    return buff_data;
}

bool EvtVifManager::isValueChanged(int32_t Sigid, bool bUpdatedBefore, int32_t oldValue, int32_t newValue)
{
    bool bSameValue = (oldValue == newValue) ? true : false;
    bool result = false;

    if(bUpdatedBefore)
    {
        if(!bSameValue)
        {
            LOGV("[isValueChanged] True %s - %d - %d", getCanName(Sigid), oldValue, newValue);
            result = true;
        }
        else
        {
            LOGE("[isValueChanged] False %s - %d - %d", getCanName(Sigid), oldValue, newValue);
        }
    }
    else
    {
        LOGV("[isValueChanged] True First Receive - %s", getCanName(Sigid));
        result = true;
    }
    return result;
}

bool EvtVifManager::isStoreChargeSetting()
{
    int32_t chargeNowReq = EvtVifManager::GetInstance()->queryCanData(Signal_ChargeNowRequest_RX);
    return (chargeNowReq == 1) ? true : false;
}

bool EvtVifManager::isChargeSettingFrame(uint16_t frameID)
{
    //Decide if the CAN Frame is for ChargeSettings or not.
    bool result = false;

    switch(frameID)
    {
        case SignalFrame_GWM_PMZ_R_Hybrid_RX:
        case SignalFrame_GWM_PMZ_S_Hybrid_RX:
        case SignalFrame_GWM_PMZ_T_Hybrid_RX:
        case SignalFrame_GWM_PMZ_U_Hybrid_RX:
        case SignalFrame_GWM_PMZ_AD_Hybrid_RX:
        {
            result = true;
            break;
        }
        default:
        {
            break;
        }
    }
    return result;
}

bool EvtVifManager::isChargeSettingSignal(int32_t sigID)
{
    //Decide if the CAN Signal is ChargeSetting or not.
    bool result = false;

    switch(sigID)
    {
        case Signal_ChargeNowRequestExt_RX:
        case Signal_ChargingModeChoice_RX:
        case Signal_BulkSoCTargetDisp_RX:
        case Signal_BulkSoCSingleUseDisp_RX:
        {
            result = true;
            break;
        }
        default:
        {
            break;
        }
    }
    return result;
}

// #if defined(PCR041ENABLE)
// void EvtVifManager::triggerHandler(int32_t what, int32_t arg1)
// {
//     LOGV("[triggerHandler]");
//     mHandler->obtainMessage(what, arg1)->sendToTarget();
// }
// #endif

// void EvtVifManager::InitEchoCanSigVec()
// {
//     // Add to This Vector if the CAN signal needs echo(Except EVC Signals)
//     // Ex) echoCanSigIdVec.push_back(Signal_ChargeNowRequestExt_RX);
// }

// void EvtVifManager::InitEchoEVCCanSigVec()
// {
//     // Add to This Vector if the CAN signal needs echo (ONLY EVC Signals)
//     echoEVCCanSigIdVec.push_back(Signal_ChargeNowRequestExt_RX);
//     echoEVCCanSigIdVec.push_back(Signal_ChargingModeChoice_RX);
//     echoEVCCanSigIdVec.push_back(Signal_BulkSoCTargetDisp_RX);
//     echoEVCCanSigIdVec.push_back(Signal_BulkSoCSingleUseDisp_RX);
// }

// bool EvtVifManager::isEchoNecessary(int32_t sigId)
// {
//     bool bEchoNecessary = false;

//     //check if echoCanSigIdVec contains signal ID echoEVCCanSigIdVec
//     if ( (std::find(echoCanSigIdVec.begin(), echoCanSigIdVec.end(), sigId) != echoCanSigIdVec.end())
//         ||( std::find(echoEVCCanSigIdVec.begin(), echoEVCCanSigIdVec.end(), sigId) != echoEVCCanSigIdVec.end()) &&  !EvChargeApp::GetInstance()->isTimeToIgnoreChange())
//     {
//         bEchoNecessary = true;
//     }
//     return bEchoNecessary;
// }

/////////////////////////////////////////////
#define INSERT_A(A) pCanName[A] = #A
inline void setCanName()
{
//TX
    INSERT_A(Signal_TCUDataControl_TX);
    INSERT_A(Signal_ClimateDisableReqTCU_TX);
    INSERT_A(Signal_HeatedSeatFLModeReqTCU_TX);
    INSERT_A(Signal_HeatedSeatFLRequestTCU_TX);
    INSERT_A(Signal_HeatedSeatFRModeReqTCU_TX);
    INSERT_A(Signal_HeatedSeatFRRequestTCU_TX);
    INSERT_A(Signal_HeatedSeatRLModeReqTCU_TX);
    INSERT_A(Signal_HeatedSeatRLReqTCU_TX);
    INSERT_A(Signal_HeatedSeatRRModeReqTCU_TX);
    INSERT_A(Signal_HeatedSeatRRReqTCU_TX);
    INSERT_A(Signal_MaxDefrostTCU_TX);
    INSERT_A(Signal_ParkClimateReqTCU_TX);
    INSERT_A(Signal_TempTargetTCU_TX);
    INSERT_A(Signal_CellularStateTCU_TX);
    INSERT_A(Signal_DataContractTCU_TX);
    INSERT_A(Signal_HotspotStateTCU_TX);
    INSERT_A(Signal_PasswordTypeTCU_TX);
    INSERT_A(Signal_PopupRequestTCU_TX);
    INSERT_A(Signal_RemRDrvFoldReq2ndRow_TX);
    INSERT_A(Signal_RemRDrvFoldReq3rdRow_TX);
    INSERT_A(Signal_RemRPasFoldReq2ndRow_TX);
    INSERT_A(Signal_RemRPasFoldReq3rdRow_TX);
    INSERT_A(Signal_RestorePwdDefaultTCU_TX);
    INSERT_A(Signal_RoamingTCU_TX);
    INSERT_A(Signal_TCUAudioMute_TX);
    INSERT_A(Signal_TCUMessageReq_TX);
    INSERT_A(Signal_WiFiStateTCU_TX);
    INSERT_A(Signal_TmdChrg1ActvExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvFriExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvMonExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvSatExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvSunExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvThurExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvTuesExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ActvWedExtUpd_TX);
    INSERT_A(Signal_TmdChrg1EndTimeAExtUpd_TX);
    INSERT_A(Signal_TmdChrg1EndTimeBExtUpd_TX);
    INSERT_A(Signal_TmdChrg1EndTimeCExtUpd_TX);
    INSERT_A(Signal_TmdChrg1EndTimeDExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ZoneAExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ZoneBExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ZoneCExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ZoneDExtUpd_TX);
    INSERT_A(Signal_TmdChrg1ZoneEExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvFriExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvMonExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvSatExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvSunExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvThurExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvTuesExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ActvWedExtUpd_TX);
    INSERT_A(Signal_TmdChrg2EndTimeAExtUpd_TX);
    INSERT_A(Signal_TmdChrg2EndTimeBExtUpd_TX);
    INSERT_A(Signal_TmdChrg2EndTimeCExtUpd_TX);
    INSERT_A(Signal_TmdChrg2EndTimeDExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ZoneAExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ZoneBExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ZoneCExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ZoneDExtUpd_TX);
    INSERT_A(Signal_TmdChrg2ZoneEExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvFriExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvMonExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvSatExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvSunExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvThurExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvTuesExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ActvWedExtUpd_TX);
    INSERT_A(Signal_TmdChrg3EndTimeAExtUpd_TX);
    INSERT_A(Signal_TmdChrg3EndTimeBExtUpd_TX);
    INSERT_A(Signal_TmdChrg3EndTimeCExtUpd_TX);
    INSERT_A(Signal_TmdChrg3EndTimeDExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ZoneAExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ZoneBExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ZoneCExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ZoneDExtUpd_TX);
    INSERT_A(Signal_TmdChrg3ZoneEExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvFriExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvMonExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvSatExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvSunExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvThurExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvTuesExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ActvWedExtUpd_TX);
    INSERT_A(Signal_TmdChrg4EndTimeAExtUpd_TX);
    INSERT_A(Signal_TmdChrg4EndTimeBExtUpd_TX);
    INSERT_A(Signal_TmdChrg4EndTimeCExtUpd_TX);
    INSERT_A(Signal_TmdChrg4EndTimeDExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ZoneAExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ZoneBExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ZoneCExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ZoneDExtUpd_TX);
    INSERT_A(Signal_TmdChrg4ZoneEExtUpd_TX);
    INSERT_A(Signal_BulkSoCSetExtUpdate_TX);
    INSERT_A(Signal_ChargeNowReqExtUpdte_TX);
    INSERT_A(Signal_ChrgModeChoiceExtUpdte_TX);
    INSERT_A(Signal_FirstTimeChargeSetReq_TX);
    INSERT_A(Signal_TelematicWakeUpReq_TX);
    INSERT_A(Signal_TesterFunctionalReq);
    INSERT_A(Signal_TesterPhysicalReqTCU);
    INSERT_A(Signal_TesterPhysicalResTCU);
    INSERT_A(Signal_TX_CANMsg_End);
    INSERT_A(Signal_BulkSoCSingleUseExtUpd_TX);
    INSERT_A(Signal_FFHPreClimReqTCU_TX);
    INSERT_A(Signal_FFHPreClimSetTCU_TX);
    INSERT_A(Signal_PreClimRngCmfReqTCU_TX);
    INSERT_A(Signal_PreClimRngCmfSetTCU_TX);
    INSERT_A(Signal_RemRPasFoldReq1stRow_TX);
    INSERT_A(Signal_RouteVersionTCU_TX);
    INSERT_A(Signal_PreDepTimeActFriExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActiveExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActMonExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActSatExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActSunExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActThuExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActTueExtUpd_TX);
    INSERT_A(Signal_PreDepTimeActWedExtUpd_TX);
    INSERT_A(Signal_PreDepTimeDateExtUpd_TX);
    INSERT_A(Signal_PreDepTimeHourExtUpd_TX);
    INSERT_A(Signal_PreDepTimeIndexExtUpd_TX);
    INSERT_A(Signal_PreDepTimeMinsExtUpd_TX);
    INSERT_A(Signal_PreDepTimeMnthExtUpd_TX);
    INSERT_A(Signal_PreDepTimeTypeExtUpd_TX);
    INSERT_A(Signal_PreDepTimeYearExtUpd_TX);
    INSERT_A(Signal_MatrixAverageSpeed_TX);
    INSERT_A(Signal_MatrixCounter_TX);
    INSERT_A(Signal_MatrixGradient1_TX);
    INSERT_A(Signal_MatrixGradient2_TX);
    INSERT_A(Signal_MatrixGradient3_TX);
    INSERT_A(Signal_MatrixLength1_TX);
    INSERT_A(Signal_MatrixLength2_TX);
    INSERT_A(Signal_MatrixLength3_TX);
    INSERT_A(Signal_EHMessageCounter_TX);
    INSERT_A(Signal_EHSegmentContent1_TX);
    INSERT_A(Signal_EHSegmentContent2_TX);
    INSERT_A(Signal_EHSegmentContent3_TX);
    INSERT_A(Signal_EHSegmentIdentifier1_TX);
    INSERT_A(Signal_EHSegmentIdentifier2_TX);
    INSERT_A(Signal_EHSegmentIdentifier3_TX);
    INSERT_A(Signal_EHSegmentOffset1_TX);
    INSERT_A(Signal_EHSegmentOffset2_TX);
    INSERT_A(Signal_EHSegmentOffset3_TX);
    //NCR 16WK20
    INSERT_A(Signal_ChargeNowReqExtUpdte2_TX);

//RX
    INSERT_A(Signal_TimedCharge1Active_RX);
    INSERT_A(Signal_TimedCharge1ActiveFri_RX);
    INSERT_A(Signal_TimedCharge1ActiveMon_RX);
    INSERT_A(Signal_TimedCharge1ActiveSat_RX);
    INSERT_A(Signal_TimedCharge1ActiveSun_RX);
    INSERT_A(Signal_TimedCharge1ActiveThur_RX);
    INSERT_A(Signal_TimedCharge1ActiveTues_RX);
    INSERT_A(Signal_TimedCharge1ActiveWed_RX);
    INSERT_A(Signal_TimedCharge1EndTimeA_RX);
    INSERT_A(Signal_TimedCharge1EndTimeB_RX);
    INSERT_A(Signal_TimedCharge1EndTimeC_RX);
    INSERT_A(Signal_TimedCharge1EndTimeD_RX);
    INSERT_A(Signal_TimedCharge1ZoneA_RX);
    INSERT_A(Signal_TimedCharge1ZoneB_RX);
    INSERT_A(Signal_TimedCharge1ZoneC_RX);
    INSERT_A(Signal_TimedCharge1ZoneD_RX);
    INSERT_A(Signal_TimedCharge1ZoneE_RX);
    INSERT_A(Signal_TimedCharge2Active_RX);
    INSERT_A(Signal_TimedCharge2ActiveFri_RX);
    INSERT_A(Signal_TimedCharge2ActiveMon_RX);
    INSERT_A(Signal_TimedCharge2ActiveSat_RX);
    INSERT_A(Signal_TimedCharge2ActiveSun_RX);
    INSERT_A(Signal_TimedCharge2ActiveThur_RX);
    INSERT_A(Signal_TimedCharge2ActiveTues_RX);
    INSERT_A(Signal_TimedCharge2ActiveWed_RX);
    INSERT_A(Signal_TimedCharge2EndTimeA_RX);
    INSERT_A(Signal_TimedCharge2EndTimeB_RX);
    INSERT_A(Signal_TimedCharge2EndTimeC_RX);
    INSERT_A(Signal_TimedCharge2EndTimeD_RX);
    INSERT_A(Signal_TimedCharge2ZoneA_RX);
    INSERT_A(Signal_TimedCharge2ZoneB_RX);
    INSERT_A(Signal_TimedCharge2ZoneC_RX);
    INSERT_A(Signal_TimedCharge2ZoneD_RX);
    INSERT_A(Signal_TimedCharge2ZoneE_RX);
    INSERT_A(Signal_TimedCharge3Active_RX);
    INSERT_A(Signal_TimedCharge3ActiveFri_RX);
    INSERT_A(Signal_TimedCharge3ActiveMon_RX);
    INSERT_A(Signal_TimedCharge3ActiveSat_RX);
    INSERT_A(Signal_TimedCharge3ActiveSun_RX);
    INSERT_A(Signal_TimedCharge3ActiveThur_RX);
    INSERT_A(Signal_TimedCharge3ActiveTues_RX);
    INSERT_A(Signal_TimedCharge3ActiveWed_RX);
    INSERT_A(Signal_TimedCharge3EndTimeA_RX);
    INSERT_A(Signal_TimedCharge3EndTimeB_RX);
    INSERT_A(Signal_TimedCharge3EndTimeC_RX);
    INSERT_A(Signal_TimedCharge3EndTimeD_RX);
    INSERT_A(Signal_TimedCharge3ZoneA_RX);
    INSERT_A(Signal_TimedCharge3ZoneB_RX);
    INSERT_A(Signal_TimedCharge3ZoneC_RX);
    INSERT_A(Signal_TimedCharge3ZoneD_RX);
    INSERT_A(Signal_TimedCharge3ZoneE_RX);
    INSERT_A(Signal_TimedCharge4Active_RX);
    INSERT_A(Signal_TimedCharge4ActiveFri_RX);
    INSERT_A(Signal_TimedCharge4ActiveMon_RX);
    INSERT_A(Signal_TimedCharge4ActiveSat_RX);
    INSERT_A(Signal_TimedCharge4ActiveSun_RX);
    INSERT_A(Signal_TimedCharge4ActiveThur_RX);
    INSERT_A(Signal_TimedCharge4ActiveTues_RX);
    INSERT_A(Signal_TimedCharge4ActiveWed_RX);
    INSERT_A(Signal_TimedCharge4EndTimeA_RX);
    INSERT_A(Signal_TimedCharge4EndTimeB_RX);
    INSERT_A(Signal_TimedCharge4EndTimeC_RX);
    INSERT_A(Signal_TimedCharge4EndTimeD_RX);
    INSERT_A(Signal_TimedCharge4ZoneA_RX);
    INSERT_A(Signal_TimedCharge4ZoneB_RX);
    INSERT_A(Signal_TimedCharge4ZoneC_RX);
    INSERT_A(Signal_TimedCharge4ZoneD_RX);
    INSERT_A(Signal_TimedCharge4ZoneE_RX);
    INSERT_A(Signal_BulkSoCTargetDisp_RX);
    INSERT_A(Signal_ChargeNowRequest_RX);
    INSERT_A(Signal_ChargingModeChoice_RX);
    INSERT_A(Signal_FirstTimeChargeSetStat_RX);
    INSERT_A(Signal_Timer1Time_RX);
    INSERT_A(Signal_Timer2Time_RX);
    INSERT_A(Signal_ParkHeatVentTime_RX);
    INSERT_A(Signal_AmbTempImpr_RX);
    INSERT_A(Signal_ParkClimateMode_RX);
    INSERT_A(Signal_ParkClimateStatus_RX);
    // INSERT_A(Signal_InCarTemp_RX);
    // INSERT_A(Signal_InCarTempQF_RX);
    INSERT_A(Signal_ChargDataWakeUpAllowed_RX);
    INSERT_A(Signal_ChargePwrInterruptDisp_RX);
    INSERT_A(Signal_ChargingStatusDisp_RX);
    INSERT_A(Signal_HybridStatusMsgDisp_RX);
    INSERT_A(Signal_kWhConsumedLastCharge_RX);
    INSERT_A(Signal_kWhConsumedperKm_RX);
    INSERT_A(Signal_TimeToFullSoC_RX);
    INSERT_A(Signal_ChargeReminderDisp_RX);
    INSERT_A(Signal_ChrgNotReadyByDepDisp_RX);
    INSERT_A(Signal_HVBattChargeRateKmDisp_RX);
    INSERT_A(Signal_HVBattChrgeRateSOCDisp_RX);
    INSERT_A(Signal_HVBatteryUsableKmDisp_RX);
    INSERT_A(Signal_HVBatteryUsableSOCDisp_RX);
    INSERT_A(Signal_HybridPluginDisp_RX);
    INSERT_A(Signal_EVRangeSelectedSOC_RX);
    INSERT_A(Signal_EVRangeComfort_RX);
    INSERT_A(Signal_EVRangeDynamic_RX);
    INSERT_A(Signal_EVRangeEco_RX);
    INSERT_A(Signal_EVRangeGetMeHome_RX);
    INSERT_A(Signal_EVRangePredictStatus_RX);
    INSERT_A(Signal_TimeToBulkSoC_RX);
    INSERT_A(Signal_WlChargingStatusDisp_RX);
    INSERT_A(Signal_PreDepTimeActFri_RX);
    INSERT_A(Signal_PreDepTimeActive_RX);
    INSERT_A(Signal_PreDepTimeActMon_RX);
    INSERT_A(Signal_PreDepTimeActSat_RX);
    INSERT_A(Signal_PreDepTimeActSun_RX);
    INSERT_A(Signal_PreDepTimeActThu_RX);
    INSERT_A(Signal_PreDepTimeActTue_RX);
    INSERT_A(Signal_PreDepTimeActWed_RX);
    INSERT_A(Signal_PreDepTimeDate_RX);
    INSERT_A(Signal_PreDepTimeHour_RX);
    INSERT_A(Signal_PreDepTimeIndex_RX);
    INSERT_A(Signal_PreDepTimeMins_RX);
    INSERT_A(Signal_PreDepTimeMnth_RX);
    INSERT_A(Signal_PreDepTimeType_RX);
    INSERT_A(Signal_PreDepTimeYear_RX);
    INSERT_A(Signal_NextPresetDepTimeDate_RX);
    INSERT_A(Signal_NextPresetDepTimeHour_RX);
    INSERT_A(Signal_NextPresetDepTimeMins_RX);
    INSERT_A(Signal_NextPresetDepTimeMnth_RX);
    INSERT_A(Signal_NextPresetDepTimeYear_RX);
    INSERT_A(Signal_BulkSoCSingleUseDisp_RX);
    INSERT_A(Signal_PasSeatInhib1stRow_RX);
    INSERT_A(Signal_PasSeatPosition1stRow_RX);
    INSERT_A(Signal_FFHPreClimSetting_RX);
    INSERT_A(Signal_HVBattCoolingStatus_RX);
    INSERT_A(Signal_HVBattHeatingStatus_RX);
    INSERT_A(Signal_PreClimRangeComfort_RX);
    INSERT_A(Signal_PreClimateRequests_RX);
    INSERT_A(Signal_PreClimateStatus_RX);
    INSERT_A(Signal_HVBattChrgRateMileDisp_RX);
    INSERT_A(Signal_EVRangeMilesDisp_RX);
    INSERT_A(Signal_EVRangeDisp_RX);
    INSERT_A(Signal_VSCRangeOnRouteFlag_RX);
    INSERT_A(Signal_EVRangeSelectSOCMiles_RX);
    INSERT_A(Signal_TimeToBulkSoCHoursDisp_RX);
    INSERT_A(Signal_TimeToBulkSoCMinsDisp_RX);
    INSERT_A(Signal_TimeToFullSoCHoursDisp_RX);
    INSERT_A(Signal_TimeToFullSoCMinsDisp_RX);
    INSERT_A(Signal_CombinedRangeDisp_RX);
    INSERT_A(Signal_CombinedRangeMilesDisp_RX);
    INSERT_A(Signal_TimedChrgStartDateDisp_RX);
    INSERT_A(Signal_TimedChrgStartHourDisp_RX);
    INSERT_A(Signal_TimedChrgStartMinsDisp_RX);
    INSERT_A(Signal_TimedChrgStartMnthDisp_RX);
    INSERT_A(Signal_TimedChrgStartYearDisp_RX);
    INSERT_A(Signal_FFHOperatingStatus_RX);
    INSERT_A(Signal_HVBattCoolingStatusExt_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd1_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd2_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd3_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd4_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd5_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd6_RX);
    INSERT_A(Signal_VSCHVBattConsumpSpd7_RX);
    INSERT_A(Signal_VSCHVEnergyAscent_RX);
    INSERT_A(Signal_VSCHVEnergyDescent_RX);
    INSERT_A(Signal_VSCHVEnergyTimePen_RX);
    INSERT_A(Signal_VSCInitialHVBattEnergy_RX);
    INSERT_A(Signal_VSCRegenEnergyAvailable_RX);
    INSERT_A(Signal_VSCRangeMapRefactrComf_RX);
    INSERT_A(Signal_VSCRangeMapRefactrEco_RX);
    INSERT_A(Signal_VSCRangeMapRefactrGMH_RX);
    INSERT_A(Signal_VSCRegenEnergyFactor_RX);
    INSERT_A(Signal_VSCRevisedHVBattEnergy_RX);
    INSERT_A(Signal_VSCVehAccelFactor_RX);
    INSERT_A(Signal_ChargePlugDiscAlarmReq_RX);
    INSERT_A(Signal_HVBattCellVoltageMax_RX);
    INSERT_A(Signal_HVBattCellVoltageMin_RX);
    INSERT_A(Signal_HVBattCurrentExt_RX);
    INSERT_A(Signal_HVBattVoltageExt_RX);
    INSERT_A(Signal_HVBattAvgSOCOAtEvent_RX);
    INSERT_A(Signal_HVBattAvTempAtEvent_RX);
    INSERT_A(Signal_HVBattFastChgCounter_RX);
    INSERT_A(Signal_HVBattLogEvent_RX);
    INSERT_A(Signal_HVBattTempColdCellID_RX);
    INSERT_A(Signal_HVBattTempHottCellID_RX);
    INSERT_A(Signal_HVBattVoltMaxCellID_RX);
    INSERT_A(Signal_HVBattVoltMinCellID_RX);
    // INSERT_A(Signal_DCDCOperatingModeExt_RX);
    // INSERT_A(Signal_DCDCTemperature_RX);
    // INSERT_A(Signal_EMOperatingModeExt_RX);
    // INSERT_A(Signal_EMOperatingModeF_RX);
    // INSERT_A(Signal_EMSpeed_RX);
    // INSERT_A(Signal_EMSpeedF_RX);
    // INSERT_A(Signal_InverterTemperature_RX);
    // INSERT_A(Signal_InverterTemperatureF_RX);
    // INSERT_A(Signal_EMCurrentDcLink_RX);
    // INSERT_A(Signal_EMCurrentDCLinkF_RX);
    // INSERT_A(Signal_EMTemperature_RX);
    // INSERT_A(Signal_EMTemperatureF_RX);
    //INSERT_A(Signal_EMTorqueExt_RX);
    // INSERT_A(Signal_EMTorqueF_RX);
    INSERT_A(Signal_ChargeNowRequestExt_RX);
    INSERT_A(Signal_HVBattVoltageExt_RX); //PCR027 HBD DOTA
    INSERT_A(Signal_HVBattCurrentExt_RX);
    INSERT_A(Signal_HVBattCellVoltageMax_RX);
    INSERT_A(Signal_HVBattVoltMaxCellID_RX);
    INSERT_A(Signal_HVBattCellVoltageMin_RX);
    INSERT_A(Signal_HVBattVoltMinCellID_RX);
    INSERT_A(Signal_HVBattCellTempHottest_RX);
    INSERT_A(Signal_HVBattTempHottCellID_RX);
    INSERT_A(Signal_HVBattCellTempColdest_RX);
    INSERT_A(Signal_HVBattTempColdCellID_RX);
    INSERT_A(Signal_HVBattStatusCAT4Derate_RX);
    INSERT_A(Signal_HVBattStatusCAT6DlyBPO_RX);
    INSERT_A(Signal_HVBattStatusCAT7NowBPO_RX);
    INSERT_A(Signal_HVBattStateofHealth_RX);
    INSERT_A(Signal_HVBattStateofHealthMin_RX);
    INSERT_A(Signal_HVBattStateofHealthMax_RX);
    INSERT_A(Signal_HVBattStateofHealthPwr_RX);
    INSERT_A(Signal_HVBattSoHcMinCellID_RX);
    INSERT_A(Signal_HVBattSoHcMaxCellID_RX);
    INSERT_A(Signal_HVBattFastChgCounter_RX);
    INSERT_A(Signal_HVBattInletCoolantTemp_RX);
    INSERT_A(Signal_HVBattOutletCoolantTmp_RX);
    INSERT_A(Signal_HVBattCoolingEnrgyUsd_RX);
    INSERT_A(Signal_HVBattThrmlMngrMode_RX);
    INSERT_A(Signal_HVBattBalancingStatus_RX);
    INSERT_A(Signal_HVBattBlncngTrgrCellID_RX);
    INSERT_A(Signal_HVBattFuseTemperature_RX);
    INSERT_A(Signal_HVBattBEMTemperature_RX);
    INSERT_A(Signal_AmbientTemp_RX);
    INSERT_A(Signal_HVBattAvgSOCOAtEvent_RX);
    INSERT_A(Signal_HVBattAvTempAtEvent_RX);
    INSERT_A(Signal_WirelessConnectStatus_RX);// END
}
#undef INSERT_A
/////////////////////////////////////////////
