#ifndef LED_SCREEN_128_64_HPP
#define LED_SCREEN_128_64_HPP

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Device.hpp"

// Forward declaration
class GraphicsAsset;

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1  // Reset pin (or -1 if sharing Arduino reset pin)
#define MAX_SCREEN_ASSETS 20

class LedScreen128_64 : public Device {
private:
    Adafruit_SSD1306* display;
    bool display_initialized;
    
    // Graphics assets management
    GraphicsAsset* assets[MAX_SCREEN_ASSETS];
    int assetCount;
    
public:
    // Constructor - default address is 0x3C for most SSD1306 displays
    LedScreen128_64(uint8_t address = 0x3C);
    
    // Destructor
    ~LedScreen128_64();
    
    // Initialize the display
    bool begin() override;
    
    // Display control methods
    void clearDisplay();
    void displayBuffer();  // Call this to actually show changes on screen
    void invertDisplay(bool invert);
    void dim(bool dimmed);
    
    // Screen operations
    void fillScreen(bool white = true);
    void setRotation(uint8_t rotation);  // 0, 1, 2, or 3
    
    // Pixel operations
    void drawPixel(int16_t x, int16_t y, bool white = true);
    
    // Line drawing
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool white = true);
    void drawFastVLine(int16_t x, int16_t y, int16_t length, bool white = true);
    void drawFastHLine(int16_t x, int16_t y, int16_t length, bool white = true);
    
    // Shape drawing - outlined
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white = true);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool white = true);
    void drawCircle(int16_t x, int16_t y, int16_t r, bool white = true);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool white = true);
    
    // Shape drawing - filled
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white = true);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool white = true);
    void fillCircle(int16_t x, int16_t y, int16_t r, bool white = true);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool white = true);
    
    // Text operations
    void setCursor(int16_t x, int16_t y);
    void setTextSize(uint8_t size);  // 1, 2, 3, or 4
    void setTextColor(bool white, bool background = false);
    void setTextWrap(bool wrap);
    void print(const char* text);
    void print(int value);
    void print(long value);
    void print(unsigned long value);
    void print(float value, int decimals = 2);
    void println(const char* text);
    void println(int value);
    void println(long value);
    void println(unsigned long value);
    void println(float value, int decimals = 2);
    void drawChar(int16_t x, int16_t y, char c, bool white = true, bool bg = false, uint8_t size = 1);
    
    // Helper methods for common display patterns
    void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t percentage);
    void drawCenteredText(const char* text, int16_t y, uint8_t textSize = 1);
    void drawRightAlignedNumber(int value, int16_t x, int16_t y, uint8_t textSize = 1);
    
    // Bitmap drawing
    void drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, bool white = true);
    
    // Scrolling operations
    void startScrollRight(uint8_t start, uint8_t stop);
    void startScrollLeft(uint8_t start, uint8_t stop);
    void startScrollDiagRight(uint8_t start, uint8_t stop);
    void startScrollDiagLeft(uint8_t start, uint8_t stop);
    void stopScroll();
    
    // Get display object for advanced operations
    Adafruit_SSD1306* getDisplayObject();
    
    // Screen dimensions
    int16_t getWidth() const;
    int16_t getHeight() const;
    
    // Graphics assets management
    bool addAsset(GraphicsAsset* asset);
    bool removeAsset(GraphicsAsset* asset);
    void clearAssets();
    void drawAssets();  // Draw all assets in z-index order
    int getAssetCount() const;
};

#endif // LED_SCREEN_128_64_HPP
