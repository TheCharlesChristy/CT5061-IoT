#ifndef GRAPHICS_ASSET_HPP
#define GRAPHICS_ASSET_HPP

#include <Arduino.h>

// Forward declaration to avoid circular dependency
class LedScreen128_64;

// Asset type enumeration for type identification without RTTI
enum class AssetType {
    BASE,
    TEXTBOX,
    FUNCTIONPLOT,
    DATAPLOT,
    TABLE,
    GEOMETRY,
    BITMAP
};

class GraphicsAsset {
protected:
    int16_t x;          // X position
    int16_t y;          // Y position
    int16_t width;      // Width of the asset
    int16_t height;     // Height of the asset
    bool visible;       // Visibility flag
    bool border;        // Whether to draw border
    bool animate;       // Whether to use animated drawing
    int16_t zIndex;     // Z-index for layering (higher values drawn on top)
    AssetType assetType; // Type identifier
    
public:
    // Constructor
    GraphicsAsset(int16_t x = 0, int16_t y = 0, int16_t width = 0, int16_t height = 0, AssetType type = AssetType::BASE);
    
    // Virtual destructor for proper cleanup
    virtual ~GraphicsAsset();
    
    // Pure virtual draw method - must be implemented by derived classes
    virtual void draw(LedScreen128_64* screen) = 0;
    
    // Position getters and setters
    int16_t getX() const;
    int16_t getY() const;
    void setX(int16_t x);
    void setY(int16_t y);
    void setPosition(int16_t x, int16_t y);
    
    // Size getters and setters
    int16_t getWidth() const;
    int16_t getHeight() const;
    void setWidth(int16_t width);
    void setHeight(int16_t height);
    void setSize(int16_t width, int16_t height);
    
    // Visibility control
    bool isVisible() const;
    void setVisible(bool visible);
    void show();
    void hide();
    
    // Border control
    bool hasBorder() const;
    void setBorder(bool border);
    
    // Animation control
    bool isAnimated() const;
    void setAnimate(bool animate);
    
    // Z-index control
    int16_t getZIndex() const;
    void setZIndex(int16_t zIndex);
    
    // Check if a point is inside the asset bounds
    bool contains(int16_t px, int16_t py) const;
    
    // Type identification
    AssetType getAssetType() const;
};

#endif // GRAPHICS_ASSET_HPP
