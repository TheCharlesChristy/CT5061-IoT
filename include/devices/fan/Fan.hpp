#ifndef FAN_HPP
#define FAN_HPP

#include <Arduino.h>
#include "Device.hpp"

/**
 * @brief PWM-controlled fan device that fits into the Device framework.
 *
 * The physical fan typically exposes only VIN and GND pins for power.
 * This wrapper assumes the VIN rail is switched through a transistor that is
 * driven from a configurable PWM-capable MCU pin. The device stores the
 * requested duty cycle (0-255) and exposes it through send/receive wrappers so
 * it can participate in the DeviceRegistry queue like I2C devices.
 */
class Fan : public Device {
public:
    Fan(uint8_t virtualAddress = 0x70,
        int pwmPin = 2,
        uint32_t pwmFrequency = 25000,
        uint8_t resolutionBits = 8,
        uint8_t ledcChannel = 0);

    bool begin() override;

    bool send(const uint8_t* data, size_t length) override;
    bool receive(uint8_t* buffer, size_t length) override;

    bool setSpeed(uint8_t speed);
    bool setSpeedPercent(float percent);

    uint8_t getSpeed() const;
    float getSpeedPercent() const;
    int getPwmPin() const;

private:
    int pwm_pin;
    uint32_t pwm_frequency;
    uint8_t pwm_resolution_bits;
    uint8_t ledc_channel;
    uint8_t current_speed;
    bool pwm_ready;

    void applySpeed();
    uint32_t resolutionMax() const;
};

#endif // FAN_HPP
