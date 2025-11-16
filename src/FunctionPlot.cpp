#include "FunctionPlot.hpp"
#include <math.h>

// Constructor
FunctionPlot::FunctionPlot(int16_t x, int16_t y, int16_t width, int16_t height, MathFunction func)
    : GraphicsAsset(x, y, width, height, AssetType::FUNCTIONPLOT), function(func), 
      minX(-10.0f), maxX(10.0f), minY(-10.0f), maxY(10.0f),
      autoScaleY(false), showAxes(true), showGrid(false), gridSpacing(10), animationFrame(0) {
}

// Destructor
FunctionPlot::~FunctionPlot() {
}

// Draw method implementation
void FunctionPlot::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr || function == nullptr) {
        return;
    }
    
    // Draw border if enabled
    if (border) {
        screen->drawRect(x, y, width, height, true);
    }
    
    // Calculate Y range if auto-scaling is enabled
    if (autoScaleY) {
        calculateYRange();
    }
    
    // Draw grid if enabled
    if (showGrid) {
        drawGrid(screen);
    }
    
    // Draw axes if enabled
    if (showAxes) {
        drawAxes(screen);
    }
    
    // Plot the function
    int16_t prevScreenX = -1;
    int16_t prevScreenY = -1;
    bool prevValid = false;
    
    // Determine how many pixels to draw (for animation)
    int maxPixels = width;
    if (animate && animationFrame < width) {
        maxPixels = animationFrame;
        // Auto-advance animation on each draw
        animationFrame++;
    }
    
    // Sample the function across the width
    for (int16_t i = 0; i < maxPixels; i++) {
        // Calculate the x value in function space
        float fx = minX + (maxX - minX) * (float)i / (float)(width - 1);
        
        // Evaluate the function
        float fy = function(fx);
        
        // Check if the result is valid (not NaN or Inf)
        if (isnan(fy) || isinf(fy)) {
            prevValid = false;
            continue;
        }
        
        // Check if y is within range
        if (fy < minY || fy > maxY) {
            prevValid = false;
            continue;
        }
        
        // Map to screen coordinates
        int16_t screenX = mapX(fx);
        int16_t screenY = mapY(fy);
        
        // Draw line from previous point if valid
        if (prevValid && abs(screenY - prevScreenY) < height) {
            screen->drawLine(prevScreenX, prevScreenY, screenX, screenY, true);
        } else {
            // Just draw a point
            screen->drawPixel(screenX, screenY, true);
        }
        
        prevScreenX = screenX;
        prevScreenY = screenY;
        prevValid = true;
    }
}

// Function management
void FunctionPlot::setFunction(MathFunction func) {
    this->function = func;
}

MathFunction FunctionPlot::getFunction() const {
    return function;
}

// Range settings
void FunctionPlot::setXRange(float minX, float maxX) {
    if (minX < maxX) {
        this->minX = minX;
        this->maxX = maxX;
    }
}

void FunctionPlot::setYRange(float minY, float maxY) {
    if (minY < maxY) {
        this->minY = minY;
        this->maxY = maxY;
        this->autoScaleY = false;
    }
}

void FunctionPlot::getXRange(float& minX, float& maxX) const {
    minX = this->minX;
    maxX = this->maxX;
}

void FunctionPlot::getYRange(float& minY, float& maxY) const {
    minY = this->minY;
    maxY = this->maxY;
}

// Display options
void FunctionPlot::setAutoScaleY(bool autoScale) {
    this->autoScaleY = autoScale;
}

bool FunctionPlot::getAutoScaleY() const {
    return autoScaleY;
}

void FunctionPlot::setShowAxes(bool show) {
    this->showAxes = show;
}

bool FunctionPlot::getShowAxes() const {
    return showAxes;
}

void FunctionPlot::setShowGrid(bool show) {
    this->showGrid = show;
}

bool FunctionPlot::getShowGrid() const {
    return showGrid;
}

void FunctionPlot::setGridSpacing(uint8_t spacing) {
    if (spacing > 0) {
        this->gridSpacing = spacing;
    }
}

uint8_t FunctionPlot::getGridSpacing() const {
    return gridSpacing;
}

// Helper methods
int16_t FunctionPlot::mapX(float fx) const {
    // Map function x to screen x
    float normalized = (fx - minX) / (maxX - minX);
    return x + (int16_t)(normalized * (width - 1));
}

int16_t FunctionPlot::mapY(float fy) const {
    // Map function y to screen y (inverted because screen y increases downward)
    float normalized = (fy - minY) / (maxY - minY);
    return y + height - 1 - (int16_t)(normalized * (height - 1));
}

void FunctionPlot::drawAxes(LedScreen128_64* screen) {
    // Draw x-axis if y=0 is in range
    if (minY <= 0.0f && maxY >= 0.0f) {
        int16_t y0 = mapY(0.0f);
        screen->drawFastHLine(x, y0, width, true);
    }
    
    // Draw y-axis if x=0 is in range
    if (minX <= 0.0f && maxX >= 0.0f) {
        int16_t x0 = mapX(0.0f);
        screen->drawFastVLine(x0, y, height, true);
    }
}

void FunctionPlot::drawGrid(LedScreen128_64* screen) {
    // Draw vertical grid lines
    for (int16_t i = gridSpacing; i < width; i += gridSpacing) {
        int16_t gridX = x + i;
        for (int16_t j = 0; j < height; j += 2) {
            screen->drawPixel(gridX, y + j, true);
        }
    }
    
    // Draw horizontal grid lines
    for (int16_t i = gridSpacing; i < height; i += gridSpacing) {
        int16_t gridY = y + i;
        for (int16_t j = 0; j < width; j += 2) {
            screen->drawPixel(x + j, gridY, true);
        }
    }
}

void FunctionPlot::calculateYRange() {
    if (function == nullptr) {
        return;
    }
    
    float calcMinY = 1e6;
    float calcMaxY = -1e6;
    bool foundValid = false;
    
    // Sample the function to find min/max
    int samples = width * 2; // Sample more densely than pixels
    for (int i = 0; i < samples; i++) {
        float fx = minX + (maxX - minX) * (float)i / (float)(samples - 1);
        float fy = function(fx);
        
        // Skip invalid values
        if (isnan(fy) || isinf(fy)) {
            continue;
        }
        
        if (fy < calcMinY) calcMinY = fy;
        if (fy > calcMaxY) calcMaxY = fy;
        foundValid = true;
    }
    
    if (foundValid && calcMinY < calcMaxY) {
        // Add 10% padding
        float range = calcMaxY - calcMinY;
        float padding = range * 0.1f;
        minY = calcMinY - padding;
        maxY = calcMaxY + padding;
    }
}

// Animation control
void FunctionPlot::resetAnimation() {
    animationFrame = 0;
}

void FunctionPlot::advanceAnimation() {
    if (animationFrame < width) {
        animationFrame++;
    }
}

int FunctionPlot::getAnimationFrame() const {
    return animationFrame;
}
