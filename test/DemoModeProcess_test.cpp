#include <gtest/gtest.h>
#include "../DemoModeProcess.h"
#include "../SpecialModeBaseProcess.h"
#include "../SpecialModeDataType.h"
#include "../SpecialModeApplication.h"
#include "../SpecialModeServicesManager.h"
#include "doubles/MockApplicationManagerService.h"
#include "doubles/MockConfigurationManagerService.h"
#include "doubles/MockNGTPManagerService.h"
#include "doubles/MockSystemManagerService.h"
#include "doubles/PartialMockHandler.h"
#include "utils/makeSp.h"

using namespace ::testing;
//fixture
class DemoModeProcessTest : public Test
{
protected:
    sp<SpecialModeServicesManager> mp_ServiceMgr;

    sp<MockConfigurationManagerService>    mp_MockConfigMgr;
    sp<MockApplicationManagerService>      mp_MockAppMgr;
    sp<MockSystemManagerService>           mp_MockSystemMgr;
    sp<MockNGTPManagerService>             mp_MockNGTPMgr;

    DemoModeProcessTest()
    {
        mp_ServiceMgr       = makeSp<SpecialModeServicesManager>();

        mp_MockAppMgr       = makeSp<MockApplicationManagerService>();
        mp_MockNGTPMgr      = makeSp<MockNGTPManagerService>();
        mp_MockConfigMgr    = makeSp<MockConfigurationManagerService>();
        mp_MockSystemMgr    = makeSp<MockSystemManagerService>();

        mp_ServiceMgr->m_ApplicationMgr     = mp_MockAppMgr;
        mp_ServiceMgr->m_ConfigurationMgr   = mp_MockConfigMgr;
        mp_ServiceMgr->m_NGTPMgr            = mp_MockNGTPMgr;
        mp_ServiceMgr->m_SystemMgr          = mp_MockSystemMgr;
    }
};


// constructor
TEST_F(DemoModeProcessTest, constructor_001)
{
    DemoModeProcess proc;
    EXPECT_FALSE(proc.m_CheckPower);
}
TEST_F(DemoModeProcessTest, constructor_002)
{
    DemoModeProcess proc;
    int32_t expectRunningTime = 0;
    EXPECT_EQ(expectRunningTime, proc.m_RunningTime);
}
TEST_F(DemoModeProcessTest, constructor_003)
{
    DemoModeProcess proc;
    uint8_t expectTimeUnit = 0x01;
    EXPECT_EQ(expectTimeUnit, proc.m_TimeUnit);
}
TEST_F(DemoModeProcessTest, constructor_004)
{
    DemoModeProcess proc;
    DemoModeStatus expectStatus = E_DEMOMODE_INIT;
    EXPECT_EQ(expectStatus, proc.m_DemoModeStatus);
}
TEST_F(DemoModeProcessTest, constructor_005)
{
    DemoModeProcess proc;
    WiFiStatus expectStatus = E_CONNECTED;
    EXPECT_EQ(expectStatus, proc.m_WifiStatus);
}
TEST_F(DemoModeProcessTest, constructor_006)
{
    DemoModeProcess proc;
    EXPECT_NE(nullptr, proc.mp_PowerLock);
}

// destructor
TEST_F(DemoModeProcessTest, destructor_001)
{
    DemoModeProcess proc;
    // TODO?
}

// initializeProcess
TEST_F(DemoModeProcessTest, initializeProcess_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));

    // launch test
    process.initializeProcess();
    EXPECT_EQ(E_DEMOMODE_INIT, process.getDemoStatus());

    delete pApp;
}

// wifi connection state
TEST_F(DemoModeProcessTest, setWifiStatus_001)
{
    DemoModeProcess proc;
    WiFiStatus wifiStatus = E_CONNECTED;
    proc.setWifiStatus(wifiStatus);
    EXPECT_EQ(proc.m_WifiStatus, E_CONNECTED);

}
TEST_F(DemoModeProcessTest, getWiFiStatus_001)
{
    DemoModeProcess proc;
    proc.setWifiStatus(E_DISCONNECTED);
    EXPECT_EQ(proc.m_WifiStatus, proc.getWifiStatus());
}
TEST_F(DemoModeProcessTest, getWiFiStatus_002)
{
    DemoModeProcess proc;
    auto undefineStatus = 20;
    EXPECT_NE(undefineStatus, proc.getWifiStatus());
}

// power constrain
TEST_F(DemoModeProcessTest, lockPowerMode_001)
{
    DemoModeProcess proc;
    proc.m_CheckPower = true;
    proc.lockPowerMode();
}
TEST_F(DemoModeProcessTest, lockPowerMode_002)
{
    DemoModeProcess proc;
    proc.m_CheckPower = false;
    proc.lockPowerMode();
    EXPECT_EQ(true, proc.m_CheckPower);
}
TEST_F(DemoModeProcessTest, releasePowerMode_001)
{
    DemoModeProcess proc;
    proc.m_CheckPower = true;
    proc.releasePoweMode();
    EXPECT_EQ(false, proc.m_CheckPower);
}
TEST_F(DemoModeProcessTest, releasePowerMode_002)
{
    DemoModeProcess proc;
    proc.m_CheckPower = false;
    proc.releasePoweMode();
}

// handler timer event
TEST_F(DemoModeProcessTest, handleTimerEvent_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_))
        .Times(AtLeast(1));

    // test lauch
    auto timeId = 0;
    process.handleTimerEvent(timeId);
}
TEST_F(DemoModeProcessTest, handleTimerEvent_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    auto timeId = 1;
    process.handleTimerEvent(timeId);

    delete pApp;
}

// handler wifi event
TEST_F(DemoModeProcessTest, onWiFiStateOn_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_PENDING);

    // launch test
    process.onWiFiStateOn();
    EXPECT_EQ(E_DEMOMODE_START, process.getDemoStatus());

    delete pApp;

}
TEST_F(DemoModeProcessTest, onWiFiStateOn_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_STOP);

    // launch test
    process.onWiFiStateOn();
    EXPECT_NE(E_DEMOMODE_START, process.getDemoStatus());

    delete pApp;
}
TEST_F(DemoModeProcessTest, onWiFiStateOff_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_START);

    // launch test
    process.onWiFiStateOff();
    EXPECT_EQ(E_DEMOMODE_PENDING, process.getDemoStatus());

    delete pApp;
}
TEST_F(DemoModeProcessTest, onWiFiStateOff_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_STOP);

    // launch test
    process.onWiFiStateOff();
    EXPECT_NE(E_DEMOMODE_PENDING, process.getDemoStatus());

    delete pApp;
}

// handler message
TEST_F(DemoModeProcessTest, doSpecialModeHandler_001)
{
    // mock obj
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    // create sl::Message
    auto pMessage = sl::Message::obtain();
    pMessage->what = DEMOMODE_SLDD_ON;
    pMessage->arg1 = 1;
    pMessage->arg2 = 2;

    // launch test
    process.doSpecialModeHandler(pMessage);

    delete pApp;
}

// handler configchange event

// start demo mode
TEST_F(DemoModeProcessTest, demoModeStart_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    uint8_t time_unit_second = 0x00;  // second
    process.m_TimeUnit = time_unit_second;
    process.demoModeStart();

    delete pApp;
}
TEST_F(DemoModeProcessTest, demoModeStart_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    uint8_t time_unit_hour = 0x01;  // second
    process.m_TimeUnit = time_unit_hour;
    process.demoModeStart();

    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOnDemoMode_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);

    uint8_t timeUnit = 0x00;  // second
    int32_t timeValue = 100;
    process.m_WifiStatus = E_DISCONNECTED;

    // launch test
    EXPECT_FALSE(process.turnOnDemoMode(timeUnit, timeValue));
    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOnDemoMode_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    uint8_t timeUnit = 0x00;  // second
    int32_t timeValue = 100;
    process.m_WifiStatus = E_CONNECTED;

    // launch test
    EXPECT_TRUE(process.turnOnDemoMode(timeUnit, timeValue));
    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOnDemoMode_003)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    uint8_t timeUnit = 0x01;  // hour
    int32_t timeValue = 1;
    process.m_WifiStatus = E_CONNECTED;

    // launch test
    EXPECT_TRUE(process.turnOnDemoMode(timeUnit, timeValue));
    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOnDemoMode_004)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_)).Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_)).Times(AtLeast(1));

    uint8_t timeUnit = 0x01;  // hour
    int32_t timeValue = 2;
    process.m_WifiStatus = E_CONNECTED;
    process.turnOnDemoMode(timeUnit, timeValue);
    // launch test
    EXPECT_GE(3600, process.m_RunningTime);
    delete pApp;
}
// stop demo mode
TEST_F(DemoModeProcessTest, demoModeStop_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_))
        .Times(AtLeast(1));
    // launch test
    process.demoModeStop();

    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOffDemoMode_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_START);

    // launch test
    EXPECT_TRUE(process.turnOffDemoMode());

    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOffDemoMode_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    EXPECT_CALL(*mp_MockNGTPMgr, setDemoMode(_))
        .Times(AtLeast(1));
    process.setDemoStatus(E_DEMOMODE_PENDING);

    // launch test
    EXPECT_TRUE(process.turnOffDemoMode());

    delete pApp;
}
TEST_F(DemoModeProcessTest, turnOffDemoMode_003)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess process;
    process.initialize(mp_ServiceMgr, pApp);
    process.setDemoStatus(E_DEMOMODE_STOP);

    // launch test
    EXPECT_FALSE(process.turnOffDemoMode());

    delete pApp;
}
// clock reset
TEST_F(DemoModeProcessTest, demoModeClockReset_001)
{
    DemoModeProcess proc;
    proc.demoModeClockReset();
}

// get-set demo mode status
TEST_F(DemoModeProcessTest, setDemoStatus_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess demoProcess;
    demoProcess.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    demoProcess.setDemoStatus(E_DEMOMODE_START);

    delete pApp;
}
TEST_F(DemoModeProcessTest, setDemoStatus_002)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess demoProcess;
    demoProcess.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    demoProcess.setDemoStatus(E_DEMOMODE_STOP);

    delete pApp;
}
TEST_F(DemoModeProcessTest, setDemoStatus_003)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess demoProcess;
    demoProcess.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    demoProcess.setDemoStatus(E_DEMOMODE_PENDING);

    delete pApp;
}
TEST_F(DemoModeProcessTest, getDemoStatus_001)
{
    ISpecialModeApplication* pApp = new SpecialModeApplication();
    DemoModeProcess demoProcess;
    demoProcess.initialize(mp_ServiceMgr, pApp);

    EXPECT_CALL(*mp_MockAppMgr, broadcastSystemPost(_))
        .Times(AtLeast(1));
    demoProcess.setDemoStatus(E_DEMOMODE_STOP);

    // launch test
    DemoModeStatus expectStatus = E_DEMOMODE_STOP;
    EXPECT_EQ(expectStatus, demoProcess.getDemoStatus());

    delete pApp;
}
