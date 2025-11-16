#ifndef FUNCTION_PLOT_HPP
#define FUNCTION_PLOT_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

// Function pointer type for mathematical functions
typedef float (*MathFunction)(float);

class FunctionPlot : public GraphicsAsset {
private:
    MathFunction function;
    float minX;
    float maxX;
    float minY;
    float maxY;
    bool autoScaleY;
    bool showAxes;
    bool showGrid;
    uint8_t gridSpacing;
    int animationFrame;  // Current animation frame (pixels drawn from left)
    
    // Helper methods
    int16_t mapX(float fx) const;
    int16_t mapY(float fy) const;
    void drawAxes(LedScreen128_64* screen);
    void drawGrid(LedScreen128_64* screen);
    void calculateYRange();
    
public:
    // Constructor
    FunctionPlot(int16_t x = 0, int16_t y = 0, int16_t width = 64, int16_t height = 32,
                 MathFunction func = nullptr);
    
    // Destructor
    virtual ~FunctionPlot();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Function management
    void setFunction(MathFunction func);
    MathFunction getFunction() const;
    
    // Range settings
    void setXRange(float minX, float maxX);
    void setYRange(float minY, float maxY);
    void getXRange(float& minX, float& maxX) const;
    void getYRange(float& minY, float& maxY) const;
    
    // Display options
    void setAutoScaleY(bool autoScale);
    bool getAutoScaleY() const;
    
    void setShowAxes(bool show);
    bool getShowAxes() const;
    
    void setShowGrid(bool show);
    bool getShowGrid() const;
    
    void setGridSpacing(uint8_t spacing);
    uint8_t getGridSpacing() const;
    
    // Animation control
    void resetAnimation();
    void advanceAnimation();
    int getAnimationFrame() const;
};

#endif // FUNCTION_PLOT_HPP
