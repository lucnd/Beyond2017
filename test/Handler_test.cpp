#include "../SpecialModeHandler.h"
#include "doubles/MockSpecialModeApplication.h"
#include "utils/makeSp.h"


using namespace ::testing;


TEST(Handler, handleMessagePreserveMessageContent) {
    auto pMsgToHandler = sl::Message::obtain();
    pMsgToHandler->what = -1;
    pMsgToHandler->arg1 = 1;
    pMsgToHandler->arg2 = 2;
    pMsgToHandler->arg3 = 3;

    sp<sl::Message> pMsgToApp;
    auto savePointer = [&](const sp<sl::Message>& pMsg) {pMsgToApp = pMsg;};

    MockSpecialModeApplication mockApp;
    EXPECT_CALL(mockApp, doSpecialModeHandler(_))
        .WillOnce(Invoke(savePointer));

    auto pLooper = makeSp<sl::SLLooper>();
    SpecialModeHandler handler(pLooper, mockApp);
    handler.handleMessage(pMsgToHandler);

    EXPECT_EQ(-1, pMsgToApp->what);
    EXPECT_EQ(1, pMsgToApp->arg1);
    EXPECT_EQ(2, pMsgToApp->arg2);
    EXPECT_EQ(3, pMsgToApp->arg3);
}
