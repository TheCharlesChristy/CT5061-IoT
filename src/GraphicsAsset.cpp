#include "GraphicsAsset.hpp"

// Constructor
GraphicsAsset::GraphicsAsset(int16_t x, int16_t y, int16_t width, int16_t height, AssetType type)
    : x(x), y(y), width(width), height(height), visible(true), border(false), animate(false), zIndex(0), assetType(type) {
}

// Virtual destructor
GraphicsAsset::~GraphicsAsset() {
}

// Position getters
int16_t GraphicsAsset::getX() const {
    return x;
}

int16_t GraphicsAsset::getY() const {
    return y;
}

// Position setters
void GraphicsAsset::setX(int16_t x) {
    this->x = x;
}

void GraphicsAsset::setY(int16_t y) {
    this->y = y;
}

void GraphicsAsset::setPosition(int16_t x, int16_t y) {
    this->x = x;
    this->y = y;
}

// Size getters
int16_t GraphicsAsset::getWidth() const {
    return width;
}

int16_t GraphicsAsset::getHeight() const {
    return height;
}

// Size setters
void GraphicsAsset::setWidth(int16_t width) {
    this->width = width;
}

void GraphicsAsset::setHeight(int16_t height) {
    this->height = height;
}

void GraphicsAsset::setSize(int16_t width, int16_t height) {
    this->width = width;
    this->height = height;
}

// Visibility control
bool GraphicsAsset::isVisible() const {
    return visible;
}

void GraphicsAsset::setVisible(bool visible) {
    this->visible = visible;
}

void GraphicsAsset::show() {
    visible = true;
}

void GraphicsAsset::hide() {
    visible = false;
}

// Border control
bool GraphicsAsset::hasBorder() const {
    return border;
}

void GraphicsAsset::setBorder(bool border) {
    this->border = border;
}

// Animation control
bool GraphicsAsset::isAnimated() const {
    return animate;
}

void GraphicsAsset::setAnimate(bool animate) {
    this->animate = animate;
}

// Z-index control
int16_t GraphicsAsset::getZIndex() const {
    return zIndex;
}

void GraphicsAsset::setZIndex(int16_t zIndex) {
    this->zIndex = zIndex;
}

// Check if a point is inside the asset bounds
bool GraphicsAsset::contains(int16_t px, int16_t py) const {
    return (px >= x && px < x + width && py >= y && py < y + height);
}

// Type identification
AssetType GraphicsAsset::getAssetType() const {
    return assetType;
}
