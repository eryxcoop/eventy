#include "eventy/QueueBroker.h"


namespace eventy {

QueueBroker::QueueBroker(QueueRXHandler *queue_handler) :
        _queue_handler(queue_handler),
        _num_registered_message_handlers(0) {
}

BaseType_t QueueBroker::registerEventHandler(Topic topic, EventHandler *message_handler) {
    if (_num_registered_message_handlers >= MAX_NUMBER_MESSAGE_HANDLERS)
        return pdFALSE;

    _handler_topic_table[_num_registered_message_handlers].event_handler = message_handler;
    _handler_topic_table[_num_registered_message_handlers].topic = topic;
    _num_registered_message_handlers++;
    return pdTRUE;
}

bool QueueBroker::topicMatches(Topic topic_left, Topic topic_right) {
    if (topic_right[strlen(topic_right) - 1] == '#')
        return strncmp(topic_left, topic_right, strlen(topic_right) - 1) == 0;
    return strcmp(topic_left, topic_right) == 0;
}

EventCollection QueueBroker::run() {
    EventCollection events;

    while (true) {
        EventPtr event_ptr = _queue_handler->receive();
        if (event_ptr == nullptr)
            break;

        for (int i = 0; i < _num_registered_message_handlers; i++) {
            if (topicMatches(event_ptr->topic(), _handler_topic_table[i].topic)) {
                EventCollection handler_events = _handler_topic_table[i].event_handler->handle(event_ptr);
                while (!handler_events.isEmpty()) {
                    events.push(handler_events.pop());
                }
            }
        }
        EventFactory::destroy(event_ptr);
    }
    return events;
}

}
