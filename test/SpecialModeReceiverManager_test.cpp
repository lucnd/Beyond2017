#include <gtest/gtest.h>
#include <SpecialModeReceiverManager.h>

TEST(SpecialModeConfigurationReceiver, onConfigDataChanged) {

    sp<Buffer> buf = new Buffer();
    buf->setSize(1);

    SpecialModeReceiverManager specialModeReceiverManager;

    specialModeReceiverManager.m_ServicesMgr =  new SpecialModeServicesManager();

    specialModeReceiverManager.initializeReceiver();


    buf->setSize(1);
    specialModeReceiverManager.m_configReceiver->onConfigDataChanged(buf);

    buf->setSize(0);
    specialModeReceiverManager.m_configReceiver->onConfigDataChanged(buf);
}

TEST(SpecialModeReceiverManager, initializeReceiver) {

    SpecialModeReceiverManager specialModeReceiverManager;
    SpecialModeServicesManager specialModeServiceManager;

    specialModeReceiverManager.m_configManager = specialModeServiceManager.getConfigurationManager();

    specialModeReceiverManager.initializeReceiver();
}

TEST(SpecialModeConfigurationReceiver, specialModeConfigurationReceiverDetructor) {
    SpecialModeReceiverManager specialModeReceiverManager;
    SpecialModeReceiverManager::SpecialModeConfigurationReceiver specialModeConfigurationReceiver(*&specialModeReceiverManager);
    
}
