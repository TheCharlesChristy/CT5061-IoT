#ifndef SERIAL_LED_CONTROL_HPP
#define SERIAL_LED_CONTROL_HPP

#include <Arduino.h>
#include "../../include/LedScreen128_64.hpp"
#include "../../include/GraphicsAsset.hpp"
#include "../../include/TextBox.hpp"
#include "../../include/FunctionPlot.hpp"
#include "../../include/DataPlot.hpp"
#include "../../include/Table.hpp"
#include "../../include/Geometry.hpp"
#include "../../include/Bitmap.hpp"

#define MAX_GRAPHICS_ASSETS 10

class SerialLedControl {
private:
    LedScreen128_64* screen;
    Stream* serial;  // Use Stream base class for compatibility
    String input_buffer;
    bool echo_commands;
    
    // Graphics assets storage
    GraphicsAsset* assets[MAX_GRAPHICS_ASSETS];
    int assetCount;
    
    // Command parsing
    void processCommand(String command);
    void parseAndExecute(String cmd, String args);
    
    // Command handlers
    void handleClear();
    void handleDisplay();
    void handleText(String args);
    void handleTextSize(String args);
    void handleCursor(String args);
    void handlePixel(String args);
    void handleLine(String args);
    void handleRect(String args);
    void handleFillRect(String args);
    void handleCircle(String args);
    void handleFillCircle(String args);
    void handleTriangle(String args);
    void handleFillTriangle(String args);
    void handleProgressBar(String args);
    void handleInvert(String args);
    void handleDim(String args);
    void handleRotation(String args);
    void handleScroll(String args);
    void handleHelp();
    
    // Graphics asset command handlers
    void handleCreateTextBox(String args);
    void handleCreateDataPlot(String args);
    void handleCreateTable(String args);
    void handleCreateGeometry(String args);
    void handleCreateBitmap(String args);
    void handleSetCell(String args);
    void handleAddPoint(String args);
    void handleDrawAsset(String args);
    void handleDrawAllAssets();
    void handleListAssets();
    void handleDeleteAsset(String args);
    void handleDeleteAllAssets();
    void handleSetAssetPos(String args);
    void handleSetAssetSize(String args);
    void handleSetAssetBorder(String args);
    void handleSetAssetVisible(String args);
    void handleSetText(String args);
    void handleSetAnimate(String args);
    void handleSetZIndex(String args);
    void handleSetTextBoxSize(String args);
    
    // Utility methods
    void printPrompt();
    void printOk();
    void printError(const char* message);
    int parseIntArg(String& args);
    float parseFloatArg(String& args);
    String parseStringArg(String& args);
    
public:
    // Constructor
    SerialLedControl(LedScreen128_64* ledScreen, Stream* serialPort = &Serial);
    
    // Initialize the serial control
    void begin(unsigned long baudRate = 115200);
    
    // Main run method - call this in loop()
    void run();
    
    // Enable/disable command echo
    void setEcho(bool enable);
    
    // Get screen reference
    LedScreen128_64* getScreen();
};

#endif // SERIAL_LED_CONTROL_HPP
