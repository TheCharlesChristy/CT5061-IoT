#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <Arduino.h>
#include <Wire.h>
#include <queue>
#include <vector>

// Action structure for the global queue
struct DeviceAction {
    uint8_t device_address;
    uint8_t action_type;  // 0 = read, 1 = write, etc.
    std::vector<uint8_t> data; // Owns the data buffer for safety
    unsigned long timestamp;
    
    DeviceAction(uint8_t addr, uint8_t type, const uint8_t* d, size_t len)
        : device_address(addr), action_type(type), data(), timestamp(millis()) {
        if (d != nullptr && len > 0) {
            data.assign(d, d + len);
        }
    }
};

class Device {
protected:
    uint8_t i2c_address;
    TwoWire* wire_instance;
    bool initialized;

public:
    // Global action queue shared across all Device instances
    static std::queue<DeviceAction> action_queue;
    
    // Constructor
    Device(uint8_t address, TwoWire* wire = &Wire);
    
    // Destructor
    virtual ~Device();
    
    // Initialize the device
    virtual bool begin();
    
    // Generic send function - sends data to the device over I2C
    bool send(const uint8_t* data, size_t length);
    
    // Generic receive function - receives data from the device over I2C
    bool receive(uint8_t* buffer, size_t length);
    
    // Write to a specific register
    bool writeRegister(uint8_t reg, uint8_t value);
    
    // Read from a specific register
    bool readRegister(uint8_t reg, uint8_t* value);
    
    // Read multiple bytes from a register
    bool readRegisters(uint8_t reg, uint8_t* buffer, size_t length);
    
    // Check if device is connected
    bool isConnected() const;
    
    // Get device address
    uint8_t getAddress() const;
    
    // Add action to global queue (copies data into the queue)
    void addActionToQueue(uint8_t action_type, const uint8_t* data, size_t length);
};

#endif // DEVICE_HPP
