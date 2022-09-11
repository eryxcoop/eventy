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
    void *dummy = nullptr;
    xQueueSend(((HardwareInterruptExecutionContext*)instance)->_event_queue, &dummy, 1);
}

void HardwareInterruptExecutionContext::execute(Task* task, QueueTXHandler *queue_handler) {
    while (1) {
        void* dummy;
        if (xQueueReceive(_event_queue, &dummy, portMAX_DELAY) == pdTRUE) {
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


}
