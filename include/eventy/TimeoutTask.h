#ifndef EVENTY_TIMEOUT_TASK_H
#define EVENTY_TIMEOUT_TASK_H

#include "Arduino.h"
#include "Task.h"

namespace eventy {

class TimeoutTask : public Task {
public:
    TimeoutTask(EventCollection(*function)(void), int timeout);
    EventCollection run() override;
    bool isReady() override;
private:
    EventCollection (*_function)(void);
    int _timeout;
};

}

#endif // EVENTY_TIMEOUT_TASK_H
