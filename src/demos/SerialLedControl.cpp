#include "SerialLedControl.hpp"

// Constructor
SerialLedControl::SerialLedControl(LedScreen128_64* ledScreen, Stream* serialPort)
    : screen(ledScreen), serial(serialPort), echo_commands(true), assetCount(0) {
    input_buffer.reserve(128);
    
    // Initialize assets array
    for (int i = 0; i < MAX_GRAPHICS_ASSETS; i++) {
        assets[i] = nullptr;
    }
}

// Initialize the serial control
void SerialLedControl::begin(unsigned long baudRate) {
    // ESP32S3 USB CDC (Serial) auto-initializes, so we just wait a moment
    delay(100);
    
    serial->println("\n=================================");
    serial->println("  Serial LED Screen Controller");
    serial->println("=================================");
    serial->println("Type 'help' for available commands");
    serial->println();
    printPrompt();
}

// Main run method - call this in loop()
void SerialLedControl::run() {
    while (serial->available() > 0) {
        char c = serial->read();
        
        if (c == '\n' || c == '\r') {
            if (input_buffer.length() > 0) {
                if (echo_commands) {
                    serial->println();
                }
                processCommand(input_buffer);
                input_buffer = "";
                printPrompt();
            }
        } else if (c == '\b' || c == 127) {
            // Backspace
            if (input_buffer.length() > 0) {
                input_buffer.remove(input_buffer.length() - 1);
                if (echo_commands) {
                    serial->print("\b \b");
                }
            }
        } else if (c >= 32 && c < 127) {
            // Printable character
            input_buffer += c;
            if (echo_commands) {
                serial->print(c);
            }
        }
    }
}

// Enable/disable command echo
void SerialLedControl::setEcho(bool enable) {
    echo_commands = enable;
}

// Get screen reference
LedScreen128_64* SerialLedControl::getScreen() {
    return screen;
}

// Command parsing
void SerialLedControl::processCommand(String command) {
    command.trim();
    
    if (command.length() == 0) {
        return;
    }
    
    // Split command and arguments
    int space_index = command.indexOf(' ');
    String cmd, args;
    
    if (space_index == -1) {
        cmd = command;
        args = "";
    } else {
        cmd = command.substring(0, space_index);
        args = command.substring(space_index + 1);
        args.trim();
    }
    
    cmd.toLowerCase();
    parseAndExecute(cmd, args);
}

void SerialLedControl::parseAndExecute(String cmd, String args) {
    if (cmd == "help" || cmd == "?") {
        handleHelp();
    } else if (cmd == "clear" || cmd == "cls") {
        handleClear();
    } else if (cmd == "display" || cmd == "show" || cmd == "update") {
        handleDisplay();
    } else if (cmd == "text" || cmd == "print") {
        handleText(args);
    } else if (cmd == "textsize" || cmd == "size") {
        handleTextSize(args);
    } else if (cmd == "cursor" || cmd == "pos") {
        handleCursor(args);
    } else if (cmd == "pixel" || cmd == "px") {
        handlePixel(args);
    } else if (cmd == "line" || cmd == "ln") {
        handleLine(args);
    } else if (cmd == "rect" || cmd == "rectangle") {
        handleRect(args);
    } else if (cmd == "fillrect" || cmd == "frect") {
        handleFillRect(args);
    } else if (cmd == "circle" || cmd == "circ") {
        handleCircle(args);
    } else if (cmd == "fillcircle" || cmd == "fcirc") {
        handleFillCircle(args);
    } else if (cmd == "triangle" || cmd == "tri") {
        handleTriangle(args);
    } else if (cmd == "filltriangle" || cmd == "ftri") {
        handleFillTriangle(args);
    } else if (cmd == "progress" || cmd == "bar") {
        handleProgressBar(args);
    } else if (cmd == "invert") {
        handleInvert(args);
    } else if (cmd == "dim") {
        handleDim(args);
    } else if (cmd == "rotate" || cmd == "rotation") {
        handleRotation(args);
    } else if (cmd == "scroll") {
        handleScroll(args);
    } else if (cmd == "textbox" || cmd == "createtextbox") {
        handleCreateTextBox(args);
    } else if (cmd == "dataplot" || cmd == "createdataplot") {
        handleCreateDataPlot(args);
    } else if (cmd == "table" || cmd == "createtable") {
        handleCreateTable(args);
    } else if (cmd == "geometry" || cmd == "creategeometry" || cmd == "geom") {
        handleCreateGeometry(args);
    } else if (cmd == "bitmap" || cmd == "createbitmap" || cmd == "bmp") {
        handleCreateBitmap(args);
    } else if (cmd == "setcell") {
        handleSetCell(args);
    } else if (cmd == "addpoint") {
        handleAddPoint(args);
    } else if (cmd == "drawasset" || cmd == "draw") {
        handleDrawAsset(args);
    } else if (cmd == "drawallassets" || cmd == "drawall") {
        handleDrawAllAssets();
    } else if (cmd == "listassets" || cmd == "list") {
        handleListAssets();
    } else if (cmd == "deleteasset" || cmd == "delete") {
        handleDeleteAsset(args);
    } else if (cmd == "deleteall") {
        handleDeleteAllAssets();
    } else if (cmd == "setpos") {
        handleSetAssetPos(args);
    } else if (cmd == "setsize") {
        handleSetAssetSize(args);
    } else if (cmd == "setborder") {
        handleSetAssetBorder(args);
    } else if (cmd == "setvisible") {
        handleSetAssetVisible(args);
    } else if (cmd == "settext") {
        handleSetText(args);
    } else if (cmd == "setanimate") {
        handleSetAnimate(args);
    } else if (cmd == "setzindex" || cmd == "setz") {
        handleSetZIndex(args);
    } else if (cmd == "settextsize" || cmd == "setsize") {
        handleSetTextBoxSize(args);
    } else {
        printError("Unknown command. Type 'help' for available commands.");
    }
}

// Command handlers
void SerialLedControl::handleClear() {
    screen->clearDisplay();
    printOk();
}

void SerialLedControl::handleDisplay() {
    screen->displayBuffer();
    printOk();
}

void SerialLedControl::handleText(String args) {
    if (args.length() == 0) {
        printError("Usage: text <string>");
        return;
    }
    
    screen->print(args.c_str());
    printOk();
}

void SerialLedControl::handleTextSize(String args) {
    int size = parseIntArg(args);
    if (size < 1 || size > 4) {
        printError("Text size must be 1-4");
        return;
    }
    
    screen->setTextSize(size);
    printOk();
}

void SerialLedControl::handleCursor(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    
    if (x < 0 || x >= 128 || y < 0 || y >= 64) {
        printError("Cursor position out of bounds (0-127, 0-63)");
        return;
    }
    
    screen->setCursor(x, y);
    printOk();
}

void SerialLedControl::handlePixel(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    
    if (x < 0 || x >= 128 || y < 0 || y >= 64) {
        printError("Pixel position out of bounds");
        return;
    }
    
    screen->drawPixel(x, y, true);
    printOk();
}

void SerialLedControl::handleLine(String args) {
    int x0 = parseIntArg(args);
    int y0 = parseIntArg(args);
    int x1 = parseIntArg(args);
    int y1 = parseIntArg(args);
    
    screen->drawLine(x0, y0, x1, y1, true);
    printOk();
}

void SerialLedControl::handleRect(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    screen->drawRect(x, y, w, h, true);
    printOk();
}

void SerialLedControl::handleFillRect(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    screen->fillRect(x, y, w, h, true);
    printOk();
}

void SerialLedControl::handleCircle(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int r = parseIntArg(args);
    
    screen->drawCircle(x, y, r, true);
    printOk();
}

void SerialLedControl::handleFillCircle(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int r = parseIntArg(args);
    
    screen->fillCircle(x, y, r, true);
    printOk();
}

void SerialLedControl::handleTriangle(String args) {
    int x0 = parseIntArg(args);
    int y0 = parseIntArg(args);
    int x1 = parseIntArg(args);
    int y1 = parseIntArg(args);
    int x2 = parseIntArg(args);
    int y2 = parseIntArg(args);
    
    screen->drawTriangle(x0, y0, x1, y1, x2, y2, true);
    printOk();
}

void SerialLedControl::handleFillTriangle(String args) {
    int x0 = parseIntArg(args);
    int y0 = parseIntArg(args);
    int x1 = parseIntArg(args);
    int y1 = parseIntArg(args);
    int x2 = parseIntArg(args);
    int y2 = parseIntArg(args);
    
    screen->fillTriangle(x0, y0, x1, y1, x2, y2, true);
    printOk();
}

void SerialLedControl::handleProgressBar(String args) {
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    int percent = parseIntArg(args);
    
    if (percent < 0 || percent > 100) {
        printError("Percentage must be 0-100");
        return;
    }
    
    screen->drawProgressBar(x, y, w, h, percent);
    printOk();
}

void SerialLedControl::handleInvert(String args) {
    args.toLowerCase();
    bool invert = (args == "1" || args == "true" || args == "on" || args == "yes");
    screen->invertDisplay(invert);
    printOk();
}

void SerialLedControl::handleDim(String args) {
    args.toLowerCase();
    bool dim = (args == "1" || args == "true" || args == "on" || args == "yes");
    screen->dim(dim);
    printOk();
}

void SerialLedControl::handleRotation(String args) {
    int rotation = parseIntArg(args);
    if (rotation < 0 || rotation > 3) {
        printError("Rotation must be 0-3");
        return;
    }
    
    screen->setRotation(rotation);
    printOk();
}

void SerialLedControl::handleScroll(String args) {
    args.toLowerCase();
    
    if (args.startsWith("stop")) {
        screen->stopScroll();
        printOk();
    } else if (args.startsWith("right") || args.startsWith("left")) {
        String remaining = args;
        String direction = parseStringArg(remaining);
        int start = parseIntArg(remaining);
        int stop = parseIntArg(remaining);
        
        if (direction == "right") {
            screen->startScrollRight(start, stop);
        } else {
            screen->startScrollLeft(start, stop);
        }
        printOk();
    } else {
        printError("Usage: scroll [right|left] <start> <stop> OR scroll stop");
    }
}

void SerialLedControl::handleHelp() {
    serial->println("\n--- Available Commands ---");
    serial->println("Display Control:");
    serial->println("  clear              - Clear display buffer");
    serial->println("  display            - Update screen with buffer");
    serial->println("  invert [0|1]       - Invert display colors");
    serial->println("  dim [0|1]          - Dim display");
    serial->println("  rotate <0-3>       - Set screen rotation");
    serial->println();
    serial->println("Text Commands:");
    serial->println("  text <string>      - Print text at cursor");
    serial->println("  textsize <1-4>     - Set text size");
    serial->println("  cursor <x> <y>     - Set cursor position");
    serial->println();
    serial->println("Drawing Commands:");
    serial->println("  pixel <x> <y>                     - Draw pixel");
    serial->println("  line <x0> <y0> <x1> <y1>          - Draw line");
    serial->println("  rect <x> <y> <w> <h>              - Draw rectangle");
    serial->println("  fillrect <x> <y> <w> <h>          - Draw filled rectangle");
    serial->println("  circle <x> <y> <r>                - Draw circle");
    serial->println("  fillcircle <x> <y> <r>            - Draw filled circle");
    serial->println("  triangle <x0> <y0> <x1> <y1> <x2> <y2> - Draw triangle");
    serial->println("  filltriangle <x0> <y0> <x1> <y1> <x2> <y2> - Draw filled triangle");
    serial->println("  progress <x> <y> <w> <h> <%>     - Draw progress bar");
    serial->println();
    serial->println("Graphics Assets:");
    serial->println("  textbox <x> <y> <w> <h> <text>   - Create text box");
    serial->println("  dataplot <x> <y> <w> <h>          - Create data plot");
    serial->println("  table <x> <y> <w> <h> <rows> <cols> - Create table");
    serial->println("  geometry <x> <y> <w> <h> <shape> [filled] - Create geometry");
    serial->println("    shapes: rect, circle, line, triangle, rrect");
    serial->println("  bitmap <x> <y> <w> <h>            - Create bitmap");
    serial->println("  drawasset <id>                    - Draw asset by ID");
    serial->println("  drawallassets                     - Draw all assets (z-order)");
    serial->println("  listassets                        - List all assets");
    serial->println("  deleteasset <id>                  - Delete asset by ID");
    serial->println("  deleteall                         - Delete all assets");
    serial->println("  setpos <id> <x> <y>               - Set asset position");
    serial->println("  setsize <id> <w> <h>              - Set asset size");
    serial->println("  setborder <id> <0|1>              - Set asset border");
    serial->println("  setvisible <id> <0|1>             - Set asset visibility");
    serial->println("  setzindex <id> <z>                - Set asset z-index");
    serial->println("  addpoint <id> <x> <y>             - Add point to DataPlot");
    serial->println("  setcell <id> <row> <col> <text>  - Set table cell");
    serial->println("  settext <id> <text>               - Set TextBox text");
    serial->println("  setanimate <id> <0|1>             - Set asset animation");
    serial->println("  settextsize <id> <1-4>            - Set TextBox text size");
    serial->println("    (1=6x8px, 2=12x16px, 3=18x24px, 4=24x32px)");
    serial->println();
    serial->println("Scroll Commands:");
    serial->println("  scroll right <start> <stop>       - Scroll right");
    serial->println("  scroll left <start> <stop>        - Scroll left");
    serial->println("  scroll stop                       - Stop scrolling");
    serial->println();
    serial->println("Other:");
    serial->println("  help               - Show this help");
    serial->println("\nNote: Most commands require 'display' to show changes");
    serial->println("Screen size: 128x64 pixels (x: 0-127, y: 0-63)");
    serial->println();
}

// Utility methods
void SerialLedControl::printPrompt() {
    serial->print("> ");
}

void SerialLedControl::printOk() {
    serial->println("OK");
}

void SerialLedControl::printError(const char* message) {
    serial->print("ERROR: ");
    serial->println(message);
}

int SerialLedControl::parseIntArg(String& args) {
    args.trim();
    int space_index = args.indexOf(' ');
    String value_str;
    
    if (space_index == -1) {
        value_str = args;
        args = "";
    } else {
        value_str = args.substring(0, space_index);
        args = args.substring(space_index + 1);
    }
    
    return value_str.toInt();
}

float SerialLedControl::parseFloatArg(String& args) {
    args.trim();
    int space_index = args.indexOf(' ');
    String value_str;
    
    if (space_index == -1) {
        value_str = args;
        args = "";
    } else {
        value_str = args.substring(0, space_index);
        args = args.substring(space_index + 1);
    }
    
    return value_str.toFloat();
}

String SerialLedControl::parseStringArg(String& args) {
    args.trim();
    int space_index = args.indexOf(' ');
    String value_str;
    
    if (space_index == -1) {
        value_str = args;
        args = "";
    } else {
        value_str = args.substring(0, space_index);
        args = args.substring(space_index + 1);
    }
    
    return value_str;
}

// Graphics asset command handlers
void SerialLedControl::handleCreateTextBox(String args) {
    if (assetCount >= MAX_GRAPHICS_ASSETS) {
        printError("Maximum number of assets reached");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    // Remaining args is the text
    args.trim();
    
    TextBox* textBox = new TextBox(x, y, w, h, args.c_str());
    textBox->setBorder(true);
    assets[assetCount] = textBox;
    
    serial->print("Created TextBox with ID: ");
    serial->println(assetCount);
    assetCount++;
}

void SerialLedControl::handleCreateDataPlot(String args) {
    if (assetCount >= MAX_GRAPHICS_ASSETS) {
        printError("Maximum number of assets reached");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    DataPlot* dataPlot = new DataPlot(x, y, w, h, 50);
    dataPlot->setBorder(true);
    assets[assetCount] = dataPlot;
    
    serial->print("Created DataPlot with ID: ");
    serial->println(assetCount);
    assetCount++;
}

void SerialLedControl::handleCreateTable(String args) {
    if (assetCount >= MAX_GRAPHICS_ASSETS) {
        printError("Maximum number of assets reached");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    int rows = parseIntArg(args);
    int cols = parseIntArg(args);
    
    if (rows <= 0 || cols <= 0) {
        printError("Rows and columns must be positive");
        return;
    }
    
    Table* table = new Table(x, y, w, h, rows, cols);
    table->setBorder(true);
    assets[assetCount] = table;
    
    serial->print("Created Table with ID: ");
    serial->println(assetCount);
    assetCount++;
}

void SerialLedControl::handleSetCell(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    // Check if it's a Table
    if (assets[id]->getAssetType() != AssetType::TABLE) {
        printError("Asset is not a Table");
        return;
    }
    
    Table* table = static_cast<Table*>(assets[id]);
    
    int row = parseIntArg(args);
    int col = parseIntArg(args);
    
    // Remaining args is the cell content
    args.trim();
    
    table->setCell(row, col, args.c_str());
    printOk();
}

void SerialLedControl::handleAddPoint(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    // Check if it's a DataPlot
    if (assets[id]->getAssetType() != AssetType::DATAPLOT) {
        printError("Asset is not a DataPlot");
        return;
    }
    
    DataPlot* dataPlot = static_cast<DataPlot*>(assets[id]);
    
    float x = parseFloatArg(args);
    float y = parseFloatArg(args);
    
    dataPlot->addPoint(x, y);
    printOk();
}

void SerialLedControl::handleDrawAsset(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    assets[id]->draw(screen);
    printOk();
}

void SerialLedControl::handleListAssets() {
    serial->println("\n--- Graphics Assets ---");
    
    if (assetCount == 0) {
        serial->println("No assets created");
    } else {
        for (int i = 0; i < assetCount; i++) {
            if (assets[i] != nullptr) {
                serial->print("ID ");
                serial->print(i);
                serial->print(": ");
                
                // Determine type
                switch (assets[i]->getAssetType()) {
                    case AssetType::TEXTBOX:
                        serial->print("TextBox");
                        break;
                    case AssetType::DATAPLOT:
                        serial->print("DataPlot");
                        break;
                    case AssetType::TABLE:
                        serial->print("Table");
                        break;
                    case AssetType::FUNCTIONPLOT:
                        serial->print("FunctionPlot");
                        break;
                    case AssetType::GEOMETRY:
                        serial->print("Geometry");
                        break;
                    case AssetType::BITMAP:
                        serial->print("Bitmap");
                        break;
                    default:
                        serial->print("GraphicsAsset");
                        break;
                }
                
                serial->print(" at (");
                serial->print(assets[i]->getX());
                serial->print(",");
                serial->print(assets[i]->getY());
                serial->print(") size ");
                serial->print(assets[i]->getWidth());
                serial->print("x");
                serial->print(assets[i]->getHeight());
                serial->print(" z=");
                serial->print(assets[i]->getZIndex());
                serial->print(" visible=");
                serial->println(assets[i]->isVisible() ? "yes" : "no");
            }
        }
    }
    serial->println();
}

void SerialLedControl::handleDeleteAsset(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    delete assets[id];
    assets[id] = nullptr;
    printOk();
}

void SerialLedControl::handleDeleteAllAssets() {
    for (int i = 0; i < assetCount; i++) {
        if (assets[i] != nullptr) {
            delete assets[i];
            assets[i] = nullptr;
        }
    }
    assetCount = 0;
    printOk();
}

void SerialLedControl::handleSetAssetPos(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    
    assets[id]->setPosition(x, y);
    printOk();
}

void SerialLedControl::handleSetAssetSize(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    assets[id]->setSize(w, h);
    printOk();
}

void SerialLedControl::handleSetAssetBorder(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int border = parseIntArg(args);
    assets[id]->setBorder(border != 0);
    printOk();
}

void SerialLedControl::handleSetAssetVisible(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int visible = parseIntArg(args);
    assets[id]->setVisible(visible != 0);
    printOk();
}

void SerialLedControl::handleSetText(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    // Check if it's a TextBox
    if (assets[id]->getAssetType() != AssetType::TEXTBOX) {
        printError("Asset is not a TextBox");
        return;
    }
    
    TextBox* textBox = static_cast<TextBox*>(assets[id]);
    
    // Remaining args is the new text
    args.trim();
    
    textBox->setText(args.c_str());
    printOk();
}

void SerialLedControl::handleSetAnimate(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int animate = parseIntArg(args);
    assets[id]->setAnimate(animate != 0);
    
    // Reset animation frame when enabling animation
    if (animate != 0) {
        AssetType type = assets[id]->getAssetType();
        if (type == AssetType::TEXTBOX) {
            static_cast<TextBox*>(assets[id])->resetAnimation();
        } else if (type == AssetType::FUNCTIONPLOT) {
            static_cast<FunctionPlot*>(assets[id])->resetAnimation();
        } else if (type == AssetType::DATAPLOT) {
            static_cast<DataPlot*>(assets[id])->resetAnimation();
        }
    }
    
    printOk();
}

void SerialLedControl::handleCreateGeometry(String args) {
    if (assetCount >= MAX_GRAPHICS_ASSETS) {
        printError("Maximum number of assets reached");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    String shapeStr = parseStringArg(args);
    shapeStr.toLowerCase();
    
    int filled = 0;
    if (args.length() > 0) {
        filled = parseIntArg(args);
    }
    
    Geometry* geom = new Geometry(x, y, w, h);
    
    if (shapeStr == "rect" || shapeStr == "rectangle") {
        geom->setAsRectangle(x, y, w, h, filled != 0);
    } else if (shapeStr == "circle" || shapeStr == "circ") {
        geom->setAsCircle(x, y, w, filled != 0); // w is radius
    } else if (shapeStr == "line") {
        geom->setAsLine(x, y, w, h); // w and h are x1, y1
    } else if (shapeStr == "rrect" || shapeStr == "roundrect") {
        int radius = 5; // Default radius
        geom->setAsRoundedRectangle(x, y, w, h, radius, filled != 0);
    } else {
        // Default to rectangle
        geom->setAsRectangle(x, y, w, h, filled != 0);
    }
    
    assets[assetCount] = geom;
    
    serial->print("Created Geometry with ID: ");
    serial->println(assetCount);
    assetCount++;
}

void SerialLedControl::handleCreateBitmap(String args) {
    if (assetCount >= MAX_GRAPHICS_ASSETS) {
        printError("Maximum number of assets reached");
        return;
    }
    
    int x = parseIntArg(args);
    int y = parseIntArg(args);
    int w = parseIntArg(args);
    int h = parseIntArg(args);
    
    Bitmap* bitmap = new Bitmap(x, y, w, h);
    
    // Create a default checkerboard pattern
    bitmap->createCheckerboard(4);
    bitmap->setBorder(true);
    
    assets[assetCount] = bitmap;
    
    serial->print("Created Bitmap with ID: ");
    serial->println(assetCount);
    assetCount++;
}

void SerialLedControl::handleDrawAllAssets() {
    screen->drawAssets();
    printOk();
}

void SerialLedControl::handleSetZIndex(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    int z = parseIntArg(args);
    assets[id]->setZIndex(z);
    printOk();
}

void SerialLedControl::handleSetTextBoxSize(String args) {
    int id = parseIntArg(args);
    
    if (id < 0 || id >= assetCount || assets[id] == nullptr) {
        printError("Invalid asset ID");
        return;
    }
    
    // Check if it's a TextBox
    if (assets[id]->getAssetType() != AssetType::TEXTBOX) {
        printError("Asset is not a TextBox");
        return;
    }
    
    int size = parseIntArg(args);
    if (size < 1 || size > 4) {
        printError("Text size must be 1-4 (1=6x8px, 2=12x16px, 3=18x24px, 4=24x32px)");
        return;
    }
    
    TextBox* textBox = static_cast<TextBox*>(assets[id]);
    textBox->setTextSize(size);
    printOk();
}

