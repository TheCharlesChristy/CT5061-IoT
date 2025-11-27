#include "Table.hpp"

// Constructor
Table::Table(int16_t x, int16_t y, int16_t width, int16_t height, int rows, int cols)
    : GraphicsAsset(x, y, width, height, AssetType::TABLE), cells(nullptr), rows(rows), cols(cols),
      colWidths(nullptr), rowHeight(10), textSize(1), showHeaders(true),
      showGridLines(true), autoFitColumns(true) {
    
    // Allocate cell data
    if (rows > 0 && cols > 0) {
        cells = new String[rows * cols];
        colWidths = new int[cols];
        
        // Initialize column widths
        if (autoFitColumns) {
            calculateColumnWidths();
        } else {
            int defaultWidth = width / cols;
            for (int i = 0; i < cols; i++) {
                colWidths[i] = defaultWidth;
            }
        }
    }
}

// Destructor
Table::~Table() {
    if (cells != nullptr) {
        delete[] cells;
        cells = nullptr;
    }
    if (colWidths != nullptr) {
        delete[] colWidths;
        colWidths = nullptr;
    }
}

// Draw method implementation
void Table::draw(LedScreen128_64* screen) {
    if (!visible || screen == nullptr || rows == 0 || cols == 0) {
        return;
    }
    
    // Draw border if enabled
    if (border) {
        screen->drawRect(x, y, width, height, true);
    }
    
    // Recalculate column widths if auto-fit is enabled
    if (autoFitColumns) {
        calculateColumnWidths();
    }
    
    // Calculate starting positions
    int16_t currentY = y + 1;
    
    // Draw each row
    for (int row = 0; row < rows && currentY < y + height; row++) {
        int16_t currentX = x + 1;
        int16_t currentRowHeight = rowHeight;
        
        // Make header row slightly taller if enabled
        if (showHeaders && row == 0) {
            currentRowHeight = rowHeight + 2;
        }
        
        // Don't draw if row extends beyond bounds
        if (currentY + currentRowHeight > y + height) {
            break;
        }
        
        // Draw each column
        for (int col = 0; col < cols && currentX < x + width; col++) {
            int16_t cellWidth = colWidths[col];
            
            // Don't draw if cell extends beyond bounds
            if (currentX + cellWidth > x + width) {
                cellWidth = x + width - currentX;
            }
            
            // Draw the cell
            drawCell(screen, row, col, currentX, currentY, cellWidth, currentRowHeight);
            
            // Draw vertical grid line
            if (showGridLines && col < cols - 1) {
                screen->drawFastVLine(currentX + cellWidth, currentY, currentRowHeight, true);
            }
            
            currentX += cellWidth;
        }
        
        // Draw horizontal grid line
        if (showGridLines && row < rows - 1) {
            screen->drawFastHLine(x + 1, currentY + currentRowHeight, width - 2, true);
        }
        
        currentY += currentRowHeight;
    }
}

// Cell content management
void Table::setCell(int row, int col, const char* text) {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        cells[index] = text;
    }
}

void Table::setCell(int row, int col, const String& text) {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        cells[index] = text;
    }
}

void Table::setCell(int row, int col, int value) {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        cells[index] = String(value);
    }
}

void Table::setCell(int row, int col, float value, int decimals) {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        cells[index] = String(value, decimals);
    }
}

String Table::getCell(int row, int col) const {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        return cells[index];
    }
    return "";
}

void Table::clearCell(int row, int col) {
    int index = getCellIndex(row, col);
    if (index >= 0) {
        cells[index] = "";
    }
}

void Table::clearAllCells() {
    for (int i = 0; i < rows * cols; i++) {
        cells[i] = "";
    }
}

// Table structure
int Table::getRows() const {
    return rows;
}

int Table::getCols() const {
    return cols;
}

bool Table::resize(int newRows, int newCols) {
    if (newRows <= 0 || newCols <= 0) {
        return false;
    }
    
    // Allocate new arrays
    String* newCells = new String[newRows * newCols];
    int* newColWidths = new int[newCols];
    
    // Copy existing data
    for (int r = 0; r < newRows && r < rows; r++) {
        for (int c = 0; c < newCols && c < cols; c++) {
            newCells[r * newCols + c] = cells[r * cols + c];
        }
    }
    
    // Copy column widths or initialize new ones
    for (int c = 0; c < newCols; c++) {
        if (c < cols) {
            newColWidths[c] = colWidths[c];
        } else {
            newColWidths[c] = width / newCols;
        }
    }
    
    // Delete old arrays and assign new ones
    delete[] cells;
    delete[] colWidths;
    cells = newCells;
    colWidths = newColWidths;
    rows = newRows;
    cols = newCols;
    
    return true;
}

// Column width management
void Table::setColumnWidth(int col, int width) {
    if (col >= 0 && col < cols && width > 0) {
        colWidths[col] = width;
        autoFitColumns = false;
    }
}

int Table::getColumnWidth(int col) const {
    if (col >= 0 && col < cols) {
        return colWidths[col];
    }
    return 0;
}

void Table::setAllColumnWidths(int width) {
    if (width > 0) {
        for (int i = 0; i < cols; i++) {
            colWidths[i] = width;
        }
        autoFitColumns = false;
    }
}

// Row height management
void Table::setRowHeight(int height) {
    if (height > 0) {
        this->rowHeight = height;
    }
}

int Table::getRowHeight() const {
    return rowHeight;
}

// Display options
void Table::setTextSize(uint8_t size) {
    if (size >= 1 && size <= 4) {
        this->textSize = size;
    }
}

uint8_t Table::getTextSize() const {
    return textSize;
}

void Table::setShowHeaders(bool show) {
    this->showHeaders = show;
}

bool Table::getShowHeaders() const {
    return showHeaders;
}

void Table::setShowGridLines(bool show) {
    this->showGridLines = show;
}

bool Table::getShowGridLines() const {
    return showGridLines;
}

void Table::setAutoFitColumns(bool autoFit) {
    this->autoFitColumns = autoFit;
    if (autoFit) {
        calculateColumnWidths();
    }
}

bool Table::getAutoFitColumns() const {
    return autoFitColumns;
}

// Helper methods
int Table::getCellIndex(int row, int col) const {
    if (row >= 0 && row < rows && col >= 0 && col < cols) {
        return row * cols + col;
    }
    return -1;
}

void Table::calculateColumnWidths() {
    if (cols == 0) return;
    
    // Simple equal distribution for now
    int availableWidth = width - 2; // Account for borders
    int equalWidth = availableWidth / cols;
    
    for (int i = 0; i < cols; i++) {
        colWidths[i] = equalWidth;
    }
    
    // Give remaining pixels to last column
    int remainder = availableWidth - (equalWidth * cols);
    if (remainder > 0 && cols > 0) {
        colWidths[cols - 1] += remainder;
    }
}

void Table::drawCell(LedScreen128_64* screen, int row, int col, int16_t cellX, int16_t cellY, int16_t cellW, int16_t cellH) {
    int index = getCellIndex(row, col);
    if (index < 0) return;
    
    String cellText = cells[index];
    
    // Set text properties
    screen->setTextSize(textSize);
    screen->setTextColor(true, false);
    
    // Calculate text position (centered vertically in cell)
    int16_t charHeight = 8 * textSize;
    int16_t charWidth = 6 * textSize;
    int16_t textY = cellY + (cellH - charHeight) / 2;
    int16_t textX = cellX + 2; // Small padding from left
    
    // Calculate maximum characters that fit in cell
    int maxChars = (cellW - 4) / charWidth;
    
    // Truncate text if necessary
    String displayText = cellText;
    if (displayText.length() > maxChars && maxChars > 0) {
        displayText = displayText.substring(0, maxChars);
    }
    
    // Draw text
    screen->setCursor(textX, textY);
    screen->print(displayText.c_str());
    
    // Highlight header cells if enabled
    if (showHeaders && row == 0) {
        // Draw underline for header
        screen->drawFastHLine(cellX, cellY + cellH - 1, cellW, true);
    }
}
