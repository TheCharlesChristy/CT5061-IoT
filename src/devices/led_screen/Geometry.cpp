#include "Geometry.hpp"

// Constructor
Geometry::Geometry(int16_t x, int16_t y, int16_t width, int16_t height, GeometryShape shape)
    : GraphicsAsset(x, y, width, height, AssetType::GEOMETRY), shape(shape), filled(false),
      x1(0), y1(0), x2(0), y2(0), radius(0) {
}

// Destructor
Geometry::~Geometry() {
}

// Draw method implementation
void Geometry::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr) {
        return;
    }
    
    switch (shape) {
        case GeometryShape::RECTANGLE:
            if (filled) {
                screen->fillRect(x, y, width, height, true);
            } else {
                screen->drawRect(x, y, width, height, true);
            }
            break;
            
        case GeometryShape::ROUNDED_RECTANGLE:
            if (filled) {
                screen->fillRoundRect(x, y, width, height, radius, true);
            } else {
                screen->drawRoundRect(x, y, width, height, radius, true);
            }
            break;
            
        case GeometryShape::CIRCLE:
            if (filled) {
                screen->fillCircle(x, y, radius, true);
            } else {
                screen->drawCircle(x, y, radius, true);
            }
            break;
            
        case GeometryShape::LINE:
            screen->drawLine(x, y, x1, y1, true);
            break;
            
        case GeometryShape::TRIANGLE:
            if (filled) {
                screen->fillTriangle(x, y, x1, y1, x2, y2, true);
            } else {
                screen->drawTriangle(x, y, x1, y1, x2, y2, true);
            }
            break;
    }
    
    // Draw border if enabled (for shapes other than line)
    if (border && shape != GeometryShape::LINE) {
        if (shape == GeometryShape::RECTANGLE || shape == GeometryShape::ROUNDED_RECTANGLE) {
            screen->drawRect(x - 1, y - 1, width + 2, height + 2, true);
        } else if (shape == GeometryShape::CIRCLE) {
            screen->drawCircle(x, y, radius + 1, true);
        }
    }
}

// Shape management
void Geometry::setShape(GeometryShape shape) {
    this->shape = shape;
}

GeometryShape Geometry::getShape() const {
    return shape;
}

void Geometry::setFilled(bool filled) {
    this->filled = filled;
}

bool Geometry::isFilled() const {
    return filled;
}

// Shape-specific setters
void Geometry::setAsRectangle(int16_t x, int16_t y, int16_t w, int16_t h, bool filled) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    this->filled = filled;
    this->shape = GeometryShape::RECTANGLE;
}

void Geometry::setAsRoundedRectangle(int16_t x, int16_t y, int16_t w, int16_t h, int16_t radius, bool filled) {
    this->x = x;
    this->y = y;
    this->width = w;
    this->height = h;
    this->radius = radius;
    this->filled = filled;
    this->shape = GeometryShape::ROUNDED_RECTANGLE;
}

void Geometry::setAsCircle(int16_t centerX, int16_t centerY, int16_t radius, bool filled) {
    this->x = centerX;
    this->y = centerY;
    this->radius = radius;
    this->width = radius * 2;
    this->height = radius * 2;
    this->filled = filled;
    this->shape = GeometryShape::CIRCLE;
}

void Geometry::setAsLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    this->x = x0;
    this->y = y0;
    this->x1 = x1;
    this->y1 = y1;
    this->width = abs(x1 - x0);
    this->height = abs(y1 - y0);
    this->filled = false;
    this->shape = GeometryShape::LINE;
}

void Geometry::setAsTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, bool filled) {
    this->x = x0;
    this->y = y0;
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    
    // Calculate bounding box
    int16_t minX = min(x0, min(x1, x2));
    int16_t maxX = max(x0, max(x1, x2));
    int16_t minY = min(y0, min(y1, y2));
    int16_t maxY = max(y0, max(y1, y2));
    
    this->width = maxX - minX;
    this->height = maxY - minY;
    this->filled = filled;
    this->shape = GeometryShape::TRIANGLE;
}

// Parameter getters
void Geometry::getLinePoints(int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1) const {
    x0 = this->x;
    y0 = this->y;
    x1 = this->x1;
    y1 = this->y1;
}

void Geometry::getTrianglePoints(int16_t& x0, int16_t& y0, int16_t& x1, int16_t& y1, int16_t& x2, int16_t& y2) const {
    x0 = this->x;
    y0 = this->y;
    x1 = this->x1;
    y1 = this->y1;
    x2 = this->x2;
    y2 = this->y2;
}

int16_t Geometry::getRadius() const {
    return radius;
}
