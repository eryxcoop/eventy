#ifndef EVENTY_H
#define EVENTY_H

#include "QueueBroker.h"
#include "EventQueue.h"
#include "Event.h"
#include "EventHandler.h"
#include "Task.h"
#include "TaskRunner.h"
#include "FunctionTask.h"

#define MAX_ALLOWED_FUNCTION_TASKS 20
#define MAIN_EVENTS_QUEUE_MAX_SIZE 300
#define DEFAULT_TASK_STACK_SIZE 2048
#define DEFAULT_TASK_CORE 1
#define DEFAULT_TASK_NAME "EventyTask"

namespace eventy {

class Eventy {
public:
    Eventy(unsigned int poll_delay_in_ms = 5, int stack_size = 4096, BaseType_t core = 1);
    ~Eventy();
    QueueTXHandler *getEventTXHandler();

    // TimerTask
    TaskHandle_t registerTimerTask(Task *task, unsigned int timer_delay_in_ms, const char *name = DEFAULT_TASK_NAME,
                              int stack_size = DEFAULT_TASK_STACK_SIZE, UBaseType_t priority = tskIDLE_PRIORITY,
                              BaseType_t core = DEFAULT_TASK_CORE);
    TaskHandle_t registerTimerTask(EventCollection(*function)(void), unsigned int timer_delay_in_ms,
                              const char *name = DEFAULT_TASK_NAME, int stack_size = DEFAULT_TASK_STACK_SIZE,
                              UBaseType_t priority = tskIDLE_PRIORITY, BaseType_t core = DEFAULT_TASK_CORE);

    // HardwareInterrupt
    TaskHandle_t registerHardwareInterruptTask(Task *task, int pin, int debounce_ms, int pin_mode, 
                              int interrupt_mode, const char *name = DEFAULT_TASK_NAME,
                              int stack_size = DEFAULT_TASK_STACK_SIZE, UBaseType_t priority = tskIDLE_PRIORITY,
                              BaseType_t core = DEFAULT_TASK_CORE);
    TaskHandle_t registerHardwareInterruptTask(EventCollection(*function)(void), int pin, int debounce_ms, 
                              int pin_mode, int interrupt_mode, const char *name = DEFAULT_TASK_NAME,
                              int stack_size = DEFAULT_TASK_STACK_SIZE, UBaseType_t priority = tskIDLE_PRIORITY,
                              BaseType_t core = DEFAULT_TASK_CORE);

    // EventHandler
    BaseType_t registerEventHandler(Topic topic, EventHandler *message_handler);
    template<class T>
    EventHandler *registerEventHandler(Topic topic, EventCollection(*function)(Event<T>));
private:
    class TaskData {
    public:
        Task *task = nullptr;
        TaskHandle_t task_handle = nullptr;
    };

    BaseType_t getFreeSpot(int &index);
    BaseType_t getTaskHandleIndex(TaskHandle_t task_handle, int &index);

    TaskData _task_table[MAX_ALLOWED_FUNCTION_TASKS];
    QueueHandle_t _event_queue = nullptr;
    QueueTXHandler *_event_tx_handler = nullptr;
    QueueRXHandler *_event_rx_handler = nullptr;
    TaskRunner *_task_runner = nullptr;
    QueueBroker *_event_manager = nullptr;
};

template<class T>
EventHandler *Eventy::registerEventHandler(Topic topic, EventCollection(*function)(Event<T>)) {
    FunctionEventHandler<T> *event_handler = new FunctionEventHandler<T>(function);
    if (registerEventHandler(topic, event_handler) != pdFALSE)
        return event_handler;
    delete event_handler;
    return nullptr;
}

}

#endif // EVENTY_H
