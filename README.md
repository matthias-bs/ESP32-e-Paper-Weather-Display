# ESP32-e-Paper-Weather-Display
[![CI](https://github.com/matthias-bs/ESP32-e-Paper-Weather-Display/actions/workflows/CI.yml/badge.svg)](https://github.com/matthias-bs/ESP32-e-Paper-Weather-Display/actions/workflows/CI.yml)
[![GitHub release](https://img.shields.io/github/release/matthias-bs/ESP32-e-Paper-Weather-Display?maxAge=3600)](https://github.com/matthias-bs/ESP32-e-Paper-Weather-Display/releases)

<img src="https://github.com/matthias-bs/ESP32-e-Paper-Weather-Display/blob/main/weather_station_architecture.png" alt="Weather Station Architecture Diagram" width="1080">

## Features
* [Open Weather Map](https://openweathermap.org/) Weather Report / Forecast Display (from [G6EJD/ESP32-e-Paper-Weather-Display](https://github.com/G6EJD/ESP32-e-Paper-Weather-Display))
* Local Sensor Data Display
    * [Theengs Decoder](https://github.com/theengs/decoder) Bluetooth Low Energy Sensors Integration
    * [BME280 Temperature/Humidity/Barometric Pressure Sensor](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/) Integration
    * [Sensirion SCD4x CO<sub>2</sub> Sensor](https://developer.sensirion.com/sensirion-products/scd4x-co2-sensors/) Integration
* Remote Sensor Data Display
    * MQTT Client Integration (e.g. for [BresserWeatherSensorReceiver](https://github.com/matthias-bs/BresserWeatherSensorReceiver) or [BresserWeatherSensorLW](https://github.com/matthias-bs/BresserWeatherSensorLW))
* Publishing of local Sensor Data via MQTT (separate MQTT Broker Configuration)
* Publishing of MQTT discovery messages for integration of local Sensor Data in Home Assistant
* Switching between virtual Screens via TTP223 Touch Sensors
* Sensor Histogram Data stored in ESP32's RTC RAM (persistent in Deep-Sleep Mode)
* Currently only 7.5" e-Paper Displays supported

> [!CAUTION]
> [arduino-esp32 v3.0.7](https://github.com/espressif/arduino-esp32/releases/tag/3.0.7) must be used &ndash; newer versions will result in a linker error! (see https://github.com/matthias-bs/ESP32-e-Paper-Weather-Display/issues/34)

## Screens
**Note:** Display quality is much better in reality than in the images below! 
### Weather Report / Forecast
![1_forecast](https://github.com/user-attachments/assets/5e772a0b-022a-402c-842e-f8d6b7082d9c)

<!-- ![2-weather_report_forecast](https://user-images.githubusercontent.com/83612361/219954116-dd68a860-7884-4ef7-af2b-0ddd452a2d07.jpg) --> 
### Local Sensor Data
![2_local](https://github.com/user-attachments/assets/53efd31b-5702-4a25-946c-d51dc811b924)

<!-- ![3-weather_local](https://user-images.githubusercontent.com/83612361/219953502-6f0e3b16-58f8-4845-b5d6-c796484c778f.jpg) -->
### Remote Sensor Data
![3_remote](https://github.com/user-attachments/assets/5ee694ae-c27d-4be2-95a9-a7f3766b9e78)

<!-- ![4-weather_remote](https://user-images.githubusercontent.com/83612361/219953834-cd48c8b0-d533-40d9-b4aa-15b58e0bcb52.png) -->


## Setup

For standalone use, download the ZIP file to your desktop.

Go to Sketch > Include Library... > Add .ZIP Library... Then, choose the ZIP file.

After inclusion, Go to File > Examples and scroll down to 'ESP32-e-paperWeather-display' and choose `Waveshare_7_5_T7_Sensors.ino` <!--your version/screen size-->. Make sure to come back to this dialog from time to time to keep each library up to date. Also make sure that you only have one version of each of the following libraries installed.

Also see: https://www.arduino.cc/en/Guide/Libraries#toc4

- [GxEPD2 library](https://github.com/ZinggJM/GxEPD2) by Jean-Marc Zingg
   - [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library) by Adafruit Industries
   - [U8g2_for_Adafruit_GFX](https://github.com/olikraus/U8g2_for_Adafruit_GFX) by Oli Kraus
- [Arduino JSON](https://github.com/bblanchon/ArduinoJson) (v6 or above) by Benoît Blanchon
- [pocketBME280](https://github.com/angrest/pocketBME280) by Axel Grewe
- [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino) by h2zero
- [Theengs Decoder](https://github.com/theengs/decoder) by Florian Robert
- [I2C SCD4x Arduino Library](https://github.com/Sensirion/arduino-i2c-scd4x) by Sensirion
- [Arduino MQTT](https://github.com/256dpi/arduino-mqtt) by Joël Gähwiler
- [cQueue](https://github.com/SMFSW/cQueue) by SMFSW

Download the software to your Arduino's library directory.

1. From the examples, choose <!--depending on your module either-->
   - Waveshare_7_5_T7_Sensors
   <!-- Waveshare_7_5_T7 (newer 800x480 version of the older 640x384)-->

2. Obtain your [OWM API key](https://openweathermap.org/appid) - it's free

3. Edit the `owm_credentials.h` file in the IDE (TAB at top of IDE) and edit
   * the Bluetooth LE sensor's address
   * Language
   * Country
   * Time Zone
   * Units (Metric or Imperial)
   * MQTT settings (for remote data)
   * OpenWeatherMap API Key
   * a valid weather station location on OpenWeatherMap

5. If your are using the older style Waveshare HAT then you need to use:
  
  **display.init(); //for older Waveshare HAT's 
  
  In the InitialiseDisplay() function, comment out the variant as required 

6. Save your files.

NOTE: See schematic for the wiring diagram, all displays are wired the same, so wire a 7.5" the same as a 4.2", 2.9" or 1.54" display! Both 2.13" TTGO T5 and 2.7" T5S boards come pre-wired. The 3.7" FireBeetle example contains wiring details.

The Battery monitor assumes the use of a Lolin D32 board which uses GPIO-35 as an ADC input, also it has an on-board 100K+100K voltage divider directly connected to the Battery terminals. On other boards, you will need to change the analogRead(35) statement to your board e.g. (39) and attach a voltage divider to the battery terminals. The TTGO T5 and T5S boards already contain the resistor divider on the correct pin. The FireBeetle has a battery monitor on GPIO-36.

7. Change the **Partition Scheme** in the Arduino IDE to "Mnimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"

8. Optional: Personalize your display
   * change the text on the start screen (define `TXT_START` in `owm_credentials.h`)
   * change the screen titles (string `LOCATIONS_TXT` in `owm_credentials.h`)
   * replace the bitmap images on the local/remote screens (`bitmap_local.h` and `bitmap_remote.h`)
     
     see [Image to C++ - Conversion to Bitmap](https://javl.github.io/image2cpp/)

Compile and upload the code - Enjoy!

7.5" 800x480 E-Paper Layout

![alt text width="600"](/Waveshare_7_5_new.jpg)

7.5" 640x384 E-Paper Layout

![alt text width="600"](/Waveshare_7_5.jpg)


**** NOTE change needed for latest Waveshare HAT versions ****

Ensure you have the latest GxEPD2 library

See here: https://github.com/ZinggJM/GxEPD2/releases/

Modify this line in the code:

display.init(115200, true, 2); // init(uint32_t serial_diag_bitrate, bool initial, uint16_t reset_duration, bool pulldown_rst_mode)

Wiring Schematic for ALL Waveshare E-Paper Displays
![alt_text, width="300"](/Schematic.JPG)
