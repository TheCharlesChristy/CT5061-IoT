#include "DeviceRegistry.hpp"

// Private constructor
DeviceRegistry::DeviceRegistry() : action_in_progress(false) {
}

// Get singleton instance
DeviceRegistry& DeviceRegistry::getInstance() {
    static DeviceRegistry instance;
    return instance;
}

// Destructor
DeviceRegistry::~DeviceRegistry() {
    // Note: We don't delete the Device pointers as we don't own them
    registered_devices.clear();
}

// Register a device with the registry
bool DeviceRegistry::registerDevice(Device* device) {
    if (device == nullptr) {
        return false;
    }
    
    // Check if device is already registered
    if (isDeviceRegistered(device)) {
        return false;
    }
    
    registered_devices.push_back(device);
    return true;
}

// Unregister a device from the registry
bool DeviceRegistry::unregisterDevice(Device* device) {
    if (device == nullptr) {
        return false;
    }
    
    for (auto it = registered_devices.begin(); it != registered_devices.end(); ++it) {
        if (*it == device) {
            registered_devices.erase(it);
            return true;
        }
    }
    
    return false;
}

// Get the number of registered devices
size_t DeviceRegistry::getDeviceCount() const {
    return registered_devices.size();
}

// Check if a device is registered
bool DeviceRegistry::isDeviceRegistered(Device* device) const {
    if (device == nullptr) {
        return false;
    }
    
    for (const auto& registered_device : registered_devices) {
        if (registered_device == device) {
            return true;
        }
    }
    
    return false;
}

// Get a device by index
Device* DeviceRegistry::getDevice(size_t index) const {
    if (index >= registered_devices.size()) {
        return nullptr;
    }
    
    return registered_devices[index];
}

// Get a device by I2C address
Device* DeviceRegistry::getDeviceByAddress(uint8_t address) const {
    for (const auto& device : registered_devices) {
        if (device->getAddress() == address) {
            return device;
        }
    }
    
    return nullptr;
}

// Get the next action from the global queue without removing it
bool DeviceRegistry::getNextAction(DeviceAction& action) {
    if (Device::action_queue.empty()) {
        return false;
    }
    
    action = Device::action_queue.front();
    return true;
}

// Perform the next action in the queue
bool DeviceRegistry::performNextAction() {
    if (Device::action_queue.empty()) {
        return false;
    }
    
    if (action_in_progress) {
        return false;
    }
    
    action_in_progress = true;
    
    DeviceAction action = Device::action_queue.front();
    Device::action_queue.pop();
    
    // Find the device by address
    Device* device = getDeviceByAddress(action.device_address);
    
    if (device == nullptr) {
        action_in_progress = false;
        return false;
    }
    
    bool success = false;
    
    // Perform the action based on action_type
    switch (action.action_type) {
        case 0: // Read operation
            if (action.data != nullptr && action.data_length > 0) {
                success = device->receive(action.data, action.data_length);
            }
            break;
            
        case 1: // Write operation
            if (action.data != nullptr && action.data_length > 0) {
                success = device->send(action.data, action.data_length);
            }
            break;
            
        default:
            // Unknown action type
            success = false;
            break;
    }
    
    action_in_progress = false;
    return success;
}

// Skip the next action in the queue (remove without performing)
bool DeviceRegistry::skipNextAction() {
    if (Device::action_queue.empty()) {
        return false;
    }
    
    Device::action_queue.pop();
    return true;
}

// Check if there are pending actions
bool DeviceRegistry::hasPendingActions() const {
    return !Device::action_queue.empty();
}

// Get the number of pending actions
size_t DeviceRegistry::getPendingActionCount() const {
    return Device::action_queue.size();
}

// Clear all pending actions
void DeviceRegistry::clearAllActions() {
    while (!Device::action_queue.empty()) {
        Device::action_queue.pop();
    }
}
