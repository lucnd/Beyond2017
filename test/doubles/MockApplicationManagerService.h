#ifndef TEST_DOUBLES_MOCK_APPLICATION_MANAGER_SERVICE_H_INCLUDED
#define TEST_DOUBLES_MOCK_APPLICATION_MANAGER_SERVICE_H_INCLUDED

#include <services/ApplicationManagerService/IApplicationManagerService.h>
#include <gmock/gmock.h>

class MockApplicationManagerService: public IApplicationManagerService {
public:
    MOCK_CONST_METHOD0(getInterfaceDescriptor,
            const android::String16&());

    MOCK_METHOD1(enumerateAvailableApplication,
            error_t(bool));
    MOCK_METHOD0(disableAllApplication,
            error_t(void));
    MOCK_METHOD2(attachApplication,
            error_t(const char* app, android::sp<android::IBinder> proxy));
    MOCK_METHOD2(enableApplication,
            error_t(const char* app, int32_t reason));

    MOCK_METHOD2(broadcastHMI,
            void(const uint32_t type, const uint32_t action));

    MOCK_METHOD2(sendPost,
            error_t(const appid_t appid, const android::sp<Post>& post));

    MOCK_METHOD2(getProperty,
            android::sp<Buffer>(appid_t id, const char* name));
    MOCK_METHOD3(setProperty,
            void(appid_t id, const char* name, const char* value));
    MOCK_METHOD3(setProperty,
            void(appid_t id, const char* name, const int32_t i_value));

    MOCK_METHOD1(isEnabledApplication,
            bool(appid_t id));
    MOCK_METHOD1(inquireApplicationStatus,
            error_t(int32_t service_sig));

    MOCK_METHOD1(ReadyToDestroy,
            error_t(appid_t id));
    MOCK_METHOD1(readyToRun,
            error_t(const char* app));

    MOCK_METHOD2(sendPost,
            error_t(const char* app, const android::sp<Post>& post));
    MOCK_METHOD1(broadcastPost,
            error_t(const android::sp<Post>& post));
    MOCK_METHOD1(broadcastSystemPost,
            error_t(const android::sp<Post>& systemPost));
    MOCK_METHOD2(registerSystemPostReceiver,
            error_t(const android::sp<ISystemPostReceiver>& receiver, const int32_t post));
    MOCK_METHOD1(unregisterSystemPostReceiver,
            error_t(const android::sp<ISystemPostReceiver>& receiver));

    MOCK_METHOD2(disableApplication,
            error_t(const char* app, int32_t reason));
    MOCK_METHOD1(isEnabled,
            bool(const char* app));

    MOCK_METHOD2(getProperty,
            android::sp<Buffer>(const char* app, const char* name));
    MOCK_METHOD3(setProperty,
            void(const char* app, const char* name, const char* value));
    MOCK_METHOD3(setProperty,
            void(const char* app, const char* name, const int32_t i_value));

    MOCK_METHOD1(setBootCompleted,
            void(bool complete));
    MOCK_METHOD0(getBootCompleted,
            bool());

    MOCK_METHOD0(onAsBinder,
            android::IBinder*());
};


#endif  // TEST_DOUBLES_MOCK_APPLICATION_MANAGER_SERVICE_H_INCLUDED
