#include "SoilMoistureSensor.hpp"

#include <algorithm>
#include <cstring>

SoilMoistureSensor::SoilMoistureSensor(uint8_t virtualAddress, int analogPin)
    : Device(virtualAddress),
      analog_pin(analogPin),
      dry_calibration(3600),
      wet_calibration(1500),
      samples_per_read(8),
      last_raw_value(0),
      last_percent_value(0.0f),
      last_read_timestamp(0) {
    if (analog_pin < 0) {
        analog_pin = 1;
    }
}

bool SoilMoistureSensor::begin() {
    pinMode(analog_pin, INPUT);
    initialized = true;
    readRaw();
    return true;
}

bool SoilMoistureSensor::receive(uint8_t* buffer, size_t length) {
    if (buffer == nullptr || length < sizeof(uint16_t)) {
        return false;
    }

    uint16_t raw = readRaw();
    float percent = last_percent_value;

    buffer[0] = static_cast<uint8_t>(raw & 0xFF);
    buffer[1] = static_cast<uint8_t>((raw >> 8) & 0xFF);

    if (length >= sizeof(uint16_t) + sizeof(float)) {
        std::memcpy(buffer + sizeof(uint16_t), &percent, sizeof(float));
    }

    return true;
}

uint16_t SoilMoistureSensor::readRaw() {
    if (!initialized) {
        begin();
    }

    uint16_t sample = sampleAnalog();
    last_raw_value = sample;
    last_percent_value = convertToPercentage(sample);
    last_read_timestamp = millis();
    return sample;
}

float SoilMoistureSensor::readMoisturePercent() {
    readRaw();
    return last_percent_value;
}

void SoilMoistureSensor::setCalibration(uint16_t dryReading, uint16_t wetReading) {
    dry_calibration = dryReading;
    wet_calibration = wetReading;
}

void SoilMoistureSensor::setSamplesPerReading(uint8_t samples) {
    samples_per_read = constrain(samples, static_cast<uint8_t>(1), static_cast<uint8_t>(32));
}

void SoilMoistureSensor::getCalibration(uint16_t& dryReading, uint16_t& wetReading) const {
    dryReading = dry_calibration;
    wetReading = wet_calibration;
}

uint8_t SoilMoistureSensor::getSamplesPerReading() const {
    return samples_per_read;
}

uint16_t SoilMoistureSensor::getLastRawReading() const {
    return last_raw_value;
}

float SoilMoistureSensor::getLastPercentReading() const {
    return last_percent_value;
}

unsigned long SoilMoistureSensor::getLastReadTimestamp() const {
    return last_read_timestamp;
}

int SoilMoistureSensor::getAnalogPin() const {
    return analog_pin;
}

float SoilMoistureSensor::convertToPercentage(uint16_t rawReading) const {
    if (dry_calibration == wet_calibration) {
        return 0.0f;
    }

    uint16_t minCal = std::min(dry_calibration, wet_calibration);
    uint16_t maxCal = std::max(dry_calibration, wet_calibration);
    uint16_t clamped = rawReading;
    if (clamped < minCal) {
        clamped = minCal;
    } else if (clamped > maxCal) {
        clamped = maxCal;
    }

    float percent = static_cast<float>(clamped - dry_calibration) /
                    static_cast<float>(wet_calibration - dry_calibration);
    if (percent < 0.0f) {
        percent = 0.0f;
    } else if (percent > 1.0f) {
        percent = 1.0f;
    }
    return percent * 100.0f;
}

uint16_t SoilMoistureSensor::sampleAnalog() const {
    uint32_t total = 0;
    uint8_t iterations = samples_per_read == 0 ? 1 : samples_per_read;

    for (uint8_t i = 0; i < iterations; i++) {
        total += analogRead(analog_pin);
        delayMicroseconds(200);
    }

    return static_cast<uint16_t>(total / iterations);
}
