#include "SpecialModeConfigurationReceiver.h"
#include "SpecialModeHandler.h"
#include <utils/Message.h>

void SpecialModeConfigurationReceiver::onConfigDataChanged(sp<Buffer>& name) {
    sp<sl::Message> message = m_Handler->obtainMessage(E_FROM_CONFIG);
    if(name->size() > 0) {
        message->buffer.setTo(name->data(), name->size());
    }
    message->sendToTarget();
}
