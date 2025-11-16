# Device Framework and Graphics System Classes Documentation

## Purpose

This document describes the device framework classes and graphics system used in the CT5061-IoT project. The framework provides:

1. **Device Framework**: Structured approach to managing external I2C devices (sensors, actuators, etc.)
2. **Graphics System**: Extensible graphics asset classes for rendering on the SSD1306 OLED display

---

# Part 1: Device Framework

## Architecture Overview

The framework consists of three main components:

1. **Device** - Base class for all I2C devices
2. **DeviceAction** - Structure representing queued device operations
3. **DeviceRegistry** - Singleton registry for device management and action execution

---

## Device Class

### Purpose

The `Device` class serves as the parent class for all external I2C devices. It provides generic I2C communication methods and access to a global action queue shared across all device instances.

### Location

- Header: `include/Device.hpp`
- Implementation: `include/Device.cpp`

### Key Features

- **Global Action Queue**: Static `action_queue` shared by all Device instances
- **I2C Communication**: Generic send/receive functions using Arduino Wire library
- **Register Access**: Helper methods for reading/writing device registers
- **Connection Verification**: Check if device is available on I2C bus

### Constructor

```cpp
Device(uint8_t address, TwoWire* wire = &Wire)
```

**Parameters:**
- `address` - I2C address of the device (7-bit format)
- `wire` - Pointer to Wire instance (defaults to `&Wire`)

### Core Methods

#### Initialization

```cpp
bool begin()
```
Initializes I2C and verifies device connection. Call this before any I2C operations.

#### Generic Communication

```cpp
bool send(const uint8_t* data, size_t length)
```
Sends raw data to the device over I2C.

```cpp
bool receive(uint8_t* buffer, size_t length)
```
Receives raw data from the device over I2C.

#### Register Operations

```cpp
bool writeRegister(uint8_t reg, uint8_t value)
```
Writes a single byte to a specific register.

```cpp
bool readRegister(uint8_t reg, uint8_t* value)
```
Reads a single byte from a specific register.

```cpp
bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length)
```
Reads multiple consecutive bytes starting from a register.

#### Utility Methods

```cpp
bool isConnected()
```
Checks if the device responds on the I2C bus.

```cpp
uint8_t getAddress() const
```
Returns the I2C address of the device.

```cpp
void addActionToQueue(uint8_t action_type, uint8_t* data, size_t length)
```
Adds an action to the global queue for deferred execution.

### Creating Child Classes

To create a device-specific class, inherit from `Device` and implement device-specific functionality.

#### Example: Temperature Sensor

```cpp
// TemperatureSensor.hpp
#ifndef TEMPERATURE_SENSOR_HPP
#define TEMPERATURE_SENSOR_HPP

#include "Device.hpp"

class TemperatureSensor : public Device {
private:
    // Device-specific registers
    static const uint8_t TEMP_REG = 0x00;
    static const uint8_t CONFIG_REG = 0x01;
    
public:
    // Constructor - pass I2C address to parent
    TemperatureSensor(uint8_t address) : Device(address) {}
    
    // Override begin() if device needs special initialization
    bool begin() override {
        if (!Device::begin()) {
            return false;
        }
        
        // Device-specific initialization
        return configureDevice();
    }
    
    // Device-specific methods
    bool configureDevice() {
        // Write configuration to device
        return writeRegister(CONFIG_REG, 0x60);
    }
    
    float readTemperature() {
        uint8_t raw_data[2];
        if (readRegisters(TEMP_REG, raw_data, 2)) {
            int16_t raw_temp = (raw_data[0] << 8) | raw_data[1];
            return raw_temp / 256.0; // Convert to Celsius
        }
        return NAN;
    }
    
    // Queue a temperature read action for later
    void queueTemperatureRead() {
        uint8_t reg = TEMP_REG;
        addActionToQueue(0, &reg, 1); // 0 = read action
    }
};

#endif
```

#### Usage Example

```cpp
#include <Arduino.h>
#include "TemperatureSensor.hpp"
#include "DeviceRegistry.hpp"

TemperatureSensor tempSensor(0x48); // I2C address 0x48

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize the sensor
    if (tempSensor.begin()) {
        Serial.println("Temperature sensor initialized");
    }
    
    // Register with the registry
    DeviceRegistry::getInstance().registerDevice(&tempSensor);
    
    // Read temperature directly
    float temp = tempSensor.readTemperature();
    Serial.print("Temperature: ");
    Serial.println(temp);
}
```

### Best Practices for Child Classes

1. **Call Parent Constructor**: Always pass the I2C address to the `Device` constructor
2. **Override `begin()` Carefully**: Call `Device::begin()` first, then add device-specific initialization
3. **Use Protected Members**: Access `i2c_address`, `wire_instance`, and `initialized` as needed
4. **Add Virtual Destructors**: If your child class allocates resources, implement a destructor
5. **Document Register Maps**: Define register addresses as named constants
6. **Error Handling**: Check return values from all I2C operations

---

## DeviceAction Structure

### Purpose

Represents a queued operation to be performed on a device. Stored in the global `Device::action_queue`.

### Structure

```cpp
struct DeviceAction {
    uint8_t device_address;   // I2C address of target device
    uint8_t action_type;      // 0 = read, 1 = write
    uint8_t* data;            // Pointer to data buffer
    size_t data_length;       // Length of data
    unsigned long timestamp;  // Time when action was created
};
```

### Action Types

- **0**: Read operation - receive data from device
- **1**: Write operation - send data to device

---

## DeviceRegistry Class

### Purpose

The `DeviceRegistry` is a singleton class that provides centralized management of all Device instances and coordinates execution of actions from the global queue.

### Location

- Header: `include/DeviceRegistry.hpp`
- Implementation: `include/DeviceRegistry.cpp`

### Getting the Instance

```cpp
DeviceRegistry& registry = DeviceRegistry::getInstance();
```

### Device Management Methods

#### Register a Device

```cpp
bool registerDevice(Device* device)
```
Adds a device to the registry. Returns `false` if device is null or already registered.

#### Unregister a Device

```cpp
bool unregisterDevice(Device* device)
```
Removes a device from the registry.

#### Query Devices

```cpp
size_t getDeviceCount() const
```
Returns the number of registered devices.

```cpp
bool isDeviceRegistered(Device* device) const
```
Checks if a specific device is registered.

```cpp
Device* getDevice(size_t index) const
```
Gets a device by index (0-based).

```cpp
Device* getDeviceByAddress(uint8_t address) const
```
Finds a device by its I2C address.

### Action Queue Methods

#### Get Next Action

```cpp
bool getNextAction(DeviceAction& action)
```
Peeks at the next action without removing it from the queue. Returns `false` if queue is empty.

#### Perform Next Action

```cpp
bool performNextAction()
```
Executes and removes the next action from the queue. Automatically finds the device by address and performs the appropriate I2C operation. Returns `false` if queue is empty, device not found, or operation fails.

#### Skip Next Action

```cpp
bool skipNextAction()
```
Removes the next action from the queue without executing it. Returns `false` if queue is empty.

#### Query Queue Status

```cpp
bool hasPendingActions() const
```
Returns `true` if there are actions in the queue.

```cpp
size_t getPendingActionCount() const
```
Returns the number of pending actions.

```cpp
void clearAllActions()
```
Removes all actions from the queue without executing them.

### Usage Example

```cpp
#include <Arduino.h>
#include "Device.hpp"
#include "DeviceRegistry.hpp"
#include "TemperatureSensor.hpp"

TemperatureSensor tempSensor(0x48);
TemperatureSensor tempSensor2(0x49);

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Get registry instance
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    
    // Initialize devices
    tempSensor.begin();
    tempSensor2.begin();
    
    // Register devices
    registry.registerDevice(&tempSensor);
    registry.registerDevice(&tempSensor2);
    
    Serial.print("Registered devices: ");
    Serial.println(registry.getDeviceCount());
    
    // Queue some actions
    uint8_t data1[] = {0x01, 0x60};
    tempSensor.addActionToQueue(1, data1, 2); // Write action
    
    uint8_t data2[] = {0x00};
    tempSensor2.addActionToQueue(0, data2, 1); // Read action
}

void loop() {
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    
    // Process pending actions
    if (registry.hasPendingActions()) {
        Serial.print("Pending actions: ");
        Serial.println(registry.getPendingActionCount());
        
        // Peek at next action
        DeviceAction nextAction;
        if (registry.getNextAction(nextAction)) {
            Serial.print("Next action for device: 0x");
            Serial.println(nextAction.device_address, HEX);
        }
        
        // Execute the action
        if (registry.performNextAction()) {
            Serial.println("Action performed successfully");
        } else {
            Serial.println("Action failed, skipping...");
            registry.skipNextAction();
        }
    }
    
    delay(1000);
}
```

### Thread Safety Note

The `DeviceRegistry` uses an `action_in_progress` flag to prevent concurrent action execution. However, for multi-threaded environments (FreeRTOS tasks), additional synchronization mechanisms (mutexes, semaphores) should be implemented.

---

## Complete Usage Workflow

### 1. Define Child Device Class

Create a class inheriting from `Device` with device-specific functionality.

### 2. Initialize I2C and Devices

```cpp
Wire.begin(); // or Wire.begin(SDA_PIN, SCL_PIN) for custom pins
myDevice.begin();
```

### 3. Register Devices

```cpp
DeviceRegistry::getInstance().registerDevice(&myDevice);
```

### 4. Direct Communication

Use device-specific methods or generic `send()`/`receive()` for immediate operations.

### 5. Queue Actions

Use `addActionToQueue()` for deferred operations.

### 6. Process Queue

Call `performNextAction()` in your main loop or dedicated task to execute queued operations.

---

## Summary

This framework provides a flexible, extensible architecture for managing I2C devices:

- **Inheritance**: Create device-specific classes by inheriting from `Device`
- **Centralized Management**: Use `DeviceRegistry` singleton for device lifecycle management
- **Action Queue**: Coordinate operations across multiple devices using the global queue
- **Arduino Compatible**: Built on Arduino Wire library for ESP32S3 compatibility

For questions or issues, refer to the source files in the `include/` directory.

---

## SHT45HumidityTempSensor Class

### Purpose

The `SHT45HumidityTempSensor` class provides a high-level interface for the Adafruit SHT45 precision temperature and humidity sensor. This sensor offers excellent accuracy and reliability for environmental monitoring applications.

### Location

- Header: `include/SHT45HumidityTempSensor.hpp`
- Implementation: `src/SHT45HumidityTempSensor.cpp`

### Sensor Specifications

The SHT45 is a high-precision digital humidity and temperature sensor with the following specifications:

- **Humidity Accuracy**: ±1% typical relative humidity accuracy from 25 to 75%
- **Temperature Accuracy**: ±0.1°C typical accuracy from 0 to 75°C
- **Interface**: I2C communication (default address: 0x44)
- **Supply Voltage**: 1.08V to 3.6V (breakout board supports 3V or 5V)
- **Precision Modes**: Three modes for different power/speed trade-offs

### Key Features

- Inherits from `Device` base class for standard I2C operations
- Multiple precision modes (high, medium, low)
- Built-in heater for condensation removal
- Data caching with validity checking
- Temperature readings in both Celsius and Fahrenheit
- Unique serial number reading
- Soft reset capability

### Constructor

```cpp
SHT45HumidityTempSensor(uint8_t address = 0x44)
```

**Parameters:**
- `address` - I2C address of the SHT45 sensor (default: 0x44)

**Example:**
```cpp
SHT45HumidityTempSensor tempSensor;  // Uses default address 0x44
```

### Public Methods

#### Initialization

```cpp
bool begin()
```
Initializes the sensor and performs an initial reading. Must be called before using any other methods.

**Returns:** `true` if initialization successful, `false` otherwise

**Example:**
```cpp
if (!tempSensor.begin()) {
    Serial.println("Failed to initialize SHT45 sensor!");
}
```

---

#### Configuration Methods

```cpp
void setPrecision(sht4x_precision_t precision)
```
Sets the measurement precision mode.

**Parameters:**
- `precision` - One of:
  - `SHT4X_HIGH_PRECISION` - ~8.3ms read time, highest accuracy (default)
  - `SHT4X_MED_PRECISION` - ~4.5ms read time, medium accuracy
  - `SHT4X_LOW_PRECISION` - ~1.7ms read time, lowest accuracy

**Example:**
```cpp
tempSensor.setPrecision(SHT4X_HIGH_PRECISION);
```

---

```cpp
void setHeater(sht4x_heater_t duration)
```
Enables the built-in heater for a specified duration. Useful for driving off condensation.

**Parameters:**
- `duration` - One of:
  - `SHT4X_NO_HEATER` - Heater off
  - `SHT4X_HIGH_HEATER_1S` - High power, 1 second
  - `SHT4X_HIGH_HEATER_100MS` - High power, 100ms
  - `SHT4X_MED_HEATER_1S` - Medium power, 1 second
  - `SHT4X_MED_HEATER_100MS` - Medium power, 100ms
  - `SHT4X_LOW_HEATER_1S` - Low power, 1 second
  - `SHT4X_LOW_HEATER_100MS` - Low power, 100ms

**Note:** Heating will temporarily affect temperature readings.

**Example:**
```cpp
tempSensor.setHeater(SHT4X_HIGH_HEATER_1S);
```

---

#### Data Reading Methods

```cpp
bool readSensor()
```
Reads temperature and humidity from the sensor and updates cached values.

**Returns:** `true` if read successful, `false` otherwise

**Example:**
```cpp
if (tempSensor.readSensor()) {
    float temp = tempSensor.getTemperature();
    float hum = tempSensor.getHumidity();
}
```

---

```cpp
float getTemperature()
```
Returns the most recent temperature reading in Celsius.

**Returns:** Temperature in degrees Celsius

---

```cpp
float getTemperatureFahrenheit()
```
Returns the most recent temperature reading in Fahrenheit.

**Returns:** Temperature in degrees Fahrenheit

---

```cpp
float getHumidity()
```
Returns the most recent humidity reading.

**Returns:** Relative humidity in percent (0-100%)

---

#### Data Validation Methods

```cpp
bool isDataValid()
```
Checks if the cached sensor data is still recent (within 5 seconds).

**Returns:** `true` if data is fresh, `false` if stale or no reading taken

**Example:**
```cpp
if (!tempSensor.isDataValid()) {
    tempSensor.readSensor();  // Refresh the data
}
```

---

```cpp
unsigned long getTimeSinceLastRead()
```
Returns the time elapsed since the last successful sensor reading.

**Returns:** Milliseconds since last `readSensor()` call

---

#### Utility Methods

```cpp
uint32_t getSerialNumber()
```
Reads the unique 32-bit serial number from the sensor.

**Returns:** Serial number of the sensor

**Example:**
```cpp
uint32_t serial = tempSensor.getSerialNumber();
Serial.print("Sensor serial: 0x");
Serial.println(serial, HEX);
```

---

```cpp
bool softReset()
```
Performs a soft reset of the sensor without power cycling.

**Returns:** `true` if reset successful, `false` otherwise

---

```cpp
bool isSensorReady()
```
Checks if the sensor is properly initialized and responding on the I2C bus.

**Returns:** `true` if sensor is ready, `false` otherwise

---

### Complete Usage Example

```cpp
#include <Arduino.h>
#include "SHT45HumidityTempSensor.hpp"

// Create sensor instance
SHT45HumidityTempSensor tempSensor;

void setup() {
    Serial.begin(115200);
    
    // Initialize I2C and sensor
    if (!tempSensor.begin()) {
        Serial.println("Failed to initialize SHT45 sensor!");
        while (1) delay(10);
    }
    
    Serial.println("SHT45 sensor initialized successfully");
    
    // Set high precision mode for best accuracy
    tempSensor.setPrecision(SHT4X_HIGH_PRECISION);
    
    // Read and display serial number
    uint32_t serial = tempSensor.getSerialNumber();
    Serial.print("Sensor Serial Number: 0x");
    Serial.println(serial, HEX);
}

void loop() {
    // Read sensor data
    if (tempSensor.readSensor()) {
        // Get readings
        float tempC = tempSensor.getTemperature();
        float tempF = tempSensor.getTemperatureFahrenheit();
        float humidity = tempSensor.getHumidity();
        
        // Display results
        Serial.print("Temperature: ");
        Serial.print(tempC, 2);
        Serial.print(" °C (");
        Serial.print(tempF, 2);
        Serial.print(" °F), Humidity: ");
        Serial.print(humidity, 2);
        Serial.println(" %");
    } else {
        Serial.println("Failed to read sensor!");
    }
    
    delay(2000);  // Read every 2 seconds
}
```

### Serial Communication Protocol

When implementing host communication for reading sensor data, consider the following command structure:

**Request Format:**
```
Command: READ_TEMP_HUMIDITY
Response: <temperature>,<humidity>
```

**Example Implementation:**
```cpp
void handleSerialCommand() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        
        if (command == "READ_TEMP_HUMIDITY") {
            if (tempSensor.readSensor()) {
                Serial.print(tempSensor.getTemperature(), 2);
                Serial.print(",");
                Serial.println(tempSensor.getHumidity(), 2);
            } else {
                Serial.println("ERROR: Sensor read failed");
            }
        }
        else if (command == "GET_SERIAL") {
            Serial.println(tempSensor.getSerialNumber(), HEX);
        }
        else if (command == "RESET_SENSOR") {
            if (tempSensor.softReset()) {
                Serial.println("OK: Sensor reset");
            } else {
                Serial.println("ERROR: Reset failed");
            }
        }
    }
}
```

### Integration with DeviceRegistry

The SHT45HumidityTempSensor can be registered with the DeviceRegistry for centralized management:

```cpp
#include "DeviceRegistry.hpp"
#include "SHT45HumidityTempSensor.hpp"

void setup() {
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    
    // Create and register sensor
    SHT45HumidityTempSensor* tempSensor = new SHT45HumidityTempSensor();
    
    if (registry.registerDevice(tempSensor)) {
        Serial.println("SHT45 sensor registered successfully");
    }
    
    // Initialize all registered devices
    registry.initializeAll();
}
```

### Troubleshooting

**Sensor Not Detected:**
- Verify I2C connections (SDA, SCL, VCC, GND)
- Check I2C address (default is 0x44)
- Ensure pull-up resistors are present on I2C lines
- Verify power supply voltage (3.3V or 5V)

**Stale Data Readings:**
- Use `isDataValid()` to check if data needs refreshing
- Call `readSensor()` regularly in your main loop

**Inconsistent Readings:**
- Allow sensor to stabilize after power-on (2-3 seconds)
- Avoid rapid heating/cooling of the sensor
- Ensure adequate air circulation around the sensor
- Consider using the heater function to remove condensation

### Notes

- The SHT45 sensor shares the same I2C address (0x44) as other SHT4x family members (SHT40, SHT41)
- The Adafruit SHT4x library supports all variants (SHT40, SHT41, SHT45) with the same code
- Higher precision modes consume more power and take longer to complete measurements
- The built-in heater is useful for sensor verification and condensation removal
- Maximum measurement frequency depends on precision mode selected

---

# Part 2: Graphics System

## Overview

The graphics system provides an object-oriented framework for creating and managing visual elements on the SSD1306 OLED display (128x64 pixels). It includes a base `GraphicsAsset` class and several derived classes for different types of visual elements.

### Key Features

- **Inheritance-based Architecture**: All graphics elements inherit from `GraphicsAsset`
- **Z-Index Layering**: Control drawing order with z-index values
- **Animation Support**: Built-in animation capabilities for dynamic effects
- **Centralized Management**: `LedScreen128_64` class manages and renders all assets
- **Serial Command Interface**: `SerialLedControl` demo provides runtime control

---

## GraphicsAsset Base Class

### Purpose

Abstract base class for all drawable graphics elements. Provides common properties and interface for position, size, visibility, borders, animation, and layering.

### Location

- Header: `include/GraphicsAsset.hpp`
- Implementation: `src/GraphicsAsset.cpp`

### AssetType Enum

Used for runtime type identification (since RTTI is disabled):

```cpp
enum class AssetType {
    GRAPHICSASSET,  // Base type
    TEXTBOX,
    FUNCTIONPLOT,
    DATAPLOT,
    TABLE,
    GEOMETRY,
    BITMAP
};
```

### Protected Members

```cpp
int x, y;              // Position on screen
int width, height;     // Dimensions
bool visible;          // Visibility flag
bool border;           // Draw border around asset
bool animate;          // Enable animation
int zIndex;            // Layer order (higher = on top)
AssetType assetType;   // Runtime type identification
```

### Constructor

```cpp
GraphicsAsset(int x, int y, int width, int height, AssetType type)
```

### Core Methods

#### Pure Virtual Method

```cpp
virtual void draw(LedScreen128_64* screen) = 0;
```
Must be implemented by all derived classes to define rendering behavior.

#### Getters

```cpp
int getX() const;
int getY() const;
int getWidth() const;
int getHeight() const;
bool isVisible() const;
bool hasBorder() const;
bool isAnimated() const;
int getZIndex() const;
AssetType getAssetType() const;
```

#### Setters

```cpp
void setPosition(int x, int y);
void setSize(int width, int height);
void setVisible(bool visible);
void setBorder(bool border);
void setAnimate(bool animate);
void setZIndex(int z);
```

---

## TextBox Class

### Purpose

Displays text with configurable size, alignment, word wrapping, and optional typewriter animation effect.

### Location

- Header: `include/TextBox.hpp`
- Implementation: `src/TextBox.cpp`

### Additional Members

```cpp
String text;              // Text content
uint8_t textSize;         // Size multiplier (1-4)
TextAlign alignment;      // LEFT, CENTER, RIGHT
bool wordWrap;            // Enable word wrapping
bool fillBackground;      // Fill with background color
int animationFrame;       // Current animation frame
```

### Text Alignment Enum

```cpp
enum class TextAlign {
    LEFT,
    CENTER,
    RIGHT
};
```

### Constructor

```cpp
TextBox(int x, int y, int width, int height, const char* text = "")
```

### Key Methods

```cpp
void setText(const char* newText);
void setTextSize(uint8_t size);        // 1-4 multiplier
void setAlignment(TextAlign align);
void setWordWrap(bool wrap);
void setFillBackground(bool fill);
void resetAnimation();
```

### Text Size Reference

- **Size 1**: 6×8 pixels per character
- **Size 2**: 12×16 pixels per character
- **Size 3**: 18×24 pixels per character
- **Size 4**: 24×32 pixels per character

### Animation Behavior

When `animate` is true, text appears character-by-character (typewriter effect). Call `resetAnimation()` to restart the effect.

---

## FunctionPlot Class

### Purpose

Plots mathematical functions with configurable axes, grid, and animation that reveals the plot from left to right.

### Location

- Header: `include/FunctionPlot.hpp`
- Implementation: `src/FunctionPlot.cpp`

### Additional Members

```cpp
MathFunction func;        // Function pointer
float minX, maxX;         // X-axis range
float minY, maxY;         // Y-axis range
bool showAxes;            // Draw X and Y axes
bool showGrid;            // Draw background grid
int animationFrame;       // Current animation frame
```

### Function Type

```cpp
typedef float (*MathFunction)(float x);
```

### Constructor

```cpp
FunctionPlot(int x, int y, int width, int height, 
             MathFunction func, float minX, float maxX)
```

### Key Methods

```cpp
void setFunction(MathFunction func);
void setXRange(float minX, float maxX);
void setYRange(float minY, float maxY);
void setShowAxes(bool show);
void setShowGrid(bool show);
void resetAnimation();
```

### Usage Example

```cpp
float sineWave(float x) {
    return sin(x);
}

FunctionPlot* plot = new FunctionPlot(0, 0, 128, 64, sineWave, -3.14, 3.14);
plot->setShowAxes(true);
plot->setShowGrid(true);
```

---

## DataPlot Class

### Purpose

Plots data points from arrays with multiple visualization styles (lines, points, or both) and point-by-point animation.

### Location

- Header: `include/DataPlot.hpp`
- Implementation: `src/DataPlot.cpp`

### Plot Style Enum

```cpp
enum class PlotStyle {
    LINES,
    POINTS,
    LINES_POINTS
};
```

### Additional Members

```cpp
float* dataX;             // X coordinates array
float* dataY;             // Y coordinates array
int dataCount;            // Number of data points
int capacity;             // Maximum data points
PlotStyle style;          // Visualization style
bool autoScale;           // Auto-calculate axis range
float minX, maxX;         // X-axis range
float minY, maxY;         // Y-axis range
bool showAxes;            // Draw axes
bool showGrid;            // Draw grid
int animationFrame;       // Current animation frame
```

### Constructor

```cpp
DataPlot(int x, int y, int width, int height, int maxPoints = 50)
```

### Key Methods

```cpp
void addPoint(float x, float y);
void clearData();
void setPlotStyle(PlotStyle style);
void setAutoScale(bool autoScale);
void setXRange(float minX, float maxX);
void setYRange(float minY, float maxY);
void setShowAxes(bool show);
void setShowGrid(bool show);
void resetAnimation();
```

### Animation Behavior

When animated, points appear one at a time from left to right.

---

## Table Class

### Purpose

Displays tabular data with configurable rows, columns, headers, and automatic column sizing.

### Location

- Header: `include/Table.hpp`
- Implementation: `src/Table.cpp`

### Additional Members

```cpp
String* cellData;         // Cell contents (row-major order)
int rows;                 // Number of rows
int cols;                 // Number of columns
int* colWidths;           // Width of each column
bool showHeaders;         // Draw header row differently
bool autoFitColumns;      // Auto-calculate column widths
```

### Constructor

```cpp
Table(int x, int y, int width, int height, int rows, int cols)
```

### Key Methods

```cpp
void setCell(int row, int col, const char* value);
void setCell(int row, int col, int value);
void setCell(int row, int col, float value, int decimals = 2);
const char* getCell(int row, int col) const;
void setColumnWidth(int col, int width);
void setShowHeaders(bool show);
void setAutoFitColumns(bool autoFit);
void clear();
```

### Usage Example

```cpp
Table* table = new Table(0, 0, 128, 64, 3, 2);
table->setShowHeaders(true);
table->setCell(0, 0, "Name");
table->setCell(0, 1, "Value");
table->setCell(1, 0, "Temp");
table->setCell(1, 1, 25.6, 1);
```

---

## Geometry Class

### Purpose

Renders geometric shapes (primitives) including rectangles, circles, lines, triangles, and rounded rectangles.

### Location

- Header: `include/Geometry.hpp`
- Implementation: `src/Geometry.cpp`

### Geometry Shape Enum

```cpp
enum class GeometryShape {
    RECTANGLE,
    CIRCLE,
    TRIANGLE,
    LINE,
    ROUNDED_RECTANGLE
};
```

### Additional Members

```cpp
GeometryShape shape;      // Current shape type
bool filled;              // Fill vs outline
int x2, y2;               // Second point (line, etc.)
int x3, y3;               // Third point (triangle)
int radius;               // Circle radius / corner radius
```

### Constructor

```cpp
Geometry(int x, int y, int width, int height)
```

### Shape-Specific Methods

```cpp
void setAsRectangle(int x, int y, int w, int h, bool filled = false);
void setAsCircle(int cx, int cy, int r, bool filled = false);
void setAsLine(int x1, int y1, int x2, int y2);
void setAsTriangle(int x1, int y1, int x2, int y2, int x3, int y3, bool filled = false);
void setAsRoundedRectangle(int x, int y, int w, int h, int r, bool filled = false);
```

### Usage Example

```cpp
Geometry* circle = new Geometry(64, 32, 20, 20);
circle->setAsCircle(64, 32, 10, true);

Geometry* line = new Geometry(0, 0, 128, 64);
line->setAsLine(0, 0, 127, 63);
```

---

## Bitmap Class

### Purpose

Displays bitmap images from byte arrays with optional color inversion and built-in pattern generators.

### Location

- Header: `include/Bitmap.hpp`
- Implementation: `src/Bitmap.cpp`

### Additional Members

```cpp
const uint8_t* bitmapData;    // Pointer to bitmap data
bool ownsData;                 // Memory management flag
bool inverted;                 // Invert colors
```

### Constructor

```cpp
Bitmap(int x, int y, int width, int height, const uint8_t* data = nullptr)
```

### Key Methods

```cpp
void setBitmapData(const uint8_t* data, bool takeOwnership = false);
void setInverted(bool inv);
```

### Pattern Generators

```cpp
static uint8_t* createCheckerboard(int width, int height, int squareSize);
static uint8_t* createGradient(int width, int height, bool horizontal);
```

### Usage Example

```cpp
uint8_t* pattern = Bitmap::createCheckerboard(32, 32, 4);
Bitmap* bmp = new Bitmap(48, 16, 32, 32);
bmp->setBitmapData(pattern, true);  // true = bitmap owns the data
```

---

## LedScreen128_64 Class Extensions

### Asset Management

The `LedScreen128_64` class has been extended to manage multiple graphics assets with z-index sorting.

### Additional Members

```cpp
static const int MAX_SCREEN_ASSETS = 20;
GraphicsAsset* assets[MAX_SCREEN_ASSETS];
int assetCount;
```

### Asset Management Methods

```cpp
bool addAsset(GraphicsAsset* asset);
bool removeAsset(GraphicsAsset* asset);
void clearAssets();
void drawAssets();
```

### Drawing Behavior

The `drawAssets()` method:
1. Sorts assets by z-index (bubble sort)
2. Draws each visible asset in order
3. Lower z-index values are drawn first (background)
4. Higher z-index values are drawn last (foreground)

---

## SerialLedControl Demo Extensions

The demo application provides a comprehensive command interface for testing and using the graphics system.

### Graphics Asset Commands

#### Creating Assets

```
textbox <x> <y> <w> <h> <text>
dataplot <x> <y> <w> <h>
table <x> <y> <w> <h> <rows> <cols>
geometry <x> <y> <w> <h> <shape> [filled]
bitmap <x> <y> <w> <h>
```

Geometry shapes: `rect`, `circle`, `line`, `triangle`, `rrect`

#### Modifying Assets

```
setpos <id> <x> <y>           - Set position
setsize <id> <w> <h>          - Set dimensions
setborder <id> <0|1>          - Toggle border
setvisible <id> <0|1>         - Toggle visibility
setzindex <id> <z>            - Set layer order
setanimate <id> <0|1>         - Toggle animation
```

#### TextBox-Specific

```
settext <id> <text>           - Change text content
settextsize <id> <1-4>        - Change text size
```

Text sizes: 1=6×8px, 2=12×16px, 3=18×24px, 4=24×32px

#### DataPlot-Specific

```
addpoint <id> <x> <y>         - Add data point
```

#### Table-Specific

```
setcell <id> <row> <col> <text>  - Set cell value
```

#### Drawing Commands

```
drawasset <id>                - Draw single asset
drawallassets                 - Draw all assets (z-order)
```

#### Asset Management

```
listassets                    - Show all assets
deleteasset <id>              - Delete asset by ID
deleteall                     - Delete all assets
```

### Usage Examples

#### Create Animated Text

```
textbox 10 10 108 20 Hello World!
setanimate 0 1
settextsize 0 2
drawallassets
display
```

#### Create Data Plot

```
dataplot 0 0 128 64
addpoint 0 0 10
addpoint 0 1 15
addpoint 0 2 12
addpoint 0 3 18
setanimate 0 1
drawallassets
display
```

#### Create Table

```
table 0 0 128 64 3 2
setcell 0 0 0 Name
setcell 0 1 0 Value
setcell 0 0 1 Temp
setcell 0 1 1 25.6
drawallassets
display
```

#### Layer Multiple Assets

```
geometry 0 0 128 64 rect 1
textbox 20 20 88 24 Layered Text
geometry 10 10 108 44 rrect 0
setzindex 0 1
setzindex 1 3
setzindex 2 2
drawallassets
display
```

---

## Animation System

### How Animation Works

1. Set `animate` flag to `true` on an asset
2. Each call to `draw()` advances the animation frame
3. Different asset types have different animation effects:
   - **TextBox**: Typewriter effect (character-by-character)
   - **FunctionPlot**: Left-to-right reveal
   - **DataPlot**: Point-by-point appearance

### Controlling Animation Speed

Animation speed depends on how often `draw()` is called. Use delays between `drawallassets` commands to control speed:

```cpp
void loop() {
    screen.drawAssets();
    screen.displayBuffer();
    delay(50);  // 20 FPS animation
}
```

### Resetting Animation

Use `resetAnimation()` methods or the `setanimate` command to restart animations:

```
setanimate 0 0
setanimate 0 1
```

---

## Best Practices

### Memory Management

1. **Heap Allocation**: Assets are typically allocated with `new` and stored as pointers
2. **Ownership**: Track asset ownership carefully to avoid memory leaks
3. **Cleanup**: Delete assets when no longer needed
4. **Capacity Limits**: 
   - LedScreen128_64: Max 20 assets
   - SerialLedControl: Max 10 assets

### Z-Index Strategy

- **Background**: -10 to 0
- **Content**: 1 to 10
- **Foreground/UI**: 11 to 20
- **Overlays**: 21+

### Performance Tips

1. **Minimize Redraws**: Only call `drawAssets()` when content changes
2. **Use Visibility**: Hide assets instead of deleting them for temporary removal
3. **Limit Data Points**: DataPlot performs best with <100 points
4. **Optimize Text**: Smaller text sizes render faster

### Common Patterns

#### HUD (Heads-Up Display)

```cpp
TextBox* title = new TextBox(0, 0, 128, 10, "System Monitor");
title->setZIndex(10);
title->setAlignment(TextAlign::CENTER);

Table* data = new Table(0, 12, 128, 52, 4, 2);
data->setZIndex(5);
```

#### Animated Dashboard

```cpp
DataPlot* plot = new DataPlot(0, 0, 128, 40, 50);
plot->setAnimate(true);
plot->setZIndex(1);

TextBox* label = new TextBox(0, 42, 128, 22, "Temperature");
label->setZIndex(2);
label->setAlignment(TextAlign::CENTER);
```

---

## Graphics System Summary

The graphics system provides a flexible, extensible framework for creating rich visual interfaces:

- **Object-Oriented**: Clean inheritance hierarchy
- **Layering**: Z-index support for complex compositions
- **Animation**: Built-in effects for dynamic displays
- **Type-Safe**: AssetType enum for runtime identification
- **Serial Control**: Complete command interface for testing

For implementation details, refer to the source files in `include/` and `src/`.
