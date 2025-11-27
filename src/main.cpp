#include <Arduino.h>
#include "DeviceRegistry.hpp"
#include "FanTestApp.hpp"

#ifndef FAN_PWM_PIN
#define FAN_PWM_PIN 2
#endif

static FanTestApp fanApp(FAN_PWM_PIN);
static bool fanReady = false;

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println();
    Serial.println(F("========================================"));
    Serial.println(F("  Fan Test Application"));
    Serial.println(F("========================================"));

    fanReady = fanApp.begin();
}

void loop() {
    if (fanReady) {
        fanApp.loop();
    }

    DeviceRegistry& registry = DeviceRegistry::getInstance();
    if (registry.hasPendingActions()) {
        registry.performNextAction();
    }
}