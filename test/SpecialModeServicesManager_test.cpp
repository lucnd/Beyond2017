#include <gtest/gtest.h>
#include <SpecialModeServicesManager.h>

#include <services/ConfigurationManagerService/IConfigurationManagerService.h>
#include <services/ConfigurationManagerService/IConfigurationManagerServiceType.h>

#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include <services/ApplicationManagerService/IApplicationManagerServiceType.h>

#include <services/NGTPManagerService/INGTPManagerService.h>
#include <services/NGTPManagerService/INGTPManagerServiceType.h>

#include <services/SystemManagerService/ISystemManagerService.h>
#include <services/SystemManagerService/ISystemManagerServiceType.h>

#include "services/PowerManagerService/IPowerManagerServiceType.h"
#include "services/PowerManagerService/IPowerManagerService.h"


TEST(SpecialModeServicesManager, getConfigurationManager_001) {
    auto specialModeServiceManger = new SpecialModeServicesManager();
    auto configurationManagerService_test = specialModeServiceManger->getConfigurationManager();
}


TEST(SpecialModeServicesManager, getApplicationManager_001) {
    auto specialModeServiceManger = new SpecialModeServicesManager();
    auto applicationManagerService_test = specialModeServiceManger->getApplicationManager();
}

TEST(SpecialModeServicesManager, getSystemManager_001) {
    auto specialModeServiceManger = new SpecialModeServicesManager();
    auto nGTPManagerService_test = specialModeServiceManger->getNGTPManager();
}

TEST(SpecialModeServicesManager, getNGTPManager_001) {
    auto specialModeServiceManger = new SpecialModeServicesManager();
    auto systemManagerService_test = specialModeServiceManger->getSystemManager();
}

TEST(SpecialModeServicesManager, getPowerManager_001) {
    auto specialModeServiceManger = new SpecialModeServicesManager();
    auto powerManagerServiceType_test = specialModeServiceManger->getPowerManager();
}