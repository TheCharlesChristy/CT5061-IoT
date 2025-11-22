// SHT45 Sensor Demo - ESP32S3 with SSD1306 OLED Display
// Demonstrates SHT45HumidityTempSensor with real-time display and plotting
//
// Hardware:
//   - ESP32S3 (Seeed XIAO ESP32S3)
//   - SSD1306 OLED Display 128x64 (I2C, address 0x3C)
//   - Adafruit SHT45 Temperature & Humidity Sensor (I2C, address 0x44)
//
// Features:
//   - Real-time temperature and humidity display
//   - Live plotting of sensor data over time
//   - Serial command interface
//   - 1-second polling interval

#include <Arduino.h>
#include <Wire.h>
#include "SHT45HumidityTempSensor.hpp"
#include "DeviceRegistry.hpp"
#include "LedScreen128_64.hpp"
#include "DataPlot.hpp"

// Pin definitions for ESP32S3
#define I2C_SDA 5  // Default SDA for Seeed XIAO ESP32S3
#define I2C_SCL 6  // Default SCL for Seeed XIAO ESP32S3

// Create sensor and display instances
SHT45HumidityTempSensor* tempSensor;
LedScreen128_64* display;

// Create data plots
DataPlot* tempPlot;
DataPlot* humidityPlot;

// Reading interval
const unsigned long READ_INTERVAL = 1000;  // 1 second
unsigned long lastReadTime = 0;

// Data buffers for plotting
#define MAX_DATA_POINTS 50
float tempData[MAX_DATA_POINTS];
float humData[MAX_DATA_POINTS];
int dataIndex = 0;
int dataCount = 0;

// Function declarations
void handleCommand(String command);

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("\n========================================");
    Serial.println("  SHT45 Sensor Demo with Live Display");
    Serial.println("========================================\n");
    
    // Initialize I2C with custom pins
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(100);
    
    // Create and initialize display
    display = new LedScreen128_64(0x3C);
    Serial.print("Initializing display... ");
    if (!display->begin()) {
        Serial.println("FAILED!");
        Serial.println("Check display connections and restart.");
        while (1) delay(10);
    }
    Serial.println("SUCCESS!");
    
    // Create sensor instance
    tempSensor = new SHT45HumidityTempSensor();
    
    // Initialize sensor
    Serial.print("Initializing SHT45 sensor... ");
    if (!tempSensor->begin()) {
        Serial.println("FAILED!");
        Serial.println("Check sensor connections and restart.");
        
        // Display error on screen
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(true);
        display->setCursor(0, 0);
        display->println("SHT45 Init FAILED!");
        display->println("Check connections");
        display->displayBuffer();
        
        while (1) delay(10);
    }
    Serial.println("SUCCESS!");
    
    // Set high precision mode for best accuracy
    tempSensor->setPrecision(SHT4X_HIGH_PRECISION);
    Serial.println("Precision mode: HIGH");
    
    // Read and display sensor serial number
    uint32_t serial = tempSensor->getSerialNumber();
    Serial.print("Sensor Serial Number: 0x");
    Serial.println(serial, HEX);
    
    // Register with DeviceRegistry
    DeviceRegistry& registry = DeviceRegistry::getInstance();
    if (registry.registerDevice(tempSensor)) {
        Serial.println("Sensor registered with DeviceRegistry");
    }
    if (registry.registerDevice(display)) {
        Serial.println("Display registered with DeviceRegistry");
    }
    
    // Create data plots
    // Temperature plot on left side
    tempPlot = new DataPlot(0, 18, 64, 46, MAX_DATA_POINTS);
    tempPlot->setAutoScale(true);
    tempPlot->setShowAxes(true);
    tempPlot->setShowGrid(true);
    tempPlot->setShowAxisLabels(true);
    tempPlot->setUseTinyAxisLabels(true);
    tempPlot->setTinyAxisLabelScale(1);
    tempPlot->setAxisLabelSize(1);
    tempPlot->setPlotStyle(PlotStyle::LINES);
    
    // Humidity plot on right side
    humidityPlot = new DataPlot(64, 18, 64, 46, MAX_DATA_POINTS);
    humidityPlot->setYRange(0, 100);  // Fixed scale for humidity
    humidityPlot->setAutoScale(false);
    humidityPlot->setShowAxes(true);
    humidityPlot->setShowGrid(true);
    humidityPlot->setShowAxisLabels(true);
    humidityPlot->setUseTinyAxisLabels(true);
    humidityPlot->setTinyAxisLabelScale(1);
    humidityPlot->setAxisLabelSize(1);
    humidityPlot->setPlotStyle(PlotStyle::LINES);
    
    // Initialize data arrays
    for (int i = 0; i < MAX_DATA_POINTS; i++) {
        tempData[i] = 0.0f;
        humData[i] = 0.0f;
    }
    
    // Display welcome message
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(true);
    display->setCursor(10, 10);
    display->println("SHT45 Sensor");
    display->setCursor(5, 25);
    display->println("Initializing...");
    display->displayBuffer();
    delay(1000);
    
    Serial.println("\nReading sensor data every second...");
    Serial.println("Available commands:");
    Serial.println("  READ - Read current temperature and humidity");
    Serial.println("  SERIAL - Display sensor serial number");
    Serial.println("  RESET - Soft reset the sensor");
    Serial.println("  CELSIUS - Display temperature in Celsius");
    Serial.println("  FAHRENHEIT - Display temperature in Fahrenheit");
    Serial.println("  HELP - Display available commands");
    Serial.println();
}

void loop() {
    // Periodic sensor reading (every 1 second)
    if (millis() - lastReadTime >= READ_INTERVAL) {
        lastReadTime = millis();
        
        if (tempSensor->readSensor()) {
            float tempC = tempSensor->getTemperature();
            float humidity = tempSensor->getHumidity();
            
            // Store data in circular buffer
            tempData[dataIndex] = tempC;
            humData[dataIndex] = humidity;
            dataIndex = (dataIndex + 1) % MAX_DATA_POINTS;
            if (dataCount < MAX_DATA_POINTS) {
                dataCount++;
            }
            
            // Update plots with all data points
            tempPlot->clearData();
            humidityPlot->clearData();
            
            for (int i = 0; i < dataCount; i++) {
                int idx = (dataIndex - dataCount + i + MAX_DATA_POINTS) % MAX_DATA_POINTS;
                tempPlot->addPoint((float)i, tempData[idx]);
                humidityPlot->addPoint((float)i, humData[idx]);
            }
            
            // Update display
            display->clearDisplay();
            
            // Draw labels and current values
            display->setTextSize(1);
            display->setTextColor(true);
            display->setCursor(2, 1);
            display->print("T:");
            display->setCursor(14, 1);
            display->print(tempC, 1);
            display->print("C");
            
            display->setCursor(68, 1);
            display->print("H:");
            display->setCursor(80, 1);
            display->print(humidity, 1);
            display->print("%");
            
            // Draw separator line
            display->drawFastHLine(0, 16, 128, true);
            
            // Draw plots
            tempPlot->draw(display);
            humidityPlot->draw(display);
            
            // Draw plot labels and coordinate values (index, value), compact to avoid overlaps
            display->setTextSize(1);
            int lastIndex = (dataIndex - 1 + MAX_DATA_POINTS) % MAX_DATA_POINTS;
            float lastTemp = tempData[lastIndex];
            float lastHum = humData[lastIndex];
            char leftLabel[20];
            char rightLabel[20];
            // Format: "T idx valC" and "H idx val%"
            snprintf(leftLabel, sizeof(leftLabel), "T %d %.1fC", dataCount - 1, lastTemp);
            snprintf(rightLabel, sizeof(rightLabel), "H %d %.1f%%", dataCount - 1, lastHum);
            display->setCursor(2, 10);
            display->print(leftLabel);
            display->setCursor(68, 10);
            display->print(rightLabel);
            
            display->displayBuffer();
            
            // Serial output
            Serial.print("Temperature: ");
            Serial.print(tempC, 2);
            Serial.print(" °C, Humidity: ");
            Serial.print(humidity, 2);
            Serial.println(" %RH");
        } else {
            Serial.println("ERROR: Failed to read sensor!");
            
            // Display error on screen
            display->clearDisplay();
            display->setTextSize(1);
            display->setCursor(10, 28);
            display->println("Sensor Error!");
            display->displayBuffer();
        }
    }
    
    // Handle serial commands from host
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toUpperCase();
        
        handleCommand(command);
    }
}

void handleCommand(String command) {
    if (command == "READ") {
        // Read and display current values
        if (tempSensor->readSensor()) {
            Serial.print("Temperature: ");
            Serial.print(tempSensor->getTemperature(), 2);
            Serial.print(" °C (");
            Serial.print(tempSensor->getTemperatureFahrenheit(), 2);
            Serial.print(" °F), Humidity: ");
            Serial.print(tempSensor->getHumidity(), 2);
            Serial.println(" %RH");
        } else {
            Serial.println("ERROR: Sensor read failed");
        }
    }
    else if (command == "SERIAL") {
        // Display sensor serial number
        uint32_t serial = tempSensor->getSerialNumber();
        Serial.print("Sensor Serial: 0x");
        Serial.println(serial, HEX);
    }
    else if (command == "RESET") {
        // Perform soft reset
        Serial.print("Resetting sensor... ");
        if (tempSensor->softReset()) {
            Serial.println("SUCCESS");
        } else {
            Serial.println("FAILED");
        }
    }
    else if (command == "CELSIUS") {
        // Display temperature in Celsius
        if (tempSensor->isDataValid()) {
            Serial.print("Temperature: ");
            Serial.print(tempSensor->getTemperature(), 2);
            Serial.println(" °C");
        } else {
            Serial.println("No valid data. Reading sensor...");
            if (tempSensor->readSensor()) {
                Serial.print("Temperature: ");
                Serial.print(tempSensor->getTemperature(), 2);
                Serial.println(" °C");
            }
        }
    }
    else if (command == "FAHRENHEIT") {
        // Display temperature in Fahrenheit
        if (tempSensor->isDataValid()) {
            Serial.print("Temperature: ");
            Serial.print(tempSensor->getTemperatureFahrenheit(), 2);
            Serial.println(" °F");
        } else {
            Serial.println("No valid data. Reading sensor...");
            if (tempSensor->readSensor()) {
                Serial.print("Temperature: ");
                Serial.print(tempSensor->getTemperatureFahrenheit(), 2);
                Serial.println(" °F");
            }
        }
    }
    else if (command == "HELP") {
        Serial.println("\nAvailable commands:");
        Serial.println("  READ - Read current temperature and humidity");
        Serial.println("  SERIAL - Display sensor serial number");
        Serial.println("  RESET - Soft reset the sensor");
        Serial.println("  CELSIUS - Display temperature in Celsius");
        Serial.println("  FAHRENHEIT - Display temperature in Fahrenheit");
        Serial.println("  HELP - Display this help message");
    }
    else {
        Serial.print("Unknown command: ");
        Serial.println(command);
        Serial.println("Type HELP for available commands");
    }
}