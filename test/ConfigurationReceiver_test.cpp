#include "SpecialModeConfigurationReceiver.h"
#include "doubles/PartialMockHandler.h"
#include "utils/makeSp.h"


using namespace ::testing;


class ConfigurationReceiverTest : public Test {
protected:
    ConfigurationReceiverTest() :
        mpHandler(makeSp<PartialMockHandler>()),
        mpBuffer(makeSp<Buffer>()) {}

    const sp<PartialMockHandler> mpHandler;
    sp<Buffer> mpBuffer;  // can't be const here because onConfigDataChanged isn't const-correct
};


TEST_F(ConfigurationReceiverTest, onConfigDataChangedEmptyBuffer) {
    SpecialModeConfigurationReceiver receiver(mpHandler);
    receiver.onConfigDataChanged(mpBuffer);
}


TEST_F(ConfigurationReceiverTest, onConfigDataChangedNonEmptyBuffer) {
    SpecialModeConfigurationReceiver receiver(mpHandler);
    mpBuffer->setSize(1);
    receiver.onConfigDataChanged(mpBuffer);
}
