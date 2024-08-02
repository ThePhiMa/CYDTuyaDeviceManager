[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/rzeldent/library/esp32_smartdisplay.svg)](https://registry.platformio.org/libraries/rzeldent/esp32_smartdisplay)
![C++](https://img.shields.io/badge/C%2B%2B-Arduino-blue)
![Visual Studio Code](https://img.shields.io/badge/Visual_Studio_Code-blue)
![Tuya Cloud](https://img.shields.io/badge/Tuya-Cloud-orange)



# Online Tuya Devices Manager for Cheap Yellow Display (CYD) boards. 
(Tested and Running on the ESP32 2432S024Rv3)

#### This is a basic device manager for all kinds of devices connected to the Tuya cloud via e.g. the Smart Life app. I use it to display the status for my door, window, socket and temperature sensors.

![TuyaDeviceManager running on CYD](https://notenoughsleep.eu/files/tuya/img/CYDBoardTuyaDeviceManager.jpg)

Developed with Visual Studio Code and [PlatformIO](https://platformio.org/)

>[!WARNING]
>Still very much work-in-progress! There is no error handling for now.

Credits go to Rzeldent's [esp32-smartdisplay](https://github.com/rzeldent/esp32-smartdisplay) without I would never have been able to set this up so quickly.

## Features
- Fetches all devices from the Tuya cloud connected to the clientID
- For each device ID, fetch the current device status
- If the device type matches one of the declarations in the `ui_globals.cpp`, parse and display the infos
  - Don't forget to also add new devices types to the parsing function _updateDevicesDisplay()_ in `ui.cpp`

## Requirements for running
- A clientID and clientSecret are needed -> they are obtainable in the Tuya cloud at *cloud->project->Overview* (https://platform.tuya.com/cloud/basic)
  - (The account might need to be set as a dev account, but I forgot where exactly this can be set ;))


## Setup
1. Clone this repository (all sub modules will automatically be cloned by platformIO)
2. Copy `config_template.h` to `config.h`
3. Edit `config.h` and fill in your personal information:
   - WiFi SSID and password
   - Tuya client ID and secret
4. Set your Arduino board in the platformio.ini
5. Compile and upload the project to your Arduino board
6. Happy coding!

Note: `config.h` is ignored by git to keep your personal information private.

Note #2: The method _updateTimeOffset()_ in `main.cpp` only works for CET, it needs to be adjusted for other timezones for the clock and api calls to work correctly.

### Version history
v0.1: first public version, 01.08.24


The Tuya docs are really extensive and well done (https://developer.tuya.com/en/docs/iot/device-intelligentize-in-5-minutes?id=K914joxbogkm6)

### Links
- LVGL:
  - ESP32 smartdisplay demo by Rzeldent: https://github.com/rzeldent/esp32-smartdisplay-demo
  - LVGL examples: https://docs.lvgl.io/master/examples.html
- Tuya: 
  - API explorer: https://eu.platform.tuya.com/cloud/explorer
  - Get access token: https://developer.tuya.com/en/docs/cloud/6c1636a9bd?id=Ka7kjumkoa53v
  - Sign request for cloud auth: https://developer.tuya.com/en/docs/iot/new-singnature?id=Kbw0q34cs2e5g
  - Verify signature result: https://developer.tuya.com/en/docs/iot/check-postman-sign?id=Kavfn3820sxg4
  - API calls samples: https://developer.tuya.com/en/docs/iot/set-up-postman-environment?id=Ka7o385w1svns
  - Limits on  api request frequency: https://developer.tuya.com/en/docs/iot/frequency-control?id=Kcojz2r2dg1f6
  - GET postman env for api calls: https://developer.tuya.com/en/docs/iot/set-up-postman-environment?id=Ka7o385w1svns
