#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

// Geometry shape types
enum class GeometryShape {
    RECTANGLE,
    CIRCLE,
    TRIANGLE,
    LINE,
    ROUNDED_RECTANGLE
};

class Geometry : public GraphicsAsset {
private:
    GeometryShape shape;
    bool filled;
    
    // Additional parameters for different shapes
    int16_t x1, y1;     // For line, triangle
    int16_t x2, y2;     // For triangle
    int16_t radius;     // For circle, rounded rect
    
public:
    // Constructor
    Geometry(int16_t x = 0, int16_t y = 0, int16_t width = 10, int16_t height = 10,
             GeometryShape shape = GeometryShape::RECTANGLE);
    
    // Destructor
    virtual ~Geometry();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Shape management
    void setShape(GeometryShape shape);
    GeometryShape getShape() const;
    
    void setFilled(bool filled);
    bool isFilled() const;
    
    // Shape-specific setters
    void setAsRectangle(int16_t x, int16_t y, int16_t w, int16_t h, bool filled = false);
    void setAsRoundedRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, bool filled = false);
    void setAsCircle(int16_t centerX, int16_t centerY, int16_t radius, bool filled = false);
    void setAsLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void setAsTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool filled = false);
    
    // Parameter getters
    void getLinePoints(int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1) const;
    void getTrianglePoints(int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1, int16_t& x2, int16_t& y2) const;
    int16_t getRadius() const;
};

#endif // GEOMETRY_HPP
