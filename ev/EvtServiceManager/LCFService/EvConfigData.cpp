#include "EvConfigData.h"
#include "include/ApplicationTable.h"
#include "EvFileIo.h"

#include "EvtUtil.h"
#define LOG_TAG "[EVT]EvConfig"
#include "Log.h"


inline bool getVehicleType(uint8_t type);

void EvConfigData::onStart()
{
    mProperty = new SLProperty (APP_ID_EV, true);
    mProperty->initPropertyCache();
    configData["POWER_MODE"] = -1;
}

bool EvConfigData::setEvBootConfig(std::string configDataMonitorList[], uint8_t size)
{
    // Checking debug Files
    EvtFileIo* pStoredData = new EvtFileIo(); // alloc FileIo
    if(pStoredData->onStart() && configData["EV_JSON_VERSION"] >= MIN_EV_JSON_VERSION)
    {
        LOGE("Start Ev debug mode! : ev.json version is [%d]", configData["EV_JSON_VERSION"]);
        debug_EH_Data = pStoredData->EH_Test_Data;
    }
    else
    {
        LOGE("Start Ev normal mode!");

        // Get Vehicle Type.
        configData["VEHICLE_TYPE"] = getConfigData("hybridType");
    }
    delete pStoredData; // release FileIo

    bool result = readEvConfigDataFromXml(configDataMonitorList, size);
    printEvData();
    return result;
}

void EvConfigData::setPropertyData(std::string name, std::string value)
{
    mProperty->setProperty(name.c_str(), value.c_str());
}

void EvConfigData::setPropertyData(std::string name, const int32_t i_value)
{
    mProperty->setProperty(name.c_str(), i_value);
}

void EvConfigData::syncPropertyData()
{
    mProperty->syncProperties();
}
char* EvConfigData::getPropertyData(std::string name)
{
    char* result = mProperty->getProperty(name.c_str());
    return result;
}

void EvConfigData::setConfigurationService(sp<IConfigurationManagerService> service)
{
    m_configManagerService = service;
}
int32_t EvConfigData::getConfigData(std::string lookup_data)
{
    //TODO: Need to More improve!! It is risky code.
    int32_t result = m_configManagerService->get_int(CONFIG_FILE, 2, lookup_data.c_str());

    if(result == -1)
    {
        if(lookup_data.compare("EVC_IMC_TF2_TIME")==0)
        {
            LOGV("[getConfigData] EVC_IMC_TF2_TIME(EVC_IMC_FT_TIME) is setting as default value [40]s");
            return 40;
        }

        if(lookup_data.compare("EVC_IMC_WKUP_TIME")==0)
        {
            LOGV("[getConfigData] EVC_IMC_WKUP_TIME is setting as default value [10]s");
            return 10;
        }
    }
    return result;
}
std::string EvConfigData::getConfigDataAsString(std::string lookup_data)
{
    /*
    sp<Buffer> get_string(const char* file_name, int32_t style, const char* lookup_data);
    */
    sp<Buffer> result = m_configManagerService->get_string(CONFIG_FILE, 2, lookup_data.c_str());
    if (result == NULL)
    {
        return "";
    }
    return std::string(result->data());
}
error_t EvConfigData::setConfigData(std::string lookup_data, int change_value)
{
    error_t result = m_configManagerService->change_set_int(CONFIG_FILE, 2, lookup_data.c_str(), change_value);
    LOGE("[ERROR] Fail Setting [%s:%d] on ConfigMgr. ERROR Reason:[%d]", lookup_data.c_str(), change_value, result);
    return result;
}

void EvConfigData::printEvData()
{
    std::string str("");
    for(auto it = configData.begin(); it != configData.end(); ++it)
    {
        str += it->first;
        str += " : ";
        str += std::to_string(it->second);
        str += "\n";
    }
    LOG_EV(str.c_str());
}

bool EvConfigData::readEvConfigDataFromXml(std::string configDataMonitorList[], uint8_t size)
{
    bool isEv = getVehicleType(configData["VEHICLE_TYPE"]);

    if(!isEv)
    {
        LOGE("This vehicle is not PHEV or BEV type. CCF::HybridType=[0x%02X]! Bye JLR", configData["VEHICLE_TYPE"]);
        return false;
    }

    // for(auto it = configList.begin() ; it != configList.end() ; ++it)
    for(uint8_t i=0 ; i<size ; i++)
    {
        const char* name = configDataMonitorList[i].c_str();
        configData[name] = getConfigData(name);
        //LOG_EV("readEvConfigDataFromXml[%s][%d]", name, configData[name]);
    }
    return true;
}

void EvConfigData::onChangeConfigData(std::string lookup_data)
{
    int32_t newValue = getConfigData(lookup_data.c_str());
    LOGE("onConfigDataChanged : Name[%s] : [%d] => [%d] ", lookup_data.c_str(), configData[lookup_data], newValue);
    configData[lookup_data] = newValue;

    //TODO: trigger dynamic trigger by changing XML (Except Provisioning Status check!!!)
    // for(auto element : EVT_configDataMonitorList)
    // {
    //     if(lookup_data.compare(element) == 0)
    //     {

    //     }
    // }

    //Temprary condition.
    if(lookup_data.compare("eCallstatus") == 0 || lookup_data.compare("bCallstatus") == 0)
    {
        EvChargeApp::GetInstance()->triggerECWstate();
    }
}

inline bool getVehicleType(uint8_t type)
{
    switch(type)
    {
        case _PHEV:      // PHEV
        {
            LOGV("This vehicle is PHEV Type!");
            return true;
        }
        case _BEV:      //BEV
        {
            LOGV("This vehicle is BEV Type!");
            return true;
        }
        default:
        {
            return false;
        }
    }
}

int32_t EvConfigData::getEvCfgData(std::string name)
{
    auto it = configData.find(name.c_str());
    if(it != configData.end())
    {
        return configData[name.c_str()];
    }
    else
    {
        LOGE("[ERROR] <%s> is not defined in Configure Data!! ", name.c_str());
        return -1;
    }
}

  void EvConfigData::readEvcTestData()
  {
       LOGE("EvConfigData::readEvcTestData() start");
       EvcTestFileIo* testFileIo = new EvcTestFileIo();
       testFileIo->onStart();
       LOGE("EvConfigData::readEvcTestData() End");
  }