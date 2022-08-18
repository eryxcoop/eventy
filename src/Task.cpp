#include "eventy/Task.h"


namespace eventy {

void TaskRunner::TaskData::set(Task *task, unsigned int timer_delay_in_ms, QueueTXHandler *queue_handler) {
    this->task = task;
    this->timer_delay_in_ms = timer_delay_in_ms;
    this->queue_handler = queue_handler;
}

void TaskRunner::TaskData::unset() {
    this->task = nullptr;
    this->timer_delay_in_ms = 0;
    this->queue_handler = nullptr;
    this->task_handle = nullptr;
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

TaskHandle_t
TaskRunner::begin(
        Task *task,
        unsigned int timer_delay_in_ms,
        const char *name,
        int stack_size,
        UBaseType_t priority,
        BaseType_t core) {
    int index;
    if (getFreeSpot(index) == pdFALSE) {
        return nullptr;
    }

    _task_data_table[index].set(task, timer_delay_in_ms, _queue_handler);

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
    long initialization_delay = 1;
    while (1) {
        task->begin();
        while (1) {
            if (!task->isReady())
                break;

            EventCollection events = task->run();

            while (!events.isEmpty()) {
                EventPtr event_ptr = events.pop();
                running_task_data->queue_handler->send(event_ptr);
            }
            delay(running_task_data->timer_delay_in_ms);
        }
        int five_minutes = 5 * 60 * 1000;
        initialization_delay = initialization_delay * 10 < five_minutes ? initialization_delay * 10 : five_minutes;
        delay(initialization_delay);
    }
}

}

