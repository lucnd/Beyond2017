#include <gtest/gtest.h>
#include "../SpecialModeBaseProcess.h"
#include "../DemoModeProcess.h"
#include "../SpecialModeApplication.h"

TEST(SpecialModeBaseProcessTest, constructor_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    EXPECT_EQ(nullptr, baseProcess->m_Handler.get());
    EXPECT_EQ(nullptr, baseProcess->mp_Application);
    EXPECT_EQ(nullptr, baseProcess->mp_DemoModeTimer);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, destructor_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, initialize_001)
{
    ISpecialModeApplication* app = new SpecialModeApplication();
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    sp<SpecialModeServicesManager> serviceMgr;
    baseProcess->initialize(serviceMgr, app);   // TODO : add #ifdef if segment fault
    delete baseProcess;
    delete app;
}

TEST(SpecialModeBaseProcessTest, getPropertyWrap_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
//    const char* name = '\0';
    auto actualResult = std::stoi(baseProcess->getPropertyWrap(nullptr));
    auto expectResult = -1;
    EXPECT_EQ(expectResult, actualResult);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, getPropertyWrap_002)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    const char* name = "demoStatus";
    auto actualResult = std::stoi(baseProcess->getPropertyWrap(name));
    auto expectResult = 1;
    EXPECT_EQ(expectResult, actualResult);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, setPropertyChar_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    std::string str("0");
    baseProcess->setPropertyChar("demoModeTime", str.c_str());

    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, setPropertyChar_002)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    baseProcess->setPropertyChar(nullptr, nullptr);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, setPropertyInt_001)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    const char* name = "demoStatus";
    uint32_t value = 1;
    baseProcess->setPropertyInt(name, value);
    delete baseProcess;
}

TEST(SpecialModeBaseProcessTest, setPropertyInt_002)
{
    SpecialModeBaseProcess* baseProcess = new DemoModeProcess();
    uint32_t value = 0;
    baseProcess->setPropertyInt(nullptr, value);
}
