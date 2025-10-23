#!/bin/bash

echo "Setting up IoT Agricultural Management System Development Environment..."

# Update package lists
sudo apt-get update

# Install C/C++ build tools
echo "Installing C/C++ build tools..."
sudo apt-get install -y \
    build-essential \
    gcc \
    g++ \
    gdb \
    cmake \
    make \
    ninja-build \
    clang \
    clang-format \
    clang-tidy

# Install ARM toolchain for embedded development
echo "Installing ARM toolchain..."
sudo apt-get install -y \
    gcc-arm-none-eabi \
    gdb-multiarch \
    binutils-arm-none-eabi \
    libnewlib-arm-none-eabi

# Install Python development tools
echo "Installing Python tools..."
pip install --upgrade pip setuptools wheel
pip install \
    pylint \
    black \
    autopep8 \
    flake8 \
    mypy \
    pytest \
    pytest-cov

# Install IoT and microcontroller Python libraries
echo "Installing IoT Python libraries..."
pip install \
    platformio \
    esptool \
    adafruit-ampy \
    pyserial \
    paho-mqtt \
    flask \
    fastapi \
    uvicorn \
    requests \
    numpy \
    pandas \
    matplotlib \
    scipy

# Install additional tools for embedded development
echo "Installing additional embedded development tools..."
sudo apt-get install -y \
    minicom \
    screen \
    picocom \
    cu \
    usbutils \
    stlink-tools \
    openocd

# Install AVR tools (for Arduino)
echo "Installing AVR tools..."
sudo apt-get install -y \
    avr-libc \
    avrdude \
    gcc-avr

# Set up USB permissions for microcontroller access
echo "Setting up USB permissions..."
sudo usermod -a -G dialout vscode
sudo usermod -a -G plugdev vscode

# Create udev rules for common development boards
sudo tee /etc/udev/rules.d/99-platformio-udev.rules > /dev/null << 'EOF'
# FTDI FT232 USB-Serial converter
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6015", MODE:="0666"

# CP210x USB to UART Bridge Controller
SUBSYSTEMS=="usb", ATTRS{idVendor}=="10c4", ATTRS{idProduct}=="ea60", MODE:="0666"

# CH340 USB-Serial converter
SUBSYSTEMS=="usb", ATTRS{idVendor}=="1a86", ATTRS{idProduct}=="7523", MODE:="0666"

# ESP32/ESP8266 boards
SUBSYSTEMS=="usb", ATTRS{idVendor}=="303a", ATTRS{idProduct}=="*", MODE:="0666"

# Arduino boards
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2341", ATTRS{idProduct}=="*", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2a03", ATTRS{idProduct}=="*", MODE:="0666"

# STM32 ST-Link
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", MODE:="0666"
SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="374b", MODE:="0666"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger

# Initialize PlatformIO core
echo "Initializing PlatformIO..."
pio platform install atmelavr espressif32 espressif8266 ststm32

# Create project directories if they don't exist
mkdir -p Software/firmware
mkdir -p Software/backend
mkdir -p Software/scripts
mkdir -p Hardware/schematics
mkdir -p Hardware/pcb

# Create a README for the devcontainer
cat > .devcontainer/README.md << 'EOF'
# IoT Agricultural Management System - Development Container

This devcontainer provides a complete development environment for the IoT Agricultural Management System project.

## Included Tools

### Languages & Compilers
- Python 3.11 with pip
- GCC/G++ (C/C++ compilers)
- ARM GCC toolchain (for ARM Cortex-M microcontrollers)
- AVR GCC toolchain (for Arduino/AVR microcontrollers)

### Microcontroller Development
- PlatformIO Core & IDE
- ESP-IDF tools (for ESP32/ESP8266)
- AVRDude (for Arduino programming)
- OpenOCD (for debugging)
- ST-Link tools (for STM32)

### Python Libraries
- PlatformIO CLI
- PySerial (serial communication)
- Paho-MQTT (MQTT protocol)
- Flask/FastAPI (web frameworks)
- NumPy, Pandas, Matplotlib (data analysis)
- esptool (ESP32/ESP8266 flashing)

### Development Tools
- Git with Git Graph and GitLens
- CMake and Ninja build systems
- Debugging tools (GDB, GDB-multiarch)
- Serial monitors (minicom, screen, picocom)

## USB Device Access

The container is configured with privileged access to USB devices for programming microcontrollers. Common devices are mapped:
- `/dev/ttyUSB*` - USB-to-serial adapters
- `/dev/ttyACM*` - Arduino and similar boards

## Forwarded Ports

- 5000 - Flask development server
- 8080 - General web server
- 1883 - MQTT broker

## Quick Start

1. Open the project in VS Code
2. When prompted, reopen in container
3. Wait for the setup to complete
4. Start developing!

### Testing Microcontroller Connection

```bash
# List USB devices
lsusb

# List serial ports
ls -l /dev/tty*

# Test serial connection
screen /dev/ttyUSB0 115200
```

### PlatformIO Quick Start

```bash
# Initialize a new PlatformIO project
cd Software/firmware
pio project init --board esp32dev

# Build the project
pio run

# Upload to board
pio run --target upload

# Open serial monitor
pio device monitor
```

## Troubleshooting

### USB devices not accessible
If you can't access USB devices, ensure:
1. The container has privileged access
2. Your user is in the dialout and plugdev groups
3. Restart the container after making changes

### PlatformIO not finding boards
Try updating the platform definitions:
```bash
pio platform update
```
EOF

echo ""
echo "=========================================="
echo "Setup complete! 🎉"
echo "=========================================="
echo ""
echo "Your development environment includes:"
echo "  ✓ Python 3.11 with IoT libraries"
echo "  ✓ C/C++ build tools (GCC/G++)"
echo "  ✓ ARM & AVR toolchains for microcontrollers"
echo "  ✓ PlatformIO for embedded development"
echo "  ✓ Serial communication tools"
echo "  ✓ MQTT, Flask, and data analysis libraries"
echo ""
echo "Note: You may need to restart the container for USB"
echo "      device permissions to take full effect."
echo ""
echo "See .devcontainer/README.md for more information."
echo "=========================================="
