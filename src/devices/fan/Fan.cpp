#include "Fan.hpp"

#include <cstring>

#ifndef FAN_DEFAULT_PWM_PIN
#define FAN_DEFAULT_PWM_PIN 2
#endif

Fan::Fan(uint8_t virtualAddress,
         int pwmPin,
         uint32_t pwmFrequency,
         uint8_t resolutionBits,
         uint8_t ledcChannel)
    : Device(virtualAddress),
      pwm_pin(pwmPin),
      pwm_frequency(pwmFrequency),
      pwm_resolution_bits(resolutionBits),
      ledc_channel(ledcChannel),
      current_speed(0),
      pwm_ready(false) {
    if (pwm_pin < 0) {
        pwm_pin = FAN_DEFAULT_PWM_PIN;
    }
}

bool Fan::begin() {
    if (initialized) {
        return true;
    }

    if (pwm_pin < 0) {
        return false;
    }

#if defined(ARDUINO_ARCH_ESP32)
    ledcSetup(ledc_channel, pwm_frequency, pwm_resolution_bits);
    ledcAttachPin(pwm_pin, ledc_channel);
#else
    pinMode(pwm_pin, OUTPUT);
#endif
    pwm_ready = true;
    initialized = true;
    applySpeed();
    return true;
}

bool Fan::send(const uint8_t* data, size_t length) {
    if (data == nullptr || length == 0) {
        return false;
    }
    return setSpeed(data[0]);
}

bool Fan::receive(uint8_t* buffer, size_t length) {
    if (buffer == nullptr || length == 0) {
        return false;
    }

    buffer[0] = current_speed;

    if (length >= sizeof(uint8_t) + sizeof(float)) {
        float percent = getSpeedPercent();
        std::memcpy(buffer + sizeof(uint8_t), &percent, sizeof(float));
    }

    return true;
}

bool Fan::setSpeed(uint8_t speed) {
    current_speed = speed;
    if (!initialized) {
        begin();
    }
    applySpeed();
    return true;
}

bool Fan::setSpeedPercent(float percent) {
    if (percent < 0.0f) {
        percent = 0.0f;
    } else if (percent > 100.0f) {
        percent = 100.0f;
    }
    uint8_t scaled = static_cast<uint8_t>((percent / 100.0f) * 255.0f);
    return setSpeed(scaled);
}

uint8_t Fan::getSpeed() const {
    return current_speed;
}

float Fan::getSpeedPercent() const {
    return (static_cast<float>(current_speed) / 255.0f) * 100.0f;
}

int Fan::getPwmPin() const {
    return pwm_pin;
}

uint32_t Fan::resolutionMax() const {
    if (pwm_resolution_bits == 0 || pwm_resolution_bits > 16) {
        return 255;
    }
    return (1UL << pwm_resolution_bits) - 1UL;
}

void Fan::applySpeed() {
    if (!pwm_ready) {
        return;
    }

#if defined(ARDUINO_ARCH_ESP32)
    uint32_t maxDuty = resolutionMax();
    uint32_t duty = (static_cast<uint32_t>(current_speed) * maxDuty) / 255UL;
    ledcWrite(ledc_channel, duty);
#else
    analogWrite(pwm_pin, current_speed);
#endif
}
