#ifndef EVENTY_MESSAGE_H
#define EVENTY_MESSAGE_H

#include "Arduino.h"
#include "Stack.tpp"

#define EVENT_TYPE_UNDEFINED    0
#define EVENT_TYPE_CSTRING      1
#define EVENT_TYPE_STRING       2
#define EVENT_TYPE_BOOL         3
#define EVENT_TYPE_INT          4
#define EVENT_TYPE_UINT         5
#define EVENT_TYPE_LONG         6
#define EVENT_TYPE_ULONG        7
#define EVENT_TYPE_FLOAT        8
#define EVENT_TYPE_DOUBLE       9

#define EVENT_TYPE_WITHOUT_VALUE EVENT_TYPE_BOOL

#define EVENT_TYPE_REGISTER(T, code) template<> eventy::EventType eventy::Event<T>::type() {return code;}

namespace eventy {

typedef const char *Topic;

typedef unsigned char EventType;

class EventBase {
public:
    virtual ~EventBase() = default;
    virtual EventType type() = 0;
    virtual Topic topic() = 0;
};

template<class T>
class Event : public EventBase {
public:
    EventType type() override;
    Topic topic() override { return _topic; }
    T value() { return _value; }
private:
    Event(Topic topic, T value) : _topic(topic), _value(value) {}
    Topic _topic;
    T _value;

    friend class EventFactory;
};

class EventFactory {
public:
    static Event<bool> *create(Topic topic) { return new Event<bool>(topic, true); }

    template<class T>
    static Event<T> *create(Topic topic, T value) { return new Event<T>(topic, value); }

    static void destroy(EventBase *event) { delete event; }
};

typedef Event<bool> EventWithoutValue;

template<class T>
inline EventType Event<T>::type() { return EVENT_TYPE_UNDEFINED; }

template<>
inline EventType Event<const char *>::type() { return EVENT_TYPE_CSTRING; }

template<>
inline EventType Event<String>::type() { return EVENT_TYPE_STRING; }

template<>
inline EventType Event<bool>::type() { return EVENT_TYPE_BOOL; }

template<>
inline EventType Event<int>::type() { return EVENT_TYPE_INT; }

template<>
inline EventType Event<unsigned int>::type() { return EVENT_TYPE_UINT; }

template<>
inline EventType Event<long>::type() { return EVENT_TYPE_LONG; }

template<>
inline EventType Event<unsigned long>::type() { return EVENT_TYPE_ULONG; }

template<>
inline EventType Event<float>::type() { return EVENT_TYPE_FLOAT; }

template<>
inline EventType Event<double>::type() { return EVENT_TYPE_DOUBLE; }

typedef EventBase *EventPtr;

typedef Stack<EventPtr> EventCollection;

}

#endif // EVENTY_MESSAGE_H
