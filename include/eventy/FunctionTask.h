#ifndef EVENTY_FUNCTIONTASK_H
#define EVENTY_FUNCTIONTASK_H

#include "Task.h"

namespace eventy {

class FunctionTask : public Task {
public:
    FunctionTask(EventCollection(*function)(void));
    EventCollection run() override;
    bool isReady() override;
private:
    EventCollection (*_function)(void);
};

} // eventy

#endif //EVENTY_FUNCTIONTASK_H
