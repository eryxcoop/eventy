#ifndef EVENTY_TASK_RUNNER_H
#define EVENTY_TASK_RUNNER_H

#include "Task.h"

namespace eventy {

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
#endif // EVENTY_TASK_RUNNER_H