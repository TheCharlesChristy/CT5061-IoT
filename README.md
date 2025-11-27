# CT5061-IoT

IoT experiments for CT5061, featuring a modular device framework, OLED graphics primitives, and a growing catalog of sensor demos.

## Available Demos

| Demo | Source | Description |
|------|--------|-------------|
| Fan Speed Test | `src/FanTestApp.cpp` | Serial-console driven PWM test harness for the new `Fan` device (VIN/GND + PWM pin) with manual and ramp modes. |
| Soil Moisture Graph | `src/SoilMoistureGraphApp.cpp` | Samples the `SoilMoistureSensor` and renders a rolling graph plus telemetry on the SSD1306 display. |

> **Tip:** The default `src/main.cpp` focuses on the fan test. To run the soil moisture graph, instantiate `SoilMoistureGraphApp` (or use `src/demos/SoilMoistureGraphDemo.cpp`) in your own sketch.

## New Soil Moisture Sensor

- Header: `include/SoilMoistureSensor.hpp`
- Analog pin configurable (defaults to GPIO 1 on the XIAO ESP32S3)
- Set calibration points with `setCalibration(dryReading, wetReading)` to convert ADC values to 0-100% moisture
- `receive()` now supports analog transports, so the device can participate in the `DeviceRegistry` queue alongside I2C devices

## New Fan Device

- Header: `include/Fan.hpp`
- PWM-capable virtual `Device` that drives a VIN/GND fan through a configurable MCU pin (defaults to GPIO 2)
- `send()` writes the duty cycle (0-255) and `receive()` reports the current duty plus optional percentage for queued actions
- `FanTestApp` (`include/FanTestApp.hpp`) exposes `FAN`, `FANP`, `FAN?`, and `RAMP ON|OFF` commands via the serial console for quick validation

## Running a Demo

1. Connect the Seeed XIAO ESP32S3 (or compatible) plus the required peripherals (SSD1306 OLED, SHT45 or soil probe).
2. Select the desired sketch in `platformio.ini` (default environment is already configured).
3. Build/flash via PlatformIO:

```bash
pio run -t upload
```

Use the serial monitor at 115200 baud to watch console output.
