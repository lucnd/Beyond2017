#ifndef TEST_DOUBLES_MOCK_SPECIAL_MODE_APPLICATION_H_INCLUDED
#define TEST_DOUBLES_MOCK_SPECIAL_MODE_APPLICATION_H_INCLUDED

#include "../../ISpecialModeApplication.h"
#include <gmock/gmock.h>

class MockSpecialModeApplication: public ISpecialModeApplication {
public:
    MOCK_METHOD1(getPropertyWrap,
            std::string(const char* name));

    void setPropertyChar(const char* name, const char* value, bool sync_now = false) override {
        mockSetPropertyChar(name, value, sync_now);
    }
    MOCK_METHOD3(mockSetPropertyChar,
            void(const char*, const char*, bool));

    void setPropertyInt(const char* name, const int32_t i_value, bool sync_now = true) override {
        mockSetPropertyInt(name, i_value, sync_now);
    }
    MOCK_METHOD3(mockSetPropertyInt,
            void(const char*, int32_t, bool));

    MOCK_METHOD1(doSpecialModeHandler,
            void(const sp<sl::Message>& message));
};

#endif  // TEST_DOUBLES_MOCK_SPECIAL_MODE_APPLICATION_H_INCLUDED
