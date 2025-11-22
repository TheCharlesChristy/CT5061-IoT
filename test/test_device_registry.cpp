#include <Arduino.h>
#include <unity.h>
#include "DeviceRegistry.hpp"
#include "mocks/MockDevice.hpp"

void setUp(void) {
    // Not needed
}

void tearDown(void) {
    // Not needed
}

void test_register_unregister_devices(void) {
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    registry.clearAllActions();

    // Ensure registry is empty to start
    while (registry.getDeviceCount() > 0) {
        registry.unregisterDevice(registry.getDevice(0));
    }

    MockDevice d1(0x10);
    MockDevice d2(0x11);

    TEST_ASSERT_TRUE(registry.registerDevice(&d1));
    TEST_ASSERT_TRUE(registry.registerDevice(&d2));
    TEST_ASSERT_EQUAL(2, registry.getDeviceCount());
    TEST_ASSERT_TRUE(registry.isDeviceRegistered(&d1));
    TEST_ASSERT_TRUE(registry.isDeviceRegistered(&d2));

    TEST_ASSERT_TRUE(registry.unregisterDevice(&d1));
    TEST_ASSERT_EQUAL(1, registry.getDeviceCount());
    TEST_ASSERT_FALSE(registry.isDeviceRegistered(&d1));

    TEST_ASSERT_TRUE(registry.unregisterDevice(&d2));
    TEST_ASSERT_EQUAL(0, registry.getDeviceCount());
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_register_unregister_devices);
    UNITY_END();
}

void loop() {
    // No continuous loop needed for unit tests
}
