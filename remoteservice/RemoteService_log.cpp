/**
* \file    RemoteService_log.cpp
* \brief     Implementation of RemoteService_RCC Class.
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.11.20
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/
#define LOG_TAG "RVC_log"
#include "Log.h"

#include "RemoteService_log.h"


#define PATH_APPS       "/userdata/apps/"
#define PATH_APPS_RVC   "/userdata/apps/rvc/"
#define PATH_LOG_FILE   "/userdata/apps/rvc/TF2_LOG.dat"


RemoteService_log::RemoteService_log()
{
    for(int i = 0 ; i++ ; i < LOG_INDEX_MAX )
    {
        initLogData(i);
    }
    //memset(logData, 0, sizeof(tf2_tcu_logging) * LOG_INDEX_MAX);
    mLogIndex = 0;
    logFile = NULL;
}

void RemoteService_log::initLogData(uint8_t index)
{
    logData[index].routineResult            = 1; // invalid data
    logData[index].eventCreatTime           = 0;
    logData[index].serviceType              = LOG_NOT_AVAILABLE;
    logData[index].successStatus            = LOG_NOT_AVAILABLE;
    logData[index].simpleStatus             = LOG_NOT_AVAILABLE;
    logData[index].heaterStatus             = LOG_NOT_AVAILABLE;
    logData[index].windowStatus             = LOG_NOT_AVAILABLE;
    logData[index].lockStatus               = LOG_NOT_AVAILABLE;
    logData[index].alarmStatus              = LOG_NOT_AVAILABLE;
    logData[index].hazardStatus             = LOG_NOT_AVAILABLE;
    logData[index].brakePedalStatus         = LOG_NOT_AVAILABLE;
    logData[index].stationaryStatus         = LOG_NOT_AVAILABLE;
    logData[index].transmissionStatus       = LOG_NOT_AVAILABLE;
    logData[index].currCrashStatus          = LOG_NOT_AVAILABLE;
    logData[index].prevCrashStatus          = LOG_NOT_AVAILABLE;
    logData[index].fuelLevel                = LOG_NOT_AVAILABLE;
    logData[index].maxRunTime               = LOG_NOT_AVAILABLE;
    logData[index].svtStatus                = LOG_NOT_AVAILABLE;
    logData[index].transitionNormal         = LOG_NOT_AVAILABLE;
    logData[index].prerequisiteStatus       = LOG_NOT_AVAILABLE;
    logData[index].powerMode                = LOG_NOT_AVAILABLE;
    logData[index].currResSequence          = 0;
    logData[index].prevResSequence          = 0;
    logData[index].resRunTimer              = 0;
    logData[index].ffhRunTimer              = 0;
    logData[index].ffhStatusWhenFFHStopped  = LOG_NOT_AVAILABLE;
}

sp<Buffer> RemoteService_log::getLoggingData(uint8_t index)
{
    sp<Buffer> buf = new Buffer();
    uint8_t returnIndex = mLogIndex;

    if( index >= LOG_INDEX_MAX )
    {
        tf2_tcu_logging logDataInvalid = {1,};
        LOGE("getLoggingData ERROR index:%d should be 0 ~ %d", index, LOG_INDEX_MAX-1);
        logDataInvalid.routineResult = 1; // invalid data
        buf->setTo((uint8_t*)&logDataInvalid, DIAG_PARAM_SIZE);
        return buf;
    }

    if(index <= mLogIndex)
    {
        returnIndex = mLogIndex - index;
    }
    else
    {
        returnIndex = mLogIndex + LOG_INDEX_MAX - index;
    }
    LOGI("getLoggingData index:%d mLogIndex:%d returnIndex:%d ", index, mLogIndex, returnIndex);

    buf->setTo((uint8_t*)&logData[returnIndex], DIAG_PARAM_SIZE);

    return buf;
}

bool RemoteService_log::checkLogDataPath()
{
    if(mkdir(PATH_APPS, 0755) < 0)
    {
        if(errno != EEXIST)
        {
            LOGE("Directory %s can't create. errno [%s] !", PATH_APPS, strerror(errno));
            return false;
        }
        else
        {
            //LOGV("Directory %s already exist.", PATH_APPS);
        }
    }

    if(mkdir(PATH_APPS_RVC, 0755) < 0)
    {
        if(errno != EEXIST)
        {
            LOGE("Directory %s can't create. errno [%s] !", PATH_APPS_RVC, strerror(errno));
            return false;
        }
        else
        {
            //LOGV("Directory %s already exist.", PATH_APPS_RVC);
        }
    }
    //LOGV("Directory %s create success(or already exist).", PATH_APPS_RVC);

    return true;
}

bool RemoteService_log::readLogfromFile()
{
    LOGE("readLogfromFile \n");

    if(checkLogDataPath() == false)
    {
        LOGE("file path ERROR!!");
        return false;
    }

    logFile = fopen(PATH_LOG_FILE, "rb");

    if(!logFile)
    {
        LOGE("Unable to open file");
        return false;
    }
/*
    if(fread((char*)&logData,sizeof(logData), LogIndexMax, logFile ) == NULL )
    {
        LOGE("fread NULL \n");
        fclose(logFile);
        return false;
    }
*/
    int rtn = 0;
    for(int i = 0 ; i < LOG_INDEX_MAX ; i++ )
    {
        rtn = fread(&logData[i].eventCreatTime,           sizeof(uint32_t), 1, logFile);
        rtn = fread(&logData[i].serviceType,              sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].successStatus,            sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].simpleStatus,             sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].heaterStatus,             sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].windowStatus,             sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].lockStatus,               sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].alarmStatus,              sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].hazardStatus,             sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].brakePedalStatus,         sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].stationaryStatus,         sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].transmissionStatus,       sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].currCrashStatus,          sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].prevCrashStatus,          sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].fuelLevel,                sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].maxRunTime,               sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].svtStatus,                sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].transitionNormal,         sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].prerequisiteStatus,       sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].powerMode,                sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].currResSequence,          sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].prevResSequence,          sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].resRunTimer,              sizeof(uint16_t), 1, logFile);
        rtn = fread(&logData[i].ffhRunTimer,              sizeof(uint16_t), 1, logFile);
        rtn = fread(&logData[i].ffhStatusWhenFFHStopped,  sizeof(uint8_t ), 1, logFile);
        rtn = fread(&logData[i].routineResult,            sizeof(uint8_t ), 1, logFile);

        //LOGI("logData[%d].eventCreatTime : %ld", i, logData[i].eventCreatTime);
    }

    fclose(logFile);
    return true;
}

bool RemoteService_log::writeLogToFile()
{
//    LOGI("writeLogToFile start\n");

    if(checkLogDataPath() == false)
    {
        LOGE("file path ERROR!!");
        return false;
    }

    logFile = fopen(PATH_LOG_FILE,"wb");
    if(!logFile)
    {
        LOGE("Unable to open file");
        return false;
    }

    //fwrite((char*)&logData, sizeof(logData), LogIndexMax, logFile);
    for(int i = 0 ; i < LOG_INDEX_MAX ; i++ )
    {
//        LOGI("logData[%d].eventCreatTime : %ld", i, logData[i].eventCreatTime);
        fwrite((uint32_t*)  &logData[i].eventCreatTime,             sizeof(uint32_t), 1, logFile);
        fwrite((uint8_t*)   &logData[i].serviceType,                sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].successStatus,              sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].simpleStatus,               sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].heaterStatus,               sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].windowStatus,               sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].lockStatus,                 sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].alarmStatus,                sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].hazardStatus,               sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].brakePedalStatus,           sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].stationaryStatus,           sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].transmissionStatus,         sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].currCrashStatus,            sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].prevCrashStatus,            sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].fuelLevel,                  sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].maxRunTime,                 sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].svtStatus,                  sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].transitionNormal,           sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].prerequisiteStatus,         sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].powerMode,                  sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].currResSequence,            sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].prevResSequence,            sizeof(uint8_t ), 1, logFile);
        fwrite((uint16_t*)  &logData[i].resRunTimer,                sizeof(uint16_t), 1, logFile);
        fwrite((uint16_t*)  &logData[i].ffhRunTimer,                sizeof(uint16_t), 1, logFile);
        fwrite((uint8_t*)   &logData[i].ffhStatusWhenFFHStopped,    sizeof(uint8_t ), 1, logFile);
        fwrite((uint8_t*)   &logData[i].routineResult,              sizeof(uint8_t ), 1, logFile);
    }

    fclose(logFile);
    LOGI("writeLogToFile end\n");
    return true;
}


uint8_t RemoteService_log::findLogIndex()
{
    //LOGE("findLogIndex \n");

    if(!logFile)
    {
        LOGE("logfile NOT opened");
        setLogIndex(0);
        return 0;
    }

    int index = LOG_INDEX_MAX -1;
    int i = (LOG_INDEX_MAX - 2) ;
    //LOGI("logData[index].eventCreatTime :%ld \n", logData[index].eventCreatTime);
    for( ; i >= 0 ; i-- )
    {
        //LOGI("logData[%d].eventCreatTime :%ld \n", i, logData[i].eventCreatTime);
        if(logData[index].eventCreatTime < logData[i].eventCreatTime)
        {
            break;
        }
    }
    if( i < 0 )
    {
        index = LOG_INDEX_MAX -1;
    }
    else
    {
        index = i;
    }
    setLogIndex(index);
    LOGE("findLogIndex index: %d \n", index);
    return index;
}

uint8_t RemoteService_log::getCurrentLogIndex()
{
    return mLogIndex;
}
void RemoteService_log::setLogIndex(uint8_t index)
{
    if(index >= LOG_INDEX_MAX)
    {
        LOGE("index ERROR : %d -->> 0 \n", index);
        index = 0;
    }
    mLogIndex = index;
}
uint8_t RemoteService_log::increaseLogIndex()
{
    ++mLogIndex;
    mLogIndex = mLogIndex % LOG_INDEX_MAX ;
    return mLogIndex;
}


void RemoteService_log::set_eventCreateTime(uint32_t seconds, uint8_t index)
{
    // check byte order
    logData[index].eventCreatTime = seconds;
}

void RemoteService_log::set_serviceType(uint8_t type, uint8_t index)
{
    logData[index].serviceType = type;
}

void RemoteService_log::set_successStatus(uint8_t status, uint8_t index)
{
    logData[index].successStatus = status;
}

void RemoteService_log::set_simpleStatus(uint8_t status, uint8_t index)
{
    logData[index].simpleStatus = status;
}

void RemoteService_log::set_heaterStatus(uint8_t status, uint8_t index)
{
    logData[index].heaterStatus = status;
}

void RemoteService_log::set_windowStatus(uint8_t status, uint8_t index)
{
    logData[index].windowStatus = status;
}

void RemoteService_log::set_lockStatus(uint8_t status, uint8_t index)
{
    logData[index].lockStatus = status;
}

void RemoteService_log::set_alarmStatus(uint8_t status, uint8_t index)
{
    logData[index].alarmStatus = status;
}

void RemoteService_log::set_hazardStatus(uint8_t status, uint8_t index)
{
    logData[index].hazardStatus = status;
}

void RemoteService_log::set_brakePedalStatus(uint8_t status, uint8_t index)
{
    logData[index].brakePedalStatus = status;
}

void RemoteService_log::set_stationaryStatus(uint8_t status, uint8_t index)
{
    logData[index].stationaryStatus = status;
}

void RemoteService_log::set_transmissionStatus(uint8_t status, uint8_t index)
{
    logData[index].transmissionStatus = status;
}

void RemoteService_log::set_currCrashStatus(uint8_t status, uint8_t index)
{
    logData[index].currCrashStatus = status;
}

void RemoteService_log::set_prevCrashStatus(uint8_t status, uint8_t index)
{
    logData[index].prevCrashStatus = status;
}

void RemoteService_log::set_fuelLevel(uint8_t status, uint8_t index)
{
    logData[index].fuelLevel = status;
}

void RemoteService_log::set_maxRunTime(uint8_t status, uint8_t index)
{
    logData[index].maxRunTime = status;
}

void RemoteService_log::set_svtStatus(uint8_t status, uint8_t index)
{
    logData[index].svtStatus = status;
}

void RemoteService_log::set_transitionNormal(uint8_t status, uint8_t index)
{
    logData[index].transitionNormal = status;
}

void RemoteService_log::set_prerequisiteStatus(uint8_t status, uint8_t index)
{
    logData[index].prerequisiteStatus = status;
}

void RemoteService_log::set_powerMode(uint8_t status, uint8_t index)
{
    logData[index].powerMode = status;
}

void RemoteService_log::set_currResSequence(uint8_t status, uint8_t index)
{
    logData[index].currResSequence = status;
}

void RemoteService_log::set_prevResSequence(uint8_t status, uint8_t index)
{
    logData[index].prevResSequence = status;
}

void RemoteService_log::set_resRunTimer(uint16_t seconds, uint8_t index)
{
    logData[index].resRunTimer = seconds;
}

void RemoteService_log::set_ffhRunTimer(uint16_t seconds, uint8_t index)
{
    logData[index].ffhRunTimer = seconds;
}

void RemoteService_log::set_ffhStatusWhenFFHStopped(uint8_t status, uint8_t index)
{
    logData[index].ffhStatusWhenFFHStopped = status;
}

void RemoteService_log::set_routineResult(uint8_t status, uint8_t index)
{
    logData[index].routineResult = status;
}

