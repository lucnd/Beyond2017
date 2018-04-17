#include "../SpecialModeServicesManager.h"
#include "doubles/MockConfigurationManagerService.h"
#include "doubles/MockApplicationManagerService.h"
#include "doubles/MockSystemManagerService.h"
#include "doubles/MockNGTPManagerService.h"
#include "utils/makeSp.h"


TEST(ServicesManagerTest, getConfigurationManager) {
    SpecialModeServicesManager smSvcMan;
    auto mockConfigMan = makeSp<MockConfigurationManagerService>();
    smSvcMan.m_ConfigurationMgr = mockConfigMan;
    EXPECT_EQ(mockConfigMan, smSvcMan.getConfigurationManager());
}


TEST(ServicesManagerTest, getApplicationManager) {
    SpecialModeServicesManager smSvcMan;
    auto mockAppMan = makeSp<MockApplicationManagerService>();
    smSvcMan.m_ApplicationMgr = mockAppMan;
    EXPECT_EQ(mockAppMan, smSvcMan.getApplicationManager());
}


TEST(ServicesManagerTest, getSystemManager) {
    SpecialModeServicesManager smSvcMan;
    auto mockSysMan = makeSp<MockSystemManagerService>();
    smSvcMan.m_SystemMgr = mockSysMan;
    EXPECT_EQ(mockSysMan, smSvcMan.getSystemManager());
}


TEST(ServicesManagerTest, getNGTPManager) {
    SpecialModeServicesManager smSvcMan;
    auto mockNgtpMan = makeSp<MockNGTPManagerService>();
    smSvcMan.m_NGTPMgr = mockNgtpMan;
    EXPECT_EQ(mockNgtpMan, smSvcMan.getNGTPManager());
}
