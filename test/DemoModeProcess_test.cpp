#include <gtest/gtest.h>
#include <DemoModeProcess.h>

// TEST(DemoModeProcessTest, byteToInt1) {
    // DemoModeProcess process;
    // byte data[] = {0x12, 0x34, 0x56, 0x78};
    // EXPECT_EQ(0x78563412, process.byteToInt1(data, 0));
// }

TEST(DemoModeProcessTest, DemoModeProcessTestContructorValid) {
    DemoModeProcess process;
    process.m_DemoModeStatus = E_DEMOMODE_INIT;
    EXPECT_EQ(E_DEMOMODE_INIT, process.m_DemoModeStatus);
}

TEST(DemoModeProcessTest, handleTimerEvent)
{
    auto timerID = 0;
    DemoModeProcess process;
    process.handleTimerEvent(timerID);
}

TEST(DemoModeProcessTest, demoModeClockReset)
{
    DemoModeProcess process;
    process.demoModeClockReset();
}

TEST(DemoModeProcessTest, doSpecialModeHandler)
{
    int32_t what {0x100};
    sp<sl::Message> message;
    DemoModeProcess process;
    process.doSpecialModeHandler(message);
}

TEST(DemoModeProcessTest, initializeProcess)
{
    DemoModeProcess process;
    process.initializeProcess();
}

TEST(DemoModeProcessTest, turnOnDemoMode)
{
    uint8_t timeUnit {0x00};
    int32_t timeValue {0x05};
    DemoModeProcess process;
    process.turnOnDemoMode(timeUnit, timeValue);
}

TEST(DemoModeProcessTest, demoModeStart)
{
    DemoModeProcess process;
    process.demoModeStart();
}

TEST(DemoModeProcessTest, turnOffDemoMode)
{
    DemoModeProcess process;
    process.turnOffDemoMode();
}

TEST(DemoModeProcessTest, demoModeStop)
{
    DemoModeProcess process;
    process.demoModeStop();
}

TEST(DemoModeProcessTest, getDemoStatus)
{
    DemoModeProcess process;
    process.getDemoStatus();
}

TEST(DemoModeProcessTest, setGetWifiStatus)
{
    WiFiStatus status {WiFiStatus::E_CONNECTED};
    DemoModeProcess process;
    process.setWifiStatus(status);
    EXPECT_EQ(1, process.getWifiStatus());
}

TEST(DemoModeProcessTest, checkBcallStatus)
{
    DemoModeProcess process;
    process.checkBcallStatus();
}

TEST(DemoModeProcessTest, checkEcallStatus)
{
    DemoModeProcess process;
    process.checkEcallStatus();
}

TEST(DemoModeProcessTest, disableBCall)
{
    DemoModeProcess process;
    process.disableBCall();
}
TEST(DemoModeProcessTest, disableECall)
{
    DemoModeProcess process;
    process.disableECall();
}

TEST(DemoModeProcessTest, lockPowerMode)
{
    DemoModeProcess process;
    process.lockPowerMode();
}

TEST(DemoModeProcessTest, releasePoweMode)
{
    DemoModeProcess process;
    process.releasePoweMode();
}

