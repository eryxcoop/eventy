#include "Arduino.h"
#include "Eventy.h"

#define BUTTON_PIN 14

using namespace eventy;

Eventy eventy_manager;

EventCollection onInterrupt() {
    Serial.println("button pressed!");
    return {};
}

void setup() {
    Serial.begin(115200);
    eventy_manager.registerHardwareInterrupt(BUTTON_PIN, onInterrupt, 500, INPUT_PULLUP, FALLING);
}

void loop() {
    vTaskDelete(NULL);
}
