#ifndef NCI_MOCK_MSG_HANDLER_H
#define NCI_MOCK_MSG_HANDLER_H
#include "message_handler.h"

class NciMockMsgHandler : public osal::MessageHandler {
public:
    NciMockMsgHandler();
    virtual int HandleMessage(std::unique_ptr<osal::Message>&);
    static void SendNfcEvent(int eventId);
};
#endif /* NCI_MOCK_MSG_HANDLER_H */
