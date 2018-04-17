#include <gtest/gtest.h>
#include "../DemoModeTimer.h"

TEST(DemoModeTimerTest, constructor_001)
{
    DemoModeTimerSet timer;
    EXPECT_EQ(false, timer.m_InitFlag);
    EXPECT_EQ(0, timer.mp_DemoModeTimerSet[0]);
    EXPECT_EQ(0, timer.mp_DemoModeTimerSet[1]);
}

TEST(DemoModeTimerTest, initialize_001)
{
    TimerTimeoutHandler *pTimerHandler = nullptr;
    DemoModeTimerSet timer;
    timer.initialize(pTimerHandler);
}

TEST(DemoModeTimerTest, initialize_002)
{
    DemoModeTimerSet timer;
    timer.initialize(nullptr);
}

TEST(DemoModeTimerTest, release_001)
{
    DemoModeTimerSet timer;
    timer.m_InitFlag = true;
    timer.release();
}

TEST(DemoModeTimerTest, release_002)
{
    DemoModeTimerSet timer;
    timer.m_InitFlag = false;
    timer.release();
}

TEST(DemoModeTimerTest, startTimer_001)
{
    uint32_t index = 0, startTimeout = 0, cyclicTimeout = 0;
    (void) cyclicTimeout;
    DemoModeTimerSet timer;
    EXPECT_EQ(true, timer.startTimer(index, startTimeout));
}

TEST(DemoModeTimerTest, stopTimer_001)
{
    uint32_t index = 0;
    DemoModeTimerSet timer;
    EXPECT_EQ(true, timer.stopTimer(index));
}

TEST(DemoModeTimerTest, stopTimer_002)
{
    uint32_t index = 2;
    DemoModeTimerSet timer;
    EXPECT_FALSE(timer.stopTimer(index));
}
