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

class TaskRunner {
public:
    TaskRunner(QueueTXHandler *queue_handler);
    ~TaskRunner();
    TaskHandle_t begin(Task *task, unsigned int _timer_delay_in_ms, const char *name,
                       int stack_size, UBaseType_t priority, BaseType_t core);
    void stop(TaskHandle_t task_handle);
private:
    class TaskData {
    public:
        Task *task = nullptr;
        unsigned int timer_delay_in_ms = 0;
        QueueTXHandler *queue_handler = nullptr;
        TaskHandle_t task_handle = nullptr;
        void set(Task *, unsigned int, QueueTXHandler *);
        void unset();
    };

    QueueTXHandler *_queue_handler = nullptr;
    BaseType_t getTaskHandleIndex(TaskHandle_t task_handle, int &index);
    BaseType_t getFreeSpot(int &index);
    TaskData _task_data_table[MAX_ALLOWED_RUNNING_TASKS];
    static void run(void *parameter);

};

}

#endif // EVENTY_TASK_H
