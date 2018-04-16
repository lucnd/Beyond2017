#ifndef TEST_DOUBLES_PARTIAL_MOCK_HANDLER_H_INCLUDED
#define TEST_DOUBLES_PARTIAL_MOCK_HANDLER_H_INCLUDED


#include <utils/Handler.h>
#include <gmock/gmock.h>


class PartialMockHandler : public sl::Handler {
public:
    MOCK_METHOD1(handleMessage, void(const android::sp<sl::Message>&));
};


#endif  // TEST_DOUBLES_PARTIAL_MOCK_HANDLER_H_INCLUDED
