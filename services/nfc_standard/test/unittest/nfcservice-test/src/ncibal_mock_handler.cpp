#include "ncibal_mock_handler.h"
static const std::string NCI_MOCK_SERVICE_NAME = "NCI-MOCK";
NciMockMsgHandler::NciMockMsgHandler() : MessageHandler(NCI_MOCK_SERVICE_NAME) {}

int NciMockMsgHandler::HandleMessage(std::unique_ptr<osal::Message>&)
{
    return 0;
}

void NciMockMsgHandler::SendNfcEvent(int eventId) {}
