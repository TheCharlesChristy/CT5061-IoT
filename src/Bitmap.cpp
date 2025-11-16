#include "Bitmap.hpp"

// Constructor
Bitmap::Bitmap(int16_t x, int16_t y, int16_t width, int16_t height, const uint8_t* bitmapData)
    : GraphicsAsset(x, y, width, height, AssetType::BITMAP), bitmapData(bitmapData),
      ownsData(false), inverted(false) {
}

// Destructor
Bitmap::~Bitmap() {
    freeBitmapData();
}

// Draw method implementation
void Bitmap::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr || bitmapData == nullptr) {
        return;
    }
    
    // Draw border if enabled
    if (border) {
        screen->drawRect(x, y, width, height, true);
    }
    
    // Draw the bitmap
    screen->drawBitmap(x, y, bitmapData, width, height, !inverted);
}

// Bitmap data management
void Bitmap::setBitmapData(const uint8_t* data, bool takeOwnership) {
    // Free old data if we own it
    if (ownsData && bitmapData != nullptr) {
        delete[] bitmapData;
    }
    
    bitmapData = data;
    ownsData = takeOwnership;
}

const uint8_t* Bitmap::getBitmapData() const {
    return bitmapData;
}

// Create bitmap from simple pattern (allocates memory)
void Bitmap::createFromPattern(bool pattern[], int size) {
    freeBitmapData();
    
    if (size <= 0) {
        return;
    }
    
    // Calculate bytes needed for bitmap
    int bytesNeeded = ((width * height) + 7) / 8;
    uint8_t* newData = new uint8_t[bytesNeeded];
    
    // Initialize to zero
    for (int i = 0; i < bytesNeeded; i++) {
        newData[i] = 0;
    }
    
    // Fill bitmap from pattern
    int patternIdx = 0;
    for (int i = 0; i < width * height && i < size; i++) {
        if (pattern[i]) {
            int byteIdx = i / 8;
            int bitIdx = i % 8;
            newData[byteIdx] |= (1 << (7 - bitIdx));
        }
    }
    
    bitmapData = newData;
    ownsData = true;
}

void Bitmap::createCheckerboard(int squareSize) {
    freeBitmapData();
    
    if (squareSize <= 0) {
        squareSize = 1;
    }
    
    // Calculate bytes needed
    int bytesNeeded = ((width * height) + 7) / 8;
    uint8_t* newData = new uint8_t[bytesNeeded];
    
    // Initialize to zero
    for (int i = 0; i < bytesNeeded; i++) {
        newData[i] = 0;
    }
    
    // Create checkerboard pattern
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int squareRow = row / squareSize;
            int squareCol = col / squareSize;
            bool isWhite = ((squareRow + squareCol) % 2) == 0;
            
            if (isWhite) {
                int bitIndex = row * width + col;
                int byteIdx = bitIndex / 8;
                int bitIdx = bitIndex % 8;
                newData[byteIdx] |= (1 << (7 - bitIdx));
            }
        }
    }
    
    bitmapData = newData;
    ownsData = true;
}

void Bitmap::createGradient(bool horizontal) {
    freeBitmapData();
    
    // Calculate bytes needed
    int bytesNeeded = ((width * height) + 7) / 8;
    uint8_t* newData = new uint8_t[bytesNeeded];
    
    // Initialize to zero
    for (int i = 0; i < bytesNeeded; i++) {
        newData[i] = 0;
    }
    
    // Create gradient pattern
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            bool isWhite = false;
            
            if (horizontal) {
                // Horizontal gradient: denser on left, sparser on right
                int threshold = (col * 100) / width;
                isWhite = ((row + col) % 4) < (4 - threshold / 25);
            } else {
                // Vertical gradient: denser on top, sparser on bottom
                int threshold = (row * 100) / height;
                isWhite = ((row + col) % 4) < (4 - threshold / 25);
            }
            
            if (isWhite) {
                int bitIndex = row * width + col;
                int byteIdx = bitIndex / 8;
                int bitIdx = bitIndex % 8;
                newData[byteIdx] |= (1 << (7 - bitIdx));
            }
        }
    }
    
    bitmapData = newData;
    ownsData = true;
}

// Color inversion
void Bitmap::setInverted(bool inverted) {
    this->inverted = inverted;
}

bool Bitmap::isInverted() const {
    return inverted;
}

// Free owned bitmap data
void Bitmap::freeBitmapData() {
    if (ownsData && bitmapData != nullptr) {
        delete[] bitmapData;
        bitmapData = nullptr;
        ownsData = false;
    }
}
