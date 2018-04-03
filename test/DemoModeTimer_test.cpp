#include <gtest/gtest.h>
#include <DemoModeTimer.h>

TEST(DemoModeTimerTest, initialize)
{
    TimerTimeoutHandler *pTimerHandler;
    DemoModeTimerSet timer;
    timer.initialize(pTimerHandler);
    timer.initialize(nullptr);
}


TEST(DemoModeTimerTest, release)
{
    DemoModeTimerSet timer;
    timer.m_InitFlag = true;
    timer.release();
    timer.m_InitFlag = false;
    timer.release();
}

TEST(DemoModeTimerTest, startTimer)
{
    uint32_t index = 0, startTimeout = 0, cyclicTimeout = 0;
    (void) cyclicTimeout;
    DemoModeTimerSet timer;
    timer.startTimer(index, startTimeout);
}

TEST(DemoModeTimerTest, stopTimer)
{
    uint32_t index = 0;
    DemoModeTimerSet timer;
    timer.stopTimer(index);
}
