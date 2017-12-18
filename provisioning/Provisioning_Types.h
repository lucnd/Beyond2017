#ifndef _PROVISIONING_TYPES_H_
#define _PROVISIONING_TYPES_H_

#define APPMODE_JL_NOTSUPPORT
#define APPMODEJL_KEY 1349438979

typedef struct diag_parameters
{
    svc::TUDiagnostic speaker;
    svc::TUDiagnostic mic;
    svc::TUDiagnostic extHandsfree;
    svc::TUDiagnostic handset;
    svc::TUDiagnostic gsmModem;
    svc::TUDiagnostic gsmExtAntenna;
    svc::TUDiagnostic extPower;
    svc::TUDiagnostic intPower;
    svc::TUDiagnostic gnss;
    svc::TUDiagnostic gnssAntenna;
    svc::TUDiagnostic can;
    svc::TUDiagnostic buttons;
    svc::TUDiagnostic crashInput;
    svc::TUDiagnostic intRtc;
    uint32_t daysSinceGnssFix;
} diag_parameters;

typedef struct battery_parameters
{
    uint32_t primaryVoltx10;
    uint32_t primaryChargePerc;
    uint32_t secondaryVoltx10;
    uint32_t secondaryChargePerc;
} battery_parameters;

typedef struct version_parameters
{
    OssString hwVersion;
    OssString swVersionMain;
    OssString swVersionSecondary;
    OssString swVersionConfiguration;
    OssString serialNumber;
    OssString imei;
} version_parameters;

typedef struct vin_parameters
{
    OssString isowmi;
    OssString isovds;
    OssString isovisModelyear;
    OssString isovisSeqPlant;
} vin_parameters;

typedef struct tuSts_parameters
{
    svc::TUPowerSource power;
    diag_parameters diag;
    battery_parameters battParam;
    svc::BOOLEXT mobilePhoneConnected;
    uint32_t sleepCyclesStartTime;
    uint32_t configVersion;
    svc::TuActivationStatus activation;
    svc::BOOLEXT usesExternalGnss;
    version_parameters version;
} tuSts_parameters;

#define DIAG_NOTSTARTED             -1
#define FTUPDATE_OK                 0x00
#define FTUPDATE_FAILED_TO_SEND     0x01
#define FTUPDATE_NOACK_FROM_SERVER  0x02
#define FTUPDATE_STOP_BY_DIAG       0x03
#define FTUPDATE_NACK_FROM_SERVER   0x04

#define PDI_OK                      0x00
#define PDI_TSP_TIMEOUT             0x01
#define PDI_UNSPECIFIED_FAILURE     0x02
#define PDI_BUSY                    0x03
#define PDI_ABORTED                 0x04

#define PDI_CMD_GO_ACTIVATED        0x02

#define SERVICETYPE_PROVISIONING    8
#define SERVICETYPE_FACTORY_TEST    34
#define EQUIPMENTID_KEY             0x5072ba06
#define TIMEOUT_BCALLBUTTON         2000
#define TIMEOUT_FTUPDATE            120
#define TIMEOUT_CONFIG              120
#define TIMEOUT_TUSTS               500
#define TIMEOUT_RETRY               15
#define TIMEOUT_DUPLICATE           5000
#define TIMEOUT_DUPLICATE_CHECK     3000 // if same service data is received within this, treat this as duplicate message.

#define DEALER_ACTIVATION_KEY1      1354080877
#define DEALER_ACTIVATION_KEY2      1354080883
#define DEALER_ACTIVATION_NAME1     "DealerAndActivationCode1"
#define DEALER_ACTIVATION_NAME2     "DealerAndActivationCode2"

#define CONFIG_XML_PATH             "/usr/config/jlr_tcu_data_dictionary_swr_9_3.xml"
#define BCALL_LED_DURATION_NAME     "ConfigOkLedDurationMs"
#define VERSION_INFO_LENGTH         20
#define VIN_LENGTH                  17
#define VIN_ISOWMI_LENGTH           3
#define VIN_ISOVDS_LENGTH           6
#define VIN_ISOVDS_INDEX            3
#define VIN_ISOVISMODELYEAR_LENGTH  1
#define VIN_ISOVISMODELYEAR_INDEX   9
#define VIN_ISOVISSEQPLANT_LENGTH   7
#define VIN_ISOVISSEQPLANT_INDEX    10
#define STR_DEFAULT_VIN             "AAAAAAA"

#define FTUPDATE_SLDD_COMMAND       0xFEBA

#define NGTP_ACK_RETRY_CNT          5
#define NGTP_ACK_RETRY_DELAY        800
#define NGTP_REQ_RETRY_CNT          5
#define NGTP_REQ_RETRY_DELAY        1000

#define SERVICE_DATA_MAXBUFLEN      1024
#define ACK_FOR_PROVISION_TRIGGER   0xF8

#define ECALL_TRIGGER_ENABLE        71
#define BCALL_TRIGGER_ENABLE        73

#define GET_IMEI                    1
#define GET_NAD_PN                  2

#define PNMCU                       6
#define COMM_TEST_POST_ID           0x1234
#endif // _PROVISIONING_TYPES_H_
