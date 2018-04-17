#include <gtest/gtest.h>
#include "../DemoModeProcess.h"
#include "../SpecialModeBaseProcess.h"
#include "../SpecialModeDataType.h"
#include "doubles/MockApplicationManagerService.h"
#include "doubles/MockConfigurationManagerService.h"
#include "doubles/MockNGTPManagerService.h"
#include "doubles/MockSystemManagerService.h"
#include "doubles/PartialMockHandler.h"

// constructor
TEST(DemoModeProcessTest, constructor_001)
{
    DemoModeProcess proc;
    EXPECT_FALSE(proc.m_CheckPower);
}
TEST(DemoModeProcessTest, constructor_002)
{
    DemoModeProcess proc;
    int32_t expectRunningTime = 0;
    EXPECT_EQ(expectRunningTime, proc.m_RunningTime);
}
TEST(DemoModeProcessTest, constructor_003)
{
    DemoModeProcess proc;
    uint8_t expectTimeUnit = 0x01;
    EXPECT_EQ(expectTimeUnit, proc.m_TimeUnit);
}
TEST(DemoModeProcessTest, constructor_004)
{
    DemoModeProcess proc;
    DemoModeStatus expectStatus = E_DEMOMODE_INIT;
    EXPECT_EQ(expectStatus, proc.m_DemoModeStatus);
}
TEST(DemoModeProcessTest, constructor_005)
{
    DemoModeProcess proc;
    WiFiStatus expectStatus = E_CONNECTED;
    EXPECT_EQ(expectStatus, proc.m_WifiStatus);
}
TEST(DemoModeProcessTest, constructor_006)
{
    DemoModeProcess proc;
    EXPECT_NE(nullptr, proc.mp_PowerLock);
}

// destructor
TEST(DemoModeProcessTest, destructor_001)
{
    DemoModeProcess proc;
    // TODO?
}

// wifi connection state
TEST(DemoModeProcessTest, setWifiStatus_001)
{
    DemoModeProcess proc;
    WiFiStatus wifiStatus = E_CONNECTED;
    proc.setWifiStatus(wifiStatus);
    EXPECT_EQ(proc.m_WifiStatus, E_CONNECTED);

}
TEST(DemoModeProcessTest, getWiFiStatus_001)
{
    DemoModeProcess proc;
    proc.setWifiStatus(E_DISCONNECTED);
    EXPECT_EQ(proc.m_WifiStatus, proc.getWifiStatus());
}
TEST(DemoModeProcessTest, getWiFiStatus_002)
{
    DemoModeProcess proc;
    auto undefineStatus = 20;
    EXPECT_NE(undefineStatus, proc.getWifiStatus());
}

// power constrain
TEST(DemoModeProcessTest, lockPowerMode_001)
{
    DemoModeProcess proc;
    proc.m_CheckPower = true;
    proc.lockPowerMode();
}
TEST(DemoModeProcessTest, lockPowerMode_002)
{
    DemoModeProcess proc;
    proc.m_CheckPower = false;
    proc.lockPowerMode();
    EXPECT_EQ(true, proc.m_CheckPower);
}
TEST(DemoModeProcessTest, releasePowerMode_001)
{
    DemoModeProcess proc;
    proc.m_CheckPower = true;
    proc.releasePoweMode();
    EXPECT_EQ(false, proc.m_CheckPower);
}
TEST(DemoModeProcessTest, releasePowerMode_002)
{
    DemoModeProcess proc;
    proc.m_CheckPower = false;
    proc.releasePoweMode();
}

// handler event

// start

// stop
TEST(DemoModeProcess, demoModeStop_001)
{
    DemoModeProcess process;
    process.initialize();


    MockApplicationManagerService mockServiceMgr;

}

// demo mode status

