#include "eventy/FunctionTask.h"

namespace eventy {

FunctionTask::FunctionTask(EventCollection(*function)(void)) {
    _function = function;
}

bool FunctionTask::isReady() {
    return true;
}

EventCollection FunctionTask::run() {
    return _function();
}

} // eventy
