#include "Arduino.h"
#include "Eventy.h"

using namespace eventy;

Eventy eventy_manager;
bool led_status = LOW;

EventCollection toggleLed() {
    led_status = !led_status;
    digitalWrite(LED_BUILTIN, led_status);

    return {};
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    eventy_manager.registerTask(toggleLed, 1000);
}

void loop() {
}
