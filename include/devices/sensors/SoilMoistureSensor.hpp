#ifndef SOIL_MOISTURE_SENSOR_HPP
#define SOIL_MOISTURE_SENSOR_HPP

#include <Arduino.h>
#include "Device.hpp"

/**
 * @brief Soil moisture sensor that exposes analog readings through the generic Device API.
 *
 * The sensor outputs an analog voltage on its AOUT pin that maps to the moisture level of the soil.
 * This class samples that pin, applies optional averaging, and converts the reading to a percentage
 * based on user-provided calibration values (dry vs. wet). It inherits from Device so it can
 * participate in the existing DeviceRegistry/action queue system, even though it does not use I2C.
 */
class SoilMoistureSensor : public Device {
public:
    /**
     * @brief Construct a new SoilMoistureSensor
    * @param virtualAddress Virtual address used when interacting with DeviceRegistry (default 0x60)
    * @param analogPin MCU analog-capable pin connected to the sensor's AOUT pin (defaults to GPIO 1)
     */
    SoilMoistureSensor(uint8_t virtualAddress = 0x60, int analogPin = 1);

    /**
     * @brief Initialize the analog pin for moisture readings.
     * @return true Always returns true once the pin is prepared.
     */
    bool begin() override;

    /**
     * @brief Override the Device receive method so queued actions can fetch analog readings.
     *
     * Copies the most recent raw ADC value into the provided buffer (little-endian). If enough space
     * is provided (>2 bytes) the method also appends the moisture percentage as a 32-bit float.
     */
    bool receive(uint8_t* buffer, size_t length) override;

    /**
     * @brief Take a fresh averaged ADC reading.
     * @return Latest raw analog value (0-4095 on ESP32 by default).
     */
    uint16_t readRaw();

    /**
     * @brief Read the soil moisture as a percentage using the current calibration values.
     * @return Moisture percentage (0-100%).
     */
    float readMoisturePercent();

    /**
     * @brief Configure calibration reference points.
     * @param dryReading ADC reading measured in air (0% moisture)
     * @param wetReading ADC reading measured in water (100% moisture)
     */
    void setCalibration(uint16_t dryReading, uint16_t wetReading);

    /**
     * @brief Adjust the number of ADC samples averaged per reading (default 8, min 1, max 32).
     */
    void setSamplesPerReading(uint8_t samples);

    /**
     * @brief Retrieve the currently configured calibration points (dry and wet readings).
     */
    void getCalibration(uint16_t& dryReading, uint16_t& wetReading) const;

    /**
     * @brief Get the number of samples used per reading.
     */
    uint8_t getSamplesPerReading() const;

    uint16_t getLastRawReading() const;
    float getLastPercentReading() const;
    unsigned long getLastReadTimestamp() const;
    int getAnalogPin() const;

    /**
     * @brief Convert a raw ADC reading to moisture percentage using the current calibration.
     */
    float convertToPercentage(uint16_t rawReading) const;

private:
    int analog_pin;
    uint16_t dry_calibration;
    uint16_t wet_calibration;
    uint8_t samples_per_read;
    uint16_t last_raw_value;
    float last_percent_value;
    unsigned long last_read_timestamp;

    uint16_t sampleAnalog() const;
};

#endif // SOIL_MOISTURE_SENSOR_HPP
