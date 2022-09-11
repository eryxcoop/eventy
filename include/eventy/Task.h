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


class TaskRunner {
public:
    TaskRunner(QueueTXHandler *queue_handler);
    ~TaskRunner();
    TaskHandle_t beginTimerTask(Task *task, unsigned int _timer_delay_in_ms, const char *name,
                       int stack_size, UBaseType_t priority, BaseType_t core);
    TaskHandle_t beginHardwareInterruptTask(Task *task, int pin, int debounce_ms, int pin_mode, int interrupt_mode , const char *name,
                       int stack_size, UBaseType_t priority, BaseType_t core);
private:
    class TaskData {
    public:
        Task *task = nullptr;
        ExecutionContext *execution_context = nullptr;
        TaskHandle_t task_handle = nullptr;
        QueueTXHandler *queue_handler = nullptr;
        void set(Task *, ExecutionContext *, QueueTXHandler *);
        void unset();
    };

    QueueTXHandler *_queue_handler = nullptr;
    BaseType_t getTaskHandleIndex(TaskHandle_t task_handle, int &index);
    BaseType_t getFreeSpot(int &index);
    TaskHandle_t begin(Task *task, const char *name, ExecutionContext *execution_context, int stack_size, UBaseType_t priority, BaseType_t core);

    TaskData _task_data_table[MAX_ALLOWED_RUNNING_TASKS];
    static void run(void *parameter);

};

}

#endif // EVENTY_TASK_H
