/**
* \file    RemoteService_RISM.cpp
* \brief     Implementation of RemoteService_RISM Class.
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       dongjin1.seo
* \date    2015.06.25
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "RVC_RISM"
#include "Log.h"

#include "RemoteService_RISM.h"




RemoteService_RISM::RemoteService_RISM(RemoteServiceApp& app)
    :m_RemoteServiceApp(app)
{

}


RemoteService_RISM::~RemoteService_RISM(){

}

void RemoteService_RISM::requestHandler(RS_ServiceType remoteServiceType)
{
    RS_ServiceType serviceType = remoteServiceType;
    bool prerequisite = true;
    error_t result = E_OK;

    LOGV("requestHandler() serviceType: %d", serviceType);

    if(serviceType == RS_SEAT_MOVE_INIT)
    {
        if(RemoteService_PrereqCheck(serviceType) == false)
        {
            m_RemoteServiceApp.sendNackMsgToTSP_RISM(RS_RISM_MOVE, conditionsNotCorrect_chosen);
            m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE -> READY
            return ;
        }
    }

    switch(serviceType)
    {
        case RS_SEAT_MOVE_INIT:
        {
            if(RISM_delayForReqBCMauth(RS_SEAT_MOVE_INIT) == E_OK)
            {
                result = RISM_reqInitSeatMove();
                if(result == E_ERROR)
                {
                    m_RemoteServiceApp.sendNackMsgToTSP_RISM(RS_RISM_MOVE, conditionsNotCorrect_chosen);
                    m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE -> READY
                }
                else
                    m_RemoteServiceApp.reqBCMauthentication(RS_SEAT_MOVE_INIT);
            }
        } break;

        case RS_SEAT_MOVE_COMPLETE:
        {
            if(RISM_delayForReqBCMauth(RS_SEAT_MOVE_COMPLETE) == E_OK)
            {
                result = RISM_reqCompleteSeatMove();
                if(result == E_ERROR)
                {
                    m_RemoteServiceApp.sendNackMsgToTSP_RISM(RS_RISM_MOVE, conditionsNotCorrect_chosen);
                    m_RemoteServiceApp.resetSeatMoveReq();
                    m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE -> READY
                }
                else
                    m_RemoteServiceApp.reqBCMauthentication(RS_SEAT_MOVE_COMPLETE);
            }
            else
            {
                m_RemoteServiceApp.resetSeatMoveReq();
            }
        } break;

        case RS_RISM_MOVE:
        {
            result = RISM_reqSeatMove();
            //if(result == E_ERROR)
            //    LOGE("Seat Move request fail. cause:%d", result);
        } break;

        case RS_RISM_CANCEL:
        {
            //RISM_reqCompleteSeatMove();
            //m_RemoteServiceApp.reqBCMauthentication(RS_SEAT_MOVE_COMPLETE);
        } break;

        default:
            break;
    }
    LOGV("RemoteService_RISM::requestHandler() END");
}


void RemoteService_RISM::responseHandler(RS_ServiceType remoteServiceType, uint16_t sigId, uint8_t* buf, uint32_t size)
{

}

bool RemoteService_RISM::responseStatus(RS_ServiceType remoteServiceType)
{
    return false;
}

error_t RemoteService_RISM::RISM_reqInitSeatMove()
{
    LOGV("RISM_reqInitSeatMove()");
    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
    error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_INIT_SEATMOVE;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RISM::RISM_reqCompleteSeatMove()
{
    LOGV("RISM_reqCompleteSeatMove()");
    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX)]();
    sp<vifCANContainer> buf = new vifCANContainer();
     error_t result = E_OK;

    values[0] = REMOTE_FEATURE_REQUEST;
    values[1] = TCUDATACONTROL_COMP_SEATMOVE;

    buf->setData(Signal_TCUDataControl_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_TCUDataControl_TX), values);

    result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);

    delete[] values;
    return result;
}

error_t RemoteService_RISM::RISM_reqSeatMove()
{
    uint8_t vehicleType = m_RemoteServiceApp.getVehicleType();
    switch(vehicleType)
    {
        case VEHICLETYPE_L462:
            return RISM_reqSeatMove_L462();
        case VEHICLETYPE_L405:
            return RISM_reqSeatMove_L405();
        default:
            LOGV("SKIP RISM_reqSeatMove : vehicleType %d", vehicleType);
            m_RemoteServiceApp.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
            return E_OK;
    }
}
error_t RemoteService_RISM::RISM_reqSeatMove_L462()
{
    LOGV("RISM_reqSeatMove()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(CAN_VALUES_MIN_BUF_SIZE)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    seatMovementCommand move = seatMovementCommand_none;
    error_t result = E_OK;

    if(m_RemoteServiceApp.getSeatMovementInhibition_2D() != seatMovementInhibition_inhibited)
    {
        move = m_RemoteServiceApp.getSeatMovementCommand_2D();
        if(move == seatMovementCommand_fold)
        {
            values[0] = RISM_REQ_FOLD;
            buf->setData(Signal_RemRDrvFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq2ndRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_2D(seatMovementCommand_fold);
            LOGV("Driver Seat 2nd Row Request fold");
        }
        if(move == seatMovementCommand_unfold)
        {
            values[0] = RISM_REQ_UNFOLD;
            buf->setData(Signal_RemRDrvFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq2ndRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_2D(seatMovementCommand_unfold);
            LOGV("Driver Seat 2nd Row Request Unfold");
        }
    }
    if(m_RemoteServiceApp.getSeatMovementInhibition_2P() != seatMovementInhibition_inhibited)
    {
        move = m_RemoteServiceApp.getSeatMovementCommand_2P();
        if(move == seatMovementCommand_fold)
        {
            values[0] = RISM_REQ_FOLD;
            buf->setData(Signal_RemRPasFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq2ndRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_2P(seatMovementCommand_fold);
            LOGV("Passenger Seat 2nd Row Request Fold");
        }
        if(move == seatMovementCommand_unfold)
        {
            values[0] = RISM_REQ_UNFOLD;
            buf->setData(Signal_RemRPasFoldReq2ndRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq2ndRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_2P(seatMovementCommand_unfold);
            LOGV("Passenger Seat 2nd Row Request Unfold");
        }
    }
    if(m_RemoteServiceApp.getSeatMovementInhibition_3D() != seatMovementInhibition_inhibited)
    {
        move = m_RemoteServiceApp.getSeatMovementCommand_3D();
        if(move == seatMovementCommand_fold)
        {
            values[0] = RISM_REQ_FOLD;
            buf->setData(Signal_RemRDrvFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq3rdRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_3D(seatMovementCommand_fold);
            LOGV("Driver Seat 3rd Row Request Fold");
        }
        if(move == seatMovementCommand_unfold)
        {
            values[0] = RISM_REQ_UNFOLD;
            buf->setData(Signal_RemRDrvFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRDrvFoldReq3rdRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_3D(seatMovementCommand_unfold);
            LOGV("Driver Seat 3rd Row Request Unfold");
        }
    }
    if(m_RemoteServiceApp.getSeatMovementInhibition_3P() != seatMovementInhibition_inhibited)
    {
        move = m_RemoteServiceApp.getSeatMovementCommand_3P();
        if(move == seatMovementCommand_fold)
        {
            values[0] = RISM_REQ_FOLD;
            buf->setData(Signal_RemRPasFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq3rdRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_3P(seatMovementCommand_fold);
            LOGV("Passenger Seat 3rd Row Request fold");
        }
        if(move == seatMovementCommand_unfold)
        {
            values[0] = RISM_REQ_UNFOLD;
            buf->setData(Signal_RemRPasFoldReq3rdRow_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_RemRPasFoldReq3rdRow_TX), values);
            m_RemoteServiceApp.setSeatMovementRequest_3P(seatMovementCommand_unfold);
            LOGV("Passenger Seat 3rd Row Request Unfold");
        }
    }

    LOGV("buf->NumofSignals:%d", buf->NumofSignals);
    if(buf->NumofSignals > 0)
    {
        m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_WAIT_RESPONSE);  //ACTIVE -> WAIT
        result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
        if(result == E_ERROR)
        {
            LOGE("[RS_RISM_MOVE] CAN signal Tx error. cause:%d", result);
            m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_ACTIVE);  //WAIT -> ACTIVE
            //Already activated seat module. thus, should send BCM authentication completion request.
            m_RemoteServiceApp.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
        }
    }
    else
    {
        LOGE("No request available seats!");
        result = E_ERROR;
        //Already activated seat module. thus, should send BCM authentication completion request.
        m_RemoteServiceApp.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
    }

    delete[] values;
    return result;
}
error_t RemoteService_RISM::RISM_reqSeatMove_L405()
{
    LOGV("RISM_reqSeatMove_L405()");

    uint8_t* values = new uint8_t[GET_CAN_SIGNAL_VALUE_BYTE_SIZE(CAN_VALUES_MIN_BUF_SIZE)]();
    sp<vifCANContainer> buf = new vifCANContainer();

    seatMovementCommand move = seatMovementCommand_none;
    error_t result = E_OK;

#if 0 //requirement SRD_TCU_S06_v1_10_011_RVC.pdf for PCR019
    switch(m_RemoteServiceApp.getSeatType())
    {
        case SEATTYPE_6040: //Seat type 1 2nd Row 60-40
            if(m_RemoteServiceApp.getSeatMovementInhibition_2L() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2L();
                if(move == seatMovementCommand_fold)
                {
                    values[0] = RISM_REQ_FOLD;
                    buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                    LOGI("send request left fold");
                }
                if(move == seatMovementCommand_unfold)
                {
                    values[0] = RISM_REQ_UNFOLD;
                    buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                    LOGI("send request left unfold");
                }
            }
            if(m_RemoteServiceApp.getSeatMovementInhibition_2R() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2R();
                if(move == seatMovementCommand_fold)
                {
                    values[0] = RISM_REQ_FOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right fold");
                }
                if(move == seatMovementCommand_unfold)
                {
                    values[0] = RISM_REQ_UNFOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right unfold");
                }
            }
            break;
        case SEATTYPE_4040: //Seat type 2 2nd Row 40-40
            if(m_RemoteServiceApp.getSeatMovementInhibition_2R() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2R();
                if(move == seatMovementCommand_fold)
                {
                    values[0] = RISM_REQ_FOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right fold");
                }
                if(move == seatMovementCommand_unfold)
                {
                    values[0] = RISM_REQ_UNFOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right unfold");
                }
            }
            break;
        case SEATTYPE_402040: //Seat type 2 2nd Row 40-20-40
            if(m_RemoteServiceApp.getSeatMovementInhibition_2L() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2L();
                if(move == seatMovementCommand_fold)
                {
                    values[0] = RISM_REQ_FOLD;
                    buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                    LOGI("send request left fold");
                }
                if(move == seatMovementCommand_unfold)
                {
                    values[0] = RISM_REQ_UNFOLD;
                    buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                    LOGI("send request left unfold");
                }
            }
            if(m_RemoteServiceApp.getSeatMovementInhibition_2R() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2R();
                if(move == seatMovementCommand_fold)
                {
                    values[0] = RISM_REQ_FOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right fold");
                }
                if(move == seatMovementCommand_unfold)
                {
                    values[0] = RISM_REQ_UNFOLD;
                    buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                    LOGI("send request right unfold");
                }
            }
            if(m_RemoteServiceApp.getSeatMovementInhibition_2C() != seatMovementInhibition_inhibited)
            {
                move = m_RemoteServiceApp.getSeatMovementCommand_2C();
                if(move == seatMovementCommand_deploy)
                {
                    values[0] = RISM_REQ_DEPLOY;
                    buf->setData(Signal_REM2ndRowCntrMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowCntrMoveReq_TX), values);
                    LOGI("send request arm rest deploy");
                }
                if(move == seatMovementCommand_stow)
                {
                    values[0] = RISM_REQ_STOW;
                    buf->setData(Signal_REM2ndRowCntrMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowCntrMoveReq_TX), values);
                    LOGI("send request arm rest stow");
                }
            }
            move = m_RemoteServiceApp.getSeatMovementCommand_skiHatch();
            if(move == seatMovementCommand_drop)
            {
                if(m_RemoteServiceApp.getSeatMovementStatus_2C() == seatMovementStatus_fullyReclined)
                { //SRD_ReqRISM0444_V1
                    values[0] = RISM_REQ_DROP;
                    buf->setData(Signal_REMSkiHatchReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REMSkiHatchReq_TX), values);
                    LOGI("send request ski hatch drop");
                }
                else
                {
                    LOGI("skip ski hatch drop request.. arm rest position(%d) is not fullyReclined", m_RemoteServiceApp.getSeatMovementStatus_2C());
                }
            }
            break;
        default:
            break;
    }
#else //requirement SRD_TCU_S06_v1_10_012_RVC.pdf for PCR019b
    if(m_RemoteServiceApp.isSeatType3() == true)
    {
        if(m_RemoteServiceApp.getSeatMovementInhibition_2L() != seatMovementInhibition_inhibited)
        {
            move = m_RemoteServiceApp.getSeatMovementCommand_2L();
            if(move == seatMovementCommand_fold)
            {
                values[0] = RISM_REQ_FOLD;
                buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2L(seatMovementCommand_fold);
                LOGI("send request left fold");
            }
            if(move == seatMovementCommand_unfold)
            {
                values[0] = RISM_REQ_UNFOLD;
                buf->setData(Signal_REM2ndRowLeftMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowLeftMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2L(seatMovementCommand_unfold);
                LOGI("send request left unfold");
            }
        }
        if(m_RemoteServiceApp.getSeatMovementInhibition_2R() != seatMovementInhibition_inhibited)
        {
            move = m_RemoteServiceApp.getSeatMovementCommand_2R();
            if(move == seatMovementCommand_fold)
            {
                values[0] = RISM_REQ_FOLD;
                buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2R(seatMovementCommand_fold);
                LOGI("send request right fold");
            }
            if(move == seatMovementCommand_unfold)
            {
                values[0] = RISM_REQ_UNFOLD;
                buf->setData(Signal_REM2ndRowRightMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowRightMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2R(seatMovementCommand_unfold);
                LOGI("send request right unfold");
            }
        }
        if(m_RemoteServiceApp.getSeatMovementInhibition_2C() != seatMovementInhibition_inhibited)
        {
            move = m_RemoteServiceApp.getSeatMovementCommand_2C();
            if(move == seatMovementCommand_deploy)
            {
                values[0] = RISM_REQ_DEPLOY;
                buf->setData(Signal_REM2ndRowCntrMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowCntrMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2C(seatMovementCommand_fold);
                LOGI("send request arm rest deploy");
            }
            if(move == seatMovementCommand_stow)
            {
                values[0] = RISM_REQ_STOW;
                buf->setData(Signal_REM2ndRowCntrMoveReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REM2ndRowCntrMoveReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_2C(seatMovementCommand_unfold);
                LOGI("send request arm rest stow");
            }
        }

        if(m_RemoteServiceApp.getSeatMovementInhibition_2C() == seatMovementInhibition_available)
        {
            /* SRD_ReqRISM0444_V1 at version 1.10.012
            TCU shall await a successful execution of initialize remote seat movement for
            TCU_SEATS_READINESS_TIMER. The TCU shall then request execution of
            ski-hatch drop to RRSM by using REMSkiHatchReq signal. TCU will verify
            whether the value of CAN signal RMSeatInhib2ndRow is set to 0 and also setting
            the value of CAN signal REMSkiHatchReq to 1 indicating a ski-hatch drop
            request.
            Note :RMSeatInhib2ndRow = 0 indicates power armrest is available for SkiHatch request
            */
            move = m_RemoteServiceApp.getSeatMovementCommand_skiHatch();
            if(move == seatMovementCommand_drop)
            {
                values[0] = RISM_REQ_DROP;
                buf->setData(Signal_REMSkiHatchReq_TX, GET_CAN_SIGNAL_VALUE_BYTE_SIZE(Signal_REMSkiHatchReq_TX), values);
                m_RemoteServiceApp.setSeatMovementRequest_skiHatch(seatMovementCommand_drop);
                LOGI("send request ski hatch drop");
            }
        }
    }
    else
    {
        LOGE("IntelligentSeatFold %d is not available for RISM", m_RemoteServiceApp.getIntelligentSeatFold());
    }
#endif
    LOGV("buf->NumofSignals:%d", buf->NumofSignals);
    if(buf->NumofSignals > 0)
    {
        m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_WAIT_RESPONSE);  //ACTIVE -> WAIT
        result = m_RemoteServiceApp.RemoteService_reqSendCANdata(buf->NumofSignals, buf);
        if(result == E_ERROR)
        {
            LOGE("[RS_RISM_MOVE] CAN signal Tx error. cause:%d", result);
            m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_ACTIVE);  //WAIT -> ACTIVE
            //Already activated seat module. thus, should send BCM authentication completion request.
            m_RemoteServiceApp.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
        }
    }
    else
    {
        LOGE("No request available seats!");
        result = E_ERROR;
        //Already activated seat module. thus, should send BCM authentication completion request.
        m_RemoteServiceApp.RemoteService_requestHandler(RS_RISM, RS_SEAT_MOVE_COMPLETE);
    }

    delete[] values;
    return result;
}

error_t RemoteService_RISM::RISM_delayForReqBCMauth(RS_ServiceType serviceType)
{
    error_t result = E_ERROR;
    int32_t count = BCM_RESP_WAIT_MAX_TIME;

    while(count > 0)
    {
        if(m_RemoteServiceApp.getCurrBCMauthService() == RS_SERVICE_TYPE_MAX)
        {
            LOGV("RISM (serviceType:%d) TCU-BCM authentication request available.", serviceType);
            result = E_OK;
            break;
        }
        else
        {
            LOGV("wait 1 second for RISM (serviceType:%d) TCU-BCM authentication request.", serviceType);
            usleep(BCM_RESP_WAIT_TIME);
            count -= BCM_RESP_WAIT_TIME;
        }
    }

    LOGV("RISM_delayForReqBCMauth() serviceType:%d result:0x%x delayedCount:%d(microSeconds)", serviceType, result, (BCM_RESP_WAIT_MAX_TIME - count));

    if(result == E_ERROR)
    {
        m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, executionFailure_chosen); // JLRTCU-2366
        m_RemoteServiceApp.sendNackMsgToTSP_RISM(RS_RISM_MOVE, executionFailure_chosen);
        m_RemoteServiceApp.setCurrState(RS_RISM_MOVE, RS_READY);  //ACTIVE->READY
    }

    return result;
}

bool RemoteService_RISM::RemoteService_PrereqCheck(RS_ServiceType remoteServiceType)
{
    LOGV("RemoteService_PrereqCheck() service type = %d", remoteServiceType);

    bool result = true;
    bool crashStatus = false;
    int32_t  powerMode = m_RemoteServiceApp.RemoteService_getPowerMode();
    bool remoteStartStatus = m_RemoteServiceApp.RemoteService_getRemoteStartStatus();

    char* prop = m_RemoteServiceApp.getProperty(STR_CURR_CRASH);
    if(prop != NULL)
    {
        crashStatus = ((std::strcmp(prop, STR_FALSE)) ? TRUE : FALSE);
    }

    if(m_RemoteServiceApp.isRISMFitted() == false)
    {
        //m_RemoteServiceApp.setNGTPerrorCode(vehicleNotLocked_chosen); // RISM Acknowledge error code is not list type.
        LOGI("convenienceFold not fitment.");
        return false;
    }

    if((powerMode > PM_ACCESSORY_1) && (remoteStartStatus == false))
    {
        result = false;
        //m_RemoteServiceApp.setNGTPerrorCode(vehiclePowerModeNotCorrect_chosen); // RISM Acknowledge error code is not list type.
        m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, vehiclePowerModeNotCorrect_chosen); // JLRTCU-2366
        LOGV("[RISM Prerequisite] Power mode not correct");
        return result; // JLRTCU-2422
    }

    if(m_RemoteServiceApp.getDoorStatus() != ALL_DOOR_CLOSED) // JLRTCU-2422, JLRTCU-2423 any of doors or tail gate, engine hood is open
    {
        result = false;
       // m_RemoteServiceApp.setNGTPerrorCode(vehicleNotLocked_chosen); // RISM Acknowledge error code is not list type.
       m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, vehicleNotLocked_chosen); // JLRTCU-2422
       LOGV("[RISM Prerequisite] Door open status(0x%x) not correct", m_RemoteServiceApp.getDoorStatus());
       return result; // JLRTCU-2422
    }

    if(!m_RemoteServiceApp.getAllDoorLockStatus())
    {
        result = false;
       // m_RemoteServiceApp.setNGTPerrorCode(vehicleNotLocked_chosen); // RISM Acknowledge error code is not list type.
       m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, vehicleNotLocked_chosen); // JLRTCU-2422
       LOGV("[RISM Prerequisite] Door Lock status not correct");
       return result; // JLRTCU-2422
    }

    if(m_RemoteServiceApp.getCurrAlarmStatus() == ALARM_ACTIVE)
    {
        result = false;
        //m_RemoteServiceApp.setNGTPerrorCode(alarmActive_chosen); // RISM Acknowledge error code is not list type.
        m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, alarmActive_chosen); // JLRTCU-2422
        LOGV("[RISM Prerequisite] Alarm status active");
        return result; // JLRTCU-2422
    }

    if(crashStatus)
    {
        result = false;
        //m_RemoteServiceApp.setNGTPerrorCode(crashEvent_chosen); // RISM Acknowledge error code is not list type.
        m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, crashEvent_chosen); // JLRTCU-2422
        LOGV("[RISM Prerequisite] Crash status is true");
        return result; // JLRTCU-2422
    }

    if(m_RemoteServiceApp.getSVTactive())
    {
        result = false;
        //m_RemoteServiceApp.setNGTPerrorCode(theftEvent_chosen); // RISM Acknowledge error code is not list type.
        m_RemoteServiceApp.sendAckMsgToTSP_RISM(RS_RISM_MOVE, svc::acknowledge, false, theftEvent_chosen); // JLRTCU-2422
        LOGV("[RISM Prerequisite] theft event is active");
        return result; // JLRTCU-2422
    }

    LOGV("[RISM Prerequisite] all checked result:%s", (result ? STR_TRUE : STR_FALSE));
    return result;
}
