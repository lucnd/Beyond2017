#ifndef MOCK_CONFIGURATION_MANAGER_SERVICE_H
#define MOCK_CONFIGURATION_MANAGER_SERVICE_H

#include <services/ConfigurationManagerService/IConfigurationManagerService.h>

#include <gmock/gmock.h>

class MockConfigurationManagerService: public IConfigurationManagerService {
public:
    MOCK_CONST_METHOD0(getInterfaceDescriptor,
            const String16&());

    MOCK_METHOD3(get_int,
            int32_t(const char* file_name, int32_t style, const char* lookup_data));
    MOCK_METHOD3(get_string,
            sp<Buffer>(const char* file_name, int32_t style, const char* lookup_data));
    MOCK_METHOD3(get,
            sp<ConfigurationBuffer>(const char* file_name, int32_t style, const char* lookup_data));
    MOCK_METHOD3(get_write_permission,
            error_t(const char* file_name, int32_t style, const char* lookup_data));
    MOCK_METHOD3(get_data_packet,
            sp<configDataStructure>(const char* file_name, int32_t style, const char* lookup_data));

    MOCK_METHOD4(change_set_int,
            error_t(const char* file_name, int32_t style, const char* lookup_data, int change_value));
    MOCK_METHOD4(change_set_string,
            error_t(const char* file_name, int32_t style, const char* lookup_data, const char* change_value));
    MOCK_METHOD4(change_set,
            error_t(const char* file_name, int32_t style, const char* lookup_data, sp<ConfigurationBuffer> change_value));
    MOCK_METHOD4(change_write_permission,
            error_t(const char* file_name, int32_t style, const char* lookup_data, int32_t command));

    MOCK_METHOD5(new_set_int,
            error_t(const char* file_name, const char* user, const char* id, const char* name, int32_t value));
    MOCK_METHOD5(new_set_string,
            error_t(const char* file_name, const char* user, const char* id, const char* name, const char* value));

    MOCK_METHOD1(updateConfiguration,
            error_t(android::sp<configDataStructure>));
    MOCK_METHOD1(updateMutexLock,
            error_t(uint32_t lock_on_off));

    MOCK_METHOD3(registerReceiver,
            error_t(const char* file_name, const char* parameter_name, sp<IConfigurationManagerReceiver>&));
    MOCK_METHOD3(unregisterReceiver,
            error_t(const char* file_name, const char* parameter_name, sp<IConfigurationManagerReceiver>&));

    MOCK_METHOD1(get_countryDB,
            sp<CountryDBBuffer>(uint32_t mcc));
    MOCK_METHOD1(set_countryDB,
            error_t(sp<CountryDBBuffer>& input));
    MOCK_METHOD0(save_countryDB,
            error_t());

    MOCK_METHOD1(factory_save,
            error_t(const char* file_name));
    MOCK_METHOD1(factory_reset,
            error_t(const char* file_name));

    MOCK_METHOD1(clear_country_DB,
            error_t(const char* new_file));
    MOCK_METHOD0(direct_save_file,
            void());

    MOCK_METHOD3(deleteConfiguration,
            error_t(const char* file_name, int32_t style, const char* lookup_data));

    MOCK_METHOD0(onAsBinder,
            IBinder*());
};

#endif  // MOCK_CONFIGURATION_MANAGER_SERVICE
