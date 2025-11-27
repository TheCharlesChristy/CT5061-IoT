#ifndef TABLE_HPP
#define TABLE_HPP

#include "GraphicsAsset.hpp"
#include "LedScreen128_64.hpp"
#include <Arduino.h>

class Table : public GraphicsAsset {
private:
    String* cells;          // Cell data stored as 1D array (row-major order)
    int rows;               // Number of rows
    int cols;               // Number of columns
    int* colWidths;         // Width of each column in pixels
    int rowHeight;          // Height of each row in pixels
    uint8_t textSize;       // Text size for cell content
    bool showHeaders;       // First row is header
    bool showGridLines;     // Show grid lines between cells
    bool autoFitColumns;    // Automatically fit column widths
    
    // Helper methods
    int getCellIndex(int row, int col) const;
    void calculateColumnWidths();
    void drawCell(LedScreen128_64* screen, int row, int col, int16_t cellX, int16_t cellY, int16_t cellW, int16_t cellH);
    
public:
    // Constructor
    Table(int16_t x = 0, int16_t y = 0, int16_t width = 80, int16_t height = 40,
          int rows = 3, int cols = 3);
    
    // Destructor
    virtual ~Table();
    
    // Draw method implementation
    void draw(LedScreen128_64* screen) override;
    
    // Cell content management
    void setCell(int row, int col, const char* text);
    void setCell(int row, int col, const String& text);
    void setCell(int row, int col, int value);
    void setCell(int row, int col, float value, int decimals = 2);
    String getCell(int row, int col) const;
    void clearCell(int row, int col);
    void clearAllCells();
    
    // Table structure
    int getRows() const;
    int getCols() const;
    bool resize(int rows, int cols);
    
    // Column width management
    void setColumnWidth(int col, int width);
    int getColumnWidth(int col) const;
    void setAllColumnWidths(int width);
    
    // Row height management
    void setRowHeight(int height);
    int getRowHeight() const;
    
    // Display options
    void setTextSize(uint8_t size);
    uint8_t getTextSize() const;
    
    void setShowHeaders(bool show);
    bool getShowHeaders() const;
    
    void setShowGridLines(bool show);
    bool getShowGridLines() const;
    
    void setAutoFitColumns(bool autoFit);
    bool getAutoFitColumns() const;
};

#endif // TABLE_HPP
