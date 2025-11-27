#ifndef TEXT_BOX_HPP
#define TEXT_BOX_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

// Text alignment options
enum class TextAlign {
    LEFT,
    CENTER,
    RIGHT
};

class TextBox : public GraphicsAsset {
private:
    String text;
    uint8_t textSize;
    TextAlign alignment;
    bool wordWrap;
    bool fillBackground;
    int animationFrame;  // Current animation frame (character count for typewriter effect)
    
public:
    // Constructor
    TextBox(int16_t x = 0, int16_t y = 0, int16_t width = 60, int16_t height = 10,
            const char* text = "");
    
    // Destructor
    virtual ~TextBox();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Text content
    void setText(const char* text);
    void setText(const String& text);
    const String& getText() const;
    
    // Text formatting
    void setTextSize(uint8_t size);
    uint8_t getTextSize() const;
    
    void setAlignment(TextAlign align);
    TextAlign getAlignment() const;
    
    void setWordWrap(bool wrap);
    bool getWordWrap() const;
    
    void setFillBackground(bool fill);
    bool getFillBackground() const;
    
    // Animation control
    void resetAnimation();
    void advanceAnimation();
    int getAnimationFrame() const;
    
    // Utility - calculate how many lines the text will take
    int calculateLines() const;
};

#endif // TEXT_BOX_HPP
