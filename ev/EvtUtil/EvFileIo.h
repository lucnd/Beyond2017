// hojunes.lee@lge.com
#ifndef _EV_FILE_IO_H_
#define _EV_FILE_IO_H_

#include <fstream>
#include <vector>
#include <iostream>

#include "minijson_reader.hpp"
#include "EvConfigData.h"
#include "EvChargeApp.h"

using namespace std;
using namespace minijson;

#define LOG_TAG "[EVT]EvtFileIo"
#include "Log.h"

#define FILE_NAME "/userdata/ev.json"
#define EVC_TEST_FILE_NAME "/userdata/EvTest.json"

class EvtFileIo
{
public:
    EvtFileIo(){};
    ~EvtFileIo(){};
    bool onStart();
    void readJason();

    int bufSize;
    char* buffer;
    vector<uint8_t> EH_Test_Data;
};

bool EvtFileIo::onStart()
{
    std::ifstream is(FILE_NAME, std::ifstream::binary);
    if (!is) {
        is.close();
        return false;
    }
    LOGE("[DBG] Detect EV debug file. ==> working debug mode.");

    // get bufSize of file:
    is.seekg(0, is.end);
    int bufSize = is.tellg();
    is.seekg(0, is.beg);

    buffer = new char[bufSize];

    LOGI("Reading Debug initial Data => [%d]bytes... ", bufSize);
    // read data as a block:
    is.read(buffer, bufSize-1);
    if (is) {
        LOGI("Debug initial Data read successfully.");
    }else{
        LOGE("[ERROR] only [%d] could be read", is.gcount());
    }
    is.close();
    readJason();
    delete[] buffer;
    return true;
}

void EvtFileIo::readJason()
{
    EvConfigData* cfgData = EvConfigData::GetInstance();
    buffer_context ctx(buffer, bufSize - 1);
    try
    {
      parse_object(ctx, [&](const char* k, value v)
      {
          dispatch(k)

          // // Read EVC config data.
          // << "EVC_IMC_IGNR_TIME" >>           [&] { cfgData->configData["EVC_IMC_IGNR_TIME"]      = v.as_long(); }
          // << "EVC_GWM_BUSOPEN_TIME" >>        [&] { cfgData->configData["EVC_GWM_BUSOPEN_TIME"]   = v.as_long(); }
          // << "EVC_CHRG_PLUG_RMND_TIME" >>     [&] { cfgData->configData["EVC_CHRG_PLUG_RMND_TIME"] = v.as_long();}
          // << "EVC_DEAD_BAND_TIME" >>          [&] { cfgData->configData["EVC_DEAD_BAND_TIME"]     = v.as_long(); }
          // << "EVC_EVI_PUSH_TIME" >>           [&] { cfgData->configData["EVC_EVI_PUSH_TIME"]      = v.as_long(); }
          // << "EVC_EVI_PUSH_PERC" >>           [&] { cfgData->configData["EVC_EVI_PUSH_PERC"]      = v.as_long(); }
          // << "MAX_NUMBER_DEPT_TIMERS" >>      [&] { cfgData->configData["MAX_NUMBER_DEPT_TIMERS"] = v.as_long(); }

          // // Read EVR config data.
          // << "EVR_EVI_PUSH_TIME" >>           [&] { cfgData->configData["EVR_EVI_PUSH_TIME"]  = v.as_long(); }
          // << "EVR_EVI_PUSH_PERC" >>           [&] { cfgData->configData["EVR_EVI_PUSH_PERC"]  = v.as_long(); }
          // << "EVR_EVI_LOC_PUSH_EN" >>         [&] { cfgData->configData["EVR_EVI_LOC_PUSH_EN"] = v.as_long();}

          // // Read EH config data.
          // << "EHORIZON_DATA_RX_TIMEOUT" >>    [&] { cfgData->configData["EHORIZON_DATA_RX_TIMEOUT"] = v.as_long(); }

          // // Read ECC config data.
          // << "ECC_DIRECT_PRECONDTIME" >>          [&] { cfgData->configData["ECC_DIRECT_PRECONDTIME"]         = v.as_long(); }
          // << "ECC_HVAC_NO_RESP_TIMEOUT" >>        [&] { cfgData->configData["ECC_HVAC_NO_RESP_TIMEOUT"]       = v.as_long(); }
          // << "ECC_DIRECT_PRECOND_LIMIT_NO_RES" >> [&] { cfgData->configData["ECC_DIRECT_PRECOND_LIMIT_NO_RES"] = v.as_long();}
          // << "ECC_FEATURE_ENABLE_DISABLE" >>      [&] { cfgData->configData["ECC_FEATURE_ENABLE_DISABLE"]     = v.as_long(); }
          // << "ECC_RES_REQUEST_APPROVAL_TIMEOUT" >> [&] { cfgData->configData["ECC_RES_REQUEST_APPROVAL_TIMEOUT"] = v.as_long(); }
          // << "ECC_SETTING_TRIGGER_TIME" >>        [&] { cfgData->configData["ECC_SETTING_TRIGGER_TIME"]       = v.as_long(); }
          // << "ECC_CAN_SETTING_IGNORE_TIME" >>     [&] { cfgData->configData["ECC_CAN_SETTING_IGNORE_TIME"]    = v.as_long(); }
          // << "ECC_CAN_STATUS_IGNORE_TIME" >>      [&] { cfgData->configData["ECC_CAN_STATUS_IGNORE_TIME"]     = v.as_long(); }
          // << "ECC_NM_TIMEOUT" >>                  [&] { cfgData->configData["ECC_NM_TIMEOUT"]                 = v.as_long(); }

          // Read Internal config Data.
          << "VEHICLE_TYPE" >>            [&] { cfgData->configData["VEHICLE_TYPE"] = v.as_long(); }
          // << "EVC_IMC_WKUP_TIME" >>       [&] { cfgData->configData["EVC_IMC_WKUP_TIME"] = v.as_long(); }
          // << "ECC_DIRECT_PRECONDITION" >> [&] { cfgData->configData["ECC_DIRECT_PRECONDITION"] = v.as_long(); }

          // Read Internal config Data.
          << "EV_JSON_VERSION" >>            [&] { cfgData->configData["EV_JSON_VERSION"] = v.as_long(); }

          // EH test code.
          << "EH_TEST_Data" >> [&]
          {
              parse_array(ctx, [&](value v)
              {
                  EH_Test_Data.push_back(v.as_long());
              });
          }

          //<< "AppModeHBD" >>            [&] { cfgData->configData["AppModeHBD"] = v.as_long(); }
          << any >> [&] { ignore(ctx); };
      });
    }
    catch(parse_error e)
    {
        LOGE("ERROR CODE : %s, Offset : %d", e.what(), e.offset());
    }
}

class EvcTestFileIo
{
public:
    EvcTestFileIo(){};
    ~EvcTestFileIo(){};
    bool onStart();
    void readJason();
    void printChargeSettings();
    int bufSize;
    char* buffer;
};

bool  EvcTestFileIo::onStart()
{
    LOGE("EvcTestFileIo::onStart()");
    std::ifstream is(EVC_TEST_FILE_NAME, std::ifstream::binary);
    if (!is) {
        is.close();
        return false;
    }
    LOGE("[DBG] Detect EVC Test debug file. ==> working debug mode.");

    // get bufSize of file:
    is.seekg(0, is.end);
    int bufSize = is.tellg();
    is.seekg(0, is.beg);

    buffer = new char[bufSize];

    LOGI("Reading EVC Test Debug initial Data => [%d]bytes... ", bufSize);
    // read data as a block:
    is.read(buffer, bufSize-1);
    if (is) {
        LOGI("Debug initial Data read successfully.");
    }else{
        LOGE("[ERROR] only [%d] could be read", is.gcount());
    }
    is.close();
    readJason();
    printChargeSettings();
    delete[] buffer;
    return true;
}

void EvcTestFileIo::readJason()
{
    int32_t num_data;
    bool bTrue;
    int tariff_index;
    tariff_index = -1;

    eChargeSettings _eChargeSettings;
    eChargeNowSetting _eChargeNowSetting;
    eMaxStateOfCharge _eMaxStateOfCharge;
    ePermanentMaxSoc _ePermanentMaxSoc;
    eOneOffMaxSoc _eOneOffMaxSoc;
    eDepartureTimers _eDepartureTimers;
    eDepartureTimerList _eDepartureTimerList;
    eTariffs _eTariffs;
    eTariffList _eTariffList;

    _eChargeSettings.omit_chargeNowSetting();
    _eChargeSettings.omit_maxStateOfCharge();
    _eChargeSettings.omit_chargingModeChoice();
    _eChargeSettings.omit_departureTimers();
    _eChargeSettings.omit_tariffs();

    buffer_context ctx(buffer, bufSize - 1);
    try{
        parse_object(ctx, [&](const char* k, value v)
        {
          dispatch(k)
          << "ChargeSettingRequest" >>  [&]
          {
            parse_object(ctx, [&](const char* k, value v)
            {
              dispatch(k)
              << "CreationTimeStamp" >>  [&] { num_data = v.as_long(); LOGE("CreationTimeStamp : %d", num_data);}
              << "chargeNowSetting" >>   [&] {
                                            num_data = v.as_long();
                                            _eChargeNowSetting = num_data;
                                            _eChargeSettings.set_chargeNowSetting(_eChargeNowSetting);
                                            if( *(_eChargeSettings.get_chargeNowSetting()) == svc::ChargeSettings::chargeNowSetting::force_on)
                                            {
                                              LOGE("ChargeNowSetting : force_on - 1");
                                            }
                                            else if( *(_eChargeSettings.get_chargeNowSetting()) == svc::ChargeSettings::chargeNowSetting::ChargeNowSetting_default)
                                            {
                                              LOGE("ChargeNowSetting : ChargeNowSetting_default - 0");
                                            }
                                            else if( *(_eChargeSettings.get_chargeNowSetting()) == svc::ChargeSettings::chargeNowSetting::force_off)
                                            {
                                              LOGE("ChargeNowSetting : force_off - 2");
                                            }
                                            else if( *(_eChargeSettings.get_chargeNowSetting()) == svc::ChargeSettings::chargeNowSetting::ChargeNowSetting_unknown)
                                            {
                                              LOGE("ChargeNowSetting : ChargeNowSetting_unknown - 3");
                                            }
                                            else
                                            {
                                              LOGE("ChargeNowSetting : Invalid Value");
                                            }
                                            }
              << "MaxStateOfCharge">>    [&]
              {
                parse_object(ctx, [&](const char* k, value v)
                {
                  dispatch(k)
                  << "permanentMaxSoc_field" >>     [&] {
                                                        // -1 : NoChange, 0 : Clear 0 , 1~100 : MaxSOC
                                                        num_data = v.as_long();
                                                        if(num_data == -1)
                                                        {
                                                            _ePermanentMaxSoc.set_noChange(1);
                                                            LOGE("permanentMaxSoc_field : noChange");
                                                        }
                                                        else if(num_data == 0)
                                                        {
                                                            _ePermanentMaxSoc.set_clear(1);
                                                            LOGE("permanentMaxSoc_field : Clear");
                                                        }
                                                        else
                                                        {
                                                            _ePermanentMaxSoc.set_maxSoc(num_data);
                                                            LOGE("permanentMaxSoc_field : %d", num_data);
                                                        }
                                                        _eMaxStateOfCharge.set_permanentMaxSoc(_ePermanentMaxSoc);
                                                        }
                  << "oneOffMaxSoc_field" >>        [&] {
                                                        // -1 : NoChange, 0 : Clear 0 , 1~100 : MaxSOC
                                                        num_data = v.as_long();
                                                        if(num_data == -1)
                                                        {
                                                            _eOneOffMaxSoc.set_noChange(1);
                                                            LOGE("oneOffMaxSoc_field : noChange");
                                                        }
                                                        else if(num_data == 0)
                                                        {
                                                            _eOneOffMaxSoc.set_clear(1);
                                                            LOGE("oneOffMaxSoc_field : Clear");
                                                        }
                                                        else
                                                        {
                                                            _eOneOffMaxSoc.set_maxSoc(num_data);
                                                            LOGE("oneOffMaxSoc_field : %d", num_data);
                                                        }
                                                        _eMaxStateOfCharge.set_oneOffMaxSoc(_eOneOffMaxSoc);
                                                        }
                  <<any>> [&]{ ignore(ctx); };
                });
                _eChargeSettings.set_maxStateOfCharge(_eMaxStateOfCharge);
              } // << "MaxStateOfCharge">>

             << "chargingModeChoice_field">>  [&] { num_data = v.as_long();
                                                   _eChargeSettings.set_chargingModeChoice(num_data);
                                                  LOGE("chargingModeChoice_field : %d", num_data);}

             << "DepartureTimer_Count">>      [&] { num_data = v.as_long(); LOGE("DepartureTimer_Count : %d", num_data);}
             << "DepartureTimer">>  [&]
             {
                parse_array(ctx, [&] (value v)
                {
                  if(v.type() == Object)
                  {
                    eDepartureTimer _eDepartureTimer;
                    bool bTimerDefEnable = true;
                    parse_object(ctx, [&](const char* k, value v)
                    {
                        dispatch(k)
                        << "timerIndex_field">>     [&] {num_data = v.as_long();
                                                         _eDepartureTimer.set_timerIndex(num_data);
                                                         LOGE("get_timerIndex  : %d",_eDepartureTimer.get_timerIndex());}
                        <<"timerDefinition_enable">>     [&] {
                                                                 bTimerDefEnable = v.as_bool();
                                                                 LOGE("timerDefinition_enable : %s", bTimerDefEnable ? "TRUE" : "FALSE");}
                        << "_DepartureTimerDefinition">>    [&]
                        {
                          eDepartureTimerDefinition _eDepartureTimerDefinition;
                          eDepartureTimerType _eDepartureTimerType;
                          parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "timerType_field">>  [&] {
                                                          num_data = v.as_long();
                                                          switch(num_data)
                                                          {
                                                            case 0:
                                                                _eDepartureTimerType = svc::DepartureTimerType::DepartureTimerType_off;
                                                                _eDepartureTimerDefinition.set_timerType(_eDepartureTimerType);
                                                                LOGE("DepartureTimerType::DepartureTimerType_off");
                                                                break;
                                                            case 1:
                                                                _eDepartureTimerType = svc::DepartureTimerType::chargeOnly;
                                                                _eDepartureTimerDefinition.set_timerType(_eDepartureTimerType);
                                                                LOGE("DepartureTimerType::chargeOnly");
                                                                break;
                                                            case 2:
                                                                _eDepartureTimerType = svc::DepartureTimerType::preconditionOnly;
                                                                _eDepartureTimerDefinition.set_timerType(_eDepartureTimerType);
                                                                LOGE("DepartureTimerType::preconditionOnly");
                                                                break;
                                                            case 3:
                                                                _eDepartureTimerType = svc::DepartureTimerType::bothChargeAndPrecondition;
                                                                _eDepartureTimerDefinition.set_timerType(_eDepartureTimerType);
                                                                LOGE("DepartureTimerType::bothChargeAndPrecondition");
                                                                break;
                                                            default:
                                                                LOGE("[ERROR]DepartureTimerType : Invalid Value : %d", num_data);
                                                                break;
                                                          }
                                                        }//<< "timerType_field">>
                             <<"departureTime_field">>  [&]
                             {
                              eDepartureTime _eDepartureTime;
                              parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>  [&] { num_data = v.as_long();
                                                         _eDepartureTime.set_hour(num_data);
                                                         LOGE("hour_field : %d", _eDepartureTime.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                           _eDepartureTime.set_minute(num_data);
                                                           LOGE("minute_field : %d", _eDepartureTime.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eDepartureTimerDefinition.set_departureTime(_eDepartureTime);
                             } // <<"departureTime_field">>
                             <<"timerTarget_field">>  [&]
                             {
                               eTimerTarget _eTimerTarget;
                               bool isSingleDay;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 <<"isSingleDay">>  [&]{ num_data = v.as_long();
                                                         isSingleDay = (num_data == 1) ? true : false;
                                                         LOGE("isSingleDay : %d", isSingleDay);
                                                       }
                                 <<"singleDay_field">>  [&]
                                 {
                                   eSingleDay _eSingleDay;
                                   parse_object(ctx, [&](const char* k, value v)
                                   {
                                     dispatch(k)
                                     << "day">>    [&] { num_data = v.as_long();
                                                        _eSingleDay.set_day(num_data);
                                                        LOGE("day : %d", _eSingleDay.get_day());}
                                     << "month">>  [&] { num_data = v.as_long();
                                                        _eSingleDay.set_month(num_data);
                                                        LOGE("month : %d", _eSingleDay.get_month());}
                                     << "year">>   [&] { num_data = v.as_long();
                                                        _eSingleDay.set_year(num_data);
                                                        LOGE("year : %d", _eSingleDay.get_year());}
                                     <<any>> [&]{ ignore(ctx); };
                                   });
                                   if(isSingleDay)
                                   {
                                     _eTimerTarget.set_singleDay(_eSingleDay);
                                   }
                                 }// <<"singleDay_field">>
                                 <<"repeatSchedule_field">>  [&]
                                 {
                                  eDepartureRepeatSchedule _eDepartureRepeatSchedule;
                                   parse_object(ctx, [&](const char* k, value v)
                                   {
                                     dispatch(k)
                                     << "mon">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_monday(bTrue);
                                                        LOGE("mon : %d", _eDepartureRepeatSchedule.get_monday());}
                                     << "tue">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_tuesday(bTrue);
                                                        LOGE("tue : %d", _eDepartureRepeatSchedule.get_tuesday());}
                                     << "wed">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_wednesday(bTrue);
                                                        LOGE("wed : %d", _eDepartureRepeatSchedule.get_wednesday());}
                                     << "thu">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_thursday(bTrue);
                                                        LOGE("thu : %d", _eDepartureRepeatSchedule.get_thursday());}
                                     << "fri">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_friday(bTrue);
                                                        LOGE("fri : %d", _eDepartureRepeatSchedule.get_friday());}
                                     << "sat">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_saturday(bTrue);
                                                        LOGE("sat : %d", _eDepartureRepeatSchedule.get_saturday());}
                                     << "sun">>    [&] { bTrue = v.as_bool();
                                                        _eDepartureRepeatSchedule.set_sunday(bTrue);
                                                        LOGE("sun : %d", _eDepartureRepeatSchedule.get_sunday());}
                                     <<any>> [&]{ ignore(ctx); };
                                   });
                                   if(!isSingleDay)
                                   {
                                     _eTimerTarget.set_repeatSchedule(_eDepartureRepeatSchedule);
                                   }
                                 }// <<"repeatSchedule_field">>
                                 <<any>> [&]{ ignore(ctx);};
                               });
                               _eDepartureTimerDefinition.set_timerTarget(_eTimerTarget);
                             }// <<"timerTarget_field">>
                             <<any>> [&]{ ignore(ctx);};
                           });
                          if(bTimerDefEnable)
                          {
                              _eDepartureTimer.set_departureTimerDefinition(_eDepartureTimerDefinition);
                          }
                          else
                          {
                              _eDepartureTimer.omit_departureTimerDefinition();
                          }
                          LOGE("bDepartureTimerDefinition is Present : %s", _eDepartureTimer.departureTimerDefinition_is_present()?"T":"F");
                        }// << "_DepartureTimerDefinition">>
                        <<any>> [&]{ ignore(ctx); };
                    });
                    LOGE("Prepend Index : %d", _eDepartureTimer.get_timerIndex());
                    _eDepartureTimerList.prepend(_eDepartureTimer);
                    LOGE("(v.type() == Object) End");
                  } // if(v.type() == Object)
                });
                _eDepartureTimers.set_timers(_eDepartureTimerList);
                _eChargeSettings.set_departureTimers(_eDepartureTimers);
             } // << "DepartureTimer">>

             << "Tariff">>  [&]
             {
               parse_array(ctx, [&] (value v)
               {
                 if(v.type() == Object)
                 {
                   eTariff _eTariff;
                   parse_object(ctx, [&](const char* k, value v)
                   {
                     dispatch(k)
                     << "tariffIndex_field">>  [&] { num_data = v.as_long();
                                                _eTariff.set_tariffIndex(num_data);
                                                LOGE("tariffIndex_field : %d", _eTariff.get_tariffIndex());}
                     << "_TariffDefinition">>  [&]
                     {
                       eTariffDefinition _eTariffDefinition;
                       parse_object(ctx, [&](const char* k, value v)
                       {
                         dispatch(k)
                         << "isEnabled_field">>    [&] { bTrue = v.as_bool();
                                                        _eTariffDefinition.set_isEnabled(bTrue);
                                                        LOGE("isEnabled_field : %d", _eTariffDefinition.get_isEnabled());}
                         << "_DayOfWeekSchedule">>  [&]
                         {
                           eTariffRepeatSchedule _eTariffRepeatSchedule;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "mon">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_monday(bTrue);
                                                LOGE("mon : %d", _eTariffRepeatSchedule.get_monday());}
                             << "tue">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_tuesday(bTrue);
                                                LOGE("tue : %d", _eTariffRepeatSchedule.get_tuesday());}
                             << "wed">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_wednesday(bTrue);
                                                LOGE("wed : %d", _eTariffRepeatSchedule.get_wednesday());}
                             << "thu">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_thursday(bTrue);
                                                LOGE("thu : %d", _eTariffRepeatSchedule.get_thursday());}
                             << "fri">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_friday(bTrue);
                                                LOGE("fri : %d", _eTariffRepeatSchedule.get_friday());}
                             << "sat">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_saturday(bTrue);
                                                LOGE("sat : %d", _eTariffRepeatSchedule.get_saturday());}
                             << "sun">>    [&] { bTrue = v.as_bool();
                                                _eTariffRepeatSchedule.set_sunday(bTrue);
                                                LOGE("sun : %d", _eTariffRepeatSchedule.get_sunday());}
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_repeatSchedule(_eTariffRepeatSchedule);
                         } // << "_DayOfWeekSchedule">>
                         << "tariffZoneA_field">>  [&]
                         {
                           eTariifZoneA _eTariifZoneA;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "endTime_field">>  [&]
                             {
                               eTariifEndTimeA _eTariifEndTimeA;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>    [&] { num_data = v.as_long();
                                                          _eTariifEndTimeA.set_hour(num_data);
                                                          LOGE("TariffZone A EndTime hour_field : %d", _eTariifEndTimeA.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                          _eTariifEndTimeA.set_minute(num_data);
                                                          LOGE("TariffZone A EndTime minute_field : %d", _eTariifEndTimeA.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eTariifZoneA.set_endTime(_eTariifEndTimeA);
                             } // << "endTime_field">>
                             << "bandType_field">>    [&] {
                                                            eTariffBandTypeA _eTariffBandTypeA;
                                                            num_data = v.as_long();
                                                            switch(num_data)
                                                            {
                                                              case 0:
                                                                _eTariffBandTypeA = svc::TariffBandType::offPeak;
                                                                _eTariifZoneA.set_bandType(_eTariffBandTypeA);
                                                                LOGE("TariffZONE A BandType : offPeak");
                                                                break;
                                                              case 1:
                                                                _eTariffBandTypeA = svc::TariffBandType::midPeak;
                                                                _eTariifZoneA.set_bandType(_eTariffBandTypeA);
                                                                LOGE("TariffZONE A BandType : midPeak");
                                                                break;
                                                              case 2:
                                                                _eTariffBandTypeA = svc::TariffBandType::peak;
                                                                _eTariifZoneA.set_bandType(_eTariffBandTypeA);
                                                                LOGE("TariffZONE A BandType : peak");
                                                                break;
                                                              default:
                                                                LOGE("TariffZONE A BandType : Invalid Value");
                                                                break;
                                                            }
                                                            _eTariifZoneA.set_bandType(_eTariffBandTypeA);
                                                          }
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_tariffZoneA(_eTariifZoneA);
                         }// << "tariffZoneA_field">>
                         << "tariffZoneB_field">>  [&]
                         {
                           eTariifZoneB _eTariifZoneB;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "endTime_field">>  [&]
                             {
                               eTariifEndTimeB _eTariifEndTimeB;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>    [&] { num_data = v.as_long();
                                                          _eTariifEndTimeB.set_hour(num_data);
                                                          LOGE("TariffZone B EndTime hour_field : %d", _eTariifEndTimeB.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                          _eTariifEndTimeB.set_minute(num_data);
                                                          LOGE("TariffZone B EndTime minute_field : %d", _eTariifEndTimeB.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eTariifZoneB.set_endTime(_eTariifEndTimeB);
                             } // << "endTime_field">>
                             << "bandType_field">>    [&] {
                                                            eTariffBandTypeB _eTariffBandTypeB;
                                                            num_data = v.as_long();
                                                            switch(num_data)
                                                            {
                                                              case 0:
                                                                _eTariffBandTypeB = svc::TariffBandType::offPeak;
                                                                _eTariifZoneB.set_bandType(_eTariffBandTypeB);
                                                                LOGE("TariffZONE B BandType : offPeak");
                                                                break;
                                                              case 1:
                                                                _eTariffBandTypeB = svc::TariffBandType::midPeak;
                                                                _eTariifZoneB.set_bandType(_eTariffBandTypeB);
                                                                LOGE("TariffZONE B BandType : midPeak");
                                                                break;
                                                              case 2:
                                                                _eTariffBandTypeB = svc::TariffBandType::peak;
                                                                _eTariifZoneB.set_bandType(_eTariffBandTypeB);
                                                                LOGE("TariffZONE B BandType : peak");
                                                                break;
                                                              default:
                                                                LOGE("TariffZONE B BandType : Invalid Value");
                                                                break;
                                                            }
                                                            _eTariifZoneB.set_bandType(_eTariffBandTypeB);
                                                          }
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_tariffZoneB(_eTariifZoneB);
                         }// << "tariffZoneB_field">>

                         << "tariffZoneC_field">>  [&]
                         {
                           eTariifZoneC _eTariifZoneC;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "endTime_field">>  [&]
                             {
                               eTariifEndTimeC _eTariifEndTimeC;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>    [&] { num_data = v.as_long();
                                                          _eTariifEndTimeC.set_hour(num_data);
                                                          LOGE("TariffZone C EndTime hour_field : %d", _eTariifEndTimeC.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                          _eTariifEndTimeC.set_minute(num_data);
                                                          LOGE("TariffZone C EndTime minute_field : %d", _eTariifEndTimeC.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eTariifZoneC.set_endTime(_eTariifEndTimeC);
                             } // << "endTime_field">>
                             << "bandType_field">>    [&] {
                                                            eTariffBandTypeC _eTariffBandTypeC;
                                                            num_data = v.as_long();
                                                            switch(num_data)
                                                            {
                                                              case 0:
                                                                _eTariffBandTypeC = svc::TariffBandType::offPeak;
                                                                _eTariifZoneC.set_bandType(_eTariffBandTypeC);
                                                                LOGE("TariffZONE C BandType : offPeak");
                                                                break;
                                                              case 1:
                                                                _eTariffBandTypeC = svc::TariffBandType::midPeak;
                                                                _eTariifZoneC.set_bandType(_eTariffBandTypeC);
                                                                LOGE("TariffZONE C BandType : midPeak");
                                                                break;
                                                              case 2:
                                                                _eTariffBandTypeC = svc::TariffBandType::peak;
                                                                _eTariifZoneC.set_bandType(_eTariffBandTypeC);
                                                                LOGE("TariffZONE C BandType : peak");
                                                                break;
                                                              default:
                                                                LOGE("TariffZONE C BandType : Invalid Value");
                                                                break;
                                                            }
                                                            _eTariifZoneC.set_bandType(_eTariffBandTypeC);
                                                          }
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_tariffZoneC(_eTariifZoneC);
                         }// << "tariffZoneC_field">>

                         << "tariffZoneD_field">>  [&]
                         {
                           eTariifZoneD _eTariifZoneD;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "endTime_field">>  [&]
                             {
                               eTariifEndTimeD _eTariifEndTimeD;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>    [&] { num_data = v.as_long();
                                                          _eTariifEndTimeD.set_hour(num_data);
                                                          LOGE("TariffZone D EndTime hour_field : %d", _eTariifEndTimeD.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                          _eTariifEndTimeD.set_minute(num_data);
                                                          LOGE("TariffZone D EndTime minute_field : %d", _eTariifEndTimeD.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eTariifZoneD.set_endTime(_eTariifEndTimeD);
                             } // << "endTime_field">>
                             << "bandType_field">>    [&] {
                                                            eTariffBandTypeD _eTariffBandTypeD;
                                                            num_data = v.as_long();
                                                            switch(num_data)
                                                            {
                                                              case 0:
                                                                _eTariffBandTypeD = svc::TariffBandType::offPeak;
                                                                _eTariifZoneD.set_bandType(_eTariffBandTypeD);
                                                                LOGE("TariffZONE D BandType : offPeak");
                                                                break;
                                                              case 1:
                                                                _eTariffBandTypeD = svc::TariffBandType::midPeak;
                                                                _eTariifZoneD.set_bandType(_eTariffBandTypeD);
                                                                LOGE("TariffZONE D BandType : midPeak");
                                                                break;
                                                              case 2:
                                                                _eTariffBandTypeD = svc::TariffBandType::peak;
                                                                _eTariifZoneD.set_bandType(_eTariffBandTypeD);
                                                                LOGE("TariffZONE D BandType : peak");
                                                                break;
                                                              default:
                                                                LOGE("TariffZONE D BandType : Invalid Value");
                                                                break;
                                                            }
                                                            _eTariifZoneD.set_bandType(_eTariffBandTypeD);
                                                          }
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_tariffZoneD(_eTariifZoneD);
                         }// << "tariffZoneD_field">>
                         << "tariffZoneE_field">>  [&]
                         {
                           eTariifZoneE _eTariifZoneE;
                           parse_object(ctx, [&](const char* k, value v)
                           {
                             dispatch(k)
                             << "endTime_field">>  [&]
                             {
                               eTariifEndTimeE _eTariifEndTimeE;
                               parse_object(ctx, [&](const char* k, value v)
                               {
                                 dispatch(k)
                                 << "hour_field">>    [&] { num_data = v.as_long();
                                                          _eTariifEndTimeE.set_hour(num_data);
                                                          LOGE("TariffZone E EndTime hour_field : %d", _eTariifEndTimeE.get_hour());}
                                 << "minute_field">>  [&] { num_data = v.as_long();
                                                          _eTariifEndTimeE.set_minute(num_data);
                                                          LOGE("TariffZone E EndTime minute_field : %d", _eTariifEndTimeE.get_minute());}
                                 <<any>> [&]{ ignore(ctx); };
                               });
                               _eTariifZoneE.set_endTime(_eTariifEndTimeE);
                             } // << "endTime_field">>
                             << "bandType_field">>    [&] {
                                                            eTariffBandTypeE _eTariffBandTypeE;
                                                            num_data = v.as_long();
                                                            switch(num_data)
                                                            {
                                                              case 0:
                                                                _eTariffBandTypeE = svc::TariffBandType::offPeak;
                                                                _eTariifZoneE.set_bandType(_eTariffBandTypeE);
                                                                LOGE("TariffZONE E BandType : offPeak");
                                                                break;
                                                              case 1:
                                                                _eTariffBandTypeE = svc::TariffBandType::midPeak;
                                                                _eTariifZoneE.set_bandType(_eTariffBandTypeE);
                                                                LOGE("TariffZONE E BandType : midPeak");
                                                                break;
                                                              case 2:
                                                                _eTariffBandTypeE = svc::TariffBandType::peak;
                                                                _eTariifZoneE.set_bandType(_eTariffBandTypeE);
                                                                LOGE("TariffZONE E BandType : peak");
                                                                break;
                                                              default:
                                                                LOGE("TariffZONE E BandType : Invalid Value");
                                                                break;
                                                            }
                                                            _eTariifZoneE.set_bandType(_eTariffBandTypeE);
                                                          }
                             <<any>> [&]{ ignore(ctx); };
                           });
                           _eTariffDefinition.set_tariffZoneE(_eTariifZoneE);
                         }// << "tariffZoneE_field">>
                         <<any>> [&]{ ignore(ctx); };
                       });
                       _eTariff.set_tariffDefinition(_eTariffDefinition);
                       LOGE("TariffDefinition is Present : %d", _eTariff.tariffDefinition_is_present());
                     }// << "_TariffDefinition">>
                     <<any>> [&]{ LOGE("Tariff Ignore"); ignore(ctx); };
                   });
                   _eTariffList.prepend(_eTariff);
                   LOGE("Tariff - (v.type() == Object) End");
                 } // if(v.type() == Object)
               }); // parse_array
               _eTariffs.set_tariffs(_eTariffList);
               _eChargeSettings.set_tariffs(_eTariffs);
             } // << "Tariff">>
              << any >> [&] { LOGE("ChargeSettingRequest Ignore"); ignore(ctx); };
            });
          }// << "ChargeSettingRequest" >>
         << any >> [&] {  LOGE("Root Ignore"); ignore(ctx); };
        });
        LOGE("PRESENT ChargeNowSetting[%d]MaxSOC[%d]ChargeModeChoice[%d]DepartureTimer[%d]Tariffs[%d]"
            , _eChargeSettings.chargeNowSetting_is_present()
            , _eChargeSettings.maxStateOfCharge_is_present()
            , _eChargeSettings.chargingModeChoice_is_present()
            , _eChargeSettings.departureTimers_is_present()
            , _eChargeSettings.tariffs_is_present());
        EvChargeApp::GetInstance()->DBG_NgtpMessage(&_eChargeSettings);
    }
    catch(parse_error e)
    {
        LOGE("ERROR CODE : %s Offset : %d", e.what(), e.offset());
    }
}

void EvcTestFileIo::printChargeSettings()
{
    LOGE("###########Print Charge Settings###############");
}
#endif // _EV_FILE_IO_H_
