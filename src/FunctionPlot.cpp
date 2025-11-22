#include "FunctionPlot.hpp"
#include <math.h>
#include <string.h>

// Compute inner content rectangle for plotting to leave room for axis labels
static void computeContentRectFP(int16_t x, int16_t y, int16_t width, int16_t height,
                               uint8_t axisLabelSize, bool showAxisLabels,
                               int16_t &contentX, int16_t &contentY, int16_t &contentW, int16_t &contentH) {
    int16_t leftPad = 2;
    int16_t rightPad = 2;
    int16_t topPad = 2;
    int16_t bottomPad = 2;
    if (showAxisLabels) {
        leftPad = (6 * axisLabelSize) + 4;
        bottomPad = (8 * axisLabelSize) + 4;
    }
    contentX = x + leftPad;
    contentY = y + topPad;
    contentW = width - leftPad - rightPad;
    contentH = height - topPad - bottomPad;
    if (contentW < 1) contentW = 1;
    if (contentH < 1) contentH = 1;
}

// Tiny 3x5 font bitmap same as DataPlot
static const uint8_t tinyFontFP[][5] = {
    {0b111,0b101,0b101,0b101,0b111},
    {0b010,0b110,0b010,0b010,0b111},
    {0b111,0b001,0b111,0b100,0b111},
    {0b111,0b001,0b111,0b001,0b111},
    {0b101,0b101,0b111,0b001,0b001},
    {0b111,0b100,0b111,0b001,0b111},
    {0b111,0b100,0b111,0b101,0b111},
    {0b111,0b001,0b001,0b001,0b001},
    {0b111,0b101,0b111,0b101,0b111},
    {0b111,0b101,0b111,0b001,0b111},
    {0b000,0b000,0b111,0b000,0b000},
    {0b000,0b000,0b000,0b000,0b010},
    {0b111,0b100,0b100,0b100,0b111},
    {0b111,0b010,0b010,0b010,0b010},
    {0b101,0b101,0b111,0b101,0b101},
    {0b101,0b001,0b100,0b010,0b101}
};

static void drawTinyCharFP(LedScreen128_64* screen, int16_t x, int16_t y, char c, uint8_t scale=1) {
    int index = -1;
    if (c >= '0' && c <= '9') index = c - '0';
    else if (c == '-') index = 10;
    else if (c == '.') index = 11;
    else if (c == 'C' || c == 'c') index = 12;
    else if (c == 'T' || c == 't') index = 13;
    else if (c == 'H' || c == 'h') index = 14;
    else if (c == '%') index = 15;
    if (index < 0) return;

    for (int row = 0; row < 5; row++) {
        uint8_t bits = tinyFontFP[index][row];
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

static void drawTinyTextFP(LedScreen128_64* screen, int16_t x, int16_t y, const char* text, uint8_t scale=1) {
    int16_t cursorX = x;
    while (*text) {
        char c = *text++;
        if (c == ' ') cursorX += (3*scale + scale);
        else {
            drawTinyCharFP(screen, cursorX, y, c, scale);
            cursorX += (3*scale + scale);
        }
    }
}

// Constructor
FunctionPlot::FunctionPlot(int16_t x, int16_t y, int16_t width, int16_t height, MathFunction func)
        : GraphicsAsset(x, y, width, height, AssetType::FUNCTIONPLOT), function(func), 
            minX(-10.0f), maxX(10.0f), minY(-10.0f), maxY(10.0f),
            autoScaleY(false), showAxes(true), showGrid(false), gridSpacing(10), showAxisLabels(false), axisLabelSize(1), useTinyAxisLabels(false), tinyAxisLabelScale(1), autoTinyAxisLabels(true), tinyLabelAutoThreshold(36), maxTicks(0), animationFrame(0) {
    // Initialize tiny font options
    useTinyAxisLabels = false;
    tinyAxisLabelScale = 1;
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

    // Draw axis labels if enabled
    if (showAxisLabels) {
        screen->setTextSize(axisLabelSize);
        // X axis labels
        int16_t contentX, contentY, contentW, contentH;
        computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
        int16_t prevLabelX = -9999;
        bool effectiveTiny = useTinyAxisLabels || (autoTinyAxisLabels && contentW <= tinyLabelAutoThreshold);
        if (maxTicks > 1) {
            float step = (contentW > 1) ? (contentW - 1) / (float)(maxTicks - 1) : 1.0f;
            for (int k = 0; k < maxTicks; k++) {
                int16_t i = (int16_t)round(k * step);
                int16_t tickX = contentX + i;
                float normalized = (contentW > 1) ? ((float)i) / (float)(contentW - 1) : 0.0f;
                float dataValue = minX + normalized * (maxX - minX);
                char buf[12];
                if (fabs(dataValue - (int)dataValue) < 0.001f) {
                    snprintf(buf, sizeof(buf), "%d", (int)dataValue);
                } else {
                    snprintf(buf, sizeof(buf), "%.1f", dataValue);
                }
                int16_t labelY = contentY + contentH + 1;
                if (minY <= 0.0f && maxY >= 0.0f) {
                    int16_t y0 = mapY(0.0f);
                    if (y0 + (8 * axisLabelSize) + 1 < (y + height)) {
                        labelY = y0 + 1;
                    } else {
                        labelY = y0 - (8 * axisLabelSize) - 1;
                    }
                }
                if (effectiveTiny) {
                    int16_t labelW = (int)strlen(buf) * (3 * tinyAxisLabelScale + tinyAxisLabelScale);
                    int16_t labelX = tickX - labelW / 2;
                    if (prevLabelX < -1000 || abs(labelX - prevLabelX) >= (labelW + 2)) {
                        drawTinyTextFP(screen, labelX, labelY, buf, tinyAxisLabelScale);
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
                            drawTinyTextFP(screen, labelX, labelY, buf, tinyAxisLabelScale);
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
                            drawTinyTextFP(screen, labelX, labelY, buf, tinyAxisLabelScale);
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

        // Y axis labels
    int16_t prevLabelY = -9999;
    bool effectiveTinyY = useTinyAxisLabels || (autoTinyAxisLabels && contentH <= tinyLabelAutoThreshold);
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
                    drawTinyTextFP(screen, labelX, labelYAdj, buf, tinyAxisLabelScale);
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
                    drawTinyTextFP(screen, labelX, labelYAdj, buf, tinyAxisLabelScale);
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
    
    // Plot the function
    int16_t prevScreenX = -1;
    int16_t prevScreenY = -1;
    bool prevValid = false;
    
    // Determine how many pixels to draw (for animation)
    int16_t contentX, contentY, contentW, contentH;
    computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    // Determine how many pixels to draw (for animation)
    int maxPixels = contentW;
    if (animate && animationFrame < contentW) {
        maxPixels = animationFrame;
        // Auto-advance animation on each draw
        animationFrame++;
    }
    
    // Sample the function across the width
    for (int16_t i = 0; i < maxPixels; i++) {
        // Calculate the x value in function space
        float fx = minX + (maxX - minX) * (float)i / (float)(contentW - 1);
        
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

void FunctionPlot::setShowAxisLabels(bool show) {
    this->showAxisLabels = show;
}

bool FunctionPlot::getShowAxisLabels() const {
    return showAxisLabels;
}

void FunctionPlot::setAxisLabelSize(uint8_t size) {
    if (size < 1) size = 1;
    if (size > 4) size = 4;
    this->axisLabelSize = size;
}

uint8_t FunctionPlot::getAxisLabelSize() const {
    return axisLabelSize;
}

void FunctionPlot::setAutoTinyAxisLabels(bool autoEnable) {
    this->autoTinyAxisLabels = autoEnable;
}

bool FunctionPlot::getAutoTinyAxisLabels() const {
    return autoTinyAxisLabels;
}

void FunctionPlot::setTinyLabelAutoThreshold(uint8_t threshold) {
    this->tinyLabelAutoThreshold = threshold;
}

uint8_t FunctionPlot::getTinyLabelAutoThreshold() const {
    return tinyLabelAutoThreshold;
}

void FunctionPlot::setMaxTicks(uint8_t max) {
    this->maxTicks = max;
}

uint8_t FunctionPlot::getMaxTicks() const {
    return maxTicks;
}

void FunctionPlot::setUseTinyAxisLabels(bool use) {
    this->useTinyAxisLabels = use;
}

bool FunctionPlot::getUseTinyAxisLabels() const {
    return useTinyAxisLabels;
}

void FunctionPlot::setTinyAxisLabelScale(uint8_t scale) {
    if (scale < 1) scale = 1;
    this->tinyAxisLabelScale = scale;
}

uint8_t FunctionPlot::getTinyAxisLabelScale() const {
    return tinyAxisLabelScale;
}

// Helper methods
int16_t FunctionPlot::mapX(float fx) const {
    // Map function x to screen x within content rect
    int16_t contentX, contentY, contentW, contentH;
    computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    float normalized = (fx - minX) / (maxX - minX);
    return contentX + (int16_t)(normalized * (contentW - 1));
}

int16_t FunctionPlot::mapY(float fy) const {
    // Map function y to screen y (inverted because screen y increases downward)
    int16_t contentX, contentY, contentW, contentH;
    computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
    float normalized = (fy - minY) / (maxY - minY);
    return contentY + contentH - 1 - (int16_t)(normalized * (contentH - 1));
}

void FunctionPlot::drawAxes(LedScreen128_64* screen) {
    int16_t contentX, contentY, contentW, contentH;
    computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
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

void FunctionPlot::drawGrid(LedScreen128_64* screen) {
    int16_t contentX, contentY, contentW, contentH;
    computeContentRectFP(x, y, width, height, axisLabelSize, showAxisLabels, contentX, contentY, contentW, contentH);
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
