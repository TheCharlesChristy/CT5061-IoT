#ifndef DEVICE_REGISTRY_HPP
#define DEVICE_REGISTRY_HPP

#include <Arduino.h>
#include <vector>
#include "Device.hpp"

class DeviceRegistry {
private:
    // Private constructor for singleton
    DeviceRegistry();
    
    // Delete copy constructor and assignment operator
    DeviceRegistry(const DeviceRegistry&) = delete;
    DeviceRegistry& operator=(const DeviceRegistry&) = delete;
    
    // Vector to store registered devices
    std::vector<Device*> registered_devices;
    
    // Track if an action is currently being processed
    bool action_in_progress;
    
public:
    // Get singleton instance
    static DeviceRegistry& getInstance();
    
    // Destructor
    ~DeviceRegistry();
    
    // Register a device with the registry
    bool registerDevice(Device* device);
    
    // Unregister a device from the registry
    bool unregisterDevice(Device* device);
    
    // Get the number of registered devices
    size_t getDeviceCount() const;
    
    // Check if a device is registered
    bool isDeviceRegistered(Device* device) const;
    
    // Get a device by index
    Device* getDevice(size_t index) const;
    
    // Get a device by I2C address
    Device* getDeviceByAddress(uint8_t address) const;
    
    // Get the next action from the global queue without removing it
    bool getNextAction(DeviceAction& action);
    
    // Perform the next action in the queue
    bool performNextAction();
    
    // Skip the next action in the queue (remove without performing)
    bool skipNextAction();
    
    // Check if there are pending actions
    bool hasPendingActions() const;
    
    // Get the number of pending actions
    size_t getPendingActionCount() const;
    
    // Clear all pending actions
    void clearAllActions();
};

#endif // DEVICE_REGISTRY_HPP
