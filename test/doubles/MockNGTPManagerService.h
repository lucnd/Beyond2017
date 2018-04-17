#ifndef TEST_DOUBLES_MOCK_NGTP_MANAGER_SERVICE_H_INCLUDED
#define TEST_DOUBLES_MOCK_NGTP_MANAGER_SERVICE_H_INCLUDED

#include <services/NGTPManagerService/INGTPManagerService.h>
#include <gmock/gmock.h>

class MockNGTPManagerService: public INGTPManagerService {
public:
    MOCK_CONST_METHOD0(getInterfaceDescriptor,
            const android::String16&());

    MOCK_METHOD2(sendToTSDP,
            uint32_t(InternalNGTPData *data, int dataLength));
    MOCK_METHOD2(registerReceiver,
            error_t(sp<INGTPReceiver>& receiver, int service_to_register));
    MOCK_METHOD0(factoryTestDone,
            error_t());
    MOCK_METHOD1(getDiagInfo,
            error_t(NGTPDiagInfo2 *));
    MOCK_METHOD2(unregisterReceiver,
            error_t(sp<INGTPReceiver>& receiver, int service_to_register));
    MOCK_METHOD0(readConfiguration,
            error_t());
    MOCK_METHOD1(sendToTSDPRetry,
            error_t(uint32_t messageID));
    MOCK_METHOD1(deleteMessageFromRetryBuffer,
            error_t(uint32_t messageID));
    MOCK_METHOD0(verifyCertificates,
            bool());
    MOCK_METHOD0(installZippedCertificates,
            bool());
    MOCK_METHOD1(setNGTPEncryptionKey,
            int8_t(char* key));
    MOCK_METHOD1(getNGTPEncryptionKey,
            void(char* key));
    MOCK_METHOD1(setDemoMode,
            void(bool onoff));
    MOCK_METHOD0(getNGTPAvailability,
            bool());
    MOCK_METHOD0(getDemoMode,
            bool());
    MOCK_METHOD1(setIFOption,
            bool(uint8_t option));
    MOCK_METHOD1(sendFakeMessage,
            bool(uint8_t option));

    MOCK_METHOD0(onAsBinder,
            IBinder*());
};

#endif  // TEST_DOUBLES_MOCK_NGTP_MANAGER_SERVICE_H_INCLUDED
