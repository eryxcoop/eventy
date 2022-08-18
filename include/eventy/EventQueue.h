#ifndef EVENTY_MESSAGE_QUEUE_H
#define EVENTY_MESSAGE_QUEUE_H

#include "Arduino.h"
#include "Event.h"

namespace eventy {

class QueueTXHandler {
public:
    QueueTXHandler(QueueHandle_t queue_handle);
    BaseType_t send(EventPtr message);
private:
    QueueHandle_t _queue_handle;
};

class QueueRXHandler {
public:
    QueueRXHandler(QueueHandle_t queue_handle);
    EventPtr receive();
private:
    QueueHandle_t _queue_handle;
};

}

#endif  // EVENTY_MESSAGE_QUEUE_H
