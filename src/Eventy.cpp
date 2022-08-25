#include "eventy/Eventy.h"

namespace eventy {

QueueTXHandler *Eventy::getEventTXHandler() {
    return _event_tx_handler;
}

BaseType_t Eventy::registerEventHandler(Topic topic, EventHandler *message_handler) {
    return _event_manager->registerEventHandler(topic, message_handler);
}

Eventy::Eventy(unsigned int poll_delay_in_ms, int stack_size, BaseType_t core) {
    _event_queue = xQueueCreate(MAIN_EVENTS_QUEUE_MAX_SIZE, sizeof(EventPtr));
    _event_tx_handler = new QueueTXHandler(_event_queue);
    _event_rx_handler = new QueueRXHandler(_event_queue);
    _task_runner = new TaskRunner(_event_tx_handler);
    _event_manager = new QueueBroker(_event_rx_handler);
    // TODO: Check that the following succeeds and if not somehow inform the user
    registerTask(_event_manager, poll_delay_in_ms, "Eventy::EventManager", stack_size, core);
}

Eventy::~Eventy() {
    for (int i = 0; i < MAX_ALLOWED_FUNCTION_TASKS; i++) {
        if (_function_task_table[i].task_handle != nullptr) {
            delete _function_task_table[i].task;
        }
    }
    delete _event_manager;
    delete _task_runner;
    delete _event_rx_handler;
    delete _event_tx_handler;
    vQueueDelete(_event_queue);
}

BaseType_t Eventy::getTaskHandleIndex(TaskHandle_t task_handle, int &index) {
    for (int i = 0; i < MAX_ALLOWED_FUNCTION_TASKS; i++) {
        if (_function_task_table[i].task_handle == task_handle) {
            index = i;
            return pdTRUE;
        }
    }
    return pdFALSE;
}

BaseType_t Eventy::getFreeSpot(int &index) {
    return getTaskHandleIndex(nullptr, index);
}

TaskHandle_t Eventy::initializeFunctionTask(TaskHandle_t task_handle, Task *task) {
    int index;
    if ((getFreeSpot(index) == pdFALSE) || (task_handle == nullptr)) {
        if (task_handle != nullptr) {
            _task_runner->stop(task_handle);
        }
        delete task;
        return nullptr;
    }

    _function_task_table[index].task = task;
    _function_task_table[index].task_handle = task_handle;
    return task_handle;
}

TaskHandle_t Eventy::registerTask(
        Task *task,
        unsigned int timer_delay_in_ms,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    
    return _task_runner->begin(task, timer_delay_in_ms, name, stack_size, priority, core);
}

TaskHandle_t Eventy::registerTask(
        EventCollection(*function)(void),
        unsigned int timer_delay_in_ms,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    Task *task = new FunctionTask(function);
    return initializeFunctionTask(registerTask(task, timer_delay_in_ms, name, stack_size, priority, core), task);
}

TaskHandle_t Eventy::registerHardwareInterrupt(
        int pin,
        EventCollection(*function)(void),
        int debounce_ms,
        int pin_mode,
        int interrupt_mode,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    Task *task = new TimeoutTask(function, debounce_ms);
    TaskHandle_t task_handle = initializeFunctionTask(registerTask(task, 5, name, stack_size, priority, core), task);
    if (task_handle != nullptr) {
        pinMode(pin, pin_mode);
        attachInterruptArg(digitalPinToInterrupt(pin), [](TaskHandle_t handle){xTaskResumeFromISR(handle);}, task_handle, interrupt_mode);
    }
    return task_handle;
}

}
