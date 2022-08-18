#ifndef EVENTY_MESSAGE_HANDLER_H
#define EVENTY_MESSAGE_HANDLER_H

#include "Arduino.h"

#include "Event.h"

namespace eventy {

class EventHandler {
public:
    virtual EventCollection handle(EventPtr event_ptr) = 0;
};

template<class T>
class FunctionEventHandler : public EventHandler {
public:
    FunctionEventHandler(EventCollection(*function)(Event<T>)) {
        _function = function;
    }
    EventCollection handle(EventPtr event_ptr) {
        return _function(*((Event<T> *) event_ptr));
    }
private:
    EventCollection (*_function)(Event<T>);
};

} // namespace eventy

#endif  // EVENTY_MESSAGE_HANDLER_H
