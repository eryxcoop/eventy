#include "eventy/Task.h"


namespace eventy {


void ExecutionContext::run(Task* task, QueueTXHandler *queue_handler) {
    long initialization_delay = 1;
    while (1) {
        task->begin();
        if(task->isReady()) {
            execute(task, _queue_handler); // blocking function
        }
        int five_minutes = 5 * 60 * 1000;
        initialization_delay = initialization_delay * 10 < five_minutes ? initialization_delay * 10 : five_minutes;
        delay(initialization_delay);
    }
}


TimerExecutionContext::TimerExecutionContext(unsigned int timer_delay_in_ms) : _timer_delay_in_ms(timer_delay_in_ms) {
}


void TimerExecutionContext::execute(Task* task, QueueTXHandler *queue_handler) {
    while (1) {
        EventCollection events = task->run();

        while (!events.isEmpty()) {
            EventPtr event_ptr = events.pop();
            queue_handler->send(event_ptr);
        }
        delay(_timer_delay_in_ms);
    }
}


HardwareInterruptExecutionContext::HardwareInterruptExecutionContext(int pin, int debounce_ms, int pin_mode, int interrupt_mode) : 
    _debounce_ms(debounce_ms), _pin_mode(pin_mode), _interrupt_mode(interrupt_mode) 
{
    _event_queue = xQueueCreate(1, sizeof(void*));
    attachInterruptArg(pin, _callback, this, interrupt_mode);
}

void HardwareInterruptExecutionContext::_callback(void* instance) {
    Serial.println("CALLBACK");
    void *dummy = nullptr;
    xQueueSend(((HardwareInterruptExecutionContext*)instance)->_event_queue, &dummy, 1);
}

void HardwareInterruptExecutionContext::execute(Task* task, QueueTXHandler *queue_handler) {
    while (1) {
        void* dummy;
        if (xQueueReceive(_event_queue, &dummy, portMAX_DELAY) == pdTRUE) {
            Serial.println("RECEIVED");

            EventCollection events = task->run();

            while (!events.isEmpty()) {
                EventPtr event_ptr = events.pop();
                queue_handler->send(event_ptr);
            }
            delay(_debounce_ms);
            xQueueReset(_event_queue);
        }
    }
}

void TaskRunner::TaskData::set(Task *task, ExecutionContext *execution_context, QueueTXHandler *queue_handler) {
    this->task = task;
    this->execution_context = execution_context;
    this->queue_handler = queue_handler;
}

void TaskRunner::TaskData::unset() {
    this->task = nullptr;
    this->execution_context = nullptr;
    this->task_handle = nullptr;
    this->queue_handler = nullptr;
}

TaskRunner::TaskRunner(QueueTXHandler *queue_handler) : _queue_handler(queue_handler) {
}

TaskRunner::~TaskRunner() {
    for (int i = 0; i < MAX_ALLOWED_RUNNING_TASKS; i++) {
        if (_task_data_table[i].task_handle != nullptr) {
            vTaskDelete(_task_data_table[i].task_handle);
        }
    }
}

BaseType_t TaskRunner::getTaskHandleIndex(TaskHandle_t task_handle, int &index) {
    for (int i = 0; i < MAX_ALLOWED_RUNNING_TASKS; i++) {
        if (_task_data_table[i].task_handle == task_handle) {
            index = i;
            return pdTRUE;
        }
    }
    return pdFALSE;
}

BaseType_t TaskRunner::getFreeSpot(int &index) {
    return getTaskHandleIndex(nullptr, index);
}

TaskHandle_t TaskRunner::beginTimerTask(
        Task *task,
        unsigned int timer_delay_in_ms,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    TimerExecutionContext *execution_context = new TimerExecutionContext(timer_delay_in_ms);
    TaskHandle_t task_handle = begin(task, name, execution_context, stack_size, priority, core);
    if(task_handle == nullptr) {
        delete execution_context;
    }
    return task_handle;
}

TaskHandle_t TaskRunner::beginHardwareInterruptTask(
        Task *task,
        int pin,
        int debounce_ms,
        int pin_mode,
        int interrupt_mode,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    HardwareInterruptExecutionContext *execution_context = new HardwareInterruptExecutionContext(pin, debounce_ms, pin_mode, interrupt_mode);
    TaskHandle_t task_handle = begin(task, name, execution_context, stack_size, priority, core);
    if(task_handle == nullptr) {
        delete execution_context;
    }
    return task_handle;
}

TaskHandle_t TaskRunner::begin(Task *task, const char* name, ExecutionContext *execution_context, int stack_size, UBaseType_t priority, BaseType_t core) {
    int index;
    if (getFreeSpot(index) == pdFALSE) {
        return nullptr;
    }

    _task_data_table[index].set(task, execution_context, _queue_handler);

    BaseType_t status = xTaskCreatePinnedToCore(
        run,
        name,
        stack_size,
        &_task_data_table[index],
        priority,
        &_task_data_table[index].task_handle,
        core);

    if (status != pdTRUE) {
        _task_data_table[index].unset();
        return nullptr;
    }

    return _task_data_table[index].task_handle;
}

void TaskRunner::run(void *parameter) {
    TaskData *running_task_data = static_cast<TaskData *>(parameter);
    Task *task = running_task_data->task;
    QueueTXHandler *queue_handler = running_task_data->queue_handler;
    ExecutionContext *execution_context = running_task_data->execution_context;
    execution_context->run(task, queue_handler);
}

}

