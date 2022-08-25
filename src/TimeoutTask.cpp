#include "eventy/TimeoutTask.h"

namespace eventy {

TimeoutTask::TimeoutTask(EventCollection(*function)(void), int timeout) : _function(function), _timeout(timeout) {
}

EventCollection TimeoutTask::run() {
    vTaskSuspend(NULL);
    delay(_timeout);
    return _function();
}

bool TimeoutTask::isReady() {
    return true;
}

}
