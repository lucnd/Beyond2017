///////////////////////////////////////////////////////////
//  EvApp.h
//  Implementation of the Class EvApp
//  Created on:
//  Original author Dummy:
///////////////////////////////////////////////////////////

#ifndef _EV_APP_H_
#define _EV_APP_H_

#include <list>
#include <string>
#include <application/Application.h>
#include "SystemService.h"
#include "Handler.h"
#include "Message.h"
#include "SLLooper.h"
#include "Typedef.h"

#include "EvReceiver/EvIReceiver.h"
#include "EvtUtil/EvState.h"

//EVBinderService
#include "EvAppBinderService.h"
#include "IEvAppBinderService.h"

using namespace sl;
using namespace android;

class EvApp : public Application, EvIReceiver, EvState
{
public:
	EvApp();
	virtual ~EvApp(){};

	virtual void onCreate();
	virtual void onDestroy();
    virtual uint16_t getAppID() { return APP_ID_EV; }
    virtual void onPostReceived(const sp<Post>& post);

    void onNotifyFromDebugMgr(int32_t id, uint8_t cmd, uint8_t cmd2, sp<Buffer>& buf);
    void onNotifyFromVifMgr(uint16_t Sigid, sp<Buffer>& buf, bool isInitial = false);
    void onNotifyFromNgtpMgr(InternalNGTPData *data, int datalength);
    void onPowerStateChanged(int32_t newState, int32_t reason);
    void onWakeup(int32_t currentState, int32_t reason);
    void onPowerModeChanged(int8_t newMode);
    void onNotifyConfigDataChanged(sp<Buffer>& name);
    void onNotifyFrameSignalFromVifMgr(uint16_t _FrameID, sp<vifCANFrameSigData>& _FrameSigData, bool isInitial = false );

private:
    void regVifService();
    void releaseVifService();
    void regPwrService();
    void releasePwrService();
    void regNgtpService();
    void releaseNgtpService();
    void regDebugService();
    void regSVTmonitor();

    bool checkMdmVariant();

    error_t regConfigurationService();
    error_t releaseConfigurationService();
    void bindingApplication();
    virtual void toStringState(unsigned char state, unsigned char newState);

    sp<SLLooper> m_Looper;
    sp<Handler> mHandler;
    EvAppBinderService *m_EvAppBinder;

    uint8_t uHybridType;
};

// DEBUG CMD.
#define DBG_CHARGE_NOW_REQ  0x0100
#define DBG_PRECONITION     0x0200
#define DBG_DEV             0x0300
#define DBG_CHARGE_SETTING_TEST 0x0500
#define DBG_CHARGE_MODE_CHOICE 0x0600
#define DBG_MAX_STATE_OF_CHARGE 0x0700
#define DBG_DEPARTURE_TIMERS 0x0800
#define DBG_TARIFFS 0x0900

enum{
    RECV_DEBUG = 0,
    RECV_VIFCAN,
    RECV_VIFCAN_FRAME,
    RECV_NGTP,
    REVC_DIAG,
    REVC_CONFIG,
    REVC_SLDD,
    RECV_POWER_MODE,
    REG_SVT_MONITOR,
    _END_ENUM_
};

namespace EVAPP
{
    enum Status{
        OFF = 0,
        IDLE,
        _END_ENUM_
    };
}

#endif // _EV_APP_H_
