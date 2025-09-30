# esp-pid

Smart PID Control for ESP32-C3

Other: [[RGB LED LAMP](https://github.com/DrA1ex/esp_led_lamp)] [[LED](https://github.com/DrA1ex/esp_led)] [[RELAY](https://github.com/DrA1ex/esp_relay)]

## UI

<img width="382" alt="image" src="https://github.com/user-attachments/assets/a2eba0ad-ee8d-491d-8ac3-008b8cd79ef4" />


## Features

- Web/Mobile Application (PWA)
- Support for different sensor/control types
- Integration with any Smart Home Assistant (such as Alise or SmartHome) via MQTT broker
- MQTT Protocol

### Configuration
Before uploading the firmware, verify the pin assignments in `constants.h` match your hardware setup. Incorrect pin configurations can lead to motor malfunctions or unresponsive end-stops. If you’re using a Hall sensor, ensure it is calibrated to detect the magnetic field at the desired end-stop position.

For detailed pin mappings and configuration options, check [constants.h](/src/constants.h).

## Installation

### Manual Build / OTA

1. Install [Platform.IO](https://platformio.org/install):
2. (Optional) Modify the `credentials.h` file and, if necessary, customize `constants.h`. You can change these parameters later through the Web UI.
3. Upload filesystem and firmware

**Note:** This repository contains a submodule. Please use the `--recursive` option when cloning.

```bash
git clone --recursive https://github.com/DrA1ex/esp-pid.git
cd esp-pid

# Make script executable
chmod +x ./upload_fs.sh

# Specify the platform: esp32-c3
PLATFORM=esp32-c3

# Set the environment: debug, release, or ota
ENV=release

# For OTA: set your ESP's address
ADDRESS=esp_pid.local

# Additional envs if OTA enabled
if [ "$ENV" = "ota" ]; then OTA=1 else OTA=0 ADDRESS= fi

pio run -t upload -e $PLATFORM-$ENV --upload-port "$ADDRESS"
./upload_fs.sh --upload-port "$ADDRESS"
```
