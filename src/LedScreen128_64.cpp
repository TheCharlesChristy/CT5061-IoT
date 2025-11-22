#include "LedScreen128_64.hpp"
#include "GraphicsAsset.hpp"
#include <algorithm>

// Constructor
LedScreen128_64::LedScreen128_64(uint8_t address)
    : Device(address), display(nullptr), display_initialized(false) {
    // Create display object with I2C
    display.reset(new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, wire_instance, OLED_RESET));
    
    // assets vector default-initialized empty
}

// Destructor
LedScreen128_64::~LedScreen128_64() {
    // unique_ptr will auto-delete
}

// Initialize the display
bool LedScreen128_64::begin() {
    if (!Device::begin()) {
        return false;
    }
    
    // Initialize the SSD1306 display
    if (!display->begin(SSD1306_SWITCHCAPVCC, i2c_address)) {
        display_initialized = false;
        return false;
    }
    
    display_initialized = true;
    clearDisplay();
    displayBuffer();
    
    return true;
}

// Display control methods
void LedScreen128_64::clearDisplay() {
    if (display_initialized) {
        display->clearDisplay();
    }
}

void LedScreen128_64::displayBuffer() {
    if (display_initialized) {
        display->display();
    }
}

void LedScreen128_64::invertDisplay(bool invert) {
    if (display_initialized) {
        display->invertDisplay(invert);
    }
}

void LedScreen128_64::dim(bool dimmed) {
    if (display_initialized) {
        display->dim(dimmed);
    }
}

// Screen operations
void LedScreen128_64::fillScreen(bool white) {
    if (display_initialized) {
        display->fillScreen(white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::setRotation(uint8_t rotation) {
    if (display_initialized) {
        display->setRotation(rotation);
    }
}

// Pixel operations
void LedScreen128_64::drawPixel(int16_t x, int16_t y, bool white) {
    if (display_initialized) {
        display->drawPixel(x, y, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

// Line drawing
void LedScreen128_64::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool white) {
    if (display_initialized) {
        display->drawLine(x0, y0, x1, y1, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::drawFastVLine(int16_t x, int16_t y, int16_t length, bool white) {
    if (display_initialized) {
        display->drawFastVLine(x, y, length, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::drawFastHLine(int16_t x, int16_t y, int16_t length, bool white) {
    if (display_initialized) {
        display->drawFastHLine(x, y, length, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

// Shape drawing - outlined
void LedScreen128_64::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white) {
    if (display_initialized) {
        display->drawRect(x, y, w, h, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool white) {
    if (display_initialized) {
        display->drawRoundRect(x, y, w, h, r, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::drawCircle(int16_t x, int16_t y, int16_t r, bool white) {
    if (display_initialized) {
        display->drawCircle(x, y, r, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool white) {
    if (display_initialized) {
        display->drawTriangle(x0, y0, x1, y1, x2, y2, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

// Shape drawing - filled
void LedScreen128_64::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, bool white) {
    if (display_initialized) {
        display->fillRect(x, y, w, h, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, bool white) {
    if (display_initialized) {
        display->fillRoundRect(x, y, w, h, r, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::fillCircle(int16_t x, int16_t y, int16_t r, bool white) {
    if (display_initialized) {
        display->fillCircle(x, y, r, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

void LedScreen128_64::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool white) {
    if (display_initialized) {
        display->fillTriangle(x0, y0, x1, y1, x2, y2, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

// Text operations
void LedScreen128_64::setCursor(int16_t x, int16_t y) {
    if (display_initialized) {
        display->setCursor(x, y);
    }
}

void LedScreen128_64::setTextSize(uint8_t size) {
    if (display_initialized) {
        display->setTextSize(size);
    }
}

void LedScreen128_64::setTextColor(bool white, bool background) {
    if (display_initialized) {
        if (background) {
            display->setTextColor(white ? SSD1306_WHITE : SSD1306_BLACK, 
                                 white ? SSD1306_BLACK : SSD1306_WHITE);
        } else {
            display->setTextColor(white ? SSD1306_WHITE : SSD1306_BLACK);
        }
    }
}

void LedScreen128_64::setTextWrap(bool wrap) {
    if (display_initialized) {
        display->setTextWrap(wrap);
    }
}

void LedScreen128_64::print(const char* text) {
    if (display_initialized) {
        display->print(text);
    }
}

void LedScreen128_64::print(int value) {
    if (display_initialized) {
        display->print(value);
    }
}

void LedScreen128_64::print(long value) {
    if (display_initialized) {
        display->print(value);
    }
}

void LedScreen128_64::print(unsigned long value) {
    if (display_initialized) {
        display->print(value);
    }
}

void LedScreen128_64::print(float value, int decimals) {
    if (display_initialized) {
        display->print(value, decimals);
    }
}

void LedScreen128_64::println(const char* text) {
    if (display_initialized) {
        display->println(text);
    }
}

void LedScreen128_64::println(int value) {
    if (display_initialized) {
        display->println(value);
    }
}

void LedScreen128_64::println(long value) {
    if (display_initialized) {
        display->println(value);
    }
}

void LedScreen128_64::println(unsigned long value) {
    if (display_initialized) {
        display->println(value);
    }
}

void LedScreen128_64::println(float value, int decimals) {
    if (display_initialized) {
        display->println(value, decimals);
    }
}

void LedScreen128_64::drawChar(int16_t x, int16_t y, char c, bool white, bool bg, uint8_t size) {
    if (display_initialized) {
        display->drawChar(x, y, c, 
                         white ? SSD1306_WHITE : SSD1306_BLACK,
                         bg ? (white ? SSD1306_BLACK : SSD1306_WHITE) : (white ? SSD1306_WHITE : SSD1306_BLACK),
                         size);
    }
}

// Helper methods for common display patterns
void LedScreen128_64::drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t percentage) {
    if (!display_initialized) {
        return;
    }
    
    // Ensure percentage is within bounds
    if (percentage > 100) {
        percentage = 100;
    }
    
    // Draw outline
    drawRect(x, y, w, h, true);
    
    // Calculate fill width
    int16_t fill_width = ((w - 2) * percentage) / 100;
    
    // Draw filled portion
    if (fill_width > 0) {
        fillRect(x + 1, y + 1, fill_width, h - 2, true);
    }
}

void LedScreen128_64::drawCenteredText(const char* text, int16_t y, uint8_t textSize) {
    if (!display_initialized) {
        return;
    }
    
    // Calculate text width (each character is 6 pixels wide in default font)
    int16_t text_width = strlen(text) * 6 * textSize;
    int16_t x = (SCREEN_WIDTH - text_width) / 2;
    
    setTextSize(textSize);
    setCursor(x, y);
    print(text);
}

void LedScreen128_64::drawRightAlignedNumber(int value, int16_t x, int16_t y, uint8_t textSize) {
    if (!display_initialized) {
        return;
    }
    
    // Convert number to string to get length
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%d", value);
    
    // Calculate text width
    int16_t text_width = strlen(buffer) * 6 * textSize;
    int16_t actual_x = x - text_width;
    
    setTextSize(textSize);
    setCursor(actual_x, y);
    print(value);
}

// Bitmap drawing
void LedScreen128_64::drawBitmap(int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, bool white) {
    if (display_initialized) {
        display->drawBitmap(x, y, bitmap, w, h, white ? SSD1306_WHITE : SSD1306_BLACK);
    }
}

// Scrolling operations
void LedScreen128_64::startScrollRight(uint8_t start, uint8_t stop) {
    if (display_initialized) {
        display->startscrollright(start, stop);
    }
}

void LedScreen128_64::startScrollLeft(uint8_t start, uint8_t stop) {
    if (display_initialized) {
        display->startscrollleft(start, stop);
    }
}

void LedScreen128_64::startScrollDiagRight(uint8_t start, uint8_t stop) {
    if (display_initialized) {
        display->startscrolldiagright(start, stop);
    }
}

void LedScreen128_64::startScrollDiagLeft(uint8_t start, uint8_t stop) {
    if (display_initialized) {
        display->startscrolldiagleft(start, stop);
    }
}

void LedScreen128_64::stopScroll() {
    if (display_initialized) {
        display->stopscroll();
    }
}

// Get display object for advanced operations
Adafruit_SSD1306* LedScreen128_64::getDisplayObject() {
    return display.get();
}

// Screen dimensions
int16_t LedScreen128_64::getWidth() const {
    return SCREEN_WIDTH;
}

int16_t LedScreen128_64::getHeight() const {
    return SCREEN_HEIGHT;
}

// Graphics assets management
bool LedScreen128_64::addAsset(GraphicsAsset* asset) {
    if (asset == nullptr) return false;
    if (assets.size() >= MAX_SCREEN_ASSETS) return false;
    assets.push_back(asset);
    return true;
}

bool LedScreen128_64::removeAsset(GraphicsAsset* asset) {
    if (asset == nullptr) return false;
    auto it = std::find(assets.begin(), assets.end(), asset);
    if (it != assets.end()) {
        assets.erase(it);
        return true;
    }
    return false;
}

void LedScreen128_64::clearAssets() {
    assets.clear();
}

void LedScreen128_64::drawAssets() {
    if (!display_initialized) {
        return;
    }
    
    // Sort assets by z-index (lower values drawn first)
    std::sort(assets.begin(), assets.end(), [](GraphicsAsset* a, GraphicsAsset* b){
        if (a == nullptr && b == nullptr) return false;
        if (a == nullptr) return true;
        if (b == nullptr) return false;
        return a->getZIndex() < b->getZIndex();
    });

    // Draw all assets in z-index order
    for (auto asset : assets) {
        if (asset != nullptr && asset->isVisible()) {
            asset->draw(this);
        }
    }
}

int LedScreen128_64::getAssetCount() const {
    return (int)assets.size();
}
