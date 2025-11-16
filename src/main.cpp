// Device Framework Demo - ESP32S3 with SSD1306 OLED Display
// Demonstrates Device class, DeviceRegistry, LedScreen128_64, and SerialLedControl
//
// Hardware:
//   - ESP32S3 (Seeed XIAO ESP32S3)
//   - SSD1306 OLED Display 128x64 (I2C)
//   - Optional: Potentiometer on A0
//
// Features:
//   - Serial command interface for display control
//   - Automatic demo mode with graphics and text
//   - Device registry management
//   - Real-time clock display

#include <Arduino.h>
#include <Wire.h>
#include "../include/Device.hpp"
#include "../include/DeviceRegistry.hpp"
#include "../include/LedScreen128_64.hpp"
#include "demos/SerialLedControl.hpp"

// Pin definitions for ESP32S3
#define I2C_SDA 5  // Default SDA for Seeed XIAO ESP32S3
#define I2C_SCL 6  // Default SCL for Seeed XIAO ESP32S3
#define POT_PIN A0 // Optional analog input

// Display and control objects
LedScreen128_64 screen(0x3C);  // Default I2C address for SSD1306
SerialLedControl serialControl(&screen);

// Demo mode variables
bool demo_mode = true;
unsigned long last_demo_update = 0;
unsigned long last_clock_update = 0;
uint8_t demo_state = 0;

// Function declarations
void runDemoMode();
void displayWelcomeScreen();
void displayGraphicsDemo();
void displayTextDemo();
void displayClockDemo();
void displaySensorDemo();

void setup() {
    // Initialize I2C with custom pins
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(100);
    
    // Initialize serial
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n========================================");
    Serial.println("  Device Framework Demo - ESP32S3");
    Serial.println("========================================");
    
    // Initialize the LED screen
    Serial.print("Initializing SSD1306 display... ");
    if (screen.begin()) {
        Serial.println("SUCCESS");
    } else {
        Serial.println("FAILED");
        Serial.println("Check I2C connections and address!");
        while (1) delay(1000);
    }
    
    // Register the screen with the device registry
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    if (registry.registerDevice(&screen)) {
        Serial.println("Display registered with DeviceRegistry");
    }
    
    Serial.print("Registered devices: ");
    Serial.println(registry.getDeviceCount());
    
    // Initialize serial control interface
    serialControl.begin(115200);
    
    // Show welcome screen
    displayWelcomeScreen();
    
    Serial.println("\nDemo Mode: Active");
    Serial.println("Type any command to take manual control");
    Serial.println("Type 'help' for available commands");
    Serial.println("Demo cycles through examples every 3 seconds\n");
}

void loop() {
    // Check for serial input first
    if (Serial.available() > 0) {
        // Disable demo mode when user starts interacting
        if (demo_mode) {
            demo_mode = false;
            Serial.println("\n[Demo mode disabled - manual control active]");
        }
    }
    
    // Always check for serial commands
    serialControl.run();
    
    // Run demo mode if enabled
    if (demo_mode) {
        runDemoMode();
    }
}

void runDemoMode() {
    unsigned long current_time = millis();
    
    // Update demo every 3 seconds
    if (current_time - last_demo_update >= 3000) {
        last_demo_update = current_time;
        
        screen.clearDisplay();
        
        switch (demo_state) {
            case 0:
                displayWelcomeScreen();
                break;
            case 1:
                displayGraphicsDemo();
                break;
            case 2:
                displayTextDemo();
                break;
            case 3:
                displayClockDemo();
                break;
            case 4:
                displaySensorDemo();
                break;
        }
        
        screen.displayBuffer();
        
        demo_state++;
        if (demo_state > 4) {
            demo_state = 0;
        }
    }
}

void displayWelcomeScreen() {
    screen.clearDisplay();
    
    // Draw border
    screen.drawRect(0, 0, 128, 64, true);
    
    // Title
    screen.setTextSize(2);
    screen.setTextColor(true);
    screen.drawCenteredText("DEVICE", 8, 2);
    screen.drawCenteredText("FRAMEWORK", 26, 2);
    
    // Subtitle
    screen.setTextSize(1);
    screen.drawCenteredText("ESP32S3 + SSD1306", 48, 1);
    
    screen.displayBuffer();
}

void displayGraphicsDemo() {
    screen.clearDisplay();
    
    // Title
    screen.setTextSize(1);
    screen.setCursor(0, 0);
    screen.print("Graphics Demo");
    
    // Draw various shapes
    screen.drawLine(0, 10, 127, 10, true);
    
    // Rectangles
    screen.drawRect(5, 15, 30, 20, true);
    screen.fillRect(10, 20, 10, 10, true);
    
    // Circles
    screen.drawCircle(55, 30, 12, true);
    screen.fillCircle(55, 30, 6, true);
    
    // Triangle
    screen.drawTriangle(80, 35, 95, 15, 110, 35, true);
    
    // Progress bar
    screen.drawProgressBar(5, 45, 118, 10, 65);
    screen.setTextSize(1);
    screen.setCursor(56, 58);
    screen.print("65%");
    
    screen.displayBuffer();
}

void displayTextDemo() {
    screen.clearDisplay();
    
    // Different text sizes
    screen.setTextSize(1);
    screen.setCursor(0, 0);
    screen.println("Size 1: Small");
    
    screen.setTextSize(2);
    screen.println("Size 2");
    
    screen.setTextSize(1);
    screen.setCursor(0, 40);
    screen.print("Value: ");
    screen.print(random(100, 999));
    
    screen.setCursor(0, 50);
    screen.print("Temp: ");
    screen.print(23.5, 1);
    screen.print(" C");
    
    screen.displayBuffer();
}

void displayClockDemo() {
    screen.clearDisplay();
    
    unsigned long seconds = millis() / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    
    seconds = seconds % 60;
    minutes = minutes % 60;
    hours = hours % 24;
    
    // Title
    screen.setTextSize(1);
    screen.drawCenteredText("System Uptime", 5, 1);
    
    // Draw clock face
    screen.drawCircle(64, 36, 24, true);
    
    // Display time
    screen.setTextSize(2);
    screen.setCursor(28, 28);
    if (hours < 10) screen.print("0");
    screen.print(hours);
    screen.print(":");
    if (minutes < 10) screen.print("0");
    screen.print(minutes);
    
    screen.setTextSize(1);
    screen.setCursor(46, 44);
    if (seconds < 10) screen.print("0");
    screen.print(seconds);
    screen.print(" s");
    
    screen.displayBuffer();
}

void displaySensorDemo() {
    screen.clearDisplay();
    
    // Read analog value (if potentiometer connected)
    int raw_value = analogRead(POT_PIN);
    int percentage = map(raw_value, 0, 4095, 0, 100);
    
    // Title
    screen.setTextSize(1);
    screen.setCursor(0, 0);
    screen.print("Sensor Reading");
    
    // Raw value display
    screen.setCursor(0, 15);
    screen.print("Raw: ");
    screen.print(raw_value);
    
    // Percentage
    screen.setCursor(0, 25);
    screen.print("Value: ");
    screen.print(percentage);
    screen.print("%");
    
    // Progress bar
    screen.drawProgressBar(5, 40, 118, 15, percentage);
    
    // Registry info
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    screen.setCursor(0, 58);
    screen.print("Devices: ");
    screen.print((int)registry.getDeviceCount());
    
    screen.displayBuffer();
}