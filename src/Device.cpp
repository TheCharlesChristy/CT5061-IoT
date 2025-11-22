#include "Device.hpp"

// Initialize the static global queue
std::queue<DeviceAction> Device::action_queue;

// Constructor
Device::Device(uint8_t address, TwoWire* wire)
    : i2c_address(address), wire_instance(wire), initialized(false) {
}

// Destructor
Device::~Device() {
}

// Initialize the device
bool Device::begin() {
    if (!initialized) {
        wire_instance->begin();
        initialized = isConnected();
    }
    return initialized;
}

// Generic send function - sends data to the device over I2C
bool Device::send(const uint8_t* data, size_t length) {
    if (!initialized || wire_instance == nullptr) {
        return false;
    }
    
    wire_instance->beginTransmission(i2c_address);
    size_t bytes_written = wire_instance->write(data, length);
    uint8_t error = wire_instance->endTransmission();
    
    return (error == 0 && bytes_written == length);
}

// Generic receive function - receives data from the device over I2C
bool Device::receive(uint8_t* buffer, size_t length) {
    if (!initialized || wire_instance == nullptr) {
        return false;
    }
    
    size_t bytes_received = wire_instance->requestFrom(i2c_address, length);
    
    if (bytes_received != length) {
        return false;
    }
    
    for (size_t i = 0; i < length; i++) {
        if (wire_instance->available()) {
            buffer[i] = wire_instance->read();
        } else {
            return false;
        }
    }
    
    return true;
}

// Write to a specific register
bool Device::writeRegister(uint8_t reg, uint8_t value) {
    if (!initialized || wire_instance == nullptr) {
        return false;
    }
    
    wire_instance->beginTransmission(i2c_address);
    wire_instance->write(reg);
    wire_instance->write(value);
    uint8_t error = wire_instance->endTransmission();
    
    return (error == 0);
}

// Read from a specific register
bool Device::readRegister(uint8_t reg, uint8_t* value) {
    if (!initialized || wire_instance == nullptr) {
        return false;
    }
    
    // Write register address
    wire_instance->beginTransmission(i2c_address);
    wire_instance->write(reg);
    uint8_t error = wire_instance->endTransmission(false); // Keep connection alive
    
    if (error != 0) {
        return false;
    }
    
    // Read the register value
    if (wire_instance->requestFrom(i2c_address, (uint8_t)1) == 1) {
        *value = wire_instance->read();
        return true;
    }
    
    return false;
}

// Read multiple bytes from a register
bool Device::readRegisters(uint8_t reg, uint8_t* buffer, size_t length) {
    if (!initialized || wire_instance == nullptr) {
        return false;
    }
    
    // Write register address
    wire_instance->beginTransmission(i2c_address);
    wire_instance->write(reg);
    uint8_t error = wire_instance->endTransmission(false); // Keep connection alive
    
    if (error != 0) {
        return false;
    }
    
    // Read the register values
    size_t bytes_received = wire_instance->requestFrom(i2c_address, (uint8_t)length);
    
    if (bytes_received != length) {
        return false;
    }
    
    for (size_t i = 0; i < length; i++) {
        if (wire_instance->available()) {
            buffer[i] = wire_instance->read();
        } else {
            return false;
        }
    }
    
    return true;
}

// Check if device is connected
bool Device::isConnected() const {
    if (wire_instance == nullptr) {
        return false;
    }
    wire_instance->beginTransmission(i2c_address);
    uint8_t error = wire_instance->endTransmission();
    return (error == 0);
}

// Get device address
uint8_t Device::getAddress() const {
    return i2c_address;
}

// Add action to global queue
void Device::addActionToQueue(uint8_t action_type, const uint8_t* data, size_t length) {
    DeviceAction action(i2c_address, action_type, data, length);
    action_queue.push(action);
}
