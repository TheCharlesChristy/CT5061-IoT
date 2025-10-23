# Development Container Documentation

## Overview

This devcontainer provides a complete, reproducible development environment for the IoT Agricultural Management System project. It includes all necessary tools for Python development, C/C++ programming, and microcontroller firmware development.

## What's Included

### Core Development Tools

#### Programming Languages
- **Python 3.11** - For backend services, data processing, and scripts
- **GCC/G++ (C/C++)** - For native applications and libraries
- **ARM GCC Toolchain** - For ARM Cortex-M based microcontrollers (STM32, etc.)
- **AVR GCC Toolchain** - For Arduino and AVR-based microcontrollers

#### Microcontroller Platforms
- **PlatformIO** - Unified development platform supporting:
  - ESP32 (WiFi-enabled microcontroller)
  - ESP8266 (WiFi-enabled microcontroller)
  - Arduino (AVR-based boards)
  - STM32 (ARM Cortex-M series)
  - And many more...

#### Python Libraries for IoT
- **PySerial** - Serial communication with microcontrollers
- **Paho-MQTT** - MQTT protocol implementation for IoT messaging
- **Flask/FastAPI** - Web frameworks for APIs and dashboards
- **NumPy, Pandas, Matplotlib** - Data analysis and visualization
- **esptool** - ESP32/ESP8266 flashing and utilities

#### Development Tools
- **Git** with enhanced extensions (GitLens, Git Graph)
- **CMake** and **Ninja** - Build system generators
- **GDB** - GNU Debugger for C/C++ and ARM
- **OpenOCD** - On-chip debugging for embedded targets
- **Serial Monitors** - minicom, screen, picocom for serial debugging

### VS Code Extensions

The devcontainer automatically installs:
- Python language support with IntelliSense
- C/C++ language support and debugging
- PlatformIO IDE for microcontroller development
- Git visualization and history tools
- Hex editor for binary file inspection

## Getting Started

### First Time Setup

1. **Install Prerequisites** on your host machine:
   - [Docker Desktop](https://www.docker.com/products/docker-desktop)
   - [Visual Studio Code](https://code.visualstudio.com/)
   - [Remote - Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

2. **Open the Project**:
   - Open VS Code
   - File → Open Folder → Select the project directory
   - When prompted, click "Reopen in Container"
   - Wait for the container to build and setup to complete (first time takes 5-10 minutes)

3. **Verify Installation**:
   ```bash
   # Check Python
   python --version
   
   # Check C compiler
   gcc --version
   
   # Check ARM compiler
   arm-none-eabi-gcc --version
   
   # Check PlatformIO
   pio --version
   ```

### Working with Microcontrollers

#### Connecting USB Devices

On **Windows** host:
1. Install [usbipd-win](https://github.com/dorssel/usbipd-win)
2. List USB devices:
   ```powershell
   usbipd list
   ```
3. Attach your microcontroller to WSL:
   ```powershell
   usbipd bind --busid <BUSID>
   usbipd attach --wsl --busid <BUSID>
   ```

Inside the container, verify:
```bash
# List USB devices
lsusb

# List serial ports
ls -l /dev/tty*
```

#### Creating a New Microcontroller Project

```bash
# Navigate to firmware directory
cd Software/firmware

# Initialize a new PlatformIO project for ESP32
pio project init --board esp32dev

# For Arduino Uno
pio project init --board uno

# For STM32
pio project init --board nucleo_f401re
```

#### Building and Uploading Firmware

```bash
# Build the project
pio run

# Upload to connected board
pio run --target upload

# Build and upload in one command
pio run --target upload

# Open serial monitor (115200 baud default)
pio device monitor

# Or specify baud rate
pio device monitor --baud 9600
```

#### Common PlatformIO Commands

```bash
# List connected devices
pio device list

# Clean build files
pio run --target clean

# Update platform definitions
pio platform update

# Install a library
pio lib install "Adafruit Unified Sensor"

# Search for libraries
pio lib search "DHT sensor"
```

### Python Development

#### Setting Up a Virtual Environment (Optional)

```bash
cd Software/backend

# Create virtual environment
python -m venv venv

# Activate it
source venv/bin/activate

# Install requirements
pip install -r requirements.txt
```

#### Running Python Applications

```bash
# Run a Python script
python Software/scripts/sensor_logger.py

# Run Flask app
cd Software/backend
flask run --host=0.0.0.0 --port=5000

# Run FastAPI app
uvicorn main:app --reload --host=0.0.0.0 --port=8080
```

### C/C++ Development

#### Compiling Native C/C++ Code

```bash
# Compile a simple C program
gcc -o output_program source.c

# Compile C++ with debugging symbols
g++ -g -o output_program source.cpp

# Compile with optimization
gcc -O2 -o output_program source.c
```

#### Using CMake

```bash
# Create build directory
mkdir build && cd build

# Configure project
cmake ..

# Build
cmake --build .

# Or use make directly
make
```

## Project Structure

```
Project/
├── .devcontainer/
│   ├── devcontainer.json    # Container configuration
│   ├── setup.sh             # Post-create setup script
│   ├── Dockerfile           # Alternative Docker build (optional)
│   └── README.md            # This file
├── Software/
│   ├── firmware/            # Microcontroller firmware projects
│   ├── backend/             # Python backend services
│   └── scripts/             # Utility scripts
├── Hardware/
│   ├── schematics/          # Circuit schematics
│   └── pcb/                 # PCB designs
└── Report.md                # Project documentation
```

## Port Forwarding

The following ports are forwarded from the container:

- **5000** - Flask development server
- **8080** - General web server / FastAPI
- **1883** - MQTT broker (if running locally)

Access these from your host browser at `localhost:<port>`

## Common Issues and Solutions

### USB Device Not Found

**Problem**: Microcontroller not detected in container

**Solutions**:
1. On Windows, ensure usbipd is installed and device is attached to WSL
2. Verify device appears in `lsusb` output
3. Check permissions: `ls -l /dev/ttyUSB*`
4. Restart the container
5. Ensure your user is in dialout group: `groups vscode`

### Permission Denied on Serial Port

**Solution**:
```bash
# Add user to dialout group
sudo usermod -aG dialout $USER

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Restart container for changes to take effect
```

### PlatformIO Upload Failed

**Solutions**:
1. Ensure correct board is selected in platformio.ini
2. Check USB connection
3. Try manual reset of board during upload
4. Verify serial port: `pio device list`
5. Check for upload speed issues - try lower baud rate in platformio.ini:
   ```ini
   [env:myboard]
   upload_speed = 115200
   ```

### Python Package Installation Issues

**Solution**:
```bash
# Update pip
pip install --upgrade pip

# Install with verbose output
pip install -v package-name

# Use system dependencies if needed
sudo apt-get install python3-dev
```

## Advanced Configuration

### Customizing the Container

Edit `.devcontainer/devcontainer.json` to:
- Add VS Code extensions
- Change Python version
- Add custom post-create commands
- Modify port forwarding

After changes, rebuild the container:
1. Press `F1` or `Ctrl+Shift+P`
2. Select "Remote-Containers: Rebuild Container"

### Adding New Tools

Edit `.devcontainer/setup.sh` to install additional packages:
```bash
# Add to setup.sh
sudo apt-get install -y your-package-name
pip install your-python-package
```

### Using the Dockerfile Approach

If you need more control, uncomment the Dockerfile reference in devcontainer.json:
```json
{
  "build": {
    "dockerfile": "Dockerfile"
  }
}
```

## Best Practices

1. **Version Control**: The .devcontainer folder is committed to git, ensuring all team members use the same environment
2. **Dependencies**: Document all dependencies in requirements.txt (Python) or platformio.ini (firmware)
3. **Serial Debugging**: Use `pio device monitor` for real-time serial output
4. **Data Logging**: Store sensor data in Software/backend/data/ directory
5. **Testing**: Write tests for both Python code and firmware where applicable

## Useful Resources

- [PlatformIO Documentation](https://docs.platformio.org/)
- [ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/)
- [Python for IoT](https://realpython.com/python-raspberry-pi/)
- [MQTT Protocol](https://mqtt.org/)
- [VS Code Remote Containers](https://code.visualstudio.com/docs/remote/containers)

## Support

For issues specific to:
- **Devcontainer**: Check VS Code Remote Containers documentation
- **PlatformIO**: Visit PlatformIO forums
- **Hardware**: Refer to your microcontroller's datasheet
- **Project-specific**: See Report.md or contact the team

---

Happy coding! 🌱🤖
