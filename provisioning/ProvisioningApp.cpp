/**
* \file    ProvisioningApp.cpp
* \brief     Implementation of ProvisioningApp
*
* \details
*    This software is copyright protected and proprietary to
*    LG electronics. LGE grants to you only those rights as
*    set out in the license conditions. All other rights remain
*    with LG electronics.
* \author       wonsik93.park
* \date    2015.09.09
* \attention Copyright (c) 2015 by LG electronics co, Ltd. All rights reserved.
*/

#define LOG_TAG "ProvisioningApp"
#include "Log.h"

#include <stdlib.h>
#include "ProvisioningApp.h"
#include <binder/IServiceManager.h>


android::sp<Application> gApp;

ProvisioningApp::ProvisioningApp() : mTelephonyListener(NULL)
    ,m_ngtpReceiver(NULL)
    ,m_ngtpManager(NULL)
    ,mKeepPowerCnt(0)
    ,mbCallTrigger(false)
    ,mTsimLock(NULL)
    ,m_bCallActive(false)
{
    LOGV("ProvisioningApp()");
    mCurrentState = STATE_INIT;
    mCreationTime_seconds = 0;
    mCreationTime_millis = 0;
    mNGTPDecode = new NGTPDecodeMsg();

    m_myTimer = new MyTimer(*this);
    m_FTUpdateTimer = new Timer( m_myTimer , MyTimer::TIMER_FTUPDATE );
    m_FTUpdateTimer->setDuration(TIMEOUT_FTUPDATE,0);
    m_ConfigTimer = new Timer( m_myTimer , MyTimer::TIMER_CONFIG );
    m_ConfigTimer->setDuration(TIMEOUT_CONFIG,0);
    m_TuStsTimer = new Timer( m_myTimer , MyTimer::TIMER_TUSTS );
    m_TuStsTimer->setDurationMs(TIMEOUT_TUSTS, 0);
    m_RetryTimer = new Timer( m_myTimer , MyTimer::TIMER_RETRY );
    m_RetryTimer->setDuration(TIMEOUT_RETRY, 0);
    m_DuplicateTimer = new Timer( m_myTimer , MyTimer::TIMER_DUPLICATE );
    m_DuplicateTimer->setDurationMs(TIMEOUT_DUPLICATE, 0);

    diagRoutineID = 0;
}

ProvisioningApp::~ProvisioningApp()
{
    LOGV("~ProvisioningApp()");
}

void ProvisioningApp::onCreate()
{
    LOGV("ProvisioningApp onCreate()");

    mLooper = SLLooper::myLooper();
    mHandler = new MyHandler(mLooper, *this);
    if(m_ngtpReceiver == NULL)
        m_ngtpReceiver = new NgtpReceiver(*this);
    m_powerModeReceiver = new PowerModeReceiver(*this);
    m_diagManagerReceiver = new DiagManagerReceiver(*this);
    m_powerManager = interface_cast<IPowerManagerService>(defaultServiceManager()->getService(String16(PowerManagerService::getServiceName())));
    mHMIMgr = interface_cast<IHMIManagerService>(defaultServiceManager()->getService(String16(HMIManagerService::getServiceName())));
    m_diagManager = interface_cast<IDiagManagerService>(defaultServiceManager()->getService(String16(DiagManagerService::getServiceName())));
    m_configManager = interface_cast<IConfigurationManagerService>(defaultServiceManager()->getService(String16(ConfigurationManagerService::getServiceName())));
    m_AppMgr = interface_cast<IApplicationManagerService>(defaultServiceManager()->getService(String16(APP_MGR_SERVICE)));

    if(m_ngtpManager == NULL)
    {
        m_ngtpManager = interface_cast<INGTPManagerService>(defaultServiceManager()->getService(String16(NGTPManagerService::getServiceName())));
        m_ngtpManager->registerReceiver(m_ngtpReceiver, SERVICETYPE_PROVISIONING);   // provisioning
        m_ngtpManager->registerReceiver(m_ngtpReceiver, SERVICETYPE_FACTORY_TEST);   // factory test
    }

    m_powerManager->registerPowerModeReceiver(m_powerModeReceiver);
    m_diagManager->attachReceiver(getAppID(), m_diagManagerReceiver);

    int32_t events = 0;
    events = PhoneStateListener::LISTEN_PSIM_STATE |
        PhoneStateListener::LISTEN_ACTIVE_SIM_STATE;

    if(mTelephonyListener == NULL) {
        mTelephonyListener = new TelephonyListener(*this);
    }
    TelephonyManager::listen(mTelephonyListener, events, false);

    if(mTsimLock == NULL) {
        mTsimLock = new TelephonyManager::TsimLock();
    }
    //setCreationTime(0,0);
    mConfigRequest = new ConfigRequestMsg(m_ngtpManager, m_configManager);
    mConfigDataAck = new ConfigDataAckMsg(m_ngtpManager, m_configManager);
    mStatusUpdateAck = new StatusUpdateAckMsg(m_ngtpManager, m_configManager);
    mFTUpdate = new FTUpdateMsg(m_ngtpManager, m_configManager);

    m_bCallButtonTimer = new Timer( m_myTimer , MyTimer::TIMER_BCALLBUTTON );
    m_bCallButtonTimer->setDurationMs(getBCallButtonLedTimeout(), 0);

    mDiagResponse.add(DIAG_ROUTINE_ID_7, DIAG_NOTSTARTED);
    mDiagResponse.add(DIAG_ROUTINE_ID_10, DIAG_NOTSTARTED);
    asn1_set_error_handling(asn1_legacy_error_handler);

    connectToVifMgr();
    for(int i=0; i<18; i++){
        vin_data[i] = 0;
    }
    for(int j=0; j<3; j++){
        vin_flag[j] = 0;
    }
}

void ProvisioningApp::onDestroy(){
    LOGV("ProvisioningApp onDestroy()");
    //unregister NGTP Receiver
    m_ngtpManager->unregisterReceiver(m_ngtpReceiver, SERVICETYPE_PROVISIONING);
    m_ngtpManager->unregisterReceiver(m_ngtpReceiver, SERVICETYPE_FACTORY_TEST);
}

void ProvisioningApp::onHMIReceived(const uint32_t type, const uint32_t action)
{
    LOGV("onHMIReceived type:0x%02x, action:0x%02x", type, action);
    sp<sl::Message> message;
    message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_HMI, type, action);
    message->sendToTarget();
}

void ProvisioningApp::onDiagReceived(int32_t id, uint8_t type, uint8_t cmd1, uint8_t cmd2, sp<Buffer>& buf)
{
    LOGV("onDiagReceived cmd1:0x%02x, cmd2:0x%02x", cmd1, cmd2);
    sp<sl::Message> message;

    if(type == PACKET_TYPE_REQUEST)
    {
        message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_DIAG, cmd1, cmd2, id);

        if(buf->size() > 0)
        {
            message->buffer.setTo(buf->data(), buf->size());
        }
        message->sendToTarget();
    }
}

void ProvisioningApp::onVifReceive(uint16_t Sigid, sp<Buffer>& buf)
{
    sp<sl::Message> message = mHandler->obtainMessage(MyHandler::RECV_MSG_FROM_VIF, Sigid);

    if( buf->size() > 0 )
    {
            message->buffer.setTo(buf->data(), buf->size());
    }
    message->sendToTarget();
}

void ProvisioningApp::onNotifyFromTSP(InternalNGTPData *data, uint32_t size)
{
    if(data != NULL)
    {
        sp<sl::Message> messsage = mHandler->obtainMessage(
                MyHandler::RECV_MSG_FROM_TSP, data->serviceType, data->creationTime_seconds, data->encodedSize);

        messsage->buffer.setTo((uint8_t*)data, sizeof(InternalNGTPData));
        messsage->sendToTarget();
    }
    else {
        LOGE("FATAL:NGTP Data is NULL");
    }
}

void ProvisioningApp::sendConfigRequestMsg1ToTSP()
{
    LOGV("send ConfigRequestMsg1 to TSP");

    mNGTPDecode->initializeConfigMgrGenericParameter();
    mNGTPDecode->initializeConfigAckGenericParameter();
    mConfigRequest->makeMessage(&mTuStsParameters);

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = SERVICETYPE_PROVISIONING;
    internalNGTPData.dsptAckRequired = false;
    internalNGTPData.statelessNGTPmessage = false;
    internalNGTPData.creationTime_seconds = getCreationTimeSecond();
    internalNGTPData.creationTime_millis = getCreationTimeMilli();
    internalNGTPData.serialVID = false;
    uint32_t timeID = mConfigRequest->encode_sendToTSDP(false, &internalNGTPData);
    if(timeID == 0)
        LOGE("NGTP encoding FAIL");
}

void ProvisioningApp::sendConfigDataAckMsg1ToTSP(svc::Acknowledge ack)
{
    mConfigDataAck->makeMessage(mNGTPDecode->getGenericParameter(), ack);

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = SERVICETYPE_PROVISIONING;
    internalNGTPData.dsptAckRequired = false;
    internalNGTPData.statelessNGTPmessage = true;
    internalNGTPData.creationTime_seconds = getCreationTimeSecond();
    internalNGTPData.creationTime_millis = getCreationTimeMilli();
    internalNGTPData.serialVID = false;

    mConfigDataAck->encode_sendToTSDP(true, &internalNGTPData);
    mNGTPDecode->initializeConfigAckGenericParameter();
    stopTimer(MyTimer::TIMER_CONFIG);
    startTimer(MyTimer::TIMER_CONFIG);
    stopTimer(MyTimer::TIMER_RETRY);
}

void ProvisioningApp::sendStatusUpdateAckMsgToTSP(svc::Acknowledge ack)
{
    mStatusUpdateAck->makeMessage(&mTuStsParameters, ack);

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = SERVICETYPE_PROVISIONING;
    internalNGTPData.dsptAckRequired = false;
    internalNGTPData.statelessNGTPmessage = true;
    internalNGTPData.creationTime_seconds = getCreationTimeSecond();
    internalNGTPData.creationTime_millis = getCreationTimeMilli();
    internalNGTPData.serialVID = false;

    mStatusUpdateAck->encode_sendToTSDP(true, &internalNGTPData);
    stopTimer(MyTimer::TIMER_CONFIG);
}

void ProvisioningApp::sendFTUpdateMsgToTSP()
{
    LOGV("send FTUpdateMsg to TSP");
    setVersionParameters();
    mFTUpdate->makeMessage(&mTuStsParameters, mVinParameters);

    InternalNGTPData internalNGTPData;
    internalNGTPData.serviceType = SERVICETYPE_FACTORY_TEST;
    internalNGTPData.dsptAckRequired = false;
    internalNGTPData.statelessNGTPmessage = false;
    internalNGTPData.creationTime_seconds = getCreationTimeSecond();
    internalNGTPData.creationTime_millis = getCreationTimeMilli();
    internalNGTPData.serialVID = true;

    uint32_t timeID = mFTUpdate->encode_sendToTSDP(false, &internalNGTPData);
    if(timeID == 0)
        LOGE("NGTP encoding FAIL");
}

void ProvisioningApp::MyHandler::handleMessage(const sp<sl::Message>& msg)
{
    const int32_t what = msg->what;

    //LOGV("MyHandler hanleMessage: %d", what);
    sp<Buffer> buf = new Buffer();
    buf->setTo(msg->buffer.data(), msg->buffer.size());

    switch(what)
    {
        case RECV_MSG_FROM_MCU:
        {
            break;
        }
        case RECV_MSG_FROM_TSP:
        {
            InternalNGTPData *ngtp = (InternalNGTPData*)msg->buffer.data();
            mApplication.TSPMsgHandler(ngtp);
            break;
        }
        case RECV_MSG_FROM_HMI:
        {
            mApplication.HMIMsgHandler(msg->arg1, msg->arg2);
            break;
        }
        case RECV_MSG_FROM_DIAG:
        {
            LOGV("DIAG RECEIVED # id : 0x%x,  cmd1 : 0x%x, cmd2 : 0x%x",
                msg->arg3, msg->arg1, msg->arg2);
            mApplication.processDiagCommand(msg->arg1, msg->arg2, msg->arg3, buf);
            break;
        }
        case RECV_MSG_FROM_VIF:
        {
            mApplication.VIFMsgHandler(msg->arg1, buf);
            break;
        }
        case RECV_MSG_ACTIVESIM_STATE_CHANGED:
            mApplication.processActiveSimChanged(msg->arg1);
            break;
        default:
            break;
    }
}

void ProvisioningApp::HMIMsgHandler(uint8_t arg1, uint8_t arg2)
{
    if(arg1 == HMI_TYPE_CONFIGURE && arg2 == HMI_BUTTON_ACTION_TRIGER) {
        if(getState() == STATE_INIT) {
            send_bCallButton_ack();
            if(!isCallActive()) {
                // start TU request config
                LOGV("bCall config button triggered");
                enableSpyMode(false);
                setbCallTriggered(true);
                changeActSim_forConfig();
                sendTSIMlockStatus(true);
            }
            else {
                LOGV("bCall config button triggered, but call is active, discard");
                bCall_active_light_off();
                bCall_backlight_on();
            }
        }
        else
            LOGV("Not Correct State: Discard button event");
    }
}

void ProvisioningApp::VIFMsgHandler(uint8_t cmd, sp<Buffer> buf)
{
    if(cmd == Signal_VehicleInfoParametersPMZ_RX)
    {
         uint8_t* pdata = buf->data();
         //int32_t length = msg->buffer.size();
         uint8_t first_data = pdata[0];
         if(first_data == 16 && vin_flag[0] == 0){
             for(int i=0; i<3; i++){
                 vin_data[i]=pdata[5+i];
             }
             vin_flag[0]=1;
         }else if(first_data == 17 && vin_flag[1] == 0){
             for(int i=0; i<7; i++){
                 vin_data[3+i]=pdata[1+i];
             }
             vin_flag[1]=1;
         }else if(first_data == 18 && vin_flag[2] == 0){
             for(int i=0; i<7; i++){
                 vin_data[10+i]=pdata[1+i];
             }
             vin_flag[2]=1;
         }
         else{
             //Nothing to do
         }

         if(vin_flag[0]==1 && vin_flag[1] == 1 && vin_flag[2] == 1){
             vin_data[17] = (uint8_t)'\0';
             //LOGV("I get vin number %s", vin_data);
             for(int i=0; i<3; i++){
                 vin_flag[i] = 0;
             }
         }
    }
}

bool ProvisioningApp::isDuplicateMsg(uint8_t *lastData, uint8_t *receivedData, uint32_t size)
{
    uint32_t remainingTime = m_DuplicateTimer->getRemainingTimeMs();
    bool ret;
    if(!memcmp(lastData, receivedData, SERVICE_DATA_MAXBUFLEN) &&
        (remainingTime != 0 && remainingTime > TIMEOUT_DUPLICATE - TIMEOUT_DUPLICATE_CHECK)) // this is the duplicate message from TSP
        ret = true;
    else
        ret = false;

    memset(lastData, 0x00, SERVICE_DATA_MAXBUFLEN);
    memcpy(lastData, receivedData, size);
    stopTimer(MyTimer::TIMER_DUPLICATE);

    return ret;
}

void ProvisioningApp::TSPMsgHandler(InternalNGTPData *ngtp)
{
    LOGV("serviceType = 0x%x, time = 0x%x, encodedsize = %d, messagetype = %d",
                ngtp->serviceType, ngtp->creationTime_seconds, ngtp->encodedSize, ngtp->messageType);
    KeepPower::LocalKeepPower p(mKeepPower);
    if(ngtp->serviceType == SERVICETYPE_PROVISIONING) // configuration
    {
        if(ngtp->messageType == SEND_FAIL || ngtp->messageType == HTTP_SEND_FAIL) {
            LOGV("err: config message send fail");
        }
        else if((ngtp->messageType == UDP_SEND_SUCCESS || ngtp->messageType == HTTP_SEND_SUCCESS)
            && getState() == STATE_SEND_CONFIGDATA)
            startTimer(MyTimer::TIMER_RETRY);
        else if(ngtp->messageType == dspt::downServiceData)
        {
            bool duplicate = isDuplicateMsg(mLastServiceData, ngtp->encodedSvcData, ngtp->encodedSize);
            if(duplicate)
                LOGV("duplicate msg received... discard this message");
            else
            {
                decodingNGTPMessage(ngtp->encodedSvcData, ngtp->encodedSize, ngtp->creationTime_seconds, ngtp->creationTime_millis, ngtp->dsptAckRequired);
                if(getState() == STATE_RECEIVE_CONFIGDATA && !m_bCallActive)
                {
                    LOGV("sending ConfigDataACK");
                    startTimer(MyTimer::TIMER_DUPLICATE);
                    //sendDSPTackMsgToTSP();
                    //usleep(10000);
                    setCreationTime(ngtp->creationTime_seconds, ngtp->creationTime_millis);
                    if(mTuStsParameters.activation==svc::deactivated)
                        sendConfigDataAckMsg1ToTSP(svc::nack);
                    else
                        sendConfigDataAckMsg1ToTSP(svc::ack);

                    setCurrentState(STATE_SEND_CONFIGDATA_ACK);
                }
                else if(getState() == STATE_RECEIVE_STATUS_UPDATE && !m_bCallActive)
                {
                    LOGV("sending StatusUpdateACK");
                    //sendDSPTackMsgToTSP();
                    //usleep(10000);
                    setCreationTime(ngtp->creationTime_seconds, ngtp->creationTime_millis);
                    if(mTuStsParameters.activation==svc::deactivated)
                        sendStatusUpdateAckMsgToTSP(svc::nack);
                    else {
                        notifyProvisioningComplete();
                        sendStatusUpdateAckMsgToTSP(svc::ack);
                    }
                    setCurrentState(STATE_INIT);
                    TSimLockRelease();
                    sendTSIMlockStatus(false);
                    memset(mLastServiceData, 0x00, SERVICE_DATA_MAXBUFLEN);
                    if(getbCallTriggered())
                    {
                        bCall_active_light_solid();
                        startTimer(MyTimer::TIMER_BCALLBUTTON);
                    }
                    if(mTuStsParameters.activation==svc::deactivated) {
                        LOGE("deactivated mode, do not save config");
                    }
                    else {
                        if(saveGenParamToConfigMgr() == E_ERROR)
                            setDiagResponse(DIAG_ROUTINE_ID_10, PDI_ABORTED);
                        if(m_configManager->save_countryDB() == E_ERROR) {
                            LOGE("save countryDB ERROR");
                            setDiagResponse(DIAG_ROUTINE_ID_10, PDI_ABORTED);
                        }
                        else
                            LOGV("save countryDB SUCCESS");
                        if(m_configManager->factory_save("/userdata/config/after_pdi2.xml") == E_ERROR)
                            LOGE("factory_save ERROR");
                        else
                            LOGV("factory_save SUCCESS");
                    }
                    sendDiagResponse();
                    enableSpyMode(true);
                }
                else
                    LOGV("invalid msg");
            }
        }
        else {}
    }
    else if(ngtp->serviceType == SERVICETYPE_FACTORY_TEST) // factory test decoding
    {
        if(getState() == STATE_SEND_FTUPDATE)
        {
            if(ngtp->messageType == SEND_FAIL)
            {
                LOGV("Factory Test Send Fail ...");
                setDiagResponse(DIAG_ROUTINE_ID_7, FTUPDATE_FAILED_TO_SEND);
            }
            else if(ngtp->messageType == UDP_SEND_SUCCESS)
                startTimer(MyTimer::TIMER_RETRY);
            else if(ngtp->messageType == dspt::downServiceData)
            {
                decodingNGTPMessage(ngtp->encodedSvcData, ngtp->encodedSize, ngtp->creationTime_seconds, ngtp->creationTime_millis, ngtp->dsptAckRequired);
                if(mNGTPDecode->getMessageType() == svc::acknowledge) // Factory test ack received
                {
                    if(mNGTPDecode->getAckowledge()) // ACK
                    {
                        setDiagResponse(DIAG_ROUTINE_ID_7, FTUPDATE_OK);
                        LOGV("Factory Test Done send EquipmentID to configMgr equipmentID = %u", mNGTPDecode->getEquipmentID());

                        //temporal code by hue.kim 20151026
                        char equiID[11] = {0, };
                        sprintf(equiID,"%u", mNGTPDecode->getEquipmentID());

                        sp<ConfigurationBuffer> pBuffer= new ConfigurationBuffer;
                        pBuffer->setTo((uint8_t*)equiID);
                        error_t err = m_configManager->change_set(CONFIG_XML_PATH, 2, "EquipmentId", pBuffer);
                        if(err == E_ERROR)
                            LOGE("cannot update EquipmentId");
                        else {
                            m_ngtpManager->factoryTestDone();
                            sp<IUpdateManagerService> updateManager = interface_cast<IUpdateManagerService>(defaultServiceManager()->getService(String16("service_layer.UpdateManagerService")));
                            updateManager->commTestDone();
                        }
                    }
                    else                             // NACK
                    {
                        LOGV("Factory Test NACK from server");
                        setDiagResponse(DIAG_ROUTINE_ID_7, FTUPDATE_NACK_FROM_SERVER);
                    }
                    stopTimer(MyTimer::TIMER_FTUPDATE);
                    stopTimer(MyTimer::TIMER_RETRY);
                    setCurrentState(STATE_INIT);
                    sendDiagResponse();

                    TSimLockRelease();
                    sendTSIMlockStatus(false);
                }
            }
            else {}
        }
    }
    else
        LOGV("invalid serviceType");
}

void ProvisioningApp::startTimer(uint8_t timer_id)
{
    switch(timer_id)
    {
        case MyTimer::TIMER_CONFIG:
            m_ConfigTimer->start();
            break;
        case MyTimer::TIMER_BCALLBUTTON:
            m_bCallButtonTimer->start();
            break;
        case MyTimer::TIMER_FTUPDATE:
            m_FTUpdateTimer->start();
            break;
        case MyTimer::TIMER_RETRY:
            m_RetryTimer->start();
            break;
        case MyTimer::TIMER_TUSTS:
            m_TuStsTimer->start();
            break;
        case MyTimer::TIMER_DUPLICATE:
            m_DuplicateTimer->start();
            break;
        default:
            break;
    }
}

void ProvisioningApp::stopTimer(uint8_t timer_id)
{
    switch(timer_id)
    {
        case MyTimer::TIMER_CONFIG:
            m_ConfigTimer->stop();
            break;
        case MyTimer::TIMER_BCALLBUTTON:
            m_bCallButtonTimer->stop();
            break;
        case MyTimer::TIMER_FTUPDATE:
            m_FTUpdateTimer->stop();
            break;
        case MyTimer::TIMER_RETRY:
            if(mKeepPowerCnt > 0)
                releaseKeepPower();
            m_RetryTimer->stop();
            break;
        case MyTimer::TIMER_TUSTS:
            m_TuStsTimer->stop();
            break;
        case MyTimer::TIMER_DUPLICATE:
            m_DuplicateTimer->stop();
            break;
        default:
            break;
    }
}

void ProvisioningApp::decodingNGTPMessage(uint8_t *buffer, uint32_t size, uint32_t seconds, uint32_t millis, int32_t dsptAckRequired)
{
    LOGV("DECODE START");
    bool ret = mNGTPDecode->decodeMessage(buffer, size);
    if(ret == false) {
        setCurrentState(STATE_INIT);
        LOGE("DECODING FAILED!!");
    }
    else {
        if(mNGTPDecode->getMessageType() == svc::startRequest) {
            // TSP sends Config Update, TSPRequestMessage is received, start TU request Config
            if(getState() == STATE_INIT) {
                if(!isCallActive()) {
                    startTimer(MyTimer::TIMER_DUPLICATE);
                    LOGV("TSP request message is received, start TU requst config");
                    setbCallTriggered(false);
                    //sendDSPTackMsgToTSP(); // DdptAck for startRequest

                    int32_t activeSim = TelephonyManager::getActiveSim();
                    LOGV("activeSim : %d, tsim lock state = %d", activeSim, TelephonyManager::getTsimLockState());
                    setCreationTime(seconds, millis);
                    switch(activeSim) {
                        case TelephonyManager::REMOTE_SIM_TSIM: // current SIM is TSIM.
                            TSimLockAcquire();
                            trigger_configUpdate_byTSP();
                            sendTSIMlockStatus(true);
                            break;
                        case TelephonyManager::REMOTE_SIM_PSIM:
                            setCurrentState(STATE_SEND_CONFIGDATA);
                            setUpTuStsData();
                            startTimer(MyTimer::TIMER_CONFIG);
                            break;
                        default:
                            break;
                    }
                }
                else
                    LOGV("TSP request message is received, but call is active, discard");
            }
            else
                LOGV("Not Correct State: Discard startRequest");

        }
        else
            processProvisioningMessage();
    }
    LOGV("DECODE END");
}

void ProvisioningApp::processProvisioningMessage()
{
    if(getState() == STATE_SEND_CONFIGDATA || getState() == STATE_SEND_CONFIGDATA_ACK) {
        if((mNGTPDecode->getIsGenericParams() || mNGTPDecode->getIsConfiguration())
            && !mNGTPDecode->getIsRemoteOperations()) { // this is Config Data Message
            setCurrentState(STATE_RECEIVE_CONFIGDATA);
        }
        else if(mNGTPDecode->getIsRemoteOperations()) { // this is Status Update Message
            setCurrentState(STATE_RECEIVE_STATUS_UPDATE);
        }
        else
            LOGV("Not Correct State: Discard message");
    }
}

/*
void ProvisioningApp::sendDSPTackMsgToTSP()
{
    LOGV("sendDSPTackMsgToTSP()");

    //InternalNGTPData
    InternalNGTPData internalNGTPData;

    internalNGTPData.serviceType = SERVICETYPE_PROVISIONING;
    internalNGTPData.messageType = dspt::dsptAck;
    internalNGTPData.encodedSize  = 0;
    internalNGTPData.dsptAckRequired = false;
    internalNGTPData.statelessNGTPmessage = false;
    internalNGTPData.creationTime_seconds = getCreationTimeSecond();
    internalNGTPData.creationTime_millis = getCreationTimeMilli();
    internalNGTPData.transport = 0; // UDP
    internalNGTPData.serialVID = false;

    uint8_t retryNum;
    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPMaxRetryForAck");
    if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
    {
        buf->data(&retryNum);
        internalNGTPData.retryNum = retryNum;
    }
    else
        internalNGTPData.retryNum = NGTP_ACK_RETRY_CNT;

    uint16_t retryDelay;
    buf = m_configManager->get(CONFIG_XML_PATH, 2, "NGTPUDPDelayForAckMs");
    if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
    {
        buf->data(&retryDelay);
        internalNGTPData.retryDelay = retryDelay;
    }
    else
        internalNGTPData.retryDelay = NGTP_ACK_RETRY_DELAY;

    LOGV("send internalNGTPData");
    m_ngtpManager->sendToTSDP(&internalNGTPData, sizeof(InternalNGTPData));
    LOGV("END sendDSPTackMsgToTSP");
}
*/
void ProvisioningApp::makeEventCreationTime()
{
    time_t timer;
    struct tm y2k = {0};
    uint32_t seconds;

    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

    time(&timer);  /* get current time; same as: timer = time(NULL)  */

    seconds = difftime(timer,mktime(&y2k));

    struct timeval tm;
    long msec;
    gettimeofday(&tm, NULL);
    msec = tm.tv_usec/1000;

    setCreationTime(seconds, msec);
}

void ProvisioningApp::setCreationTime(uint32_t sec, uint32_t milli)
{
    mCreationTime_seconds = sec;
    mCreationTime_millis = milli;
}

uint32_t ProvisioningApp::getCreationTimeSecond()
{
    return mCreationTime_seconds;
}

uint32_t ProvisioningApp::getCreationTimeMilli()
{
    return mCreationTime_millis;
}

uint8_t* ProvisioningApp::getVersionInfoWithSpaces(const char *name)
{
    sp<Buffer> version = new Buffer();
    uint8_t remaining = 0;

    version = m_configManager->get_string(CONFIG_XML_PATH, 2, name);
    if(version == NULL || !strcmp((const char*)version->data(), "0"))
    {
        LOGV("Version INFO ERROR, set to default value");
        return NULL;
    }
    else
    {
        remaining = VERSION_INFO_LENGTH - version->size();

        if(version->size() > 1)
            version->setTo(version->data(), version->size() - 1); // remove null terminator

        // append remaining bytes with spaces
        for(int i=0;i<remaining + 1;i++)
            version->append((uint8_t*)" ", 1);

        version->append((uint8_t*)"\0", 1);
        LOGV("version info = %s==", version->data());
        return version->data();
    }
}

uint8_t* ProvisioningApp::getMCUInfoWithSpaces()
{
    uint8_t remaining = 0;
    sp<IDebugManagerService> debugManager = interface_cast<IDebugManagerService>(
            defaultServiceManager()->getService(String16(DebugManagerService::getServiceName())));

    sp<Buffer> tempBuf = new Buffer();
    debugManager->requestCmdMCU(DEVICE_MICOM, CMD1_REQ_PART_NUMBER, CMD2_REQ_SW_PART_NUMBER, tempBuf);
    usleep(500*1000);
    sp<Buffer> buf = debugManager->getCmdMCU(PNMCU);

    if(buf->data() == NULL) {
        LOGV("Version INFO ERROR, set to default value");
        return NULL;
    }
    else {
        remaining = VERSION_INFO_LENGTH - buf->size();
        if(buf->size() > 1)
            buf->setTo(buf->data(), buf->size() - 1); // remove null terminator

        // append remaining bytes with spaces
        for(int i=0;i<remaining + 1;i++)
            buf->append((uint8_t*)" ", 1);

        buf->append((uint8_t*)"\0", 1);
        LOGV("version info = %s==", buf->data());
        return buf->data();
    }
}

uint8_t* ProvisioningApp::getTelephonyInfoWithSpaces(uint8_t index)
{
    sp<Buffer> telephonyInfo = new Buffer();
    std::string telephony;
    uint8_t remaining = 0;
    if(index == GET_IMEI)
        telephony = TelephonyManager::getImei();
    else if(index == GET_NAD_PN)
        telephony = TelephonyManager::getNadPartNumber();
    else
        return NULL;

    if(telephony.empty()) {
        LOGV("Telephony INFO ERROR, set to default value");
        return NULL;
    }
    else {
        uint8_t len = strlen(telephony.c_str());
        telephonyInfo->setTo((uint8_t*)telephony.c_str(), strlen(telephony.c_str()));
        remaining = VERSION_INFO_LENGTH - len;

        for(int i=0;i < remaining;i++)
            telephonyInfo->append((uint8_t*)" ", 1);

        telephonyInfo->append((uint8_t*)"\0", 1);
        LOGV("telephony info = %s==", telephonyInfo->data());
        return telephonyInfo->data();
    }
}

void ProvisioningApp::setVersionParameters()
{
    mTuStsParameters.version.hwVersion.set((const char*)getVersionInfoWithSpaces("coreAssemblyNum"));
    mTuStsParameters.version.swVersionMain.set((const char*)getMCUInfoWithSpaces());
    mTuStsParameters.version.swVersionSecondary.set((const char*)getVersionInfoWithSpaces("telecomNetSWNum"));
    mTuStsParameters.version.swVersionConfiguration.set((const char*)getVersionInfoWithSpaces("Calibration"));
    mTuStsParameters.version.serialNumber.set((const char*)getVersionInfoWithSpaces("SerialNumber"));
    mTuStsParameters.version.imei.set((const char*)getTelephonyInfoWithSpaces(GET_IMEI));

    if(strlen((const char*)vin_data) != VIN_LENGTH)
    {
        LOGV("VIN Length Error: ");
        setDefaultVIN();
    }
    else
    {
        uint8_t vin_parameter[10];
        memcpy((void*)vin_parameter, &vin_data[0], VIN_ISOWMI_LENGTH);
        vin_parameter[VIN_ISOWMI_LENGTH] = '\0';
        LOGV("VIN isowmi = %s", vin_parameter);
        mVinParameters.isowmi.set((const char*)vin_parameter);

        memcpy((void*)vin_parameter, &vin_data[VIN_ISOVDS_INDEX], VIN_ISOVDS_LENGTH);
        vin_parameter[VIN_ISOVDS_LENGTH] = '\0';
        LOGV("VIN isovds = %s", vin_parameter);
        mVinParameters.isovds.set((const char*)vin_parameter);

        memcpy((void*)vin_parameter, &vin_data[VIN_ISOVISMODELYEAR_INDEX], VIN_ISOVISMODELYEAR_LENGTH);
        vin_parameter[VIN_ISOVISMODELYEAR_LENGTH] = '\0';
        LOGV("VIN isovisModelyear = %s", vin_parameter);
        mVinParameters.isovisModelyear.set((const char*)vin_parameter);

        memcpy((void*)vin_parameter, &vin_data[VIN_ISOVISSEQPLANT_INDEX], VIN_ISOVISSEQPLANT_LENGTH);
        vin_parameter[VIN_ISOVISSEQPLANT_LENGTH] = '\0';
        LOGV("VIN isovisSeqPlanet = %s", vin_parameter);
        mVinParameters.isovisSeqPlant.set((const char*)vin_parameter);
    }
}

void ProvisioningApp::setDefaultVIN()
{
    mVinParameters.isovisSeqPlant.set(STR_DEFAULT_VIN);
}

void ProvisioningApp::MyTimer::handlerFunction( int timer_id )
{
    LOGE("handlerFunction() timer_id: %d", timer_id);

    switch(timer_id)
    {
        case TIMER_BCALLBUTTON:
            LOGV("[TIMER_BCALLBUTTON] timer expired.");
            if(mApplication.getbCallTriggered())
            {
                mApplication.bCall_active_light_off();
                mApplication.bCall_backlight_on();
            }
            break;
        case TIMER_FTUPDATE:
            LOGV("[TIMER_FTUPDATE] timer expired.");
            mApplication.setCurrentState(STATE_INIT);
            if(mApplication.getDiagResponse(DIAG_ROUTINE_ID_7) != FTUPDATE_FAILED_TO_SEND)
                mApplication.setDiagResponse(DIAG_ROUTINE_ID_7, FTUPDATE_NOACK_FROM_SERVER);
            mApplication.stopTimer(MyTimer::TIMER_RETRY);
            mApplication.sendDiagResponse();

            mApplication.TSimLockRelease();
            mApplication.sendTSIMlockStatus(false);
            break;
        case TIMER_CONFIG:
            LOGV("[TIMER_CONFIG] timer expired.");
            if(mApplication.getState() == STATE_SEND_CONFIGDATA)
                mApplication.setDiagResponse(DIAG_ROUTINE_ID_10, PDI_TSP_TIMEOUT);
            else
                mApplication.setDiagResponse(DIAG_ROUTINE_ID_10, PDI_ABORTED);
            mApplication.setCurrentState(STATE_INIT);
            if(mApplication.getbCallTriggered())
            {
                mApplication.bCall_active_light_off();
                mApplication.bCall_backlight_on();
            }
            mApplication.stopTimer(MyTimer::TIMER_RETRY);
            mApplication.sendDiagResponse();
            mApplication.TSimLockRelease();
            mApplication.sendTSIMlockStatus(false);
            break;
        case TIMER_TUSTS:
            LOGV("[TIMER_TUSTS] timer expired.");
            mApplication.setTuStsParameters();
            if(mApplication.getState() == STATE_SEND_CONFIGDATA) {
                mApplication.acquireKeepPower();
                mApplication.sendConfigRequestMsg1ToTSP(); // send ConfigRequest after setting up Diagnostics data
            }
            else if(mApplication.getState() == STATE_SEND_FTUPDATE)
            {
                mApplication.acquireKeepPower();
                mApplication.sendFTUpdateMsgToTSP(); // send Factory Test Update after setting up Diagnostics data
            }
            else {}
            break;
        case TIMER_RETRY:
            LOGV("[TIMER_RETRY] timer expired.");
            mApplication.checkRetryTimeout();
            break;
        case TIMER_DUPLICATE:
            LOGV("[TIMER_DUPLICATE] timer expired.");
            break;
        default:
            break;
    }
}

void ProvisioningApp::bCall_backlight_on(){
    LOGV("bCall_backlight_on: ");
    mHMIMgr->setFrequenceLED(HMI_TYPE_BCALL_BACK_LIGHT,  1);
    mHMIMgr->setDutyLED(HMI_TYPE_BCALL_BACK_LIGHT, 100);
}

void ProvisioningApp::bCall_active_light_solid(){
    LOGV("bCall_active_light_solid: ");
    mHMIMgr->setFrequenceLED(HMI_TYPE_BCALL_ACTIVE_LIGHT,  1);
    mHMIMgr->setDutyLED(HMI_TYPE_BCALL_ACTIVE_LIGHT, 100);
}

void ProvisioningApp::bCall_active_light_off(){
    LOGV("bCall_active_light_off: ");
    mHMIMgr->setFrequenceLED(HMI_TYPE_BCALL_ACTIVE_LIGHT,  1);
    mHMIMgr->setDutyLED(HMI_TYPE_BCALL_ACTIVE_LIGHT, 0);
}

void ProvisioningApp::processDiagCommand(uint8_t cmd1, uint8_t cmd2, uint32_t id, sp<Buffer>& buf)
{
    LOGV("diag : cmd1 = 0x%x, cmd2 = 0x%x", cmd1, cmd2);
    uint8_t *payload = buf->data();
    diagRoutineID = (payload[0] << 8) | payload[1];
    uint8_t pdiCmd = 0;

    if(diagRoutineID == DIAG_ROUTINE_ID_10)
        pdiCmd = payload[2];

    if(cmd1 == DIAG_CMD_ROUTINE_CONTROL)
    {
        switch(cmd2)
        {
            case DIAG_SUB_StartRoutine: // start
            {
                setDiagResponse(diagRoutineID, DIAG_NOTSTARTED);

                uint8_t startResponsePayload[4];
                startResponsePayload[0] = payload[0];
                startResponsePayload[1] = payload[1];
                startResponsePayload[2] = 0x01; // routine active
                if(diagRoutineID == DIAG_ROUTINE_ID_7)
                {
                    if(getState() == STATE_INIT) {
                        buf->clear();
                        buf->setTo(startResponsePayload, 3);
                        sendFTUpdateMsgByActiveSim();
                        sendTSIMlockStatus(true);
                    }
                    else
                        LOGE("invalid state: GSM Comm Test is already started");
                }
                else if(diagRoutineID == DIAG_ROUTINE_ID_10 && pdiCmd == PDI_CMD_GO_ACTIVATED)
                {
                    if(getState() == STATE_INIT)
                    {
                        startResponsePayload[3] = pdiCmd;
                        buf->clear();
                        buf->setTo(startResponsePayload, 4);

                        sp<ConfigurationBuffer> buffer = m_configManager->get(CONFIG_XML_PATH, 2, "EquipmentId");
                        if(buffer != NULL && buffer->errorFlag() == E_OK) // value exists,
                        {
                            uint8_t *equipmentId = buffer->data();
                            if(!strcmp((const char*)equipmentId, "0")) {
                                LOGE("err: EquipmentId is 0");
                                setDiagResponse(DIAG_ROUTINE_ID_10, PDI_UNSPECIFIED_FAILURE);
                            }
                        }
                        buffer = m_configManager->get(CONFIG_XML_PATH, 2, "SerialNumber");
                        if(buffer != NULL && buffer->errorFlag() == E_OK) // value exists,
                        {
                            uint8_t *serialNumber = buffer->data();
                            if(!strcmp((const char*)serialNumber, "0")) {
                                LOGE("err: SerialNumber is 0");
                                setDiagResponse(DIAG_ROUTINE_ID_10, PDI_UNSPECIFIED_FAILURE);
                            }
                        }
                        if(getCurrentOperationalMode() == OPERATIONAL_MODE_DEACTIVATED) {
                            LOGE("err: current mode is deactivated mode");
                            setDiagResponse(DIAG_ROUTINE_ID_10, PDI_UNSPECIFIED_FAILURE);
                        }

                        if(getDiagResponse(diagRoutineID) == DIAG_NOTSTARTED) {
                            if(!isCallActive()) {
                                enableSpyMode(false);
                                setbCallTriggered(false);
                                changeActSim_forConfig();
                                sendTSIMlockStatus(true);
                            }
                            else
                                LOGE("call is active, discard PDI");
                        }
                    }
                    else
                        setDiagResponse(DIAG_ROUTINE_ID_10, PDI_BUSY);
                }
                else
                    LOGE("invalid routine ID: 0x%x", diagRoutineID);

                // send Positive Response to DiagMgr
                m_diagManager->responseDiagcmd(id, DIAG_RESP_ROUTINE_CONTROL, cmd2, buf);
                // send terminated
                if(getDiagResponse(diagRoutineID) == PDI_BUSY || getDiagResponse(diagRoutineID) == PDI_UNSPECIFIED_FAILURE)
                    sendDiagResponse();

                break;
            }
            case DIAG_SUB_StopRoutin: // stop
            {
                // send Positive Response to DiagMgr
                if(diagRoutineID == DIAG_ROUTINE_ID_7)
                    setDiagResponse(diagRoutineID, FTUPDATE_STOP_BY_DIAG);
                m_diagManager->responseDiagcmd(id, DIAG_RESP_ROUTINE_CONTROL, cmd2, buf);
                diagRoutineID = 0;
                break;
            }
            case DIAG_SUB_RoutineResults: // result
            {
                sp<Buffer> buf = new Buffer();
                // send Positive Response to DiagMgr
                uint8_t responsePayload[3];
                responsePayload[0] = (diagRoutineID >> 8) & 0xFF;
                responsePayload[1] = diagRoutineID & 0xFF;
                if(getDiagResponse(diagRoutineID) == DIAG_NOTSTARTED)
                {
                    buf->setTo(responsePayload, 2);
                }
                else
                {
                    if(diagRoutineID == DIAG_ROUTINE_ID_7 || diagRoutineID == DIAG_ROUTINE_ID_10)
                    {
                        responsePayload[2] = getDiagResponse(diagRoutineID);
                        buf->setTo(responsePayload, 3);
                    }
                }
                m_diagManager->responseDiagcmd(id, DIAG_RESP_ROUTINE_CONTROL, cmd2, buf);
                diagRoutineID = 0;
            }
            default:
                break;
        }
    }
}

void ProvisioningApp::setDiagResponse(uint8_t id, int8_t resp)
{
    mDiagResponse.replaceValueFor(id, resp);
}

void ProvisioningApp::onPostReceived(const sp<Post>& post)
{
    LOGV("onPostReceived: what = 0x%x", post->what);
    if(post->what == FTUPDATE_SLDD_COMMAND || post->what == COMM_TEST_POST_ID) {
        if(getState() == STATE_INIT)
            sendFTUpdateMsgByActiveSim();
        else
            LOGE("invalid state: GSM Comm Test is already started");
    }
    else if(post->what == ECALL_TRIGGER_ENABLE || post->what == BCALL_TRIGGER_ENABLE) {
        LOGV("bcall or ecall enters active");
        m_bCallActive = true;
    }
}

error_t ProvisioningApp::saveGenParamToConfigMgr()
{
    error_t ret = E_OK;
    bool isNewValue;
    uint32_t newValue;
    svc::NGTPUplinkCore::genericParameters genParam = mNGTPDecode->getGenericParameterForConfigMgr();
    if(genParam.empty())
    {
        LOGV("Generic Parameter is empty...");
        return E_OK;
    }
    else
    {
        OssIndex node = genParam.first();
        svc::GenericParameter *genComponent;
        while(node!=NULL)
        {
            isNewValue = false;
            genComponent = genParam.at(node);
            uint32_t *key = genComponent->get_key().get_id();
            const char buffer[11] = {0,};
            sprintf((char*)buffer, "0x%08x", *key);

            switch(genComponent->get_value().get_selection())
            {
                case svc::GenericParameterValue::uint32Val_chosen :
                {
                    uint32_t olduint32Value = m_configManager->get_int(CONFIG_XML_PATH, 0, buffer);
                    uint32_t *uint32value = genComponent->get_value().get_uint32Val();
                    if(olduint32Value != -1)
                    {
                    if(olduint32Value == *uint32value)
                        LOGV("key: %d, value :%u is the same with old value", *key, *uint32value);
                    else
                    {
                        if(m_configManager->change_set_int(CONFIG_XML_PATH, 0, buffer, *uint32value) == E_ERROR)
                        {
                            LOGE("cannot update key:%s, value:%u", buffer, *uint32value);
                            ret = E_ERROR;
                        }
                    }
                    }
                    else
                    {
                        isNewValue = true;
                        newValue = *uint32value;
                    }
                    break;
                }
                case svc::GenericParameterValue::boolVal_chosen :
                {
                    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 0, buffer);
                    bool *value = (bool*)genComponent->get_value().get_boolVal();
                    bool oldValue;
                    if(buf != NULL && buf->errorFlag() == E_OK)
                    {
                        buf->data(&oldValue);
                        if(oldValue == *value)
                            LOGV("key: %d, value :%u is the same with old value", *key, *value);
                        else
                        {
                            sp<ConfigurationBuffer> pBuffer= new ConfigurationBuffer;
                            pBuffer->setTo((bool)*value);
                            if(m_configManager->change_set(CONFIG_XML_PATH, 0, buffer, pBuffer) == E_ERROR) {
                                LOGE("cannot update key:%s, value:%u", buffer, *value);
                                ret = E_ERROR;
                            }
                        }
                    }
                    else // new key
                    {
                        isNewValue = true;
                        newValue = *value;
                    }
                    break;
                }
                case svc::GenericParameterValue::stringVal_chosen :
                {
                    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 0, buffer);
                    uint8_t *value = (uint8_t*)genComponent->get_value().get_stringVal()->get_buffer();
                    uint16_t length = genComponent->get_value().get_stringVal()->length();
                    value[length] = '\0';
                    if(buf != NULL && buf->errorFlag() == E_OK) {
                        if(!strcmp((const char*)buf->data(), (const char*)value))
                            LOGV("key: %d, value :%s is the same with old value", *key, buf->data());
                        else {
                            sp<ConfigurationBuffer> pBuffer= new ConfigurationBuffer;
                            pBuffer->setTo(value);
                            if(m_configManager->change_set(CONFIG_XML_PATH, 0, buffer, pBuffer) == E_ERROR) {
                                LOGE("cannot update key:%s, value:%u", buffer, *value);
                                ret = E_ERROR;
                            }
                        }
                    }
                    break;
                }
                default:
                    LOGV("      unknown (%d) chosen...", genComponent->get_value().get_selection());
                    break;
            }
            if(isNewValue)
            {
                const char genName[20] = {0,};
                sprintf((char*)genName, "GenParam_0x%x", *key);
                LOGV("add new key: %d, value: %u to ConfigMgr", *key, newValue);
/*
                if(m_configManager->new_set_int(CONFIG_XML_PATH, "TSP", buffer, genName, newValue) == E_ERROR)
                {
                    LOGE("cannot add key:%d, value:%u", *key, newValue);
                    ret = E_ERROR;
                }
*/
            }

            node = genParam.next(node);
        }
        return ret;
    }
}

void ProvisioningApp::notifyProvisioningComplete()
{
    sp<Post> post;
    LOGV("notifyProvisioningComplete : remoteOperation = %d", mNGTPDecode->getRemoteOperationName());
    if(mNGTPDecode->getRemoteOperationName() == svc::rotNameActivate) // Activated-Unprovisioned
    {
        mTuStsParameters.activation = svc::activated;
        post = Post::obtain(SYS_POST_PROVISIONING_COMPLETED, SYS_POST_PROVISIONING_COMPLETED_UNPROVISIONED);
        m_AppMgr->broadcastSystemPost(post);
    }
    else if(mNGTPDecode->getRemoteOperationName() == svc::rotNameProvision) // Activated-Provisioned
    {
        mTuStsParameters.activation = svc::provisioned;
        post = Post::obtain(SYS_POST_PROVISIONING_COMPLETED, SYS_POST_PROVISIONING_COMPLETED_PROVISIONED);
        m_AppMgr->broadcastSystemPost(post);
    }
    else
        LOGV("RemoteOperationName Error");

    setDiagResponse(DIAG_ROUTINE_ID_10, PDI_OK);
}

sp<IvifManagerService> ProvisioningApp::connectToVifMgr(){
    if(mvifReceiver == NULL){
        mvifReceiver = new VifReceiver(*this);
   }
   if(mvifManager == NULL){
        do{
            mvifManager = interface_cast<IvifManagerService>(
                defaultServiceManager()->getService(String16(vifManagerService::getServiceName())));
            if(mvifManager!=NULL){
                break;
            }else{
                usleep(500000);
            }
        }while(mvifManager == NULL);
        mvifManager->registerReceiver(Signal_VehicleInfoParametersPMZ_RX, vif_received_always, mvifReceiver);
   }
    return mvifManager;
}

int32_t ProvisioningApp::getBCallButtonLedTimeout()
{
    int32_t bCallButtonTimeout = m_configManager->get_int(CONFIG_XML_PATH, 2, BCALL_LED_DURATION_NAME);
    if(bCallButtonTimeout == -1)
        return TIMEOUT_BCALLBUTTON; // default value
    else
        return bCallButtonTimeout;
}

void ProvisioningApp::setUpTuStsData()
{
    m_diagManager->SetUpTUStatusData(getAppID(), DIAG_MCU_DTC_SCANNING | DIAG_CAN_DTC_SCANNING | DIAG_ADDITIONAL_DTC_SCANNING);
    startTimer(MyTimer::TIMER_TUSTS);
}

void ProvisioningApp::setTuStsParameters()
{
    sp<TUStatusInfo> tcu_status = new TUStatusInfo();
    m_diagManager->GetTUStatusData(tcu_status);

    mTuStsParameters.power = (svc::TUPowerSource)tcu_status->TCUStatus.Power;
    mTuStsParameters.mobilePhoneConnected = (svc::BOOLEXT)tcu_status->TCUStatus.mobilePhoneConnected;
    mTuStsParameters.sleepCyclesStartTime = convert_Time2Ngtptime(tcu_status->TCUStatus.Sleep_Cycles_Start_time);
    mTuStsParameters.configVersion = tcu_status->TCUStatus.configVersion;
    mTuStsParameters.activation = (svc::TuActivationStatus)tcu_status->TCUStatus.activation;
    mTuStsParameters.diag.speaker = (svc::TUDiagnostic)tcu_status->TCUStatus.speaker;
    mTuStsParameters.diag.mic = (svc::TUDiagnostic)tcu_status->TCUStatus.mic;
    mTuStsParameters.diag.extHandsfree = svc::broken;
    mTuStsParameters.diag.handset = svc::uncertain;
    mTuStsParameters.diag.gsmModem = (svc::TUDiagnostic)tcu_status->TCUStatus.gsmModem;
    mTuStsParameters.diag.gsmExtAntenna = (svc::TUDiagnostic)tcu_status->TCUStatus.gsmExtAntenna;
    mTuStsParameters.diag.extPower = (svc::TUDiagnostic)tcu_status->TCUStatus.extPower;
    mTuStsParameters.diag.intPower = (svc::TUDiagnostic)tcu_status->TCUStatus.intPower;
    mTuStsParameters.diag.gnss = svc::functioning;
    mTuStsParameters.diag.gnssAntenna = svc::functioning;
    mTuStsParameters.diag.can = (svc::TUDiagnostic)tcu_status->TCUStatus.can;
    mTuStsParameters.diag.buttons = (svc::TUDiagnostic)tcu_status->TCUStatus.button;
    mTuStsParameters.diag.crashInput = (svc::TUDiagnostic)tcu_status->TCUStatus.crashInput;
    mTuStsParameters.diag.intRtc = (svc::TUDiagnostic)tcu_status->TCUStatus.intRtc;
    mTuStsParameters.diag.daysSinceGnssFix = 0;

    mTuStsParameters.battParam.primaryVoltx10 = tcu_status->TCUStatus.Primary_Battery_Voltage;
    mTuStsParameters.battParam.primaryChargePerc = tcu_status->TCUStatus.Primary_Charge_Percentage;
    mTuStsParameters.battParam.secondaryVoltx10 = tcu_status->TCUStatus.Secondary_Battery_Voltage;
    mTuStsParameters.battParam.secondaryChargePerc = tcu_status->TCUStatus.Secondary_Charge_Percentage;
}

void ProvisioningApp::checkRetryTimeout()
{
    switch(getState())
    {
        case STATE_SEND_FTUPDATE:
            if(m_FTUpdateTimer->getRemainingTimeMs() != 0) {
                LOGV("retry sending FTUpdate Message");
                sendFTUpdateMsgToTSP();
            }
            break;
        case STATE_SEND_CONFIGDATA:
            if(m_ConfigTimer->getRemainingTimeMs() !=0) {
                LOGV("retry sending ConfigRequest");
                sendConfigRequestMsg1ToTSP();
            }
            break;
        default:
            break;
    }
}

void ProvisioningApp::deleteRetryMessage()
{
    switch(getState())
    {
    /*
        case STATE_SEND_FTUPDATE:
            m_ngtpManager->deleteMessageFromRetryBuffer(mFTUpdate->getRetryTimeID());
            break;
    */
        case STATE_SEND_CONFIGDATA:
            m_ngtpManager->deleteMessageFromRetryBuffer(mConfigRequest->getRetryTimeID());
            break;
        default:
            break;
    }
}
void ProvisioningApp::onPSimStateChanged(int32_t simAct, int32_t event, int32_t slot, int32_t errorCause)
{
    LOGV("onPSimStateChanged : %d,%d,%d,%d", simAct, event, slot, errorCause);
    mHandler->obtainMessage(MyHandler::RECV_MSG_PSIM_STATE_CHANGED, simAct, event, slot)->sendToTarget();
}

void ProvisioningApp::onActiveSimStateChanged(int32_t activeSim)
{
    LOGV("onActiveSimStateChanged : %d", activeSim);
    mHandler->obtainMessage(MyHandler::RECV_MSG_ACTIVESIM_STATE_CHANGED, activeSim)->sendToTarget();
}

void ProvisioningApp::processActiveSimChanged(int32_t activeSim)
{
    LOGV("processActiveSimChanged : %d", activeSim);
    if(activeSim == TelephonyManager::REMOTE_SIM_TSIM && getState() == STATE_SEND_FTUPDATE)
    {
        LOGV("[GSM Comm Test] changed to TSIM, do the GSM Comm Test");
        TSimLockAcquire();
        makeEventCreationTime();
        setUpTuStsData();
        startTimer(MyTimer::TIMER_FTUPDATE);
    }
    else if(activeSim == TelephonyManager::REMOTE_SIM_PSIM && getState() == STATE_SEND_FTUPDATE) {
        LOGV("[GSM Comm Test] changed to PSIM, change to TSIM");
        TelephonyManager::setActiveSim(
            TelephonyManager::REMOTE_SIM_TSIM,
            TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
            TelephonyManager::REMOTE_SIM_CAUSE_PROVISIONING);
    }
    else if(activeSim == TelephonyManager::REMOTE_SIM_TSIM && getState() == STATE_SEND_CONFIGDATA) {
        LOGV("changed to TSIM, send configdata");
        TSimLockAcquire();
        trigger_configUpdate();
    }
    else if(activeSim == TelephonyManager::REMOTE_SIM_PSIM && getState() == STATE_SEND_CONFIGDATA) {
        LOGV("changed to PSIM, change to TSIM for configupdate");
        TelephonyManager::setActiveSim(
            TelephonyManager::REMOTE_SIM_TSIM,
            TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
            TelephonyManager::REMOTE_SIM_CAUSE_PROVISIONING);
    }
    else
        LOGV("not correct state:");
}
void ProvisioningApp::sendFTUpdateMsgByActiveSim()
{
    int32_t activeSim = TelephonyManager::getActiveSim();
    LOGV("activeSim : %d, tsim lock state = %d", activeSim, TelephonyManager::getTsimLockState());
    switch(activeSim)
    {
        case TelephonyManager::REMOTE_SIM_TSIM: // current SIM is TSIM.
            LOGV("[GSM Comm Test] current is TSIM, do the GSM Comm Test");
            TSimLockAcquire();
            setCurrentState(STATE_SEND_FTUPDATE);
            makeEventCreationTime();
            setUpTuStsData();
            startTimer(MyTimer::TIMER_FTUPDATE);
            break;
        case TelephonyManager::REMOTE_SIM_PSIM: // change to TSIM.
            LOGV("[GSM Comm Test] current is PSIM, change to TSIM");
            setCurrentState(STATE_SEND_FTUPDATE);
            startTimer(MyTimer::TIMER_FTUPDATE);
            TelephonyManager::setActiveSim(
                TelephonyManager::REMOTE_SIM_TSIM,
                TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
                TelephonyManager::REMOTE_SIM_CAUSE_PROVISIONING);
            break;
        default:
            LOGV("[GSM Comm Test] waiting for sim switching...");
            setCurrentState(STATE_SEND_FTUPDATE);
            startTimer(MyTimer::TIMER_FTUPDATE);
            break;
    }
}

uint8_t ProvisioningApp::getCurrentOperationalMode()
{
    if (mOperationalMode == NULL) {
        mOperationalMode = interface_cast<IOperationModeManagerService>(defaultServiceManager()->getService(String16(OperationModeManagerService::getServiceName())));
    }
    return mOperationalMode->getCurrentOperationMode();
}

bool ProvisioningApp::getbCallTriggered()
{
    return mbCallTrigger;
}

void ProvisioningApp::setbCallTriggered(bool triggered)
{
    mbCallTrigger = triggered;
}

void ProvisioningApp::sendDiagResponse()
{
    if(diagRoutineID != 0)
    {
        LOGV("sendDiagResponse: diagRoutineID = 0x%x", diagRoutineID);
        sp<Buffer> buf = new Buffer();
        uint8_t startResponsePayload[4];
        startResponsePayload[0] = (diagRoutineID & 0xff00) >> 8;
        startResponsePayload[1] = diagRoutineID & 0xff;
        startResponsePayload[2] = 0x02; // routine terminated;
        if(diagRoutineID == DIAG_ROUTINE_ID_10) {
            startResponsePayload[3] = 0x02; // payload of F001
            buf->setTo(startResponsePayload, 4);
            m_diagManager->responseDiagcmd(0, DIAG_RESP_ROUTINE_CONTROL, DIAG_SUB_StartRoutine, buf);
        }
        else if(diagRoutineID == DIAG_ROUTINE_ID_7) {
            buf->setTo(startResponsePayload, 3);
            m_diagManager->responseDiagcmd(0, DIAG_RESP_ROUTINE_CONTROL, DIAG_SUB_StartRoutine, buf);
        }
        else
            LOGE("invalid diagRoutineID : 0x%x", diagRoutineID);
    }
}

bool ProvisioningApp::TSimLockAcquire()
{
    bool ret;
    ret = mTsimLock->acquire();
    if(ret)
        LOGV("TSIM lock is acquired");
    else
        LOGV("TSIM lock is not acquired");

    return ret;
}

bool ProvisioningApp::TSimLockRelease()
{
    bool ret;
    ret = mTsimLock->release();
    if(ret)
        LOGV("TSIM lock is released");
    else
        LOGV("TSIM lock is not released");

    return ret;
}

void ProvisioningApp::enableSpyMode(bool enable)
{
    bool spyMode = false;
    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "SpyMode");
    if(buf != NULL && buf->errorFlag() == E_OK) // value exists,
    {
        buf->data(&spyMode);
        if(spyMode) {
            if(enable) {
                LOGD("Spy Mode enabled, enable SpyMode after Configuration is done");
            }
            else {
                LOGD("Spy Mode enabled, disable SpyMode for Configuration");
            }
            bool nad_value = (bool)TelephonyManager::getTcuSpyMode();
            if( nad_value != enable )
            {
                TelephonyManager::setTcuSpyMode(enable);
            }
        }
    }
    else
        LOGE("Can't get spymode from config");
}

void ProvisioningApp::send_bCallButton_ack()
{
    LOGV("send bCallButton ACK");
    sp<CommunicationData> data = new CommunicationData;
    data->type = PACKET_TYPE_RESPONSE;
    data->src = DEVICE_NAD;

    data->dest = DEVICE_MICOM;
    data->src_mid = NAD_MODULE_APPLICATION_MGR;
    data->dest_mid = MCU_MODULE_CONFIG;

    data->cmd = ACK_FOR_PROVISION_TRIGGER;
    data->cmd2 = CMD_NOT_USED;

    sp<ICommunicationManagerService> cm = interface_cast<ICommunicationManagerService>(
     defaultServiceManager()->getService(String16(CommunicationManagerService::getServiceName())));
    cm->sendData(SESSION_MCU_TX, data->payload, data);
}

bool ProvisioningApp::isCallActive()
{
    uint32_t bcallActive = false;
    uint32_t ecallActive = false;
    sp<ConfigurationBuffer> buf = m_configManager->get(CONFIG_XML_PATH, 2, "bCallstatus");
    if(buf != NULL && buf->errorFlag() == E_OK)
        buf->data(&bcallActive);
    buf = m_configManager->get(CONFIG_XML_PATH, 2, "eCallstatus");
    if(buf != NULL && buf->errorFlag() == E_OK)
        buf->data(&ecallActive);

    if(bcallActive || ecallActive)
        return true;
    else {
        m_bCallActive = false;
        return false;
    }
}

void ProvisioningApp::acquireKeepPower()
{
    mKeepPower.acquire();
    mKeepPowerCnt++;
    LOGV("KeepPower acquire count:%d", mKeepPowerCnt);
}

void ProvisioningApp::releaseKeepPower()
{
    mKeepPower.release();
    mKeepPowerCnt--;
    LOGV("KeepPower release count:%d", mKeepPowerCnt);
}

uint32_t ProvisioningApp::convert_Time2Ngtptime(uint32_t sec)
{
    struct tm y2k = {0};
    int32_t seconds;

    y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
    y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

    seconds = difftime(sec,mktime(&y2k));

    if(seconds < 0 ){
        seconds = 0;
    }
    return seconds;
}

void ProvisioningApp::changeActSim_forConfig()
{
    int32_t activeSim = TelephonyManager::getActiveSim();
    LOGV("activeSim : %d, tsim lock state = %d", activeSim, TelephonyManager::getTsimLockState());
    switch(activeSim) {
        case TelephonyManager::REMOTE_SIM_TSIM: // current SIM is TSIM.
            LOGV("current is TSIM, do the config_update");
            TSimLockAcquire();
            trigger_configUpdate();
            break;
        case TelephonyManager::REMOTE_SIM_PSIM: // change to TSIM.
            LOGV("current is PSIM, change to TSIM");
            setCurrentState(STATE_SEND_CONFIGDATA);
            startTimer(MyTimer::TIMER_CONFIG);
            TelephonyManager::setActiveSim(
                TelephonyManager::REMOTE_SIM_TSIM,
                TelephonyManager::REMOTE_SIM_PRIORITY_HIGH,
                TelephonyManager::REMOTE_SIM_CAUSE_PROVISIONING);
            break;
        default:
            startTimer(MyTimer::TIMER_CONFIG);
            setCurrentState(STATE_SEND_CONFIGDATA);
            LOGV("waiting for sim switching...");
            break;
    }
}

void ProvisioningApp::trigger_configUpdate()
{
    setCurrentState(STATE_SEND_CONFIGDATA);
    makeEventCreationTime();
    setUpTuStsData();
    startTimer(MyTimer::TIMER_CONFIG);
}

void ProvisioningApp::trigger_configUpdate_byTSP()
{
    setCurrentState(STATE_SEND_CONFIGDATA);
    setUpTuStsData();
    startTimer(MyTimer::TIMER_CONFIG);
}

void ProvisioningApp::sendTSIMlockStatus(bool lock)
{
    sp<ISystemManagerService> systemService = interface_cast<ISystemManagerService>
        (defaultServiceManager()->getService(String16(SystemManagerService::getServiceName())));

    int32_t mdmVariant  = systemService->getMDMvariant();
    LOGI("%s mdmVariant : %d, lock = %d", __func__, mdmVariant, lock);
    if(mdmVariant == MY18_HIGH) {
        sp<INetworkManagerService> nms = interface_cast<INetworkManagerService>
           (defaultServiceManager()->getService(String16("service_layer.NetworkManagerService")));
        nms->provisionActiveSet(lock);
    }
    else if(mdmVariant == MY18_LOW) {
        sp<INLIManagerService> nlims = interface_cast<INLIManagerService>
           (defaultServiceManager()->getService(String16("service_layer.NLIManagerService")));
        nlims->provisionActiveSet(lock);
    }
    else;
}
extern "C" class Application* createApplication() {
    LOGI("createApplication() ProvisioningApp");

    gApp = new ProvisioningApp;
    return gApp.get();
}

extern "C" void destroyApplication(class Application* application) {
    delete (ProvisioningApp*)application;
}
