
#define LOG_TAG "RemoteServiceApp"
#include "Log.h"

#include "RemoteServiceApp.h"


uint8_t RemoteServiceApp::updateFromCan_FuelLevelIndicatedQFHS()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint8_t sigVal = 0;

    if(getCurrCANData(Signal_FuelLevelIndicatedQFHS_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal = buf[0];
        LOGI("updateWarmupVariable_FuelLevelIndicatedQFHS : %d ", sigVal);
        setFuelLevelIndicatedQF(sigVal);
    }
    return sigVal;
}

void RemoteServiceApp::updateFromCan_FuelLevelIndicatedHS()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint16_t sigVal16 = 0;

    if(getCurrCANData(Signal_FuelLevelIndicatedHS_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];

        //fuel level: 0-1023 * 0.2(scale)  litres
        uint16_t fuelDeciLevel = (sigVal16 & RES_FUEL_MASK) * 2; //save deci litres;

        LOGI("updateWarmupVariable_FuelLevelIndicatedHS : %d ", fuelDeciLevel);
        setFuelLevelIndicated(fuelDeciLevel);
    }
}

void RemoteServiceApp::updateFromCan_FuelGaugePosition()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint8_t sigVal;

    if(getCurrCANData(Signal_FuelGaugePosition_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal = buf[0];
        LOGI("updateWarmupVariable_FuelGaugePosition : %d ", sigVal);
        setFuelGaugePosition(sigVal);
    }
}

void RemoteServiceApp::updateFromCan_OdometerMasterValue()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint32_t sigVal24 = 0;

    if(getCurrCANData(Signal_OdometerMasterValue_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal24 |= ((uint32_t)buf[0] << 16);
        sigVal24 |= ((uint32_t)buf[1] << 8);
        sigVal24 |= buf[2];
        sigVal24 &= ODOMETER_MASK;
        LOGI("updateWarmupVariable_OdometerMasterValue : %d ", sigVal24);
        setOdometerMasterValue(sigVal24);
    }
}

void RemoteServiceApp::updateFromCan_ODODisplayedMiles()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint32_t sigVal24 = 0;

    if(getCurrCANData(Signal_ODODisplayedMiles_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal24 |= ((uint32_t)buf[0] << 16);
        sigVal24 |= ((uint32_t)buf[1] << 8);
        sigVal24 |= buf[2];
        LOGI("updateFromCan_ODODisplayedMiles : %d ", sigVal24);
        setODODisplayedMiles(sigVal24);
    }
}

void RemoteServiceApp::updateFromCan_DistanceToEmpty()
{
    sp<Buffer> sigdatabuf = new Buffer();
    uint16_t sigVal16 = 0;

    if(getCurrCANData(Signal_DistanceToEmpty_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        sigVal16 |= ((uint16_t)buf[0] << 8);
        sigVal16 |= buf[1];
        LOGI("updateWarmupVariable_DistanceToEmpty : %d ", sigVal16);
        setDistanceToEmpty(sigVal16);
    }
}

void RemoteServiceApp::updateFromCan_doorLatch()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DoorLatchStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setDoorLatchStatus(buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_WindowPosition()
{ // refer. SRD RVM 2.7.9 Window Status, Figure RVM9 ? Flowchart of Window Status Alert monitoring logic
    sp<Buffer> sigdatabuf_dw = new Buffer();
    sp<Buffer> sigdatabuf_rd = new Buffer();
    sp<Buffer> sigdatabuf_dr = new Buffer();
    sp<Buffer> sigdatabuf_pw = new Buffer();
//    sp<Buffer> sigdatabuf_rp = new Buffer();
//    sp<Buffer> sigdatabuf_pr = new Buffer();

    mDrvWindowStatus = svc::WindowStatus_unknown;
    mRDrvWindowStatus = svc::WindowStatus_unknown;
    mPasWindowStatus = svc::WindowStatus_unknown;
    mRPasWindowStatus = svc::WindowStatus_unknown;

    switch(getMovingWindows())
    {
        case 4:
            if(getCurrCANData(Signal_RearDriverWindowPos_RX, sigdatabuf_rd) == true)
            {
                uint8_t *buf_rd = sigdatabuf_rd->data();
                if(buf_rd[0] != 0)
                {
                    setWindowStatus(Signal_RearDriverWindowPos_RX, buf_rd[0]);
                }
            }
            if(mRDrvWindowStatus == svc::WindowStatus_unknown)
            {
                if(getCurrCANData(Signal_DriverRearWindowPos_RX, sigdatabuf_dr) == true)
                {
                    uint8_t *buf_dr = sigdatabuf_dr->data();
                    setWindowStatus(Signal_DriverRearWindowPos_RX, buf_dr[0]);
                }
            }

            if(getCurrCANData(Signal_RearPassWindowPos_RX, sigdatabuf_rd) == true)
            {
                uint8_t *buf_rd = sigdatabuf_rd->data();
                if(buf_rd[0] != 0)
                {
                    setWindowStatus(Signal_RearPassWindowPos_RX, buf_rd[0]);
                }
            }
            if(mRPasWindowStatus == svc::WindowStatus_unknown)
            {
                if(getCurrCANData(Signal_PassRearWindowPos_RX, sigdatabuf_dr) == true)
                {
                    uint8_t *buf_dr = sigdatabuf_dr->data();
                    setWindowStatus(Signal_PassRearWindowPos_RX, buf_dr[0]);
                }
            }
        //FALL-THROUGH
        case 2:
            if(getCurrCANData(Signal_DriverWindowPosition_RX, sigdatabuf_dw) == true)
            {
                uint8_t *buf_dw = sigdatabuf_dw->data();
                setWindowStatus(Signal_DriverWindowPosition_RX, buf_dw[0]);
            }
            if(getCurrCANData(Signal_PassWindowPosition_RX, sigdatabuf_pw) == true)
            {
                uint8_t *buf_pw = sigdatabuf_pw->data();
                setWindowStatus(Signal_PassWindowPosition_RX, buf_pw[0]);
            }
            break;

        default:
            break;
    }
    LOGI("updateSecureState_WindowPosition %d, %d, %d, %d  ", mDrvWindowStatus, mRDrvWindowStatus, mRDrvWindowStatus, mRDrvWindowStatus);
}

void RemoteServiceApp::updateFromCan_sunroofOpen()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_SunroofOpen_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setSunroofOpenStatus(buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_roofPositionStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RoofPositionStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRoofPositionStatus(buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_ParkHeatVentTime()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_ParkHeatVentTime_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        int8_t parkHeatVentTime = buf[0];
        if(getParkHeatVentTime() != parkHeatVentTime)
        { // 실제 신호 값을 저장하고, NGTP 메시지를 만들 때 offset 추가..
            setParkHeatVentTime(parkHeatVentTime);
            saveProperty_RPCvariables(Signal_ParkHeatVentTime_RX, parkHeatVentTime);
        }
    }
}

int8_t RemoteServiceApp::updateFromCan_parkClimateMode()
{
    sp<Buffer> sigdatabuf = new Buffer();
    int8_t parkClimateMode = -1;

    if(getCurrCANData(Signal_ParkClimateMode_RX, sigdatabuf, true) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        parkClimateMode = buf[0];

        if(getCurrParkClimateModeTCU() != parkClimateMode)
        {
            setCurrParkClimateModeTCU(parkClimateMode);
        }
    }
    return parkClimateMode;
}

void RemoteServiceApp::updateFromCan_RESWindowStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESWindowStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESWindowStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESVehicleLockStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESVehicleLockStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESVehicleLockStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESAlarmStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESAlarmStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESAlarmStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESHazardSwitchStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESHazardSwitchStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESHazardSwitchStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESBrakePedalStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESBrakePedalStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESBrakePedalStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESVehStationaryStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESVehStationaryStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESVehStationaryStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESTransmissionStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESTransmissionStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESTransmissionStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESEngineRunningStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESEngineRunningStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESEngineRunningStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RESSequenceStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RESSequenceStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setRESSequenceStatus((uint8_t)buf[0]);
    }
}

int8_t RemoteServiceApp::updateFromCan_FFHOperatingStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();
    int8_t val = -1;
    if(getCurrCANData(Signal_FFHOperatingStatus_RX, sigdatabuf, true) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        val = buf[0];

        if(val != getFFHOperatingStatus())
        {
            setLogffhStatusWhenFFHStopped(val);
            setFFHOperatingStatus(val);
            saveProperty_RPCvariables(Signal_FFHOperatingStatus_RX, val);
        }
    }
    return val;
}

void RemoteServiceApp::updateFromCan_TimerActivationStatus()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_TimerActivationStatus_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        bool val = buf[0];

        if(getRPCtimerActiveStatus() != val)
        {
            setRPCtimerActiveStatus((bool)buf[0]);
            saveProperty_RPCvariables(Signal_TimerActivationStatus_RX, buf[0]);
        }
    }
}

void RemoteServiceApp::updateFromCan_Timer1Time()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_Timer1Time_RX, sigdatabuf) == true)
    {
        uint32_t buffer[10];
        uint32_t sigVal32 = 0;
        uint8_t *buf = sigdatabuf->data();
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];

        if(getTimer1Time() != sigVal32)
        {
            setTimer1Time(sigVal32);
            sprintf((char*) buffer, "%d", sigVal32);
            setProperty(PROP_TIMER_1_TIME, (const char *) buffer, true);
        }
    }
}

void RemoteServiceApp::updateFromCan_Timer2Time()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_Timer2Time_RX, sigdatabuf) == true)
    {
        uint32_t buffer[10];
        uint32_t sigVal32 = 0;
        uint8_t *buf = sigdatabuf->data();
        sigVal32 |= ((uint32_t)buf[0] << 24);
        sigVal32 |= ((uint32_t)buf[1] << 16);
        sigVal32 |= ((uint32_t)buf[2] << 8);
        sigVal32 |= buf[3];

        if(getTimer2Time() != sigVal32)
        {
            setTimer2Time(sigVal32);
            sprintf((char*) buffer, "%d", sigVal32);
            setProperty(PROP_TIMER_2_TIME, (const char *) buffer, true);
        }
    }
}

void RemoteServiceApp::updateFromCan_DoorStatusHS()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DoorStatusHS_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setDoorOpenStatus(buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_HFSCommand()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_HFSCommand_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setHFSCommand((bool)buf[0]);
    }
}
void RemoteServiceApp::updateFromCan_HRWCommand()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_HRWCommand_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setHRWCommand((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_RemoteStartStatusMS()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RemoteStartStatusMS_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setCurrRemoteStartStatus((bool)buf[0]);
    }
}

void RemoteServiceApp::updateFromCan_AlarmModeHS()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_AlarmModeHS_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        setCurrAlarmStatus((RS_AlarmMode)buf[0]);
    }
}

int8_t RemoteServiceApp::updateFromCan_Internal_Serv_CAN_VARIANT()
{
    sp<Buffer> sigdatabuf = new Buffer();
    int8_t canVariant = -1;

    if(getCurrCANData(Signal_Internal_Serv_CAN_VARIANT, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        canVariant = buf[0];
        setCanVariant(canVariant);
    }
    LOGI("updateFromCan_Internal_Serv_CAN_VARIANT : %d ", canVariant);
    return canVariant;
}

int8_t RemoteServiceApp::updateFromCan_PasSeatPosition1stRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatPosition1stRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("Signal_PasSeatPosition1stRow_RX invalid(0) ");
            return -1;
        }
        if(statusRISM.pasSeat1stRowState != buf[0])
        {
            statusRISM.pasSeat1stRowState = (seatMovementStatus)buf[0];
        }
        return statusRISM.pasSeat1stRowState;
    }
    else
    {
        LOGD("Signal_PasSeatPosition1stRow_RX ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_DrvSeatPosition2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DrvSeatPosition2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_DrvSeatPosition2ndRow invalid(0) ");
            return -1;
        }
        if(statusRISM.drvSeat2ndRowState != buf[0])
        {
            statusRISM.drvSeat2ndRowState = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_DrvSeatPosition2ndRow_RX, buf[0]);
        }
        return statusRISM.drvSeat2ndRowState;
    }
    else
    {
        LOGD("updateFromCan_DrvSeatPosition2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_PasSeatPosition2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatPosition2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_PasSeatPosition2ndRow invalid(0) ");
            return -1;
        }
        if(statusRISM.pasSeat2ndRowState != buf[0])
        {
            statusRISM.pasSeat2ndRowState = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_PasSeatPosition2ndRow_RX, buf[0]);
        }
        return statusRISM.pasSeat2ndRowState;
    }
    else
    {
        LOGD("updateFromCan_PasSeatPosition2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_DrvSeatPosition3rdRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DrvSeatPosition3rdRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_DrvSeatPosition3rdRow invalid(0) ");
            return -1;
        }
        if(statusRISM.drvSeat3rdRowState != buf[0])
        {
            statusRISM.drvSeat3rdRowState = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_DrvSeatPosition3rdRow_RX, buf[0]);
        }
        return statusRISM.drvSeat3rdRowState;
    }
    else
    {
        LOGD("updateFromCan_DrvSeatPosition3rdRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_PasSeatPosition3rdRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatPosition3rdRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_PasSeatPosition3rdRo invalid(0) ");
            return -1;
        }
        if(statusRISM.pasSeat3rdRowState != buf[0])
        {
            statusRISM.pasSeat3rdRowState = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_PasSeatPosition3rdRow_RX, buf[0]);
        }
        return statusRISM.pasSeat3rdRowState;
    }
    else
    {
        LOGD("updateFromCan_PasSeatPosition3rdRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_PasSeatInhib1stRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatInhib1stRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.pasSeat1stRowInhib != buf[0])
        {
            statusRISM.pasSeat1stRowInhib = (seatMovementInhibition)buf[0];
        }
        return statusRISM.pasSeat1stRowInhib;
    }
    else
    {
        LOGD("updateFromCan_PasSeatInhib1stRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_DrvSeatInhib2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DrvSeatInhib2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.drvSeat2ndRowInhib != buf[0])
        {
            statusRISM.drvSeat2ndRowInhib = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_DrvSeatInhib2ndRow_RX, buf[0]);
        }
        return statusRISM.drvSeat2ndRowInhib;
    }
    else
    {
        LOGD("updateFromCan_DrvSeatInhib2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_PasSeatInhib2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatInhib2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.pasSeat2ndRowInhib != buf[0])
        {
            statusRISM.pasSeat2ndRowInhib = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_PasSeatInhib2ndRow_RX, buf[0]);
        }
        return statusRISM.pasSeat2ndRowInhib;
    }
    else
    {
        LOGD("updateFromCan_PasSeatInhib2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_DrvSeatInhib3rdRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_DrvSeatInhib3rdRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.drvSeat3rdRowInhib != buf[0])
        {
            statusRISM.drvSeat3rdRowInhib = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_DrvSeatInhib3rdRow_RX, buf[0]);
        }
        return statusRISM.drvSeat3rdRowInhib;
    }
    else
    {
        LOGD("updateFromCan_DrvSeatInhib3rdRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_PasSeatInhib3rdRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_PasSeatInhib3rdRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.pasSeat3rdRowInhib != buf[0])
        {
            statusRISM.pasSeat3rdRowInhib = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_PasSeatInhib3rdRow_RX, buf[0]);
        }
        return statusRISM.pasSeat3rdRowInhib;
    }
    else
    {
        LOGD("updateFromCan_PasSeatInhib3rdRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_RLSeatMovementStat()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RLSeatMovementStat_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();

        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_RLSeatMovementStat invalid(0) ");
            return -1;
        }
        if(statusRISM.RLSeatMovementStat != buf[0])
        {
          statusRISM.RLSeatMovementStat = (seatMovementStatus)buf[0];
          setProperty_seats(Signal_RLSeatMovementStat_RX, buf[0]);
        }
        return statusRISM.RLSeatMovementStat;
    }
    else
    {
        LOGD("updateFromCan_RLSeatMovementStat ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_RRSeatMovementStat()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RRSeatMovementStat_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();

        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_RRSeatMovementStat invalid(0) ");
            return -1;
        }
        if(statusRISM.RRSeatMovementStat != buf[0])
        {
            statusRISM.RRSeatMovementStat = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_RRSeatMovementStat_RX, buf[0]);
        }
        return statusRISM.RRSeatMovementStat;
    }
    else
    {
        LOGD("updateFromCan_RRSeatMovementStat ERR.. return -1 ");
        return -1;
    }
}
int8_t RemoteServiceApp::updateFromCan_RCntrSeatMovmentStat()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RCntrSeatMovmentStat_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();

        if(buf[0] == 0)
        {
            /* 8.7.3 Indicate seat movement completion request
            Note : If the seat position is read as “0 - unknown” by TCU then it should ignore it and go
            back to the last known position stored with TCU. If there is no last known position stored
            within TCU then send the current read position as it is to TSP,
            */
            LOGD("updateFromCan_RCntrSeatMovmentStat invalid(0) ");
            return -1;
        }
        if(statusRISM.RCntrSeatMovementStat != buf[0])
        {
            statusRISM.RCntrSeatMovementStat = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_RCntrSeatMovmentStat_RX, buf[0]);
        }
        return statusRISM.RCntrSeatMovementStat;
    }
    else
    {
        LOGD("updateFromCan_RCntrSeatMovmentStat ERR.. return -1 ");
        return -1;
    }
}
int8_t RemoteServiceApp::updateFromCan_SkiHatchStat()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_SkiHatchStat_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.REMSkiHatchStat != buf[0])
        {
            statusRISM.REMSkiHatchStat = (seatMovementStatus)buf[0];
            setProperty_seats(Signal_SkiHatchStat_RX, buf[0]);
        }
        return statusRISM.REMSkiHatchStat;
    }
    else
    {
        LOGD("updateFromCan_SkiHatchStat ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_RLSeatInhib2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RLSeatInhib2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.RLSeatInhib2ndRow != buf[0])
        {
            statusRISM.RLSeatInhib2ndRow = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_RLSeatInhib2ndRow_RX, buf[0]);
        }
        return statusRISM.RLSeatInhib2ndRow;
    }
    else
    {
        LOGD("updateFromCan_RLSeatInhib2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_RRSeatInhib2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RRSeatInhib2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.RRSeatInhib2ndRow != buf[0])
        {
            statusRISM.RRSeatInhib2ndRow = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_RRSeatInhib2ndRow_RX, buf[0]);
        }
        return statusRISM.RRSeatInhib2ndRow;
    }
    else
    {
        LOGD("updateFromCan_RRSeatInhib2ndRow ERR.. return -1 ");
        return -1;
    }
}

int8_t RemoteServiceApp::updateFromCan_RMSeatInhib2ndRow()
{
    sp<Buffer> sigdatabuf = new Buffer();

    if(getCurrCANData(Signal_RMSeatInhib2ndRow_RX, sigdatabuf) == true)
    {
        uint8_t *buf = sigdatabuf->data();
        if(statusRISM.RMSeatInhib2ndRow != buf[0])
        {
            statusRISM.RMSeatInhib2ndRow = (seatMovementInhibition)buf[0];
            setProperty_seats(Signal_RMSeatInhib2ndRow_RX, buf[0]);
        }
        return statusRISM.RMSeatInhib2ndRow;
    }
    else
    {
        LOGD("updateFromCan_RMSeatInhib2ndRow ERR.. return -1 ");
        return -1;
    }
}

