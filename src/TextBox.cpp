#include "TextBox.hpp"

// Constructor
TextBox::TextBox(int16_t x, int16_t y, int16_t width, int16_t height, const char* text)
    : GraphicsAsset(x, y, width, height, AssetType::TEXTBOX), text(text), textSize(1), 
      alignment(TextAlign::LEFT), wordWrap(true), fillBackground(false), animationFrame(0) {
}

// Destructor
TextBox::~TextBox() {
}

// Draw method implementation
void TextBox::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr) {
        return;
    }
    
    // Draw border if enabled
    if (border) {
        screen->drawRect(x, y, width, height, true);
    }
    
    // Fill background if enabled
    if (fillBackground) {
        screen->fillRect(x + 1, y + 1, width - 2, height - 2, false);
    }
    
    // Calculate character dimensions (approximate)
    int16_t charWidth = 6 * textSize;
    int16_t charHeight = 8 * textSize;
    int16_t padding = 2;
    
    // Calculate text bounds
    int16_t textX = x + padding;
    int16_t textY = y + padding;
    int16_t maxWidth = width - (2 * padding);
    
    // Set text properties
    screen->setTextSize(textSize);
    screen->setTextColor(true, fillBackground);
    screen->setTextWrap(false); // We handle wrapping manually
    
    // Determine the text to display (for animation)
    String displayString = text;
    if (animate && animationFrame < text.length()) {
        displayString = text.substring(0, animationFrame);
        // Auto-advance animation on each draw
        animationFrame++;
    }
    
    // Handle text alignment and rendering
    if (wordWrap) {
        // Word wrap logic
        String remainingText = displayString;
        int16_t currentY = textY;
        
        while (remainingText.length() > 0 && currentY + charHeight <= y + height - padding) {
            int maxChars = maxWidth / charWidth;
            String line;
            
            if (remainingText.length() <= maxChars) {
                line = remainingText;
                remainingText = "";
            } else {
                // Find last space before maxChars
                int spacePos = remainingText.lastIndexOf(' ', maxChars);
                if (spacePos > 0 && spacePos < maxChars) {
                    line = remainingText.substring(0, spacePos);
                    remainingText = remainingText.substring(spacePos + 1);
                } else {
                    // No space found, force break
                    line = remainingText.substring(0, maxChars);
                    remainingText = remainingText.substring(maxChars);
                }
            }
            
            // Calculate X position based on alignment
            int16_t lineX = textX;
            if (alignment == TextAlign::CENTER) {
                int16_t lineWidth = line.length() * charWidth;
                lineX = x + (width - lineWidth) / 2;
            } else if (alignment == TextAlign::RIGHT) {
                int16_t lineWidth = line.length() * charWidth;
                lineX = x + width - lineWidth - padding;
            }
            
            // Draw the line
            screen->setCursor(lineX, currentY);
            screen->print(line.c_str());
            
            currentY += charHeight;
        }
    } else {
        // No word wrap - single line with truncation
        int maxChars = maxWidth / charWidth;
        String displayText = displayString;
        if (displayText.length() > maxChars) {
            displayText = displayText.substring(0, maxChars);
        }
        
        // Calculate X position based on alignment
        int16_t lineX = textX;
        if (alignment == TextAlign::CENTER) {
            int16_t lineWidth = displayText.length() * charWidth;
            lineX = x + (width - lineWidth) / 2;
        } else if (alignment == TextAlign::RIGHT) {
            int16_t lineWidth = displayText.length() * charWidth;
            lineX = x + width - lineWidth - padding;
        }
        
        screen->setCursor(lineX, textY);
        screen->print(displayText.c_str());
    }
}

// Text content
void TextBox::setText(const char* text) {
    this->text = text;
    animationFrame = 0;  // Reset animation when text changes
}

void TextBox::setText(const String& text) {
    this->text = text;
    animationFrame = 0;  // Reset animation when text changes
}

const String& TextBox::getText() const {
    return text;
}

// Text formatting
void TextBox::setTextSize(uint8_t size) {
    if (size >= 1 && size <= 4) {
        this->textSize = size;
    }
}

uint8_t TextBox::getTextSize() const {
    return textSize;
}

void TextBox::setAlignment(TextAlign align) {
    this->alignment = align;
}

TextAlign TextBox::getAlignment() const {
    return alignment;
}

void TextBox::setWordWrap(bool wrap) {
    this->wordWrap = wrap;
}

bool TextBox::getWordWrap() const {
    return wordWrap;
}

void TextBox::setFillBackground(bool fill) {
    this->fillBackground = fill;
}

bool TextBox::getFillBackground() const {
    return fillBackground;
}

// Animation control
void TextBox::resetAnimation() {
    animationFrame = 0;
}

void TextBox::advanceAnimation() {
    if (animationFrame < text.length()) {
        animationFrame++;
    }
}

int TextBox::getAnimationFrame() const {
    return animationFrame;
}

// Utility - calculate how many lines the text will take
int TextBox::calculateLines() const {
    if (!wordWrap || text.length() == 0) {
        return text.length() > 0 ? 1 : 0;
    }
    
    int16_t charWidth = 6 * textSize;
    int16_t padding = 2;
    int16_t maxWidth = width - (2 * padding);
    int maxChars = maxWidth / charWidth;
    
    if (maxChars <= 0) return 0;
    
    int lines = 0;
    String remainingText = text;
    
    while (remainingText.length() > 0) {
        if (remainingText.length() <= maxChars) {
            lines++;
            break;
        } else {
            int spacePos = remainingText.lastIndexOf(' ', maxChars);
            if (spacePos > 0 && spacePos < maxChars) {
                remainingText = remainingText.substring(spacePos + 1);
            } else {
                remainingText = remainingText.substring(maxChars);
            }
            lines++;
        }
    }
    
    return lines;
}
