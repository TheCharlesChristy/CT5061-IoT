#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

class Bitmap : public GraphicsAsset {
private:
    const uint8_t* bitmapData;  // Pointer to bitmap data
    bool ownsData;              // Whether this object owns the bitmap data
    bool inverted;              // Invert colors
    
public:
    // Constructor
    Bitmap(int16_t x = 0, int16_t y = 0, int16_t width = 8, int16_t height = 8,
           const uint8_t* bitmapData = nullptr);
    
    // Destructor
    virtual ~Bitmap();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Bitmap data management
    void setBitmapData(const uint8_t* data, bool takeOwnership = false);
    const uint8_t* getBitmapData() const;
    
    // Create bitmap from simple pattern (allocates memory)
    void createFromPattern(bool pattern[], int size);
    void createCheckerboard(int squareSize = 4);
    void createGradient(bool horizontal = true);
    
    // Color inversion
    void setInverted(bool inverted);
    bool isInverted() const;
    
    // Free owned bitmap data
    void freeBitmapData();
};

#endif // BITMAP_HPP
