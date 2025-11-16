# Device Framework Classes Documentation

## Purpose

This document describes the device framework classes used in the CT5061-IoT project. The framework provides a structured approach to managing external I2C devices (sensors, actuators, etc.) that communicate with the ESP32S3 host. The architecture includes a base `Device` class for inheritance, a global action queue for coordinated device operations, and a singleton `DeviceRegistry` for centralized device management.

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
