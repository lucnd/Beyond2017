#include <gtest/gtest.h>
#include <SpecialModeBaseProcess.h>
#include <DemoModeProcess.h>

TEST(SpecialModeBaseProcessTest, constructor_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    EXPECT_EQ(NULL, baseProcess->m_Handler);
    EXPECT_EQ(NULL, baseProcess->mp_Application);
    EXPECT_EQ(NULL, baseProcess->mp_DemoModeTimer);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, destructor_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, initialize_001)
{
    ISpecialModeApplication* app;
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    sp<SpecialModeServicesManager> serviceMgr;
    baseProcess->initialize(serviceMgr, app);
    delete baseProcess;
}
