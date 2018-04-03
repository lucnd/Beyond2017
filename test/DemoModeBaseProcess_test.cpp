#include <gtest/gtest.h>
#include <DemoModeProcess.h>
#include <SpecialModeBaseProcess.h>

TEST(SpecialModeBaseProcessTest, initialize)
{
    sp<SpecialModeServicesManager> serviceMgr;
    ISpecialModeApplication *pApp;
    SpecialModeBaseProcess *process = new DemoModeProcess();
    process->initialize(serviceMgr, pApp);
    delete process;
}

TEST(SpecialModeBaseProcessTest, getSetPropertyWrap)
{
    const char *name = "demoModeState";
    const char *value = "0";
    SpecialModeBaseProcess *process = new DemoModeProcess();
    process->setPropertyChar(name, value);
    process->setPropertyChar(nullptr, nullptr);
    process->setPropertyInt(name, 0);
    process->setPropertyInt(nullptr, 0);
    auto valueTest = std::stoi(process->getPropertyWrap(name));
    EXPECT_EQ(-1, valueTest);

    delete process;
}

