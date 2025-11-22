#ifndef DATA_PLOT_HPP
#define DATA_PLOT_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

// Plot style options
enum class PlotStyle {
    LINES,          // Connect points with lines
    POINTS,         // Draw only points
    LINES_POINTS    // Draw both lines and points
};

class DataPlot : public GraphicsAsset {
private:
    float* dataX;           // X data array
    float* dataY;           // Y data array
    int dataSize;           // Current number of data points
    int dataCapacity;       // Maximum capacity of arrays
    float minX, maxX;       // X axis range
    float minY, maxY;       // Y axis range
    bool autoScale;         // Auto-scale axes to fit data
    PlotStyle style;        // Plot style
    bool showAxes;          // Show axes
    bool showGrid;          // Show grid
    uint8_t gridSpacing;    // Grid spacing in pixels
    bool showAxisLabels;    // Show numeric labels on axes
    uint8_t axisLabelSize;  // Font size for axis labels
    bool useTinyAxisLabels;  // Use a custom tiny 3x5 font for axis labels
    uint8_t tinyAxisLabelScale; // Scale for tiny font (1 = 3x5)
    int animationFrame;     // Current animation frame (number of points drawn)
    
    // Helper methods
    int16_t mapX(float fx) const;
    int16_t mapY(float fy) const;
    void drawAxes(LedScreen128_64* screen);
    void drawGrid(LedScreen128_64* screen);
    void calculateRanges();
    
public:
    // Constructor
    DataPlot(int16_t x = 0, int16_t y = 0, int16_t width = 64, int16_t height = 32,
             int capacity = 50);
    
    // Destructor
    virtual ~DataPlot();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Data management
    void addPoint(float x, float y);
    void setData(const float* xData, const float* yData, int size);
    void clearData();
    int getDataSize() const;
    int getDataCapacity() const;
    
    // Get data points
    bool getPoint(int index, float& x, float& y) const;
    
    // Range settings
    void setXRange(float minX, float maxX);
    void setYRange(float minY, float maxY);
    void getXRange(float& minX, float& maxX) const;
    void getYRange(float& minY, float& maxY) const;
    
    // Display options
    void setAutoScale(bool autoScale);
    bool getAutoScale() const;
    
    void setPlotStyle(PlotStyle style);
    PlotStyle getPlotStyle() const;
    
    void setShowAxes(bool show);
    bool getShowAxes() const;
    
    void setShowGrid(bool show);
    bool getShowGrid() const;
    
    void setGridSpacing(uint8_t spacing);
    uint8_t getGridSpacing() const;
    void setShowAxisLabels(bool show);
    bool getShowAxisLabels() const;
    void setAxisLabelSize(uint8_t size);
    uint8_t getAxisLabelSize() const;
    void setUseTinyAxisLabels(bool use);
    bool getUseTinyAxisLabels() const;
    void setTinyAxisLabelScale(uint8_t scale);
    uint8_t getTinyAxisLabelScale() const;
    
    // Animation control
    void resetAnimation();
    void advanceAnimation();
    int getAnimationFrame() const;
};

#endif // DATA_PLOT_HPP
