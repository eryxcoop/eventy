#ifndef EVENTY_QUEUE_BROKER_H
#define EVENTY_QUEUE_BROKER_H

#include "Event.h"
#include "Task.h"
#include "EventQueue.h"
#include "EventHandler.h"

#define MAX_NUMBER_MESSAGE_HANDLERS 15

namespace eventy {

class QueueBroker : public Task {
public:
    QueueBroker(QueueRXHandler *queue_handler);
    EventCollection run() override;
    BaseType_t registerEventHandler(Topic, EventHandler *);
    bool isReady() override { return true; } // TODO: implement
private:
    class HandlerTopicPair {
    public:
        EventHandler *event_handler;
        Topic topic;
    };

    bool topicMatches(Topic, Topic);
    QueueRXHandler *_queue_handler;
    char _num_registered_message_handlers;
    HandlerTopicPair _handler_topic_table[MAX_NUMBER_MESSAGE_HANDLERS];
};

}

#endif // EVENTY_QUEUE_BROKER_H
