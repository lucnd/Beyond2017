#ifndef TEST_DOUBLES_MOCK_SYSTEM_MANAGER_SERVICE_H_INCLUDED
#define TEST_DOUBLES_MOCK_SYSTEM_MANAGER_SERVICE_H_INCLUDED

#include <services/SystemManagerService/ISystemManagerService.h>
#include <gmock/gmock.h>

class MockSystemManagerService: public ISystemManagerService {
public:
    MOCK_CONST_METHOD0(getInterfaceDescriptor,
            const android::String16&());

    android::status_t registerSystemReceiver(android::sp<ISystemReceiver> receiver, uint32_t mask,
            bool notify_now = false) override {
        return mockRegisterSystemReceiver(receiver, mask, notify_now);
    }

    MOCK_METHOD3(mockRegisterSystemReceiver,
            android::status_t(android::sp<ISystemReceiver>, uint32_t, bool));
    MOCK_METHOD2(unregisterSystemReceiver,
            android::status_t(android::sp<ISystemReceiver> receiver, uint32_t mask));

    android::status_t registerGPIOIntReceiver(android::sp<ISystemGPIOIntReceiver> receiver, uint32_t gpio_mask,
            int32_t action = 0,
            bool notify_now = false) override {
        return mockRegisterGPIOIntReceiver(receiver, gpio_mask, action, notify_now);
    }

    MOCK_METHOD4(mockRegisterGPIOIntReceiver,
            android::status_t(android::sp<ISystemGPIOIntReceiver>, uint32_t, int32_t, bool));
    MOCK_METHOD1(unregisterGPIOIntReceiver,
            android::status_t(android::sp<ISystemGPIOIntReceiver> receiver));

    MOCK_METHOD2(getAntennaStatus,
            error_t(int8_t ant_mode, ant_status_t* ant_status));
    MOCK_METHOD1(switchCurrentAntenna,
            error_t(int8_t ant_mode));
    MOCK_METHOD0(getCurrentAntenna,
            int32_t());
    MOCK_METHOD2(switchAntennaPath,
            error_t(int8_t ant_mode, int8_t ant_ptah));
    MOCK_METHOD1(getAntennaPath,
            int32_t(int8_t ant_mode));

    MOCK_METHOD3(setCurrentTimeMs,
            error_t(int32_t, int64_t, bool));
    MOCK_METHOD0(getCurrentTime,
            tm());
    MOCK_METHOD0(getLastTimeType,
            int32_t());

    MOCK_METHOD1(setUSBCompostion,
            void(const usb_compostion_t& usb_compostion));

    MOCK_METHOD1(setTimeZone,
            error_t(int32_t time_zone));
    MOCK_METHOD0(getTimeZone,
            int32_t());

    MOCK_METHOD1(setDayLightSaving,
            error_t(int32_t flag));
    MOCK_METHOD0(getDayLightSaving,
            int32_t());

    MOCK_METHOD1(cmdControlFac,
            int32_t(fac_command_t& fac_cmd));
    MOCK_METHOD1(cmdControlGPIO,
            int32_t(const gpio_command_t& gpio_cmd));
    MOCK_METHOD1(cmdControlMCU,
            int32_t(int32_t cmd_mcu));

    MOCK_METHOD0(onAsBinder,
            android::IBinder*());
};

#endif  // TEST_DOUBLES_MOCK_SYSTEM_MANAGER_SERVICE_H_INCLUDED
