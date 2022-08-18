#ifndef EVENTY_BUTTON_TASK_H
#define EVENTY_BUTTON_TASK_H

#include "Arduino.h"
#include "Task.h"

namespace eventy {

class ButtonTask : public Task {
public:
    ButtonTask(const char *topic, int pin);
    void begin() override;
    EventCollection run() override;
    bool isReady() override;
private:
    const char *_topic;
    int _pin;
    portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
    bool _is_pressed = false;
    static void IRAM_ATTR handleButtonInterrupt(void *instance_ptr);
};

}

#endif // EVENTY_BUTTON_TASK_H
