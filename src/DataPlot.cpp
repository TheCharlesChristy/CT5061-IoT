#include "DataPlot.hpp"
#include <math.h>

// Constructor
DataPlot::DataPlot(int16_t x, int16_t y, int16_t width, int16_t height, int capacity)
    : GraphicsAsset(x, y, width, height, AssetType::DATAPLOT), dataX(nullptr), dataY(nullptr),
      dataSize(0), dataCapacity(capacity), minX(0.0f), maxX(100.0f),
      minY(0.0f), maxY(100.0f), autoScale(true), style(PlotStyle::LINES),
      showAxes(true), showGrid(false), gridSpacing(10), animationFrame(0) {
    
    // Allocate data arrays
    if (capacity > 0) {
        dataX = new float[capacity];
        dataY = new float[capacity];
    }
}

// Destructor
DataPlot::~DataPlot() {
    if (dataX != nullptr) {
        delete[] dataX;
        dataX = nullptr;
    }
    if (dataY != nullptr) {
        delete[] dataY;
        dataY = nullptr;
    }
}

// Draw method implementation
void DataPlot::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr || dataSize == 0) {
        return;
    }
    
    // Draw border if enabled
    if (border) {
        screen->drawRect(x, y, width, height, true);
    }
    
    // Calculate ranges if auto-scaling is enabled
    if (autoScale) {
        calculateRanges();
    }
    
    // Draw grid if enabled
    if (showGrid) {
        drawGrid(screen);
    }
    
    // Draw axes if enabled
    if (showAxes) {
        drawAxes(screen);
    }
    
    // Determine how many points to draw (for animation)
    int maxPoints = dataSize;
    if (animate && animationFrame < dataSize) {
        maxPoints = animationFrame;
        // Auto-advance animation on each draw
        animationFrame++;
    }
    
    // Plot the data
    for (int i = 0; i < maxPoints; i++) {
        // Check if data point is within range
        if (dataX[i] < minX || dataX[i] > maxX || 
            dataY[i] < minY || dataY[i] > maxY) {
            continue;
        }
        
        // Map to screen coordinates
        int16_t screenX = mapX(dataX[i]);
        int16_t screenY = mapY(dataY[i]);
        
        // Draw line to previous point if applicable
        if (i > 0 && (style == PlotStyle::LINES || style == PlotStyle::LINES_POINTS)) {
            // Check if previous point is also in range
            if (dataX[i-1] >= minX && dataX[i-1] <= maxX &&
                dataY[i-1] >= minY && dataY[i-1] <= maxY) {
                int16_t prevScreenX = mapX(dataX[i-1]);
                int16_t prevScreenY = mapY(dataY[i-1]);
                
                // Only draw line if points are reasonably close
                if (abs(screenX - prevScreenX) < width && 
                    abs(screenY - prevScreenY) < height) {
                    screen->drawLine(prevScreenX, prevScreenY, screenX, screenY, true);
                }
            }
        }
        
        // Draw point if applicable
        if (style == PlotStyle::POINTS || style == PlotStyle::LINES_POINTS) {
            // Draw a small cross for each point
            screen->drawPixel(screenX, screenY, true);
            if (screenX > 0) screen->drawPixel(screenX - 1, screenY, true);
            if (screenX < 127) screen->drawPixel(screenX + 1, screenY, true);
            if (screenY > 0) screen->drawPixel(screenX, screenY - 1, true);
            if (screenY < 63) screen->drawPixel(screenX, screenY + 1, true);
        }
    }
}

// Data management
void DataPlot::addPoint(float x, float y) {
    if (dataSize < dataCapacity) {
        dataX[dataSize] = x;
        dataY[dataSize] = y;
        dataSize++;
    } else {
        // Shift data left and add new point at end (rolling buffer)
        for (int i = 0; i < dataCapacity - 1; i++) {
            dataX[i] = dataX[i + 1];
            dataY[i] = dataY[i + 1];
        }
        dataX[dataCapacity - 1] = x;
        dataY[dataCapacity - 1] = y;
    }
}

void DataPlot::setData(const float* xData, const float* yData, int size) {
    clearData();
    
    int pointsToAdd = (size < dataCapacity) ? size : dataCapacity;
    for (int i = 0; i < pointsToAdd; i++) {
        dataX[i] = xData[i];
        dataY[i] = yData[i];
    }
    dataSize = pointsToAdd;
}

void DataPlot::clearData() {
    dataSize = 0;
}

int DataPlot::getDataSize() const {
    return dataSize;
}

int DataPlot::getDataCapacity() const {
    return dataCapacity;
}

// Get data points
bool DataPlot::getPoint(int index, float& x, float& y) const {
    if (index >= 0 && index < dataSize) {
        x = dataX[index];
        y = dataY[index];
        return true;
    }
    return false;
}

// Range settings
void DataPlot::setXRange(float minX, float maxX) {
    if (minX < maxX) {
        this->minX = minX;
        this->maxX = maxX;
        this->autoScale = false;
    }
}

void DataPlot::setYRange(float minY, float maxY) {
    if (minY < maxY) {
        this->minY = minY;
        this->maxY = maxY;
        this->autoScale = false;
    }
}

void DataPlot::getXRange(float& minX, float& maxX) const {
    minX = this->minX;
    maxX = this->maxX;
}

void DataPlot::getYRange(float& minY, float& maxY) const {
    minY = this->minY;
    maxY = this->maxY;
}

// Display options
void DataPlot::setAutoScale(bool autoScale) {
    this->autoScale = autoScale;
}

bool DataPlot::getAutoScale() const {
    return autoScale;
}

void DataPlot::setPlotStyle(PlotStyle style) {
    this->style = style;
}

PlotStyle DataPlot::getPlotStyle() const {
    return style;
}

void DataPlot::setShowAxes(bool show) {
    this->showAxes = show;
}

bool DataPlot::getShowAxes() const {
    return showAxes;
}

void DataPlot::setShowGrid(bool show) {
    this->showGrid = show;
}

bool DataPlot::getShowGrid() const {
    return showGrid;
}

void DataPlot::setGridSpacing(uint8_t spacing) {
    if (spacing > 0) {
        this->gridSpacing = spacing;
    }
}

uint8_t DataPlot::getGridSpacing() const {
    return gridSpacing;
}

// Helper methods
int16_t DataPlot::mapX(float fx) const {
    // Map data x to screen x
    float normalized = (fx - minX) / (maxX - minX);
    return x + (int16_t)(normalized * (width - 1));
}

int16_t DataPlot::mapY(float fy) const {
    // Map data y to screen y (inverted because screen y increases downward)
    float normalized = (fy - minY) / (maxY - minY);
    return y + height - 1 - (int16_t)(normalized * (height - 1));
}

void DataPlot::drawAxes(LedScreen128_64* screen) {
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

void DataPlot::drawGrid(LedScreen128_64* screen) {
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

void DataPlot::calculateRanges() {
    if (dataSize == 0) {
        return;
    }
    
    // Find min and max values
    float calcMinX = dataX[0];
    float calcMaxX = dataX[0];
    float calcMinY = dataY[0];
    float calcMaxY = dataY[0];
    
    for (int i = 1; i < dataSize; i++) {
        if (dataX[i] < calcMinX) calcMinX = dataX[i];
        if (dataX[i] > calcMaxX) calcMaxX = dataX[i];
        if (dataY[i] < calcMinY) calcMinY = dataY[i];
        if (dataY[i] > calcMaxY) calcMaxY = dataY[i];
    }
    
    // Add 10% padding
    float rangeX = calcMaxX - calcMinX;
    float rangeY = calcMaxY - calcMinY;
    
    // Avoid division by zero for constant data
    if (rangeX < 0.0001f) rangeX = 1.0f;
    if (rangeY < 0.0001f) rangeY = 1.0f;
    
    float paddingX = rangeX * 0.1f;
    float paddingY = rangeY * 0.1f;
    
    minX = calcMinX - paddingX;
    maxX = calcMaxX + paddingX;
    minY = calcMinY - paddingY;
    maxY = calcMaxY + paddingY;
}

// Animation control
void DataPlot::resetAnimation() {
    animationFrame = 0;
}

void DataPlot::advanceAnimation() {
    if (animationFrame < dataSize) {
        animationFrame++;
    }
}

int DataPlot::getAnimationFrame() const {
    return animationFrame;
}
