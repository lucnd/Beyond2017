
#define LOG_TAG "RVC"
#include "Log.h"

#include "RemoteServiceApp.h"

bool RemoteServiceApp::mcuSignal_Valiable(uint16_t sigid)
{
    if( (sigid == Signal_FrontSystemOnCmd_RX)
        || (sigid == Signal_RESTransmissionStatus_RX)
        || (sigid == Signal_RESVehStationaryStatus_RX)
        || (sigid == Signal_EngOBDMil_RX)
        || (sigid == Signal_DoorStatusHS_RX)
        || (sigid == Signal_CrashStatusRCM_RX)
        || (sigid == Signal_SunroofOpen_RX)
        || (sigid == Signal_RoofPositionStatus_RX)
        || (sigid == Signal_RESWindowStatus_RX)
        || (sigid == Signal_RESVehicleLockStatus_RX)
        || (sigid == Signal_RESAlarmStatus_RX)
        || (sigid == Signal_RESHazardSwitchStatus_RX)
        || (sigid == Signal_RESBrakePedalStatus_RX)
        )
    {
        return true;
    }
    return false;
}

bool RemoteServiceApp::mcuSignal_response(uint16_t sigid)
{
    if( (sigid == Signal_RESSequenceStatus_RX)
        || (sigid == Signal_AlarmModeHS_RX)
        || (sigid == Signal_HFSCommand_RX)
        || (sigid == Signal_HRWCommand_RX)
        || (sigid == Signal_HeatedSeatFLModeRequest_RX)
        || (sigid == Signal_HeatedSeatFLRequest_RX)
        || (sigid == Signal_HeatedSeatFRModeRequest_RX)
        || (sigid == Signal_HeatedSeatFRRequest_RX)
        || (sigid == Signal_HeatedSeatRLModeReq_RX)
        || (sigid == Signal_HeatedSeatRLReq_RX)
        || (sigid == Signal_HeatedSeatRRModeReq_RX)
        || (sigid == Signal_HeatedSeatRRReq_RX) )
    {
        return true;
    }
    return false;
}

bool RemoteServiceApp::mcuSignal_hvac(uint16_t sigid)
{
    if( (sigid == Signal_FFHOperatingStatus_RX)
        || (sigid == Signal_ParkClimateMode_RX)
        || (sigid == Signal_ParkHeatVentTime_RX)
        || (sigid == Signal_TimerActivationStatus_RX)
        || (sigid == Signal_Timer1Time_RX)
        || (sigid == Signal_Timer2Time_RX) )
    {
        return true;
    }
    return false;
}


void RemoteServiceApp::RemoteService_receiveMessageFromMCU(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    RS_ServiceType serviceType;
    uint8_t sigVal = 0;
    uint16_t sigVal16 = 0;
    uint32_t sigVal24 = 0;
    uint32_t sigVal32 = 0;

    if(size == SIZE_1BYTE) {
        sigVal = buf[0];
    } else if(size == SIZE_2BYTE) {
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
    } else if(size == SIZE_3BYTE) {
        sigVal24 |= ((uint32_t)buf[0] << 16);
        sigVal24 |= ((uint32_t)buf[1] << 8);
        sigVal24 |= buf[2];
    } else if(size == SIZE_4BYTE) {
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];
    } else {
        LOGE("receive abnormal CAN signal size sigid:%d size:%d", sigid, size);
    }

    LOGI("RemoteService_receiveMessageFromMCU() sigid:%d sigVal:0x%x", sigid, ((size == SIZE_4BYTE) ? sigVal32 : ((size == SIZE_2BYTE) ? sigVal16 : sigVal)));

    if(mcuSignal_Valiable(sigid))
    {
        rcvMCU_variale(sigid, buf, size);
        return;
    }
    if(mcuSignal_response(sigid))
    {
        rcvMCU_response(sigid, buf, size);
        return;
    }
    if(mcuSignal_hvac(sigid))
    {
        rcvMCU_hvac(sigid, buf, size);
        return;
    }

    if(sigid == Signal_DoorLatchStatus_RX)
    {
        //LOGD("RemoteService_receiveMessageFromMCU() DoorLatchStatusRX");
        rcvMCU_DoorLatchStatusRX(sigid, buf, size);
        return;
    }
    if( (sigid == Signal_DriverWindowPosition_RX)
        || (sigid == Signal_RearDriverWindowPos_RX)
        || (sigid == Signal_PassWindowPosition_RX)
        || (sigid == Signal_RearPassWindowPos_RX)
        || (sigid == Signal_DriverRearWindowPos_RX)
        || (sigid == Signal_PassRearWindowPos_RX) )
    {
        //LOGD("RemoteService_receiveMessageFromMCU() windowPosition");
        rcvMCU_windowPosition(sigid, buf, size);
        return;
    }
    /*if( (sigid == Signal_DrvSeatInhib2ndRow_RX)
        || (sigid == Signal_PasSeatInhib2ndRow_RX)
        || (sigid == Signal_DrvSeatInhib3rdRow_RX)
        || (sigid == Signal_PasSeatInhib3rdRow_RX) )
    {
        //LOGD("RemoteService_receiveMessageFromMCU() seatInhib");
        rcvMCU_seatInhib(sigid, buf, size);
        return;
    }
    if( (sigid == Signal_DrvSeatPosition2ndRow_RX)
        || (sigid == Signal_PasSeatPosition2ndRow_RX)
        || (sigid == Signal_DrvSeatPosition3rdRow_RX)
        || (sigid == Signal_PasSeatPosition3rdRow_RX) )
    {
        //LOGD("RemoteService_receiveMessageFromMCU() SignalSeatPosition");
        rcvMCU_seatPosition(sigid, buf, size);
        return;
    }*/
    if(sigid == Signal_RemoteStartStatusMS_RX)
    {
        //LOGD("RemoteService_receiveMessageFromMCU() SignalRemoteStartStatus");
        rcvMCU_remoteStartStatusMSRX(sigid, buf, size);
        return;
    }
    if((sigid == Signal_FuelLevelIndicatedHS_RX)
        || (sigid == Signal_FuelLevelIndicatedQFHS_RX)
        || (sigid == Signal_FuelGaugePosition_RX)
        || (sigid == Signal_DistanceToEmpty_RX)
        || (sigid == Signal_OdometerMasterValue_RX)
        || (sigid == Signal_ODODisplayedMiles_RX)
        )
    {
        //LOGD("RemoteService_receiveMessageFromMCU() warmup variable");
        rcvMCU_warmupVariable(sigid, buf, size);
        return;
    }
    if(sigid == Signal_Internal_Serv_BCM_AUTH_RESP)
    {
        //LOGV("recv [Signal_Internal_Serv_BCM_AUTH_RESP]");

        serviceType = getCurrBCMauthService();

        if(serviceType == RS_SERVICE_TYPE_MAX)
        {
            LOGI("not requested BCM authentication response.");
            return;
        }

        RemoteService_receiveBCMauthStatus(serviceType, buf, size);

        return;
    }
    if(sigid == Signal_Internal_Serv_CAN_VARIANT)
    {
        //LOGV("Signal_Internal_Serv_CAN_VARIANT size:%d sigid:%d sigVal:0x%x", size, sigid, sigVal);
        setCanVariant(sigVal);
        return;
    }
//    LOGE("RemoteService_receiveMessageFromMCU() END");
}

void RemoteServiceApp::rcvMCU_variale(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = 0;
    uint16_t sigVal16 = 0;
    uint32_t sigVal24 = 0;
    uint32_t sigVal32 = 0;

    if(size == SIZE_1BYTE) {
        sigVal = buf[0];
    } else if(size == SIZE_2BYTE) {
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
    } else if(size == SIZE_3BYTE) {
        sigVal24 |= ((uint32_t)buf[0] << 16);
        sigVal24 |= ((uint32_t)buf[1] << 8);
        sigVal24 |= buf[2];
    } else if(size == SIZE_4BYTE) {
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];
    } else {
        LOGE("receive abnormal CAN signal size sigid:%d size:%d", sigid, size);
    }

    if(sigid == Signal_FrontSystemOnCmd_RX)
    {
        setFrontSystemOnCmd(sigVal);
        return;
    }
    if(sigid == Signal_EngOBDMil_RX)
    {
        //0: MIL Off, 1: MIL On
        (sigVal & 1) ? setEngOBDMil(true) : setEngOBDMil(false);
        return;
    }
    if(sigid == Signal_DoorStatusHS_RX)
    {
        setDoorOpenStatus(sigVal);
        return;
    }
    if(sigid == Signal_CrashStatusRCM_RX)
    {
        //0: No Crash, 1: Crash
        if(sigVal & TRUE)
            setProperty(STR_CURR_CRASH, STR_TRUE);
        return;
    }
    if(sigid == Signal_SunroofOpen_RX)
    {
        setSunroofOpenStatus(sigVal);
        return;
    }
    if(sigid == Signal_RoofPositionStatus_RX)
    {
        setRoofPositionStatus(sigVal);
        return;
    }
    #if 0 //handle upon frame notification
    if(sigid == Signal_RESWindowStatus_RX)
    {
        setRESWindowStatus((bool)sigVal);
        return;
    }
    if(sigid == Signal_RESVehicleLockStatus_RX)
    {
        setRESVehicleLockStatus((bool)sigVal);
        return;
    }
    if(sigid == Signal_RESAlarmStatus_RX)
    {
        setRESAlarmStatus((bool)sigVal);
        return;
    }
    if(sigid == Signal_RESHazardSwitchStatus_RX)
    {
        setRESHazardSwitchStatus((bool)sigVal);
        return;
    }
    if(sigid == Signal_RESBrakePedalStatus_RX)
    {
        setRESBrakePedalStatus((bool)sigVal);
        return;
    }
    if(sigid == Signal_RESTransmissionStatus_RX)
    {
        //0: transmission not in park, 1: transmission in park
        (sigVal & 1) ? setRESTransmissionStatus(true) : setRESTransmissionStatus(false);
        return;
    }
    if(sigid == Signal_RESVehStationaryStatus_RX)
    {
        //0: vehicle not stationary, 1: vehicle stationary
        (sigVal & 1) ? setRESVehStationaryStatus(true) : setRESVehStationaryStatus(false);
        return;
    }
    #endif
}

void RemoteServiceApp::rcvMCU_response(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = 0;
    uint16_t sigVal16 = 0;
    uint32_t sigVal32 = 0;

    if(size == SIZE_1BYTE) {
        sigVal = buf[0];
    } else if(size == SIZE_2BYTE) {
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
    } else if(size == SIZE_4BYTE) {
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];
    } else {
        LOGE("receive abnormal CAN signal size sigid:%d size:%d", sigid, size);
    }

    #if 0 //handle upon frame notification
    if(sigid == Signal_RESSequenceStatus_RX)
    {
        LOGV("RESSequenceStatus valid?: %d %d %d %d",
            isTimerOn(MyTimer::TIMER_RES_START), isTimerOn(MyTimer::TIMER_RES_MAX_RUN),
            getCurrRemoteStartStatus(), getRESBrakePedalStatus());
        if( isTimerOn(MyTimer::TIMER_RES_START)
            || isTimerOn(MyTimer::TIMER_RES_MAX_RUN)
            || (getCurrRemoteStartStatus() == true)
            || ( (sigVal == 0x0E) && (getRESBrakePedalStatus() == 0) ) // to trace Key Not Found
            )
        {
            setLogRESSequenceStatusPrevious(getRESSequenceStatus());
            setRESSequenceStatus(sigVal);
            makeLog();

            RemoteService_responseHandler(RS_RES, RS_ENGINE_START, sigid, buf, size); //m_remoteService[RS_RES]->responseHandler(serviceType, sigid, buf, size);
        }
        return;
    }
    #endif
    if(sigid == Signal_AlarmModeHS_RX)
    {
        // if(getCurrState(RS_ALARM_RESET) == RS_WAIT_RESPONSE)
        //   saveAlarmStatus();
        // and responseHandler();
        // else
        // saveAlarmStatus();
        setCurrAlarmStatus((RS_AlarmMode) sigVal);
        RemoteService_responseHandler(RS_RVI, RS_ALARM_RESET, sigid, buf, size);//m_remoteService[RS_RVI]->responseHandler(serviceType, sigid, buf, size);
        return;
    }
    if( (sigid == Signal_HFSCommand_RX)
        || (sigid == Signal_HRWCommand_RX)
        || (sigid == Signal_HeatedSeatFLModeRequest_RX)
        || (sigid == Signal_HeatedSeatFLRequest_RX)
        || (sigid == Signal_HeatedSeatFRModeRequest_RX)
        || (sigid == Signal_HeatedSeatFRRequest_RX)
        || (sigid == Signal_HeatedSeatRLModeReq_RX)
        || (sigid == Signal_HeatedSeatRLReq_RX)
        || (sigid == Signal_HeatedSeatRRModeReq_RX)
        || (sigid == Signal_HeatedSeatRRReq_RX) )
    {
        RemoteService_responseHandler(RS_RCC, RS_TARGET_TEMP, sigid, buf, size); //m_remoteService[RS_RCC]->responseHandler(serviceType, sigid, buf, size);
        return;
    }
}

void RemoteServiceApp::rcvMCU_hvac(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = 0;
    uint16_t sigVal16 = 0;
    uint32_t sigVal32 = 0;

    if(size == SIZE_1BYTE) {
        sigVal = buf[0];
    } else if(size == SIZE_2BYTE) {
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
    } else if(size == SIZE_4BYTE) {
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];
    } else {
        LOGE("receive abnormal CAN signal size sigid:%d size:%d", sigid, size);
    }

    if(RemoteService_IsCanBusWaking() == false)
    {
        LOGI("RETURN.. can bus is not waking.. might be noise signal");
        return;
    }
    if(isTimerOn(MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT))
    {
        LOGI("RETURN.. TIMER_PARK_CLIMATE_MIN_STATUS_WAIT timer on");
        return;
    }

    if(sigid == Signal_FFHOperatingStatus_RX)
    {
        LOGD("RemoteService_receiveMessageFromMCU() FFHOperatingStatusRX %d", sigVal);
        rcvMCU_FFHOperatingStatusRX(sigid, buf, size);
        return;
    }
    if(sigid == Signal_ParkClimateMode_RX)
    {
        LOGD("RemoteService_receiveMessageFromMCU() Signal_ParkClimateMode_RX %d", sigVal);
        rcvMCU_ParkClimateModeRX(sigid, buf, size);
        return;
    }
    if(sigid == Signal_ParkHeatVentTime_RX)
    {
        LOGD("RemoteService_receiveMessageFromMCU() Signal_ParkHeatVentTime_RX %d", sigVal);
        if(getParkHeatVentTime() != sigVal)
        { // 실제 신호 값을 저장하고, NGTP 메시지를 만들 때 offset 추가..
            setParkHeatVentTime(sigVal);
            saveProperty_RPCvariables(Signal_ParkHeatVentTime_RX, sigVal);
        }
        return;
    }
    if(sigid == Signal_TimerActivationStatus_RX)
    {
        LOGD("RemoteService_receiveMessageFromMCU() Signal_TimerActivationStatus_RX %d", sigVal);
        bool val = (sigVal & 1);
        if(getRPCtimerActiveStatus() != val)
        {
            setRPCtimerActiveStatus(true);
            saveProperty_RPCvariables(Signal_TimerActivationStatus_RX, val);
        }
        return;
    }
    if(sigid == Signal_Timer1Time_RX)
    {
        if(getTimer1Time() != sigVal32)
        {
            uint32_t buffer[10];
            setTimer1Time(sigVal32);
            sprintf((char*) buffer, "%d", sigVal32);
            setProperty(PROP_TIMER_1_TIME, (const char *) buffer, true);
        }
        return;
    }
    if(sigid == Signal_Timer2Time_RX)
    {
        if(getTimer2Time() != sigVal32)
        {
            uint32_t buffer[10];
            setTimer2Time(sigVal32);
            sprintf((char*) buffer, "%d", sigVal32);
            setProperty(PROP_TIMER_2_TIME, (const char *) buffer, true);
        }
        return;
    }
}


void RemoteServiceApp::rcvMCU_ParkClimateModeRX(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    /*
    Physical Range 5 - 7 1 0 unused
    Logical Value 0 off
    Logical Value 1 startup
    Logical Value 2 engine heating
    Logical Value 3 cabin heating
    Logical Value 4 cabin venting
    */

    uint8_t sigVal = buf[0];
    int8_t prevParkClimateModeTCU = getCurrParkClimateModeTCU();

    if(prevParkClimateModeTCU == sigVal)
    {
        LOGI("RETURN.. ParkClimateModeTCU NOT CAHNGED.. do not take action");
        return;
    }

    // RPC or RES check by off-board message
    setCurrParkClimateModeTCU(sigVal);

    int8_t currParkClimateModeTCU = getCurrParkClimateModeTCU();

    LOGV("rcvMCU_ParkClimateModeRX ParkClimateModeTCU:%d -> %d, getCurrState(RS_FFH_PREREQ):%d getCurrState(RS_FFH_START):%d",
        prevParkClimateModeTCU, currParkClimateModeTCU, getCurrState(RS_FFH_PREREQ), getCurrState(RS_FFH_START));

    rcvMCU_action_ParkClimateModeRX(sigVal);
}
void RemoteServiceApp::rcvMCU_action_ParkClimateModeRX(uint8_t sigVal)
{
    int8_t prevParkClimateModeTCU = getPrevParkClimateModeTCU();
    int8_t currParkClimateModeTCU = getCurrParkClimateModeTCU();

    if( (prevParkClimateModeTCU == CLIMATE_MODE_OFF)
        && (currParkClimateModeTCU == CLIMATE_MODE_STARTUP) )
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Upon ParkClimateModeTCU changing from off=0 to startup=1, the TCU shall (re)initiate a
        timer to sample the latest HVAC status for PARK_CLIMATE_STARTUP_TIME sec.
        */
        startTimerId(MyTimer::TIMER_PARK_CLIMATE_START, true);
    }
    if(currParkClimateModeTCU == CLIMATE_MODE_OFF)
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Upon ParkClimateModeTCU changing from any state to off=0, TCU shall (re)initiate a
        timer to sample the latest HVAC status for PARK_CLIMATE_SHUTDOWN_TIME sec.
        */
        startTimerId(MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN, true);
    }

    /*
    5.9.5 Park Climate Status Notification
    SRD_ReqRPC2121_V2: The TCU shall notify the park climate status as follows
    Upon parkClimateModeTCU changing from any state to engine heating=2, cabin
    heating=3 or cabin venting=4, the TCU shall cancel the timer and proceed directly as in
    the case of timer expiry.
    */
    if( (currParkClimateModeTCU == CLIMATE_ENGINE_HEATING)
        || (currParkClimateModeTCU == CLIMATE_CABIN_HEATING)
        || (currParkClimateModeTCU == CLIMATE_CABIN_VENTING) )
    {
        stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
        RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, currParkClimateModeTCU);
    }
    /*
    HARMONY JLRTCU-1304
    RES requirement SRD_ReqRES0110_V2 DOES NOT ask for sending a separate status message to TSP. It only says to add some additional information in the message to TSP which goes as a part of RPC requirement SRD_ReqRPC2121_V2.
    SRD_ReqRES0110_V2 : : TCU shall send the FFH status information to TSP as mentioned in " ** Remote Park Climate Requirement ** ". In addition this section specifies the requirement to send the some ** additional ** information from RES perspective.
    ...............
    Note
    1. ? TCU sends RVSUpdateMessage with ClimateStatus to TSP”.
    2. FFH operation control is part of Remote Park Climate Requirement described in section 5 of the document. Please refer section 5.7 for exact details about FFH start operation
    --> So if the cause of the issue is sending a separate message for FFH status as a part of RES requirement then refer above clarification. Status should go as part of RPC requirement only with additional information for RES ( FFH Stopped & Value of FFHOperatingStatus)
    else if(currParkClimateModeTCU != CLIMATE_MODE_STARTUP)
    {
        RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, currParkClimateModeTCU);
    }
    else
    {
    }
    */
    if((prevParkClimateModeTCU == CLIMATE_MODE_STARTUP)
        && ( (currParkClimateModeTCU == CLIMATE_ENGINE_HEATING)
            || (currParkClimateModeTCU == CLIMATE_CABIN_HEATING)
            || (currParkClimateModeTCU == CLIMATE_CABIN_VENTING) ) )
    {
        /*
        5.9.12 ParkHeatVentTime sampling by TCU
        SRD_ReqRPC2149_V2: TCU shall sample ParkHeatVentTime signal after
        PARKHEATVENTTIME_SAMPLE_WAIT_TIME, the wait timer should start when
        ParkClimateMode changes from startup to engine heating, cabin heating or cabin venting.
        */
        startTimerId(MyTimer::TIMER_PARKHEATVENT_WAIT);
    }

    if((currParkClimateModeTCU == CLIMATE_MODE_OFF)&&isTimerOn(MyTimer::TIMER_FFH_MAX_RUN))
    {
        if(getCurrState(RS_FFH_STOP) == RS_ACTIVE)
        {
            setLogFfhRunTimer(false);
            stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN_FOR_FFH_STOP);
            setCurrState(RS_FFH_START, RS_READY);
            setCurrState(RS_FFH_STOP, RS_READY);
        }
        else
        {
            /*
            3.8.2.2 TCU Request for Engine Heat Prior to RES
            Case (c)
            If TCU receives ParkClimateMode=0 (off) within RES_FFH_RUN_TIME_MAX, then
            proceed according to requirement “TCU Check for User Interruption during Engine Heat”
            */
            setLogFfhRunTimer(false);
            stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
            #if 0
            setCurrState(RS_FFH_START, RS_READY);
            startTimerId(MyTimer::TIMER_FFH_USER_STOP);
            #else
            TcuCheckForUserInterruptionDuringEngineHeat();
            #endif
        }
    }

    #if 1
    if (getCurrState(RS_FFH_START) == RS_ACTIVE || getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE)
    { // 3.8.2.3 TCU Interpretation of HVAC Response to Engine Heat Request
        rcvMCU_action_ParkClimateModeRX_RES_whilstEngineHeating(sigVal);
    }
    //else if (getCurrState(RS_FFH_PREREQ) == RS_ACTIVE || getCurrState(RS_FFH_PREREQ) == RS_WAIT_RESPONSE)
    //{ // 3.8.2.2 TCU Request for Engine Heat Prior to RES
        //rcvMCU_action_ParkClimateModeRX_RES_priorEngineHeating(sigVal); will handle when RES_PREREQ_RESPONSE_TIME expires
    //}
    else // RPC
    { // 5.9.5 Park Climate Status Notification
        rcvMCU_action_ParkClimateModeRX_RPC(sigVal);
    }

    #else
    if (getCurrState(RS_FFH_START) == RS_ACTIVE || getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE) {
        if( sigVal == CLIMATE_CABIN_HEATING || sigVal == CLIMATE_CABIN_VENTING ) {
            stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
            setCurrState(RS_FFH_START, RS_READY);
            setCurrState(RS_ENGINE_START, RS_ACTIVE);
            RemoteService_requestHandler(RS_RES, RS_ENGINE_START); //m_remoteService[RS_RES]->requestHandler(RS_ENGINE_START);
        } else if( sigVal == CLIMATE_ENGINE_HEATING ) {
            startTimerId(MyTimer::TIMER_FFH_MAX_RUN, false);
        } else if( sigVal == CLIMATE_MODE_OFF || sigVal == CLIMATE_MODE_STARTUP ) {
            if( (sigVal == CLIMATE_MODE_OFF) && isTimerOn(MyTimer::TIMER_FFH_MAX_RUN) ) {
                stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                startTimerId(MyTimer::TIMER_FFH_USER_STOP);
            } else if(!isTimerOn(MyTimer::TIMER_FFH_MAX_RUN)) {
                setParkClimateModeWhenReqEngineHeating(currParkClimateModeTCU);
                RemoteService_requestHandler(RS_RES, RS_FFH_START); //m_remoteService[RS_RES]->requestHandler(RS_FFH_START);
                startTimerId(MyTimer::TIMER_CLIMATE_REQ);
            } else {
                // do nothing. wait during RES_FFH_RUN_TIME_MAX.
            }
        } else {
        // do nothing. ignore. unknown park climate mode
        }
    }
    #endif
}

void RemoteServiceApp::rcvMCU_action_ParkClimateModeRX_RES_priorEngineHeating(uint8_t sigVal)
{ // SRD_ReqRES0105_V2
#if 0
    uint8_t sigVal = buf[0];

    LOGV("rcvMCU_ParkClimateModeRX_priorEngineHeating sigVal:%d", sigVal);
    switch(sigVal)
    {
        case CLIMATE_CABIN_HEATING: //FALL-THROUGH
        case CLIMATE_CABIN_VENTING:
            /*
            Case (a) If ParkClimateMode=3 (cabin heating), the TCU shall proceed with RES,
            according to the requirement "RES Request to BCM" .
            Case (b) If ParkClimateMode=4 (cabin venting), the TCU shall proceed with RES,
            according to the requirement "RES Request to BCM" .
            */
            stopTimerId(MyTimer::TIMER_RES_PREREQ);
            setCurrState(RS_FFH_PREREQ, RS_READY);

            stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
            setCurrState(RS_FFH_START, RS_READY);
            setCurrState(RS_ENGINE_START, RS_ACTIVE);
            RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
            break;
        case CLIMATE_ENGINE_HEATING:
            /*
            Case (c) If ParkClimateMode=2 (engine heating), the TCU shall wait for a maximum of
            RES_FFH_RUN_TIME_MAX for ParkClimateMode=0 (off)
            */
            stopTimerId(MyTimer::TIMER_CLIMATE_REQ);
            setCurrState(RS_FFH_PREREQ, RS_READY);

            setCurrState(RS_FFH_START, RS_ACTIVE);
            startTimerId(MyTimer::TIMER_FFH_MAX_RUN, false);
            break;
        case CLIMATE_MODE_OFF:
            if(isTimerOn(MyTimer::TIMER_FFH_MAX_RUN))
            {
                /*
                Case (c)
                If TCU receives ParkClimateMode=0 (off) within RES_FFH_RUN_TIME_MAX, then
                proceed according to requirement “TCU Check for User Interruption during Engine Heat”
                */
                stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                setCurrState(RS_FFH_PREREQ, RS_READY);
                startTimerId(MyTimer::TIMER_FFH_USER_STOP);
                break;
            }
            else
            {
                //FALL-THROUGH
            }
        case CLIMATE_MODE_STARTUP:
            /*
            Case (d) If ParkClimateMode=1 (startup) or 0 (Off) the TCU shall send
            ParkClimateReqTCU=5 (engine heat requested (RES))
            */
            setCurrState(RS_FFH_PREREQ, RS_READY);
            setCurrState(RS_FFH_START, RS_ACTIVE);
            setParkClimateModeWhenReqEngineHeating(currParkClimateModeTCU);
            RemoteService_requestHandler(RS_RES, RS_FFH_START);
            startTimerId(MyTimer::TIMER_CLIMATE_REQ);
            break;
        default:
            break;
    }
#endif
}

void RemoteServiceApp::rcvMCU_action_ParkClimateModeRX_RES_whilstEngineHeating(uint8_t sigVal)
{ // SRD_ReqRES0106_V2
    int8_t parkClimateModeWhenReqEngineHeating = getParkClimateModeWhenReqEngineHeating();

    LOGV("rcvMCU_ParkClimateModeRX_RES_whistEngineHeating sigVal:%d parkClimateModeWhenReqEngineHeating:%d, getPrevParkClimateModeTCU():%d",
        sigVal, parkClimateModeWhenReqEngineHeating, getPrevParkClimateModeTCU());

    stopTimerId(MyTimer::TIMER_CLIMATE_REQ);

    if( (sigVal == CLIMATE_CABIN_HEATING)
        || (sigVal == CLIMATE_CABIN_VENTING) )
    {
        setCurrState(RS_FFH_PREREQ, RS_READY);
        setCurrState(RS_FFH_START, RS_READY);
        setCurrState(RS_ENGINE_START, RS_ACTIVE);
        RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
    }
    else if(parkClimateModeWhenReqEngineHeating == CLIMATE_MODE_STARTUP)
    {
        switch(sigVal)
        {
            case CLIMATE_ENGINE_HEATING:
                /*
                If ParkClimateMode becomes 2 (engine heating), the TCU shall proceed as in Case (c) of
                requirement "TCU Request for Engine Heat Prior to RES"
                */
                /*
                3.8.2.2 TCU Request for Engine Heat Prior to RES
                Case (c) If ParkClimateMode=2 (engine heating), the TCU shall wait for a maximum of
                RES_FFH_RUN_TIME_MAX for ParkClimateMode=0 (off)
                */
                startTimerId(MyTimer::TIMER_FFH_MAX_RUN, false);
                setLogFfhRunTimer(true);
                break;
            case CLIMATE_MODE_OFF:
                /*
                if ParkClimateMode becomes 0 (off), the TCU shall proceed according to
                requirement "TCU Check for User Interruption During Engine Heat"
                */
                setLogFfhRunTimer(false);
                stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                #if 0
                setCurrState(RS_FFH_START, RS_READY);
                startTimerId(MyTimer::TIMER_FFH_USER_STOP);
                #else
                TcuCheckForUserInterruptionDuringEngineHeat();
                #endif
                break;
            default:
                break;
        }
    }
    else if(parkClimateModeWhenReqEngineHeating == CLIMATE_MODE_OFF)
    {
        switch(sigVal)
        {
            case CLIMATE_MODE_STARTUP:
                break;
            case CLIMATE_MODE_OFF:
                /*
                If ParkClimateMode becomes 1 (startup), then becomes 0 (off), the TCU shall proceed
                according to requirement "TCU Check for User Interruption during Engine Heat".
                */
                if(getPrevParkClimateModeTCU() == CLIMATE_MODE_STARTUP)
                {
                    setLogFfhRunTimer(false);
                    stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                    #if 0
                    setCurrState(RS_FFH_START, RS_READY);
                    startTimerId(MyTimer::TIMER_FFH_USER_STOP);
                    #else
                    TcuCheckForUserInterruptionDuringEngineHeat();
                    #endif
                }
                break;
            case CLIMATE_ENGINE_HEATING:
                /*
                if ParkClimateMode becomes 2 (engine heating), the TCU shall proceed as in
                Case (c) of requirement "TCU Request for Engine Heat Prior to RES"
                */
                startTimerId(MyTimer::TIMER_FFH_MAX_RUN, false);
                setLogFfhRunTimer(true);
                break;
            default:
                /*
                Otherwise, if ParkClimateMode takes any other transition, the TCU shall proceed with
                RES.
                */
                stopTimerId(MyTimer::TIMER_CLIMATE_REQ);
                setCurrState(RS_FFH_PREREQ, RS_READY);
                setLogFfhRunTimer(false);
                stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                setCurrState(RS_FFH_START, RS_READY);

                setCurrState(RS_ENGINE_START, RS_ACTIVE);
                RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
                break;
        }
    }
    else
    {
    }
}

void RemoteServiceApp::rcvMCU_action_ParkClimateModeRX_RPC(uint8_t sigVal)
{ //5.9.5 Park Climate Status Notification
//    int8_t prevParkClimateModeTCU = getPrevParkClimateModeTCU();
    int8_t currParkClimateModeTCU = getCurrParkClimateModeTCU();

    switch(currParkClimateModeTCU)
    {
        case CLIMATE_MODE_OFF:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode off");
            break;
        case CLIMATE_MODE_STARTUP:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode startup");
            break;
        case CLIMATE_ENGINE_HEATING:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode engine heating");
            break;
        case CLIMATE_CABIN_HEATING:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode cabin heating");
            break;
        case CLIMATE_CABIN_VENTING:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode cabin venting");
            break;
        default:
            setRVCproperty(RS_SERVICE_TYPE_MAX, PROP_RH_PARKCLIMATEMODE, (RVCproperty)PROP_INVALID, "parkClimateMode invalid");
            break;
    }
#if 0 // move to rcvMCU_action_ParkClimateModeRX
    if( (prevParkClimateModeTCU == CLIMATE_MODE_OFF)
        && (currParkClimateModeTCU == CLIMATE_MODE_STARTUP) )
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Upon ParkClimateModeTCU changing from off=0 to startup=1, the TCU shall (re)initiate a
        timer to sample the latest HVAC status for PARK_CLIMATE_STARTUP_TIME sec.
        */
        startTimerId(MyTimer::TIMER_PARK_CLIMATE_START, true);
    }
    if(currParkClimateModeTCU == CLIMATE_MODE_OFF)
    {
        /*
        5.9.5 Park Climate Status Notification
        SRD_ReqRPC2121_V2
        Upon ParkClimateModeTCU changing from any state to off=0, TCU shall (re)initiate a
        timer to sample the latest HVAC status for PARK_CLIMATE_SHUTDOWN_TIME sec.
        */
        startTimerId(MyTimer::TIMER_PARK_CLIMATE_SHUTDOWN, true);
    }
#endif
    /*
    If status is changed for start operation no need for waiting 35 sec, TCU can send the
    status early for the faster response to the user.
    */
    if(getCurrState(RS_PARK_CLIMATE_AUTO) == RS_WAIT_RESPONSE) {
        if(currParkClimateModeTCU == CLIMATE_MODE_STARTUP)
        {
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
            sendAckMsgToTSP(RS_PARK_CLIMATE_AUTO, svc::actionResponse);
            setCurrState(RS_PARK_CLIMATE_AUTO, RS_READY);
        }
    } else if(getCurrState(RS_PARK_CLIMATE_STOP) == RS_WAIT_RESPONSE) {
        if(currParkClimateModeTCU == CLIMATE_MODE_OFF)
        {
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_STOP);
            sendAckMsgToTSP(RS_PARK_CLIMATE_STOP, svc::actionResponse);
            setCurrState(RS_PARK_CLIMATE_STOP, RS_READY);
            reqDefaultParkClimate();
        }
    } else if(getCurrState(RS_PARK_CLIMATE_VENT) == RS_WAIT_RESPONSE) {
        if(currParkClimateModeTCU == CLIMATE_MODE_STARTUP)
        {
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
            setCurrState(RS_PARK_CLIMATE_VENT, RS_READY);
        }
    } else if(getCurrState(RS_PARK_CLIMATE_HEAT) == RS_WAIT_RESPONSE) {
        if(currParkClimateModeTCU == CLIMATE_MODE_STARTUP)
        {
            stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
            setCurrState(RS_PARK_CLIMATE_HEAT, RS_READY);
        }
    }
    else
    {
    }

#if 0 // move to rcvMCU_action_ParkClimateModeRX
    /*
    5.9.5 Park Climate Status Notification
    SRD_ReqRPC2121_V2: The TCU shall notify the park climate status as follows
    Upon parkClimateModeTCU changing from any state to engine heating=2, cabin
    heating=3 or cabin venting=4, the TCU shall cancel the timer and proceed directly as in
    the case of timer expiry.
    */
    if( (currParkClimateModeTCU == CLIMATE_ENGINE_HEATING)
        || (currParkClimateModeTCU == CLIMATE_CABIN_HEATING)
        || (currParkClimateModeTCU == CLIMATE_CABIN_VENTING) )
    {
        stopTimerId(MyTimer::TIMER_PARK_CLIMATE_START);
        RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, getClimateOperatingStatus());
    }
#endif
}

void RemoteServiceApp::rcvMCU_FFHOperatingStatusRX(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    /*
    LogicalValue 0 Off
    LogicalValue 1 StartingUp
    LogicalValue 2 Running
    LogicalValue 3 Shutting Down
    LogicalValue 4 Low Voltage
    LogicalValue 5 Low Fuel
    LogicalValue 6 Service Required
    LogicalValue 7 System Failure
    LogicalValue 8 Max Operational Time Reached
    LogicalValue 9 Stopped by User
    */

     uint8_t sigVal = buf[0];

     if(getFFHOperatingStatus() == sigVal)
     {
         LOGI("RETURN.. getFFHOperatingStatus NOT CAHNGED.. do not take action");
         return;
     }

     // TODO: RES or RPC mode check
    setLogffhStatusWhenFFHStopped(sigVal);
    setFFHOperatingStatus(sigVal);
    saveProperty_RPCvariables(Signal_FFHOperatingStatus_RX, sigVal);

    rcvMCU_action_FFHOperatingStatusRX(sigVal);
}

void RemoteServiceApp::rcvMCU_action_FFHOperatingStatusRX(uint8_t sigVal)
{
    if( (sigVal == FFH_STOP_BY_USER) && isTimerOn(MyTimer::TIMER_FFH_USER_STOP))
    {
        stopTimerId(MyTimer::TIMER_FFH_USER_STOP);
        #if 0
        // abort RES.  report to TSP.
        // TODO: nedd check RES or RPC
        if(getCurrState(RS_FFH_START) == RS_ACTIVE || getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE) {
            setCurrState(RS_FFH_START, RS_READY);
            setNGTPerrorCode(executionFailure_chosen);
            sendNackMsgToTSP(RS_ENGINE_START, 11/*executionFailure_chosen*/);  //specific reason in climateOperatingStatus field.
            RemoteService_askUpdateMessageToRVM(CLIMATE_USER_INTERRUPTION);
        }
        #else
        TcuCheckForUserInterruptionDuringEngineHeat_FFHstopByUser();
        #endif
    }
}


void RemoteServiceApp::rcvMCU_DoorLatchStatusRX(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = buf[0];

    setDoorLatchStatus(sigVal);
    if(getCurrState(RS_SECURE_VEHICLE) == RS_WAIT_RESPONSE)
    {
        RemoteService_responseHandler(RS_RVI, RS_SECURE_VEHICLE, sigid, buf, size);//m_remoteService[RS_RVI]->responseHandler(serviceType, sigid, buf, size);
    }
    else if(getCurrState(RS_UNLOCK_VEHICLE) == RS_WAIT_RESPONSE)
    {
        RemoteService_responseHandler(RS_RVI, RS_UNLOCK_VEHICLE, sigid, buf, size);//m_remoteService[RS_RVI]->responseHandler(serviceType, sigid, buf, size);
    }
    else
    {
        // ignore signal
    }
}

void RemoteServiceApp::rcvMCU_windowPosition(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = buf[0];

    switch(sigid)
    {
        case Signal_DriverWindowPosition_RX:    //FALL-THROUGH
        case Signal_RearDriverWindowPos_RX:     //FALL-THROUGH
        case Signal_PassWindowPosition_RX:      //FALL-THROUGH
        case Signal_RearPassWindowPos_RX:       //FALL-THROUGH
        case Signal_DriverRearWindowPos_RX:     //FALL-THROUGH
        case Signal_PassRearWindowPos_RX:
            setWindowStatus(sigid, sigVal);
            break;
        default:
            break;
    }
}
/*
void RemoteServiceApp::rcvMCU_seatInhib(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = buf[0];
    seatMovementInhibition rcvVal = svc::SeatMovementInhibition_unknown;

    // CAN signal:   0(notInhibited), 1(inhibited)
    // NGTP message: 0(unknown), 1(inhibited), 2(notInhibited)

    if(sigVal == SEAT_NOT_INHIBITED)
        rcvVal = svc::notInhibited;
    else
        rcvVal = svc::inhibited;

    switch(sigid)
    {
        case Signal_DrvSeatInhib2ndRow_RX:
            setProperty(RISM_PROP_DRV2ND_IHB, (int32_t)rcvVal);
            if(statusRISM.drvSeat2ndRowInhib != rcvVal)
            {
                seatStatusChanged();
                statusRISM.drvSeat2ndRowInhib = rcvVal;
            }
            break;

        case Signal_PasSeatInhib2ndRow_RX:
            setProperty(RISM_PROP_PAS2ND_IHB, (int32_t)rcvVal);
            if(statusRISM.pasSeat2ndRowInhib != rcvVal)
            {
                seatStatusChanged();
                statusRISM.pasSeat2ndRowInhib = rcvVal;
            }
            break;

        case Signal_DrvSeatInhib3rdRow_RX:
            setProperty(RISM_PROP_DRV3RD_IHB, (int32_t)rcvVal);
            if(statusRISM.drvSeat3rdRowInhib != rcvVal)
            {
                seatStatusChanged();
                statusRISM.drvSeat3rdRowInhib = rcvVal;
            }
            break;

        case Signal_PasSeatInhib3rdRow_RX:
            setProperty(RISM_PROP_PAS3RD_IHB, (int32_t)rcvVal);
            if(statusRISM.pasSeat3rdRowInhib != rcvVal)
            {
                seatStatusChanged();
                statusRISM.pasSeat3rdRowInhib = rcvVal;
            }
            break;

        default:
            break;
    }

    if(getCurrState(RS_RISM_MOVE) == RS_WAIT_RESPONSE)
        checkSeatInhibitStatus(sigid, sigVal); //mRismInhibitStatus
    // TODO: IF requested seat is inhibited status THEN send Nack and End sequence. But actionResponse has no Nack message.
}

void RemoteServiceApp::rcvMCU_seatPosition(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = buf[0];

    if(sigVal > svc::moving)
    {
        LOGV("Invalid CAN signal value sigid:%d value:%d", sigid, sigVal);
        return;
    }

    switch(sigid)
    {
        case Signal_DrvSeatPosition2ndRow_RX:
            setProperty(RISM_PROP_DRV2ND, (int32_t)sigVal);
            if(statusRISM.drvSeat2ndRowState != (svc::SeatMovementStatus)sigVal)
            {
                if(sigVal != svc::moving)
                    seatStatusChanged();
                statusRISM.drvSeat2ndRowState = (svc::SeatMovementStatus)sigVal;
            }
            break;

        case Signal_PasSeatPosition2ndRow_RX:
            setProperty(RISM_PROP_PAS2ND, (int32_t)sigVal);
            if(statusRISM.pasSeat2ndRowState != (svc::SeatMovementStatus)sigVal)
            {
                if(sigVal != svc::moving)
                    seatStatusChanged();
                statusRISM.pasSeat2ndRowState = (svc::SeatMovementStatus)sigVal;
            }
            break;

        case Signal_DrvSeatPosition3rdRow_RX:
            setProperty(RISM_PROP_DRV3RD, (int32_t)sigVal);
            if(statusRISM.drvSeat3rdRowState != (svc::SeatMovementStatus)sigVal)
            {
                if(sigVal != svc::moving)
                    seatStatusChanged();
                statusRISM.drvSeat3rdRowState = (svc::SeatMovementStatus)sigVal;
            }
            break;

        case Signal_PasSeatPosition3rdRow_RX:
            setProperty(RISM_PROP_PAS3RD, (int32_t)sigVal);
            if(statusRISM.pasSeat3rdRowState != (svc::SeatMovementStatus)sigVal)
            {
                if(sigVal != svc::moving)
                    seatStatusChanged();
                statusRISM.pasSeat3rdRowState = (svc::SeatMovementStatus)sigVal;
            }
            break;

        default:
            break;
    }

    if((getCurrState(RS_RISM_MOVE) == RS_WAIT_RESPONSE) && (sigVal != svc::moving)
        && (sigVal != svc::SeatMovementStatus_unknown) )
    {
        checkSeatMoveStatus(sigid, sigVal);
    }
}
*/
void RemoteServiceApp::rcvMCU_remoteStartStatusMSRX(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = buf[0];
    LOGV("rcvMCU_remoteStartStatusMSRX sigVal:%d PM:%d RemoteStartStatus:%d ",
        sigVal, RemoteService_getPowerMode(), getCurrRemoteStartStatus());
    if(sigVal == true)
    {
        setCurrRemoteStartStatus(true);
        setLogTransistionFromREStoNormal(0x00);
        requestSimLock(RS_ENGINE_START);
    }
    else
    {
        if(RemoteService_getPowerMode() == PM_RUNNING_2)
        { // transite to normal ignition
            stopRESrunTimer();

            if((getCurrRemoteStartStatus() == true) && !isTimerOn(MyTimer::TIMER_RES_STOP) && !getRESBrakePedalStatus())
            {
                initRESremainingTime();
                RemoteService_askUpdateMessageToRVM(REMOTE_ENGINE_START_TONORMAL);
            }

            //start SRD_ReqRES0113_V2 Case E
            setLogTransistionFromREStoNormal(0x01);
            makeLog();
            //end SRD_ReqRES0113_V2 Case E
        }
        if(isTimerOn(MyTimer::TIMER_RES_STOP) && (RemoteService_getPowerMode() == PM_KEY_OUT))
        { // RE off success case
            stopRESrunTimer();

            stopTimerId(MyTimer::TIMER_RES_STOP);
            RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, getCurrParkClimateModeTCU());
            setCurrState(RS_ENGINE_STOP, RS_READY);

            setLogTransistionFromREStoNormal(LOG_NOT_AVAILABLE);
        }

        setCurrRemoteStartStatus(false);
        stopTimerId(MyTimer::TIMER_MIL_FAULT_DETECT);

        if(getRESTsimLockState())
        {
            if(RvcTsimLockRelease())
                setRESTsimLockState(false);
            else
                LOGE("Remote Engine Start: TSIM Lock release fail !");
        }
    }
}

void RemoteServiceApp::rcvMCU_warmupVariable(uint16_t sigid, uint8_t *buf, uint32_t size)
{
    uint8_t sigVal = 0;
    uint16_t sigVal16 = 0;
    uint32_t sigVal24 = 0;

    if( RemoteService_getPowerMode() <= PM_IGNITION_ON_2 )
    { //fuel signals give invalid value when pm <= 6
        LOGD("rcvMCU_warmupVariable RETURN : PM <= 6");
        return;
    }
    else if(isTimerOn(MyTimer::TIMER_SKIP_WARMUP_VALIABLE))
    { // fuel signals might take about 3.5 second to give valid value when pm become > 6
        LOGD("rcvMCU_warmupVariable : TimerOn");
        return;
    }
    else
    {
    }

    if(size == SIZE_1BYTE) {
        sigVal = buf[0];
    } else if(size == SIZE_2BYTE) {
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
    } else if(size == SIZE_3BYTE) {
        sigVal24 |= ((uint32_t)buf[0] << 16);
        sigVal24 |= ((uint32_t)buf[1] << 8);
        sigVal24 |= buf[2];
    }
    else
    {
        LOGE("rcvMCU_warmupVariable ERROR : size:%d ", size);
    }
#if 0 // handle upon frame notice
         these two signals bounded on same frame
         It could cause mismatch when getting can signal or can frame if read can upon signal change
    if(sigid == Signal_FuelLevelIndicatedHS_RX)
    {
        uint16_t resFuelLevelMin = getUint16fromConfig(RES_FUEL_LEVEL_MIN);
        LOGE("Signal_FuelLevelIndicatedHS_RX : %d ", sigVal16);
        if(updateFromCanFrame_fuelRelevantValiables() == true)
        {
            if((mFuelLevelIndicated < resFuelLevelMin) && (getCurrRemoteStartStatus() == true))
            {
                setLogfuelLevel(0x01); //low
                setRESstopReason(RES_STOP_FUELLOW);
                RemoteService_receiveMessageFromTSP(RS_ENGINE_STOP);
            }
        }
        else
        {
            //fuel level: 0-1023 * 0.2(scale)  litres
            uint16_t fuelDeciLevel = (sigVal16 & RES_FUEL_MASK) * 2; //save deci litres;
            if(getFuelLevelIndicatedQF() == RES_FUEL_QF)
            {
                setFuelLevelIndicated(fuelDeciLevel);
                if((fuelDeciLevel < resFuelLevelMin) && (getCurrRemoteStartStatus() == true))
                {
                    setLogfuelLevel(0x01); //low
                    setRESstopReason(RES_STOP_FUELLOW);
                    RemoteService_receiveMessageFromTSP(RS_ENGINE_STOP);
                }
            }
        }
        return;
    }
    if(sigid == Signal_FuelLevelIndicatedQFHS_RX)
    {
        //0: undefine, 1: temporary undefinable, 2:not specification, 3: specified accuracy
        //setFuelLevelIndicatedQF(sigVal);
        LOGE("DO nothing.. Signal_FuelLevelIndicatedQFHS_RX : %d ", sigVal);
        return;
    }
#endif
    if(sigid == Signal_FuelGaugePosition_RX)
    {
        setFuelGaugePosition(sigVal);
        return;
    }
    if(sigid == Signal_OdometerMasterValue_RX)
    {
        sigVal24 &= ODOMETER_MASK;
        setOdometerMasterValue(sigVal24);
        return;
    }
    if(sigid == Signal_ODODisplayedMiles_RX)
    {
        setODODisplayedMiles(sigVal24);
        return;
    }
    if(sigid == Signal_DistanceToEmpty_RX)
    {
        setDistanceToEmpty(sigVal16);
        return;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromTSP(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;

    LOGE("RemoteService_receiveMessageFromTSP(). serviceType: %d", serviceType);

//    m_remoteServiceLog->initLogData(0);
//    m_remoteServiceLog->set_eventCreateTime(0, get_NGTP_time());
//    m_remoteServiceLog->set_serviceType(0, (uint8_t)serviceType);

    // check service Type and service state
    if(getCurrState(serviceType) == RS_NOT_READY) {
        LOGE("Service:%d status Not ready.", serviceType);
        return;
    }

    if(serviceType != RS_TARGET_TEMP)
    {
        RvcKeepPowerAcquire();
        setLogserviceType((uint8_t)serviceType);
        setLogPowerModeWhenRequestReceived((uint8_t)RemoteService_getPowerMode());
        setLogPrerequisiteCheckRequestStatus(0x00);
    }

    if( (serviceType == RS_UNLOCK_VEHICLE)
        || (serviceType == RS_SECURE_VEHICLE)
        || (serviceType == RS_ALARM_RESET)
        || (serviceType == RS_HONK_FLASH) )
    {
        if((serviceType == RS_UNLOCK_VEHICLE) || (serviceType == RS_SECURE_VEHICLE))
            requestSimLock(remoteServiceType);

        rcvTSP_RVI(serviceType);
    }
    else if( (serviceType == RS_ENGINE_START)
            || (serviceType == RS_ENGINE_STOP) )
    {
        rcvTSP_RES(serviceType);
    }
    else if(serviceType == RS_TARGET_TEMP)
    {
        rcvTSP_RCC(serviceType);
    }
    else if( (serviceType == RS_PARK_CLIMATE_VENT)
            || (serviceType == RS_PARK_CLIMATE_HEAT)
            || (serviceType == RS_PARK_CLIMATE_AUTO)
            || (serviceType == RS_PARK_CLIMATE_STOP) )
    {
        rcvTSP_RPC(serviceType);
    }
    else if( (serviceType == RS_RISM_MOVE)
            || (serviceType == RS_RISM_CANCEL) )
    {
        rcvTSP_RISM(serviceType);
    }
    else
    {   // nothing
    }
}

void RemoteServiceApp::rcvTSP_RVI(RS_ServiceType serviceType)
{
    //if(getCurrState(serviceType) == RS_READY)
    //{
#if 0
        if(!getDBGcmdActiveStatus())
        {
            // send TUdispatcherAck msg
            sendDSPTackMsgToTSP(serviceType);
        }
#endif
        setCurrState(serviceType, RS_ACTIVE);
        RemoteService_requestHandler(RS_RVI, serviceType); //m_remoteService[RS_RVI]->requestHandler(serviceType);
    //}
    //else
    //{
        //LOGE("service Type %d duplicate request msg. current state is %d", serviceType, getCurrState(serviceType));
    //}
}

void RemoteServiceApp::rcvTSP_RES(RS_ServiceType serviceType)
{
    switch(serviceType)
    {
        case RS_ENGINE_START:
            //sendDSPTackMsgToTSP(RS_ENGINE_START);
            if(IsVehicleWithFFH()) {
                setCurrState(RS_FFH_PREREQ, RS_ACTIVE);
                RemoteService_requestHandler(RS_RES, RS_FFH_PREREQ); //m_remoteService[RS_RES]->requestHandler(RS_FFH_PREREQ);
            } else {
                setCurrState(RS_ENGINE_START, RS_ACTIVE);
                RemoteService_requestHandler(RS_RES, RS_ENGINE_START); //m_remoteService[RS_RES]->requestHandler(RS_ENGINE_START);
            }
            break;

        case RS_ENGINE_STOP:
#if 0
            if(!getDBGcmdActiveStatus() && (getRESstopReason() == RES_STOP_TSP))
                sendDSPTackMsgToTSP(RS_ENGINE_STOP);
#endif

#if 0
            // TODO: FFH is running. stop request
            if(mCurrParkClimateModeTCU >= CLIMATE_MODE_STARTUP && mCurrParkClimateModeTCU <= CLIMATE_CABIN_VENTING)   //FFH running  <=== default false
            {
                setAlreadyCheckPrereq(true);  // should not prerequisite check
                RemoteService_requestHandler(RS_RES, RS_FFH_STOP); //m_remoteService[RS_RES]->requestHandler(RS_FFH_STOP);
            }

            //setCurrState(RS_ENGINE_START, RS_READY);  //need check correct.
            setCurrState(RS_ENGINE_STOP, RS_ACTIVE);
            setAlreadyCheckPrereq(false);
            RemoteService_requestHandler(RS_RES, RS_ENGINE_STOP); //m_remoteService[RS_RES]->requestHandler(RS_ENGINE_STOP);
#else
            if (getCurrState(RS_FFH_START) == RS_ACTIVE
                || getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE)
            {
                setCurrState(RS_FFH_START, RS_READY);
                setCurrState(RS_FFH_STOP, RS_ACTIVE);
                RemoteService_requestHandler(RS_RES, RS_FFH_STOP); //m_remoteService[RS_RES]->requestHandler(RS_FFH_STOP);
            }
            else
            {
                setCurrState(RS_ENGINE_STOP, RS_ACTIVE);
                RemoteService_requestHandler(RS_RES, RS_ENGINE_STOP); //m_remoteService[RS_RES]->requestHandler(RS_ENGINE_STOP);
            }
#endif
            break;
        default:
            break;
    }
}

void RemoteServiceApp::rcvTSP_RCC(RS_ServiceType serviceType)
{
    RemoteService_requestHandler(RS_RCC, serviceType);
}

void RemoteServiceApp::rcvTSP_RPC(RS_ServiceType serviceType)
{
    switch(serviceType)
    {
         case RS_PARK_CLIMATE_VENT: //FALL-THROUGH
         case RS_PARK_CLIMATE_HEAT: //FALL-THROUGH
         case RS_PARK_CLIMATE_AUTO:
#if 0
             if(serviceType == RS_PARK_CLIMATE_AUTO)
                 sendDSPTackMsgToTSP(RS_PARK_CLIMATE_AUTO);
#endif
             //setCurrState(serviceType, RS_ACTIVE); move to set after tcu prereq chceck
             RemoteService_requestHandler(RS_RPC, serviceType);
             //timer start
             //setCurrState(serviceType, RS_WAIT_RESPONSE);
             //m_parkClimateStartTimer->setDuration(RPC_STARTUP_TIME, 0);
             //m_parkClimateStartTimer->start();
             break;

         case RS_PARK_CLIMATE_STOP:
             //sendDSPTackMsgToTSP(RS_PARK_CLIMATE_STOP);

             //setCurrState(serviceType, RS_ACTIVE); move to set after tcu prereq chceck
             RemoteService_requestHandler(RS_RPC, serviceType);
             //timer start
             //setCurrState(serviceType, RS_WAIT_RESPONSE);
             //m_parkClimateStopTimer->setDuration(RPC_SHUTDOWN_TIME, 0);
             //m_parkClimateStopTimer->start();
             break;

         default:
            break;
    }
}

void RemoteServiceApp::rcvTSP_RISM(RS_ServiceType serviceType)
{
    //sendDSPTackMsgToTSP(RS_RISM_MOVE);  //DSPT ack can skip.

    switch(serviceType)
    {
        case RS_RISM_MOVE:
            setCurrState(RS_RISM_MOVE, RS_ACTIVE);  //READY -> ACTIVE
            RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_INIT);
            break;

        case RS_RISM_CANCEL:
            //setCurrState(serviceType, RS_ACTIVE);  // TODO: if need then set state
            //setCurrRISMService(serviceType);   // TODO: need check this function use.
            RemoteService_requestHandler(RS_RISM, RS_RISM_CANCEL);
            break;
        default:
            break;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromTSP_ScheduledWakeup(RS_ServiceType remoteServiceType)
{
    //sendDSPTackMsgToTSP(RS_SCHEDULED_WAKEUP);

    error_t error = setScheduledWakeUpSecond(mScheduledWakeUpSecond);

    /*
    services/AlarmManagerService/ScheduledWakeup.h
	class ScheduledWakeup

    static const int32_t ERR_NONE = 0;
    static const int32_t ERR_UNAVAILABLE_OPMODE = -1;
    static const int32_t ERR_STARTTIME_IN_THE_PAST = -2;
    static const int32_t ERR_STARTTIME_TOOFAR_INTO_FUTURE = -3;
    static const int32_t ERR_NOT_ALLOWED = -4;
    */
    switch(error)
    {
        case 0:
            sendAckMsgToTSP(RS_SCHEDULED_WAKEUP, svc::actionResponse);
            break;
        case -1:
            setNGTPerrorCode(incorrectState_chosen);
            sendNackMsgToTSP(RS_SCHEDULED_WAKEUP);
            break;
        case -2:
            setNGTPerrorCode(startTimeInThePast_chosen);
            sendNackMsgToTSP(RS_SCHEDULED_WAKEUP);
            break;
        case -3:
            setNGTPerrorCode(startTimeTooFarIntoTheFuture_chosen);
            sendNackMsgToTSP(RS_SCHEDULED_WAKEUP);
            break;
        case -4: //FALL-THROUGH
        default:
            setNGTPerrorCode(notAllowed_chosen);
            sendNackMsgToTSP(RS_SCHEDULED_WAKEUP);
            break;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromDM(int32_t id, uint8_t cmd, uint8_t cmd2)
{
    LOGE("RemoteService_receiveMessageFromDM() cmd:0x%x cmd2:0x%x", cmd, cmd2);

    setCurrDBGcmdId(id);

    if(cmd == DBG_CMD_ALARM_RESET && cmd2 == DBG_CMD_TYPE_REQUEST)
    {
        LOGE("RS_ALARM_RESET Trigger");
        setDBGcmdActiveStatus(true);
        RemoteService_receiveMessageFromTSP(RS_ALARM_RESET);
    }
    else if (cmd == DBG_CMD_VEHICLE_UNLOCK && cmd2 == DBG_CMD_TYPE_REQUEST)
    {
        LOGE("RS_UNLOCK_VEHICLE Trigger");
        setDBGcmdActiveStatus(true);
        RemoteService_receiveMessageFromTSP(RS_UNLOCK_VEHICLE);
    }
    else if (cmd == DBG_CMD_VEHICLE_SECURE && cmd2 == DBG_CMD_TYPE_REQUEST)
    {
        LOGE("RS_SECURE_VEHICLE Trigger");
        setDBGcmdActiveStatus(true);
        RemoteService_receiveMessageFromTSP(RS_SECURE_VEHICLE);
    }
    else if (cmd == DBG_CMD_VEHICLE_LOCATION && cmd2 == DBG_CMD_TYPE_REQUEST)
    {
        LOGE("RS_HONK_FLASH Trigger");
        setDBGcmdActiveStatus(true);
        RemoteService_receiveMessageFromTSP(RS_HONK_FLASH);
    }
    else
    {
        //unknown cmd.
        LOGE("RemoteService_receiveMessageFromDM() Unknown command");
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromPM(int8_t newMode)
{
    LOGV("RemoteService_receiveMessageFromPM() getCurrPowerMode:%d newMode:%d, getCurrRemoteStartStatus:%d TIMER_RES_MAX_RUN on? :%d ",
        getCurrPowerMode(), newMode, getCurrRemoteStartStatus(), isTimerOn(MyTimer::TIMER_RES_MAX_RUN));

    if(getCurrPowerMode() != PM_RUNNING_2)
    {
        stopTimerId(MyTimer::TIMER_MIL_FAULT_DETECT);
    }

    if((newMode <= PM_ACCESSORY_1) && (isTimerOn(MyTimer::TIMER_RES_MAX_RUN)))
    {
        stopRESrunTimer();
    }
    if((getCurrPowerMode() > PM_IGNITION_ON_2) && (newMode < PM_IGNITION_ON_2))  // Ignition off
    {
/* changed to save every fuel (signal when pm>6) and (more than 4sec after pm>6)
        uint16_t fuelLevel = getFuelLevelIndicated();
        uint8_t temp[5] = {0,};

        sprintf(temp, "%d", fuelLevel);
        setProperty(STR_FUEL_LEVEL, temp);
        */
        stopTimerId(MyTimer::TIMER_SKIP_WARMUP_VALIABLE);
    }
    else if((getCurrPowerMode() <= PM_IGNITION_ON_2) && (newMode > PM_IGNITION_ON_2) ) // Normal Ignition on (not Remote start status)
    {
        startTimerId(MyTimer::TIMER_SKIP_WARMUP_VALIABLE);
        if(getCurrRemoteStartStatus() == false)
        {
            //LOGV("RES run timer reset to initial state.");
            //setRESrunTime(RES_ENGINE_RUN_MAX);
            LOGV("RemoteService Engine normally started. Crash status clear.");
            setProperty(STR_PREV_CRASH, getProperty(STR_CURR_CRASH));
            setProperty(STR_CURR_CRASH, STR_FALSE);
        }
    }
    else
    {
    }
    /*else*/ if((newMode == PM_RUNNING_2) && (getCurrRemoteStartStatus() == true))  //Remote Engine Started (Remote start status)
    {
        LOGV("RemoteService Engine started in RES mode.");
        //MIL check. timer start
        startTimerId(MyTimer::TIMER_MIL_FAULT_DETECT);

        if(!isTimerOn(MyTimer::TIMER_RES_MAX_RUN)) {
            uint32_t remainingTime_s = (uint32_t) getUint16fromConfig(RVC_RES_RUNMAX);
            LOGI("RES runtimer start remaining run time:%d", remainingTime_s);
            if(remainingTime_s != 0) {
                startTimerId(MyTimer::TIMER_RES_MAX_RUN);
                startTimerId(MyTimer::TIMER_UPDATE_RES_REMAINING);
            }
        }
        setLogTransistionFromREStoNormal(0x00);
        RemoteService_receiveMessageFromTSP(RS_TARGET_TEMP);
    }
    /*else*/ if((newMode == PM_RUNNING_2) && (getCurrRemoteStartStatus() == false))
    {
//        uint16_t remainingInitTime_s = getUint16fromConfig(RES_ENGINE_RUN_MAX);
        stopTimerId(MyTimer::TIMER_RES_MAX_RUN);
        stopTimerId(MyTimer::TIMER_UPDATE_RES_REMAINING);
        LOGV("RES run timer reset to initial state.");
//        setUint16toConfig(RVC_RES_RUNMAX, remainingInitTime_s);
        initRESremainingTime();
//        setLogTransistionFromREStoNormal(0x01);
    }
    if(newMode == PM_KEY_OUT)
    {
        if(isTimerOn(MyTimer::TIMER_RES_STOP) && (getCurrRemoteStartStatus() == false))
        {
            stopRESrunTimer();
            stopTimerId(MyTimer::TIMER_RES_STOP);
            RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, getCurrParkClimateModeTCU());
            setCurrState(RS_ENGINE_STOP, RS_READY);
        }
        setLogTransistionFromREStoNormal(LOG_NOT_AVAILABLE);
    }
    if((newMode == PM_RUNNING_2) && (getCurrPowerMode() < PM_RUNNING_2))
    {
        /*
        5.9.10 Status Update at Change to PowerMode=7
        SRD_ReqRPC2147_V1: If the PowerMode changes from < 7 to 7 during a Remote Park Climate
        operation, the TCU shall consider the Remote Park Climate operation to have ended.
        If the FBH status according to the requirement "Park Climate Status Notification" indicates that
        the FBH is still running in PowerMode=7, the TCU shall notify the current heater state to the TSP
        */
        //if(IsVehicleWithFFH())
        {
            int8_t currParkClimateModeTCU = getCurrParkClimateModeTCU();
            if( (currParkClimateModeTCU == CLIMATE_ENGINE_HEATING)
                || (currParkClimateModeTCU == CLIMATE_CABIN_HEATING)
                || (currParkClimateModeTCU == CLIMATE_CABIN_VENTING) )
            {
                RemoteService_askUpdateMessageToRVM(CLIMATE_STATUS_CHANGED, currParkClimateModeTCU);
            }
        }
    }
/*    else
    {
        LOGE("ignore powermode:%d", newMode);
    }*/
    setCurrPowerMode(newMode);
}

void RemoteServiceApp::RemoteService_receiveMessageFromDiag(uint32_t id, uint8_t cmd1, uint8_t cmd2, sp<Buffer>& buf)
{
    uint8_t *payload = buf->data();
    uint16_t diagRoutineID = (payload[0] << 8) | payload[1];
    uint8_t index = payload[2];  //need check index. 0: latest, 1:second,  JLR requirement: max 8
    uint8_t data[DIAG_RESP_SIZE] = {0, };
    error_t ret = E_OK;
    sp<Buffer> logBuf = m_remoteServiceLog->getLoggingData(index);

    data[0] = payload[0];
    data[1] = payload[1];
    data[2] = DIAG_ROUTINE_STATUS_ACTIVE;//Routine control activated.

    memcpy(&data[3], logBuf->data(), DIAG_PARAM_SIZE);

    if(cmd1 == DIAG_CMD_ROUTINE_CONTROL && cmd2 == DIAG_SUB_StartRoutine && diagRoutineID == DIAG_ROUTINE_ID_15)
    {
        sp<Buffer> buf = new Buffer();
        buf->setTo(data, DIAG_RESP_SIZE);
        ret=m_diagManager->responseDiagcmd(id, DIAG_RESP_ROUTINE_CONTROL, cmd2, buf);
        if(ret==E_OK){
            data[2] = DIAG_ROUTINE_STATUS_TERMINATED;//Routine control terminated.
            buf->setTo(data, DIAG_RESP_SIZE);
            ret=m_diagManager->responseDiagcmd(id, DIAG_RESP_ROUTINE_CONTROL, cmd2, buf);
        }
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromConfig(const sp<sl::Message> &msg)
{
    char *name  = (char *)msg->buffer.data();
    LOGV("RemoteService_receiveMessageFromConfig parameter name : %s ", name);

    if(strcmp(RCC_TARGET_TEMP, name) == 0)
    {
        RemoteService_requestHandler(RS_RCC, RS_TARGET_TEMP);
        return;
    }

    if(strcmp(APP_MODE_SVT, name) == 0)
    {
        if(getUint8fromConfig(APP_MODE_SVT) == STATUS_ENABLE)
        {
            registerSVTpostReceiver();
        }
        else
        {
            setSubscriptSVT(false);
        }
        return;
    }

    if(strcmp(APP_MODE_RISM, name) == 0)
    {
        setAppModeRISM(getUint8fromConfig(APP_MODE_RISM));
        return;
    }
    if(strcmp(CONFIG_CONVENIENCE_FOLD, name) == 0)
    {
        setConvenienceFold(getUint8fromConfig(CONFIG_CONVENIENCE_FOLD));
        return;
    }
    if(strcmp(CONFIG_VEHICLE_TYPE, name) == 0)
    {
        setVehicleType(getUint8fromConfig(CONFIG_VEHICLE_TYPE));
        return;
    }
    if(strcmp(CONFIG_INTELLIGENT_SEAT_FOLD, name) == 0)
    {
        setIntelligentSeatFold(getUint8fromConfig(CONFIG_INTELLIGENT_SEAT_FOLD));
        return;
    }

    if(strcmp(RES_FUEL_LEVEL_MIN, name) == 0)
    {
        stopRESuponLowFuel();
        return;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromTelephony(int32_t what, int32_t arg1)
{
    LOGV("RemoteService_receiveMessageFromTelephony what:%d arg1:%d", what, arg1);

    switch(what)
    {
        case MyHandler::RECV_MSG_ACTIVESIM_STATE_CHANGED:
            if(arg1 == TelephonyManager::REMOTE_SIM_TSIM)
            {
                stopTimerId(MyTimer::TIMER_PSIM_LOCK);
                //if need: RvcPsimRelease();
            }
            break;

        case MyHandler::RECV_MSG_PSIM_LOCK_STATE_CHANGED:  //FALL-THROUGH
        case MyHandler::RECV_MSG_TSIM_LOCK_STATE_CHANGED:  //FALL-THROUGH
        default:
            break;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromCallbackAlarmExpireListener(int32_t what, int32_t arg1)
{
    LOGV("RemoteService_receiveMessageFromCallbackAlarmExpireListener what:%d arg1:%d", what, arg1);

    m_myTimer->handlerFunction(MyTimer::TIMER_FFH_MAX_RUN);
}

void RemoteServiceApp::RemoteService_receiveMessageFromCanBusStateReceiver(int32_t what, int32_t arg1)
{
    LOGV("RemoteService_receiveMessageFromCanBusStateReceiver what:%d arg1:%d", what, arg1);

    switch(arg1)
    {
        case 0: // CANBUS_OFF
            //if(IsVehicleWithFFH())
        {
            uint8_t parkClimateReqTCU = getCurrParkClimateReqTCU();
            LOGI("parkClimateReqTCU:%d getCurrParkClimateModeTCU:%d getCurrState(RS_FFH_START):%d",
                parkClimateReqTCU, getCurrParkClimateModeTCU(), getCurrState(RS_FFH_START));
            if((getCurrState(RS_FFH_START) == RS_ACTIVE)||(getCurrState(RS_FFH_START) == RS_WAIT_RESPONSE))
            {
                if(parkClimateReqTCU == RES_ENGINE_HEAT_REQUEST)
                {
                    /* 3.8.2.3 TCU Interpretation of HVAC Response to Engine Heat Request
                    Otherwise, if ParkClimateMode does not change to 2 (Engine Heating) before CAN sleep
                    or with in PARK_CLIMATE_STARTUP_TIME, the TCU shall proceed with RES,
                    according to the requirement "RES Request to BCM"
                    */
                    int8_t parkClimateModeWhenReqEngineHeating = getParkClimateModeWhenReqEngineHeating();
                    LOGI("parkClimateModeWhenReqEngineHeating:%d", parkClimateModeWhenReqEngineHeating);
                    if((parkClimateModeWhenReqEngineHeating == CLIMATE_MODE_STARTUP)
                        || (parkClimateModeWhenReqEngineHeating == CLIMATE_MODE_OFF))
                    {
                        if(getCurrParkClimateModeTCU() != CLIMATE_ENGINE_HEATING)
                        {
                            stopTimerId(MyTimer::TIMER_CLIMATE_REQ);
                            setCurrState(RS_FFH_PREREQ, RS_READY);
                            setLogFfhRunTimer(false);
                            stopTimerId(MyTimer::TIMER_FFH_MAX_RUN);
                            setCurrState(RS_FFH_START, RS_READY);

                            setCurrState(RS_ENGINE_START, RS_ACTIVE);
                            RemoteService_requestHandler(RS_RES, RS_ENGINE_START);
                        }
                    }
                }
            }
            if(parkClimateReqTCU != RPC_REQ_PARK_DEFAULT)
            {
                reqDefaultParkClimate();
            }
        }
            break;
        case 1: // CANBUS_ON
            //if(IsVehicleWithFFH())
        { // keep power??
            startTimerId(MyTimer::TIMER_PARK_CLIMATE_MIN_STATUS_WAIT, true);

            RS_ServiceState currState_RS_PARK_CLIMATE_AUTO = getCurrState(RS_PARK_CLIMATE_AUTO);
            RS_ServiceState currState_RS_PARK_CLIMATE_STOP = getCurrState(RS_PARK_CLIMATE_STOP);
            RS_ServiceState currState_RS_PARK_CLIMATE_VENT = getCurrState(RS_PARK_CLIMATE_VENT);
            RS_ServiceState currState_RS_PARK_CLIMATE_HEAT = getCurrState(RS_PARK_CLIMATE_HEAT);

            LOGI("CAN bus wakeup, FFH vehicle currState auto:%d stop:%d vent:%d heat:%d",
                currState_RS_PARK_CLIMATE_AUTO, currState_RS_PARK_CLIMATE_STOP, currState_RS_PARK_CLIMATE_VENT, currState_RS_PARK_CLIMATE_HEAT);

            if( (currState_RS_PARK_CLIMATE_AUTO == RS_ACTIVE)
                || (currState_RS_PARK_CLIMATE_STOP == RS_ACTIVE)
                || (currState_RS_PARK_CLIMATE_VENT == RS_ACTIVE)
                || (currState_RS_PARK_CLIMATE_HEAT == RS_ACTIVE) )
            {
                startTimerId(MyTimer::TIMER_PARK_CLIMATE_STATUS_TIMEOUT);
            }
        }
            break;
        case -1: //CANBUS_UNKNOWN //FALL-THROUGH
        default:
            break;
    }
}

void RemoteServiceApp::RemoteService_receiveMessageFromMCUFrame(int32_t what, int32_t arg1)
{
    LOGV("RemoteService_receiveMessageFromMCUFrame what:%d arg1:%d", what, arg1);

    switch(arg1)
    {
        case SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_M_RX: //FALL-THROUGH
        case SignalFrame_MULTI_e::SignalFrame_IPC_PT_E_RX:
            rcvMCUFrame_fuel();
            return;
        case SignalFrame_PMZ_e::SignalFrame_BCM_PMZ_F_RX: //FALL-THROUGH
        case SignalFrame_MULTI_e::SignalFrame_BCM_PT_G_RX:
            rcvMCUFrame_resRelevantValiables();
            return;
        case SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AI_RX: //FALL-THROUGH
        case SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AS_RX:
            rcvMCUFrame_seat(arg1);
            return;
        default:
            break;
    }
}


void RemoteServiceApp::rcvMCUFrame_fuel()
{
    LOGV("rcvMCUFrame_fuel");

    if( RemoteService_getPowerMode() <= PM_IGNITION_ON_2 )
    { //fuel signals give invalid value when pm <= 6
        LOGD("RemoteService_receiveMessageFromMCUFrame RETURN : PM <= 6");
        return;
    }
    else if(isTimerOn(MyTimer::TIMER_SKIP_WARMUP_VALIABLE))
    { // fuel signals might take about 3.5 second to give valid value when pm become > 6
        LOGD("RemoteService_receiveMessageFromMCUFrame : TimerOn");
        return;
    }
    else
    {
    }

    if(updateFromCan_FuelLevelIndicatedQFHS() == RES_FUEL_QF)
    { // TCU shall ignore the FuelLevelIndicated if the FuelLevelIndicatedQF is not equal to 3.
        updateFromCan_FuelLevelIndicatedHS();
        stopRESuponLowFuel();
    }
}

void RemoteServiceApp::rcvMCUFrame_resRelevantValiables()
{
    bool RESAlarmStatus_previous = getRESAlarmStatus();
    bool RESBrakePedalStatus_previous = getRESBrakePedalStatus();
    //bool RESEngineRunningStatus_previous = getRESEngineRunningStatus();
    bool RESHazardSwitchStatus_previous = getRESHazardSwitchStatus();
    uint8_t RESSequenceStatus_previous = getRESSequenceStatus();
    bool RESTransmissionStatus_previous = getRESTransmissionStatus();
    bool RESVehicleLockStatus_previous = getRESVehicleLockStatus();
    bool RESVehStationaryStatus_previous = getRESVehStationaryStatus();
    bool RESWindowStatus_previous = getRESWindowStatus();

    updateResRelevantValiables();
    uint8_t RESSequenceStatus_current = getRESSequenceStatus();
    LOGV("rcvMCUFrame_resRelevantValiables RESSequenceStatus 0x%x -> 0x%x",
        RESSequenceStatus_previous, RESSequenceStatus_current);
    LOGV("valid?: %d %d %d %d",
        isTimerOn(MyTimer::TIMER_RES_START), isTimerOn(MyTimer::TIMER_RES_MAX_RUN),
        getCurrRemoteStartStatus(), getRESBrakePedalStatus());

    if( isTimerOn(MyTimer::TIMER_RES_START)
        || isTimerOn(MyTimer::TIMER_RES_MAX_RUN)
        || (getCurrRemoteStartStatus() == true)
        || ( (RESSequenceStatus_current == 0x0E) && (getRESBrakePedalStatus() == 0) ) // to trace Key Not Found
        )
    {
        if(RESSequenceStatus_previous != RESSequenceStatus_current)
        {
            setLogRESSequenceStatusPrevious(RESSequenceStatus_previous);
        }
        if( (RESAlarmStatus_previous != getRESAlarmStatus())
            || (RESBrakePedalStatus_previous != getRESBrakePedalStatus())
            //|| (RESEngineRunningStatus_previous != getRESEngineRunningStatus())
            || (RESHazardSwitchStatus_previous != getRESHazardSwitchStatus())
            || (RESSequenceStatus_previous != getRESSequenceStatus())
            || (RESTransmissionStatus_previous != getRESTransmissionStatus())
            || (RESVehicleLockStatus_previous != getRESVehicleLockStatus())
            || (RESVehStationaryStatus_previous != getRESVehStationaryStatus())
            || (RESWindowStatus_previous != getRESWindowStatus()) )
        {
            makeLog();
        }
        if(RESSequenceStatus_previous != RESSequenceStatus_current)
        {
            uint8_t* buff = new uint8_t[SIZE_1BYTE]();
            buff[0] = RESSequenceStatus_current;
            RemoteService_responseHandler(RS_RES, RS_ENGINE_START, Signal_RESSequenceStatus_RX, buff, SIZE_1BYTE);
            delete[] buff;
        }
    }
}

void RemoteServiceApp::rcvMCUFrame_seat(int32_t arg1)
{
    uint8_t vehicleType = getVehicleType();
    bool statusChanged = false;

    if(isRISMAppEnabled() == false)
    { // harmony JLRTCU-2117
        LOGD("rcvMCUFrame_seat SKIP : AppModeRISM NOT enabled");
        return;
    }
    if( (arg1 == SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AI_RX)
        && (vehicleType == VEHICLETYPE_L462) )
    {
        statusChanged = updateSeatStatus_L462();
    }
    else if( (arg1 == SignalFrame_PMZ_e::SignalFrame_GWM_PMZ_AS_RX)
        && (vehicleType == VEHICLETYPE_L405)
        /* harmony JLRTCU-2287
        Since the IntelligentSeatFold value is neither 0x04 nor 0x07, even if there is change in the seat status signals,the TCU should not send RISM update message to TSP. It should ignore these changes.*/
        && isSeatType3() )
    {
        statusChanged = updateSeatStatus_L405();
    }
    else
    {
        LOGD("rcvMCUFrame_seat invalid frame(%d) or vehicleType(%d) or intelligentSeatFold(%d)", arg1, vehicleType, getIntelligentSeatFold());
        return;
    }

    LOGV("statusChanged:%d, getCurrState(RS_RISM_MOVE):%d ", statusChanged, getCurrState(RS_RISM_MOVE));
    if(statusChanged == true)
    {
        if(!isTimerOn(MyTimer::TIMER_RISM_RELEASE_KEEPPOWER))
        {
            RvcKeepPowerAcquire();
        }
        startTimerId(MyTimer::TIMER_RISM_RELEASE_KEEPPOWER, true);

        createRISMUpdateNgtpTime();
        sendRISMUpdateMsgToTSP();
    }
    else
    {
        //LOGD("rcvMCUFrame_seat status not changed");
        return;
    }

    if(getCurrState(RS_RISM_MOVE) == RS_WAIT_RESPONSE)
    {
        bool seatMovedAsRequested = false;

        switch(vehicleType)
        {
            case VEHICLETYPE_L462:
                seatMovedAsRequested = checkSeatMovedAsRequested_L462();
                break;
            case VEHICLETYPE_L405:
                seatMovedAsRequested = checkSeatMovedAsRequested_L405();
                break;
            default:
                break;
        }
        if(seatMovedAsRequested == true)
        {
            LOGV("All requested seat move Success.");
            setCurrState(RS_RISM_MOVE, RS_ACTIVE);  // WAIT -> ACTIVE
            RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
        }
    }
}

