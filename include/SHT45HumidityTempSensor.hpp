#ifndef SHT45_HUMIDITY_TEMP_SENSOR_HPP
#define SHT45_HUMIDITY_TEMP_SENSOR_HPP

#include <Arduino.h>
#include <Adafruit_SHT4x.h>
#include "Device.hpp"

/**
 * @brief SHT45 Humidity and Temperature Sensor Class
 * 
 * This class provides an interface for the Adafruit SHT45 high-precision
 * temperature and humidity sensor. The SHT45 offers:
 * - ±1% typical relative humidity accuracy from 25 to 75%
 * - ±0.1°C typical temperature accuracy from 0 to 75°C
 * - I2C communication interface
 * - Multiple precision modes for different power/speed trade-offs
 * 
 * Inherits from the Device base class to provide standard I2C operations.
 */
class SHT45HumidityTempSensor : public Device {
private:
    Adafruit_SHT4x* sht45;          // Pointer to Adafruit SHT4x sensor object
    bool sensor_initialized;         // Tracks if sensor is properly initialized
    
    // Cached readings
    float last_temperature;          // Last temperature reading in Celsius
    float last_humidity;             // Last humidity reading in %RH
    unsigned long last_read_time;    // Timestamp of last reading
    
    // Reading validity check
    static const unsigned long READ_TIMEOUT = 5000; // 5 seconds timeout
    
public:
    /**
     * @brief Constructor for SHT45HumidityTempSensor
     * @param address I2C address of the SHT45 sensor (default 0x44)
     * 
     * Note: The SHT45 uses I2C address 0x44 by default
     */
    SHT45HumidityTempSensor(uint8_t address = 0x44);
    
    /**
     * @brief Destructor - cleans up allocated sensor object
     */
    ~SHT45HumidityTempSensor();
    
    /**
     * @brief Initialize the SHT45 sensor
     * @return true if initialization successful, false otherwise
     * 
     * This method initializes the I2C connection and verifies the sensor
     * is responding correctly. Should be called in setup() before using
     * any other sensor methods.
     */
    bool begin() override;
    
    /**
     * @brief Set the precision mode of the sensor
     * @param precision Precision mode (SHT4X_HIGH_PRECISION, SHT4X_MED_PRECISION, SHT4X_LOW_PRECISION)
     * 
     * Higher precision modes take longer to read but provide more accurate results:
     * - HIGH_PRECISION: ~8.3ms read time, highest accuracy
     * - MED_PRECISION: ~4.5ms read time, medium accuracy
     * - LOW_PRECISION: ~1.7ms read time, lowest accuracy
     */
    void setPrecision(sht4x_precision_t precision);
    
    /**
     * @brief Enable or disable the built-in heater
     * @param duration Heater duration (SHT4X_NO_HEATER, SHT4X_HIGH_HEATER_1S, etc.)
     * 
     * The heater can be used to drive off condensation and verify sensor functionality.
     * Note: Heating will affect temperature readings temporarily.
     */
    void setHeater(sht4x_heater_t duration);
    
    /**
     * @brief Read temperature and humidity from the sensor
     * @return true if read successful, false otherwise
     * 
     * This method queries the sensor and updates the internal cached values.
     * Use getTemperature() and getHumidity() to retrieve the values.
     */
    bool readSensor();
    
    /**
     * @brief Get the most recent temperature reading
     * @return Temperature in degrees Celsius
     * 
     * Returns the cached temperature from the last readSensor() call.
     * Check isDataValid() to ensure the data is fresh.
     */
    float getTemperature();
    
    /**
     * @brief Get the most recent humidity reading
     * @return Relative humidity in percent (0-100%)
     * 
     * Returns the cached humidity from the last readSensor() call.
     * Check isDataValid() to ensure the data is fresh.
     */
    float getHumidity();
    
    /**
     * @brief Get the most recent temperature reading in Fahrenheit
     * @return Temperature in degrees Fahrenheit
     * 
     * Converts the cached Celsius temperature to Fahrenheit.
     */
    float getTemperatureFahrenheit();
    
    /**
     * @brief Check if cached sensor data is still valid
     * @return true if data is recent (within READ_TIMEOUT), false if stale
     * 
     * Use this to determine if readSensor() needs to be called again.
     */
    bool isDataValid();
    
    /**
     * @brief Get time elapsed since last reading
     * @return Milliseconds since last successful readSensor() call
     */
    unsigned long getTimeSinceLastRead();
    
    /**
     * @brief Get the sensor serial number
     * @return 32-bit serial number of the sensor
     * 
     * Each SHT45 has a unique serial number that can be read.
     */
    uint32_t getSerialNumber();
    
    /**
     * @brief Perform a soft reset of the sensor
     * @return true if reset successful, false otherwise
     * 
     * Resets the sensor to its default state without power cycling.
     */
    bool softReset();
    
    /**
     * @brief Check if the sensor is properly initialized and responding
     * @return true if sensor is working, false otherwise
     */
    bool isSensorReady();
};

#endif // SHT45_HUMIDITY_TEMP_SENSOR_HPP
