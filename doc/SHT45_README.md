# SHT45 Humidity and Temperature Sensor

## Overview

The `SHT45HumidityTempSensor` class provides a comprehensive interface for the Adafruit SHT45 high-precision digital humidity and temperature sensor. This sensor is ideal for environmental monitoring applications requiring accurate and reliable measurements.

## Hardware Requirements

- **Sensor**: Adafruit SHT45 Temperature & Humidity Sensor (Product ID: 5665 or 5776 with PTFE)
- **Microcontroller**: ESP32-S3 (Seeed XIAO ESP32S3) or compatible
- **Connection**: I2C interface via STEMMA QT/Qwiic connectors or soldered headers
- **Power**: 3.3V or 5V (via onboard regulator)

## Wiring

### Using STEMMA QT/Qwiic Cables (Recommended)
Simply connect the sensor to your microcontroller using a STEMMA QT cable:
- No soldering required
- Automatic I2C connection

### Using Soldered Headers
| SHT45 Pin | ESP32 Pin | Description |
|-----------|-----------|-------------|
| VIN       | 3.3V      | Power supply |
| GND       | GND       | Ground |
| SCL       | GPIO6     | I2C Clock (default for XIAO ESP32S3) |
| SDA       | GPIO5     | I2C Data (default for XIAO ESP32S3) |

## Features

- **High Precision**: ±1% RH accuracy, ±0.1°C temperature accuracy
- **Multiple Precision Modes**: High, Medium, and Low precision options
- **Built-in Heater**: For condensation removal and sensor verification
- **Data Caching**: Reduces unnecessary I2C transactions
- **Serial Number**: Each sensor has a unique 32-bit identifier
- **Temperature Units**: Supports both Celsius and Fahrenheit

## Installation

### PlatformIO
The required library is already included in `platformio.ini`:

```ini
lib_deps = 
    adafruit/Adafruit SHT4x Library@^1.0.4
```

### Arduino IDE
Install via Library Manager:
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search for "Adafruit SHT4x"
4. Install "Adafruit SHT4x" library

## Quick Start

```cpp
#include "SHT45HumidityTempSensor.hpp"

SHT45HumidityTempSensor tempSensor;

void setup() {
    Serial.begin(115200);
    
    // Initialize sensor
    if (!tempSensor.begin()) {
        Serial.println("Sensor initialization failed!");
        while(1);
    }
    
    // Set high precision mode (optional, default)
    tempSensor.setPrecision(SHT4X_HIGH_PRECISION);
}

void loop() {
    // Read sensor
    if (tempSensor.readSensor()) {
        Serial.print("Temp: ");
        Serial.print(tempSensor.getTemperature());
        Serial.print("°C, Humidity: ");
        Serial.print(tempSensor.getHumidity());
        Serial.println("%");
    }
    
    delay(2000);
}
```

## Precision Modes

The SHT45 supports three precision modes with different trade-offs:

| Mode | Read Time | Power | Accuracy | Constant |
|------|-----------|-------|----------|----------|
| High | ~8.3ms    | Highest | Best    | `SHT4X_HIGH_PRECISION` |
| Medium | ~4.5ms  | Medium  | Good    | `SHT4X_MED_PRECISION` |
| Low | ~1.7ms     | Lowest  | Acceptable | `SHT4X_LOW_PRECISION` |

**Example:**
```cpp
tempSensor.setPrecision(SHT4X_MED_PRECISION);  // Balance of speed and accuracy
```

## Built-in Heater

The heater can remove condensation and verify sensor functionality:

```cpp
// Activate heater for 1 second at high power
tempSensor.setHeater(SHT4X_HIGH_HEATER_1S);

// Read after heating (temperature will be elevated temporarily)
delay(1000);
tempSensor.readSensor();
```

**Heater Options:**
- `SHT4X_NO_HEATER` - Off (default)
- `SHT4X_HIGH_HEATER_1S` / `SHT4X_HIGH_HEATER_100MS` - High power
- `SHT4X_MED_HEATER_1S` / `SHT4X_MED_HEATER_100MS` - Medium power
- `SHT4X_LOW_HEATER_1S` / `SHT4X_LOW_HEATER_100MS` - Low power

## API Reference

### Initialization
- `bool begin()` - Initialize sensor

### Configuration
- `void setPrecision(sht4x_precision_t precision)` - Set measurement precision
- `void setHeater(sht4x_heater_t duration)` - Configure built-in heater

### Reading Data
- `bool readSensor()` - Perform measurement
- `float getTemperature()` - Get temperature in Celsius
- `float getTemperatureFahrenheit()` - Get temperature in Fahrenheit
- `float getHumidity()` - Get relative humidity (%)

### Data Validation
- `bool isDataValid()` - Check if cached data is recent
- `unsigned long getTimeSinceLastRead()` - Time since last reading

### Utilities
- `uint32_t getSerialNumber()` - Get unique sensor ID
- `bool softReset()` - Reset sensor
- `bool isSensorReady()` - Check sensor status

## Example Applications

### 1. Environmental Monitor
```cpp
void loop() {
    if (tempSensor.readSensor()) {
        float temp = tempSensor.getTemperature();
        float hum = tempSensor.getHumidity();
        
        // Log to SD card, display on OLED, etc.
        logData(temp, hum);
    }
    delay(60000);  // Read every minute
}
```

### 2. Serial Command Interface
```cpp
void handleCommand(String cmd) {
    if (cmd == "READ") {
        tempSensor.readSensor();
        Serial.print(tempSensor.getTemperature());
        Serial.print(",");
        Serial.println(tempSensor.getHumidity());
    }
}
```

### 3. Conditional Actions
```cpp
void loop() {
    if (tempSensor.readSensor()) {
        if (tempSensor.getHumidity() > 70.0) {
            activateDehumidifier();
        }
        if (tempSensor.getTemperature() < 18.0) {
            activateHeater();
        }
    }
}
```

## Troubleshooting

### Sensor Not Found
- Check I2C connections (SDA, SCL, VCC, GND)
- Verify I2C address (should be 0x44)
- Ensure Wire.begin() is called (automatically done by begin())
- Check for I2C pull-up resistors

### Erratic Readings
- Allow 2-3 seconds for sensor stabilization after power-on
- Avoid rapid temperature changes near the sensor
- Ensure adequate air circulation
- Check power supply stability

### Always Returns Zero
- Verify sensor is initialized with `begin()`
- Check return value of `readSensor()`
- Ensure I2C bus is functioning

## Best Practices

1. **Read Frequency**: Avoid reading faster than necessary to save power
2. **Precision Selection**: Use lower precision for battery-powered applications
3. **Data Validation**: Always check `isDataValid()` before using cached data
4. **Error Handling**: Check return values of `begin()` and `readSensor()`
5. **Heater Usage**: Use heater sparingly to avoid affecting readings

## Resources

- [Product Page](https://www.adafruit.com/product/5665)
- [Datasheet](https://www.sensirion.com/en/environmental-sensors/humidity-sensors/humidity-sensor-sht4x/)
- [Adafruit Tutorial](https://learn.adafruit.com/adafruit-sht40-temperature-humidity-sensor)
- [GitHub Repository](https://github.com/adafruit/Adafruit_SHT4X)

## License

This code is part of the CT5061-IoT project and follows the project's license terms.
