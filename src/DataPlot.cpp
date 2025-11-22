#include "DataPlot.hpp"
#include <math.h>
#include <string.h>

// Compute inner content rectangle for plotting to leave room for axis labels
static void computeContentRect(int16_t x, int16_t y, int16_t width, int16_t height,
                               uint8_t axisLabelSize, bool showAxisLabels,
                               int16_t &contentX, int16_t &contentY, int16_t &contentW, int16_t &contentH) {
    int16_t leftPad = 2;
    int16_t rightPad = 2;
    int16_t topPad = 2;
    int16_t bottomPad = 2;
    if (showAxisLabels) {
        // space for y labels on the left and x labels on the bottom
        leftPad = (6 * axisLabelSize) + 4;  // estimated char width * size + margin
        bottomPad = (8 * axisLabelSize) + 4; // estimated char height * size + margin
    }
    contentX = x + leftPad;
    contentY = y + topPad;
    contentW = width - leftPad - rightPad;
    contentH = height - topPad - bottomPad;
    if (contentW < 1) contentW = 1;
    if (contentH < 1) contentH = 1;
}

// Tiny 3x5 font for digits and basic symbols
static const uint8_t tinyFont[][5] = {
    // 0
    {0b111,0b101,0b101,0b101,0b111},
    // 1
    {0b010,0b110,0b010,0b010,0b111},
    // 2
    {0b111,0b001,0b111,0b100,0b111},
    // 3
    {0b111,0b001,0b111,0b001,0b111},
    // 4
    {0b101,0b101,0b111,0b001,0b001},
    // 5
    {0b111,0b100,0b111,0b001,0b111},
    // 6
    {0b111,0b100,0b111,0b101,0b111},
    // 7
    {0b111,0b001,0b001,0b001,0b001},
    // 8
    {0b111,0b101,0b111,0b101,0b111},
    // 9
    {0b111,0b101,0b111,0b001,0b111},
    // - (index 10)
    {0b000,0b000,0b111,0b000,0b000},
    // . (index 11)
    {0b000,0b000,0b000,0b000,0b010},
    // C (index 12)
    {0b111,0b100,0b100,0b100,0b111},
    // T (index 13)
    {0b111,0b010,0b010,0b010,0b010},
    // H (index 14)
    {0b101,0b101,0b111,0b101,0b101},
    // % (index 15)
    {0b101,0b001,0b100,0b010,0b101}
};

static void drawTinyChar(LedScreen128_64* screen, int16_t x, int16_t y, char c, uint8_t scale=1) {
    int index = -1;
    if (c >= '0' && c <= '9') index = c - '0';
    else if (c == '-') index = 10;
    else if (c == '.') index = 11;
    else if (c == 'C' || c == 'c') index = 12;
    else if (c == 'T' || c == 't') index = 13;
    else if (c == 'H' || c == 'h') index = 14;
    else if (c == '%') index = 15;
    if (index < 0) return; // unsupported char

    for (int row = 0; row < 5; row++) {
        uint8_t bits = tinyFont[index][row];
        for (int col = 0; col < 3; col++) {
            bool on = (bits >> (2 - col)) & 0x1;
            if (on) {
                for (int sy = 0; sy < scale; sy++) {
                    for (int sx = 0; sx < scale; sx++) {
                        screen->drawPixel(x + col * scale + sx, y + row * scale + sy, true);
                    }
                }
            }
        }
    }
}

static void drawTinyText(LedScreen128_64* screen, int16_t x, int16_t y, const char* text, uint8_t scale=1) {
    int16_t cursorX = x;
    while (*text) {
        char c = *text++;
        if (c == ' ')
            cursorX += (3*scale + scale); // spacing
        else {
            drawTinyChar(screen, cursorX, y, c, scale);
            cursorX += (3*scale + scale); // char width + spacing
        }
    }
}

// Constructor
DataPlot::DataPlot(int16_t x, int16_t y, int16_t width, int16_t height, int capacity)
        : GraphicsAsset(x, y, width, height, AssetType::DATAPLOT), dataX(nullptr), dataY(nullptr),
      dataSize(0), dataCapacity(capacity), minX(0.0f), maxX(100.0f),
      minY(0.0f), maxY(100.0f), autoScale(true), style(PlotStyle::LINES),
    showAxes(true), showGrid(false), gridSpacing(10), showAxisLabels(false), axisLabelSize(1), useTinyAxisLabels(false), tinyAxisLabelScale(1), autoTinyAxisLabels(true), tinyLabelAutoThreshold(36), maxTicks(0), animationFrame(0) {
    
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
    
    int16_t contentX, contentY, contentW, contentH;
    computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    // Draw grid if enabled
    if (showGrid) {
        drawGrid(screen);
    }
    
    // Draw axes if enabled
    if (showAxes) {
        drawAxes(screen);
    }

    // Draw axis labels if enabled
    if (showAxisLabels) {
        // We'll draw ticks and numeric labels alongside axes
        // Simple textual labels are small to avoid overlap
        screen->setTextSize(axisLabelSize);
        int16_t contentX, contentY, contentW, contentH;
        computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    // Draw X axis tick labels
    // We use the gridSpacing to determine tick spacing in pixels
        int16_t prevLabelX = -9999;
        bool effectiveTiny = useTinyAxisLabels || (autoTinyAxisLabels && contentW <= tinyLabelAutoThreshold);
        bool effectiveTinyY = useTinyAxisLabels || (autoTinyAxisLabels && contentH <= tinyLabelAutoThreshold);
        if (maxTicks > 1) {
            float step = (contentW - 1) / (float)(maxTicks - 1);
            for (int k = 0; k < maxTicks; k++) {
                int16_t i = (int16_t)round(k * step);
                int16_t tickX = contentX + i;
                // Inverse map to get data X value at this screen X
                float normalized = (contentW > 1) ? ((float)i) / (float)(contentW - 1) : 0.0f;
                float dataValue = minX + normalized * (maxX - minX);
                // Position label slightly below x-axis if possible; otherwise above
                int16_t labelY = contentY + contentH + 1; // default below content rect
                // If the x-axis is within range, place label near axis
                if (minY <= 0.0f && maxY >= 0.0f) {
                    int16_t y0 = mapY(0.0f);
                    if (y0 + (8 * axisLabelSize) + 1 < (y + height)) {
                        // Place below axis if there's space
                        labelY = y0 + 1;
                    } else {
                        labelY = y0 - (8 * axisLabelSize) - 1;
                    }
                }
                char buf[12];
                // Print with 1 decimal when necessary
                if (fabs(dataValue - (int)dataValue) < 0.001f) {
                    snprintf(buf, sizeof(buf), "%d", (int)dataValue);
                } else {
                    snprintf(buf, sizeof(buf), "%.1f", dataValue);
                }
                    if (effectiveTiny) {
                    int16_t labelW = (int)strlen(buf) * (3 * tinyAxisLabelScale + tinyAxisLabelScale);
                    int16_t labelX = tickX - labelW / 2;
                    if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                        drawTinyText(screen, labelX, labelY, buf, tinyAxisLabelScale);
                        prevLabelX = labelX;
                    }
                } else {
                    int16_t labelW = (int)strlen(buf) * 6 * axisLabelSize;
                    int16_t labelX = tickX - labelW / 2;
                    if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                        screen->setCursor(labelX, labelY);
                        screen->print(buf);
                        prevLabelX = labelX;
                    }
                }
            }
        } else {
            if (gridSpacing <= 0 || gridSpacing >= contentW) {
                // At least draw the min and max tick
                for (int k = 0; k < 2; k++) {
                    int16_t i = (k == 0) ? 0 : (contentW - 1);
                    int16_t tickX = contentX + i;
                    float normalized = (contentW > 1) ? ((float)i) / (float)(contentW - 1) : 0.0f;
                    float dataValue = minX + normalized * (maxX - minX);
                    int16_t labelY = contentY + contentH + 1;
                    if (minY <= 0.0f && maxY >= 0.0f) {
                        int16_t y0 = mapY(0.0f);
                        if (y0 + (8 * axisLabelSize) + 1 < (y + height)) {
                            labelY = y0 + 1;
                        } else {
                            labelY = y0 - (8 * axisLabelSize) - 1;
                        }
                    }
                    char buf[12];
                    if (fabs(dataValue - (int)dataValue) < 0.001f) {
                        snprintf(buf, sizeof(buf), "%d", (int)dataValue);
                    } else {
                        snprintf(buf, sizeof(buf), "%.1f", dataValue);
                    }
                        if (effectiveTiny) {
                        int16_t labelW = (int)strlen(buf) * (3 * tinyAxisLabelScale + tinyAxisLabelScale);
                        int16_t labelX = tickX - labelW / 2;
                        if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                            drawTinyText(screen, labelX, labelY, buf, tinyAxisLabelScale);
                            prevLabelX = labelX;
                        }
                    } else {
                        int16_t labelW = (int)strlen(buf) * 6 * axisLabelSize;
                        int16_t labelX = tickX - labelW / 2;
                        if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                            screen->setCursor(labelX, labelY);
                            screen->print(buf);
                            prevLabelX = labelX;
                        }
                    }
                }
            } else {
                for (int16_t i = gridSpacing; i < contentW; i += gridSpacing) {
                    int16_t tickX = contentX + i;
                    float normalized = (contentW > 1) ? ((float)i) / (float)(contentW - 1) : 0.0f;
                    float dataValue = minX + normalized * (maxX - minX);
                    int16_t labelY = contentY + contentH + 1; // default below content rect
                    if (minY <= 0.0f && maxY >= 0.0f) {
                        int16_t y0 = mapY(0.0f);
                        if (y0 + (8 * axisLabelSize) + 1 < (y + height)) {
                            labelY = y0 + 1;
                        } else {
                            labelY = y0 - (8 * axisLabelSize) - 1;
                        }
                    }
                    char buf[12];
                    if (fabs(dataValue - (int)dataValue) < 0.001f) {
                        snprintf(buf, sizeof(buf), "%d", (int)dataValue);
                    } else {
                        snprintf(buf, sizeof(buf), "%.1f", dataValue);
                    }
                        if (effectiveTiny) {
                        int16_t labelW = (int)strlen(buf) * (3 * tinyAxisLabelScale + tinyAxisLabelScale);
                        int16_t labelX = tickX - labelW / 2;
                        if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                            drawTinyText(screen, labelX, labelY, buf, tinyAxisLabelScale);
                            prevLabelX = labelX;
                        }
                    } else {
                        int16_t labelW = (int)strlen(buf) * 6 * axisLabelSize;
                        int16_t labelX = tickX - labelW / 2;
                        if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                            screen->setCursor(labelX, labelY);
                            screen->print(buf);
                            prevLabelX = labelX;
                        }
                    }
                }
            }
        }

        // Draw Y axis tick labels
        int16_t prevLabelY = -9999;
        if (maxTicks > 1) {
            float stepY = (contentH - 1) / (float)(maxTicks - 1);
            for (int k = 0; k < maxTicks; k++) {
                int16_t i = (int16_t)round(k * stepY);
                int16_t tickY = contentY + i;
                float normalized = (contentH > 1) ? (1.0f - ((float)(i) / (float)(contentH - 1))) : 0.0f;
                float dataValue = minY + normalized * (maxY - minY);
            char buf[12];
            if (fabs(dataValue - (int)dataValue) < 0.001f) {
                snprintf(buf, sizeof(buf), "%d", (int)dataValue);
            } else {
                snprintf(buf, sizeof(buf), "%.1f", dataValue);
            }
            // Position label left of y-axis or at left edge
                int16_t labelX;
                if (minX <= 0.0f && maxX >= 0.0f) {
                    int16_t x0 = mapX(0.0f);
                    labelX = x0 - (int)strlen(buf) * 6 * axisLabelSize - 2;
                    if (labelX < x) {
                        // If the calculated position is inside the content rect, push it into the left margin
                        labelX = contentX - (int)strlen(buf) * 6 * axisLabelSize - 2;
                        if (labelX < x) labelX = x + 1;
                    }
                } else {
                    labelX = contentX - (int)strlen(buf) * 6 * axisLabelSize - 2;
                    if (labelX < x) labelX = x + 1;
                }
            if (effectiveTinyY) {
                    int16_t labelH = 5 * tinyAxisLabelScale;
                    int16_t labelYAdj = tickY - labelH / 2;
                if (prevLabelY < -1000 || abs(labelYAdj - prevLabelY) >= (labelH + 2)) {
                    drawTinyText(screen, labelX, labelYAdj, buf, tinyAxisLabelScale);
                    prevLabelY = labelYAdj;
                }
            } else {
                int16_t labelH = 8 * axisLabelSize;
                int16_t labelYAdj = tickY - labelH / 2;
                if (prevLabelY < -1000 || abs(labelYAdj - prevLabelY) >= (labelH + 2)) {
                    screen->setCursor(labelX, labelYAdj);
                    screen->print(buf);
                    prevLabelY = labelYAdj;
                }
            }
            }
        } else {
            for (int16_t i = gridSpacing; i < contentH; i += gridSpacing) {
                int16_t tickY = contentY + i;
                float normalized = 1.0f - ((float)(i) / (float)(contentH - 1));
                float dataValue = minY + normalized * (maxY - minY);
                char buf[12];
                if (fabs(dataValue - (int)dataValue) < 0.001f) {
                    snprintf(buf, sizeof(buf), "%d", (int)dataValue);
                } else {
                    snprintf(buf, sizeof(buf), "%.1f", dataValue);
                }
                int16_t labelX;
                if (minX <= 0.0f && maxX >= 0.0f) {
                    int16_t x0 = mapX(0.0f);
                    labelX = x0 - (int)strlen(buf) * 6 * axisLabelSize - 2;
                    if (labelX < x) {
                        labelX = contentX - (int)strlen(buf) * 6 * axisLabelSize - 2;
                        if (labelX < x) labelX = x + 1;
                    }
                } else {
                    labelX = contentX - (int)strlen(buf) * 6 * axisLabelSize - 2;
                    if (labelX < x) labelX = x + 1;
                }
                if (effectiveTinyY) {
                    int16_t labelH = 5 * tinyAxisLabelScale;
                    int16_t labelYAdj = tickY - labelH / 2;
                    if (prevLabelY < -1000 || abs(labelYAdj - prevLabelY) >= (labelH + 2)) {
                        drawTinyText(screen, labelX, labelYAdj, buf, tinyAxisLabelScale);
                        prevLabelY = labelYAdj;
                    }
                } else {
                    int16_t labelH = 8 * axisLabelSize;
                    int16_t labelYAdj = tickY - labelH / 2;
                    if (prevLabelY < -1000 || abs(labelYAdj - prevLabelY) >= (labelH + 2)) {
                        screen->setCursor(labelX, labelYAdj);
                        screen->print(buf);
                        prevLabelY = labelYAdj;
                    }
                }
            }
        }
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
                if (abs(screenX - prevScreenX) < contentW && 
                    abs(screenY - prevScreenY) < contentH) {
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

void DataPlot::setShowAxisLabels(bool show) {
    this->showAxisLabels = show;
}

bool DataPlot::getShowAxisLabels() const {
    return showAxisLabels;
}

void DataPlot::setAxisLabelSize(uint8_t size) {
    if (size < 1) size = 1;
    if (size > 4) size = 4;
    this->axisLabelSize = size;
}

uint8_t DataPlot::getAxisLabelSize() const {
    return axisLabelSize;
}

void DataPlot::setAutoTinyAxisLabels(bool autoEnable) {
    this->autoTinyAxisLabels = autoEnable;
}

bool DataPlot::getAutoTinyAxisLabels() const {
    return autoTinyAxisLabels;
}

void DataPlot::setTinyLabelAutoThreshold(uint8_t threshold) {
    this->tinyLabelAutoThreshold = threshold;
}

uint8_t DataPlot::getTinyLabelAutoThreshold() const {
    return tinyLabelAutoThreshold;
}

void DataPlot::setMaxTicks(uint8_t max) {
    this->maxTicks = max;
}

uint8_t DataPlot::getMaxTicks() const {
    return maxTicks;
}

void DataPlot::setUseTinyAxisLabels(bool use) {
    this->useTinyAxisLabels = use;
}

bool DataPlot::getUseTinyAxisLabels() const {
    return useTinyAxisLabels;
}

void DataPlot::setTinyAxisLabelScale(uint8_t scale) {
    if (scale < 1) scale = 1;
    this->tinyAxisLabelScale = scale;
}

uint8_t DataPlot::getTinyAxisLabelScale() const {
    return tinyAxisLabelScale;
}

// Helper methods
int16_t DataPlot::mapX(float fx) const {
    // Map data x to screen x inside content rect to leave room for labels
    int16_t contentX, contentY, contentW, contentH;
    computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    float normalized = (fx - minX) / (maxX - minX);
    return contentX + (int16_t)(normalized * (contentW - 1));
}

int16_t DataPlot::mapY(float fy) const {
    // Map data y to screen y (inverted because screen y increases downward)
    int16_t contentX, contentY, contentW, contentH;
    computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    float normalized = (fy - minY) / (maxY - minY);
    return contentY + contentH - 1 - (int16_t)(normalized * (contentH - 1));
}

void DataPlot::drawAxes(LedScreen128_64* screen) {
    int16_t contentX, contentY, contentW, contentH;
    computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    // Draw x-axis if y=0 is in range
    if (minY <= 0.0f && maxY >= 0.0f) {
        int16_t y0 = mapY(0.0f);
        screen->drawFastHLine(contentX, y0, contentW, true);
    }
    
    // Draw y-axis if x=0 is in range
    if (minX <= 0.0f && maxX >= 0.0f) {
        int16_t x0 = mapX(0.0f);
        screen->drawFastVLine(x0, contentY, contentH, true);
    }
}

void DataPlot::drawGrid(LedScreen128_64* screen) {
    int16_t contentX, contentY, contentW, contentH;
    computeContentRect(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    // Draw vertical grid lines
    for (int16_t i = gridSpacing; i < contentW; i += gridSpacing) {
        int16_t gridX = contentX + i;
        for (int16_t j = 0; j < contentH; j += 2) {
            screen->drawPixel(gridX, contentY + j, true);
        }
    }
    
    // Draw horizontal grid lines
    for (int16_t i = gridSpacing; i < contentH; i += gridSpacing) {
        int16_t gridY = contentY + i;
        for (int16_t j = 0; j < contentW; j += 2) {
            screen->drawPixel(contentX + j, gridY, true);
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
