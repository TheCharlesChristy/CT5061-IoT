#include "SHT45HumidityTempSensor.hpp"

// Constructor
SHT45HumidityTempSensor::SHT45HumidityTempSensor(uint8_t address)
    : Device(address), sht45(nullptr), sensor_initialized(false),
      last_temperature(0.0f), last_humidity(0.0f), last_read_time(0) {
    // Create the Adafruit SHT4x sensor object
    sht45 = new Adafruit_SHT4x();
}

// Destructor
SHT45HumidityTempSensor::~SHT45HumidityTempSensor() {
    if (sht45 != nullptr) {
        delete sht45;
        sht45 = nullptr;
    }
}

// Initialize the sensor
bool SHT45HumidityTempSensor::begin() {
    // First initialize the base Device class
    if (!Device::begin()) {
        return false;
    }
    
    // Now initialize the SHT45 sensor using the Wire instance from Device class
    if (!sht45->begin(wire_instance)) {
        sensor_initialized = false;
        return false;
    }
    
    sensor_initialized = true;
    
    // Set default precision to high for best accuracy
    setPrecision(SHT4X_HIGH_PRECISION);
    
    // Perform an initial reading to verify sensor is working
    if (!readSensor()) {
        sensor_initialized = false;
        return false;
    }
    
    return true;
}

// Set the precision mode
void SHT45HumidityTempSensor::setPrecision(sht4x_precision_t precision) {
    if (sensor_initialized && sht45 != nullptr) {
        sht45->setPrecision(precision);
    }
}

// Set the heater mode
void SHT45HumidityTempSensor::setHeater(sht4x_heater_t duration) {
    if (sensor_initialized && sht45 != nullptr) {
        sht45->setHeater(duration);
    }
}

// Read temperature and humidity from sensor
bool SHT45HumidityTempSensor::readSensor() {
    if (!sensor_initialized || sht45 == nullptr) {
        return false;
    }
    
    // Create sensor event structures
    sensors_event_t humidity_event, temp_event;
    
    // Read the sensor
    if (!sht45->getEvent(&humidity_event, &temp_event)) {
        return false;
    }
    
    // Update cached values
    last_temperature = temp_event.temperature;  // Celsius
    last_humidity = humidity_event.relative_humidity;  // Percent
    last_read_time = millis();
    
    return true;
}

// Get temperature in Celsius
float SHT45HumidityTempSensor::getTemperature() {
    return last_temperature;
}

// Get humidity in percent
float SHT45HumidityTempSensor::getHumidity() {
    return last_humidity;
}

// Get temperature in Fahrenheit
float SHT45HumidityTempSensor::getTemperatureFahrenheit() {
    return (last_temperature * 9.0f / 5.0f) + 32.0f;
}

// Check if cached data is still valid
bool SHT45HumidityTempSensor::isDataValid() {
    if (last_read_time == 0) {
        return false;  // No reading has been taken yet
    }
    
    unsigned long elapsed = millis() - last_read_time;
    return (elapsed < READ_TIMEOUT);
}

// Get time since last reading
unsigned long SHT45HumidityTempSensor::getTimeSinceLastRead() {
    if (last_read_time == 0) {
        return 0xFFFFFFFF;  // Max value indicates no reading taken
    }
    return millis() - last_read_time;
}

// Get sensor serial number
uint32_t SHT45HumidityTempSensor::getSerialNumber() {
    if (!sensor_initialized || sht45 == nullptr) {
        return 0;
    }
    
    return sht45->readSerial();
}

// Soft reset the sensor
bool SHT45HumidityTempSensor::softReset() {
    if (!sensor_initialized || sht45 == nullptr) {
        return false;
    }
    
    sht45->reset();
    
    // Wait for sensor to be ready after reset (spec says max 1ms)
    delay(2);
    
    // Verify sensor is still responding
    return isConnected();
}

// Check if sensor is ready
bool SHT45HumidityTempSensor::isSensorReady() {
    return sensor_initialized && isConnected();
}
