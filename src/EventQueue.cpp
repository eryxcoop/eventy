#include "eventy/EventQueue.h"

namespace eventy {

QueueTXHandler::QueueTXHandler(QueueHandle_t queue_handle) : _queue_handle(queue_handle) {
}

BaseType_t QueueTXHandler::send(EventPtr event_ptr) {
    BaseType_t result = xQueueSend(_queue_handle, (void *) (&event_ptr), 10);
    return result;
}

QueueRXHandler::QueueRXHandler(QueueHandle_t queue_handle) : _queue_handle(queue_handle) {
}

EventPtr QueueRXHandler::receive() {
    EventPtr event_ptr = nullptr;
    BaseType_t result = xQueueReceive(_queue_handle, (void *) (&event_ptr), 10);
    if (result == pdFALSE)
        return nullptr;
    return event_ptr;
}

}
