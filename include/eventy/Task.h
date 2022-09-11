#ifndef EVENTY_TASK_H
#define EVENTY_TASK_H

#include "Event.h"
#include "EventQueue.h"

#define MAX_ALLOWED_RUNNING_TASKS 20

namespace eventy {

class Task {
public:
    virtual ~Task() = default;
    virtual void begin() {};
    virtual EventCollection run() = 0;
    virtual bool isReady() = 0;
};

class ExecutionContext {
public:
    virtual ~ExecutionContext() = default;
    void run(Task* task, QueueTXHandler *queue_handler);
    virtual void execute(Task *task, QueueTXHandler *queue_handler) = 0;
private:
    QueueTXHandler *_queue_handler;
};

class TimerExecutionContext : public ExecutionContext {
public:
    TimerExecutionContext(unsigned int timer_delay_in_ms);
    void execute(Task *task, QueueTXHandler *queue_handler);
private:
    unsigned int _timer_delay_in_ms;
};

class HardwareInterruptExecutionContext : public ExecutionContext {
public:
    HardwareInterruptExecutionContext(int pin, int debounce_ms, int pin_mode, int interrupt_mode);
    void execute(Task *task, QueueTXHandler *queue_handler);
private:
    static void _callback(void*);
    int _debounce_ms, _pin_mode, _interrupt_mode;
    QueueHandle_t _event_queue = nullptr;
};

}

#endif // EVENTY_TASK_H
