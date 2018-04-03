#include <gtest/gtest.h>
#include <SpecialModeApplication.h>

TEST(SpecialModeApplication, DISABLED_CreateAndDestroy) {
    auto pApp = createApplication();
    EXPECT_NO_THROW(pApp->onCreate());
    EXPECT_NO_THROW(pApp->onDestroy());
    destroyApplication(pApp);
}


TEST(SpecialModeApplication, DISABLED_ConstructorDestructorCreateDestroy)
{
    SpecialModeApplication *smApp = new SpecialModeApplication();
    smApp->m_ServicesMgr.force_set(new SpecialModeServicesManager());
    smApp->onCreate();
    smApp->onDestroy();
    smApp->m_ServicesMgr.clear();
    smApp->m_ServicesMgr.force_set(nullptr);
    smApp->onCreate();
    smApp->onDestroy();
    delete smApp;
}

TEST(SpecialModeApplication, provisionUnProvisionSpecialMode)
{
    SpecialModeApplication smApp;
    auto ret = smApp.provisionSpecialMode();
    EXPECT_TRUE(ret == true);
    ret = smApp.unprovisionSpecialMode();
    EXPECT_TRUE(ret == true);
}

TEST(SpecialModeApplication, doAfterProvisionUnprovision)
{
    SpecialModeApplication smApp;
    smApp.doAfterProvision();
    smApp.doAfterUnprovision();
}

TEST(SpecialModeApplication, setGetSpecialModeType)
{
    SpecialModeApplication smApp;
    uint32_t type {1};
    smApp.setSpecialModeType(type);
    auto ret = smApp.getSpecialModeType();
    EXPECT_EQ(type, ret);
}

TEST(SpecialModeApplication, setProvisioningFlag)
{
    SpecialModeApplication smApp;

    smApp.setProvisioningFlag(true);
}

TEST(SpecialModeApplication, onPostReceived)
{
    SpecialModeApplication smApp;
    sp<Post> post;

    smApp.onPostReceived(post);
}

TEST(SpecialModeApplication, doSpecialModeHandler)
{
    SpecialModeApplication smApp;
    uint32_t what = 1000;
    sp<sl::Message> message;

    smApp.doSpecialModeHandler(message);
}

TEST(SpecialModeApplication, initializeSpecialModeProcess)
{
    SpecialModeApplication smApp;

    smApp.initializeSpecialModeProcess();
}

TEST(SpecialModeApplication, releaseSpecialModeProcess)
{
    SpecialModeApplication smApp;

    smApp.releaseSpecialModeProcess();
}

TEST(SpecialModeApplication, getPropertyWrap)
{
    SpecialModeApplication smApp;

    std::string ret = smApp.getPropertyWrap(nullptr);
    EXPECT_EQ(-1, std::stoi(ret));
}

TEST(SpecialModeApplication, setPropertyCharInt)
{
    SpecialModeApplication smApp;
    std::string name("demoModeTime");
    std::string value("1");
    bool sync_now {false};
    smApp.setPropertyChar(name.c_str(), value.c_str(), sync_now);
    smApp.setPropertyChar(nullptr, value.c_str(), sync_now);
    smApp.setPropertyInt(name.c_str(), std::stoi(value), sync_now);
    smApp.setPropertyInt(nullptr, std::stoi(value), sync_now);
}



