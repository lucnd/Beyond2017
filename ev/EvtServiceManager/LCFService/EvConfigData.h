#ifndef _EV_CONFIG_DATA_
#define _EV_CONFIG_DATA_

#include <map>
#include <string>
#include <vector>
#include <list>
#include "services/ConfigurationManagerService/IConfigurationManagerService.h"
#include "core/SystemService.h"
#include "EvtSingleton.h"

#define MIN_EV_JSON_VERSION 4

class EvConfigData : public EvtSingleton<EvConfigData>
{

public:
    EvConfigData(){};
    ~EvConfigData(){};

    void onStart();
    void printEvData();

    void setPropertyData(std::string name, std::string value);
    void setPropertyData(std::string name, const int32_t i_value);
    void syncPropertyData();
    char* getPropertyData(std::string name);
    std::map<std::string, int32_t> configData;

    void setConfigurationService(sp<IConfigurationManagerService> service);
    int32_t getConfigData(std::string lookup_data);
    std::string getConfigDataAsString(std::string lookup_data);
    error_t setConfigData(std::string lookup_data, int change_value);

    bool setEvBootConfig(std::string configDataMonitorList[], uint8_t size);
    void onChangeConfigData(std::string lookup_data);

    int32_t getEvCfgData(std::string name);
    void readEvcTestData();

    std::vector<uint8_t> debug_EH_Data;

private:
    friend class EHAppServer;
    sp<IConfigurationManagerService> m_configManagerService;
    SLProperty* mProperty;

    bool readEvConfigDataFromXml(std::string configDataMonitorList[], uint8_t size);
};

#define _PHEV   0x06
#define _BEV    0x07

#endif //_EV_CONFIG_DATA_