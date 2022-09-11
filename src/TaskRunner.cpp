#include "eventy/TaskRunner.h"


namespace eventy {


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
