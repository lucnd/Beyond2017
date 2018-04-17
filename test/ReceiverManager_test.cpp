#include "../SpecialModeReceiverManager.h"
#include "doubles/MockConfigurationManagerService.h"
#include "doubles/PartialMockHandler.h"
#include "utils/makeSp.h"


using namespace ::testing;


TEST(ReceiverManagerTest, registerConfigReceiverOnConstruction) {
    auto pMockConfigMan = makeSp<MockConfigurationManagerService>();
    EXPECT_CALL(*pMockConfigMan, registerReceiver(_, _, _))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(E_OK));

    auto smSvcMan = makeSp<SpecialModeServicesManager>();
    smSvcMan->m_ConfigurationMgr = pMockConfigMan;

    SpecialModeReceiverManager smReceiverMan(smSvcMan, makeSp<PartialMockHandler>());
}
