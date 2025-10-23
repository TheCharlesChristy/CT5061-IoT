# IoT Agricultural Management System - Devcontainer Setup

This directory contains the development container configuration for the project.

## Quick Start

1. **Install Prerequisites**:
   - [Docker Desktop](https://www.docker.com/products/docker-desktop)
   - [Visual Studio Code](https://code.visualstudio.com/)
   - [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

2. **Open in Container**:
   - Open this project folder in VS Code
   - Click "Reopen in Container" when prompted
   - Wait for initial setup (5-10 minutes first time)

3. **Start Developing**!

## What You Get

- ✅ Python 3.11 with IoT libraries (MQTT, Serial, Flask, etc.)
- ✅ C/C++ compilers (GCC, G++)
- ✅ ARM toolchain for STM32 and other ARM microcontrollers
- ✅ AVR toolchain for Arduino
- ✅ PlatformIO for unified embedded development
- ✅ Serial communication tools
- ✅ All necessary VS Code extensions pre-installed

## Files

- **devcontainer.json** - Main configuration file
- **setup.sh** - Post-creation setup script
- **Dockerfile** - Alternative Docker-based setup (optional)
- **README.md** - Comprehensive documentation

## Documentation

See [README.md](./README.md) for complete documentation including:
- Detailed tool list
- Microcontroller development guide
- USB device setup
- Troubleshooting
- Best practices

## Support

For issues:
1. Check the [README.md](./README.md) troubleshooting section
2. Verify Docker is running
3. Rebuild container: `Ctrl+Shift+P` → "Dev Containers: Rebuild Container"
