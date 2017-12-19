//EvtVifManager
#ifndef _EVT_CAN_MANAGER_H_
#define _EVT_CAN_MANAGER_H_

#include "vifManagerService/vifcansig.h"
#include "vifManagerService/vifManagerService.h"
#include "vifManagerService/vifManagerServiceTX.h"
#include "vifManagerService/IvifManagerService.h"
#include "vifManagerService/IvifManagerReceiver.h"
#include "vifManagerService/IvifManagerReceiverTX.h"
#include "EvtSingleton.h"
#include <vector>
#include <algorithm>
#include "EvCanFrame.h"

#define PCR041ENABLE 1

class EvtVifManager : public EvtSingleton<EvtVifManager>
{
public:
    EvtVifManager();
    ~EvtVifManager(){};

    error_t sendCanSignal(int sigID, const uint8_t data);
    error_t sendCanArraySignal(int sigID, const uint8_t data[]);
    error_t sendCanMultiSignal(uint32_t NumofSignals, sp<vifCANContainer>& sigdata);
    error_t sendCanMultiSignal_EH(uint32_t NumofSignals, sp<vifCANContainer>& sigdata);
    error_t cancelCanMultiSignal_EH(uint32_t frameIdentifier);
    int queryCanData(int sigID);

    void setVIFService(sp<IvifManagerService> service, uint16_t appId)
    {
        m_vifMgrService = service;
        mAppId = appId;

    };

    void receiveCAN(int32_t Sigid, Buffer& buf);
    void receiveCAN_Frame(EvCanFrame *pFrame);
    const char* getCanName(uint16_t canID);

    void pushSingleData(sp<vifCANContainer>& sigdata, int sigID, uint32_t data);
    void setByteDataFromInterger(uint8_t _byteData[], uint32_t _intData, uint8_t size);

    bool isValueChanged(int32_t Sigid, bool bUpdatedBefore, int32_t oldValue, int32_t newValue);
    bool isStoreChargeSetting();
    bool isChargeSettingFrame(uint16_t frameID);
    bool isChargeSettingSignal(int32_t sigID);

private:
    sp<IvifManagerService> m_vifMgrService;
    sp<SLLooper> m_Looper;
    sp<Handler> mHandler;

    sp<IvifManagerReceiverTX> mVifReceiverTx;
    sp<IvifManagerServiceTX> mVifSvcTx;

    uint16_t mAppId;

    void storeElectricVehicleInformationFromCanData(uint16_t Sigid, Buffer& buf);
};

#endif //_EVT_CAN_MANAGER_H_
