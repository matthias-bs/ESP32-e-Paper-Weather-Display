///////////////////////////////////////////////////////////////////////////////
// LocalInterface.h
//
// Local Sensor Data Interface for ESP32-e-Paper-Weather-Display
//
//
// created: 10/2024
//
//
// MIT License
//
// Copyright (c) 2024 Matthias Prinke
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
//
// History:
//
// 20241010 Extracted from Waveshare_7_5_T7_Sensors.ino
//
// ToDo:
// -
//
///////////////////////////////////////////////////////////////////////////////

#include "LocalInterface.h"

extern bool TouchTriggered();

extern local_sensors_t LocalSensors; 

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
static const int bleScanTime = 31; //!< BLE scan time in seconds
static std::vector<std::string> knownBLEAddresses = KNOWN_BLE_ADDRESSES;
#endif

#if defined(MITHERMOMETER_EN) || defined(THEENGSDECODER_EN)
static NimBLEScan *pBLEScan;
#endif

#ifdef THEENGSDECODER_EN
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks
{

  int m_devices_found = 0; //!< Number of known devices found

  std::string convertServiceData(std::string deviceServiceData)
  {
    int serviceDataLength = (int)deviceServiceData.length();
    char spr[2 * serviceDataLength + 1];
    for (int i = 0; i < serviceDataLength; i++)
      sprintf(spr + 2 * i, "%.2x", (unsigned char)deviceServiceData[i]);
    spr[2 * serviceDataLength] = 0;
    return spr;
  }

  void onResult(BLEAdvertisedDevice *advertisedDevice)
  {
    TheengsDecoder decoder;
    bool device_found = false;
    unsigned idx;
    JsonDocument doc;

    log_v("Advertised Device: %s", advertisedDevice->toString().c_str());
    JsonObject BLEdata = doc.to<JsonObject>();
    String mac_adress = advertisedDevice->getAddress().toString().c_str();

    BLEdata["id"] = (char *)mac_adress.c_str();
    for (idx = 0; idx < knownBLEAddresses.size(); idx++)
    {
      if (mac_adress == knownBLEAddresses[idx].c_str())
      {
        log_v("BLE device found at index %d", idx);
        device_found = true;
        break;
      }
    }

    if (advertisedDevice->haveName())
      BLEdata["name"] = (char *)advertisedDevice->getName().c_str();

    if (advertisedDevice->haveManufacturerData())
    {
      char *manufacturerdata = BLEUtils::buildHexData(NULL, (uint8_t *)advertisedDevice->getManufacturerData().data(), advertisedDevice->getManufacturerData().length());
      BLEdata["manufacturerdata"] = manufacturerdata;
      free(manufacturerdata);
    }

    if (advertisedDevice->haveRSSI())
      BLEdata["rssi"] = (int)advertisedDevice->getRSSI();

    if (advertisedDevice->haveTXPower())
      BLEdata["txpower"] = (int8_t)advertisedDevice->getTXPower();

    if (advertisedDevice->haveServiceData())
    {
      int serviceDataCount = advertisedDevice->getServiceDataCount();
      for (int j = 0; j < serviceDataCount; j++)
      {
        std::string service_data = convertServiceData(advertisedDevice->getServiceData(j));
        BLEdata["servicedata"] = (char *)service_data.c_str();
        std::string serviceDatauuid = advertisedDevice->getServiceDataUUID(j).toString();
        BLEdata["servicedatauuid"] = (char *)serviceDatauuid.c_str();
      }
    }

    if (decoder.decodeBLEJson(BLEdata) && device_found)
    {
      if (CORE_DEBUG_LEVEL >= ARDUHAL_LOG_LEVEL_DEBUG)
      {
        char buf[512];
        serializeJson(BLEdata, buf);
        log_d("TheengsDecoder found device: %s", buf);
      }
      BLEdata.remove("manufacturerdata");
      BLEdata.remove("servicedata");

      LocalSensors.ble_thsensor[idx].temperature = (float)BLEdata["tempc"];
      LocalSensors.ble_thsensor[idx].humidity = (float)BLEdata["hum"];
      LocalSensors.ble_thsensor[idx].batt_level = (uint8_t)BLEdata["batt"];
      LocalSensors.ble_thsensor[idx].rssi = (int)BLEdata["rssi"];
      LocalSensors.ble_thsensor[idx].valid = (LocalSensors.ble_thsensor[idx].batt_level > 0);
      log_i("Temperature:       %.1f°C", LocalSensors.ble_thsensor[idx].temperature);
      log_i("Humidity:          %.1f%%", LocalSensors.ble_thsensor[idx].humidity);
      log_i("Battery level:     %d%%", LocalSensors.ble_thsensor[idx].batt_level);
      log_i("RSSI             %ddBm", LocalSensors.ble_thsensor[idx].rssi = (int)BLEdata["rssi"]);
      m_devices_found++;
      log_d("BLE devices found: %d", m_devices_found);
    }

    // Abort scanning by touch sensor
    if (TouchTriggered())
    {
      log_i("Touch interrupt!");
      pBLEScan->stop();
    }

    // Abort scanning because all known devices have been found
    if (m_devices_found == knownBLEAddresses.size())
    {
      log_i("All devices found.");
      pBLEScan->stop();
    }
  }
};
#endif

// Get local sensor data
void LocalInterface::GetLocalData(void)
{
  LocalSensors.ble_thsensor[0].valid = false;
  LocalSensors.i2c_thpsensor[0].valid = false;
  LocalSensors.i2c_co2sensor.valid = false;

#if defined(SCD4X_EN) || defined(BME280_EN)
  TwoWire myWire = TwoWire(0);
  myWire.begin(I2C_SDA, I2C_SCL, 100000);
#endif

#ifdef SCD4X_EN
  SensirionI2CScd4x scd4x;

  uint16_t error;
  char errorMessage[256];

  scd4x.begin(myWire);

  // stop potential previously started measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error)
  {
    errorToString(error, errorMessage, 256);
    log_e("Error trying to execute stopPeriodicMeasurement(): %s", errorMessage);
  }

  // Start Measurement
  error = scd4x.measureSingleShot();
  if (error)
  {
    errorToString(error, errorMessage, 256);
    log_e("Error trying to execute measureSingleShot(): %s", errorMessage);
  }

  log_d("First measurement takes ~5 sec...");
#endif

#ifdef MITHERMOMETER_EN
  // Setup BLE Temperature/Humidity Sensors
  ATC_MiThermometer miThermometer(knownBLEAddresses); //!< Mijia Bluetooth Low Energy Thermo-/Hygrometer
  miThermometer.begin();

  // Set sensor data invalid
  miThermometer.resetData();

  // Get sensor data - run BLE scan for <bleScanTime>
  miThermometer.getData(bleScanTime);

  if (miThermometer.data[0].valid)
  {
    LocalSensors.ble_thsensor[0].valid = true;
    LocalSensors.ble_thsensor[0].temperature = miThermometer.data[0].temperature / 100.0;
    LocalSensors.ble_thsensor[0].humidity = miThermometer.data[0].humidity / 100.0;
    LocalSensors.ble_thsensor[0].batt_level = miThermometer.data[0].batt_level;
  }
  miThermometer.clearScanResults();
#endif

#ifdef THEENGSDECODER_EN

  // From https://github.com/theengs/decoder/blob/development/examples/ESP32/ScanAndDecode/ScanAndDecode.ino:
  // MyAdvertisedDeviceCallbacks are still triggered multiple times; this makes keeping track of received
  // sensors difficult. Setting ScanFilterMode to CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE seems to
  // restrict callback invocation to once per device as desired.
  // NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DEVICE);
  NimBLEDevice::setScanFilterMode(CONFIG_BTDM_SCAN_DUPL_TYPE_DATA_DEVICE);
  NimBLEDevice::setScanDuplicateCacheSize(200);
  NimBLEDevice::init("");

  pBLEScan = NimBLEDevice::getScan(); // create new scan
  // Set the callback for when devices are discovered, no duplicates.
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
  pBLEScan->setActiveScan(true); // Set active scanning, this will get more data from the advertiser.
  pBLEScan->setInterval(97);     // How often the scan occurs / switches channels; in milliseconds,
  pBLEScan->setWindow(37);       // How long to scan during the interval; in milliseconds.
  pBLEScan->setMaxResults(0);    // do not store the scan results, use callback only.
  pBLEScan->start(bleScanTime, false /* is_continue */);
#endif

  if (LocalSensors.ble_thsensor[0].valid)
  {
    log_d("Outdoor Air Temp.:   % 3.1f °C", LocalSensors.ble_thsensor[0].temperature);
    log_d("Outdoor Humidity:     %3.1f %%", LocalSensors.ble_thsensor[0].humidity);
    log_d("Outdoor Sensor Batt:    %d %%", LocalSensors.ble_thsensor[0].batt_level);
  }
  else
  {
    log_d("Outdoor Air Temp.:    --.- °C");
    log_d("Outdoor Humidity:     --   %%");
    log_d("Outdoor Sensor Batt:  --   %%");
  }

#ifdef BME280_EN
  pocketBME280 bme280;
  bme280.setAddress(0x77);
  log_v("BME280: start");
  if (bme280.begin(myWire))
  {
    LocalSensors.i2c_thpsensor[0].valid = true;
    bme280.startMeasurement();
    while (!bme280.isMeasuring())
    {
      log_v("BME280: Waiting for Measurement to start");
      delay(1);
    }
    while (bme280.isMeasuring())
    {
      log_v("BME280: Measurement in progress");
      delay(1);
    }
    LocalSensors.i2c_thpsensor[0].temperature = bme280.getTemperature() / 100.0;
    LocalSensors.i2c_thpsensor[0].pressure = bme280.getPressure() / 100.0;
    LocalSensors.i2c_thpsensor[0].humidity = bme280.getHumidity() / 1024.0;
    log_d("Indoor Temperature: %.1f °C", LocalSensors.i2c_thpsensor[0].temperature);
    log_d("Indoor Pressure: %.0f hPa", LocalSensors.i2c_thpsensor[0].pressure);
    log_d("Indoor Humidity: %.0f %%rH", LocalSensors.i2c_thpsensor[0].humidity);
  }
  else
  {
    log_d("Indoor Temperature: --.- °C");
    log_d("Indoor Pressure:    --   hPa");
    log_d("Indoor Humidity:    --  %%rH");
  }
#endif

#ifdef SCD4X_EN
  if (LocalSensors.i2c_thpsensor[0].valid)
  {
    error = scd4x.setAmbientPressure((uint16_t)LocalSensors.i2c_thpsensor[0].pressure);
    if (error)
    {
      errorToString(error, errorMessage, 256);
      log_e("Error trying to execute setAmbientPressure(): %s", errorMessage);
    }
  }
  // Read Measurement
  bool isDataReady = false;

  for (int i = 0; i < 50; i++)
  {
    error = scd4x.getDataReadyFlag(isDataReady);
    if (error)
    {
      errorToString(error, errorMessage, 256);
      log_e("Error trying to execute getDataReadyFlag(): %s", errorMessage);
    }
    if (error || isDataReady)
    {
      break;
    }
    delay(100);
  }

  if (isDataReady && !error)
  {
    error = scd4x.readMeasurement(LocalSensors.i2c_co2sensor.co2, LocalSensors.i2c_co2sensor.temperature, LocalSensors.i2c_co2sensor.humidity);
    if (error)
    {
      errorToString(error, errorMessage, 256);
      log_e("Error trying to execute readMeasurement(): %s", errorMessage);
    }
    else if (LocalSensors.i2c_co2sensor.co2 == 0)
    {
      log_e("Invalid sample detected, skipping.");
    }
    else
    {
      log_d("SCD4x CO2: %d ppm", LocalSensors.i2c_co2sensor.co2);
      log_d("SCD4x Temperature: %4.1f °C", LocalSensors.i2c_co2sensor.temperature);
      log_d("SCD4x Humidity: %3.1f %%rH", LocalSensors.i2c_co2sensor.humidity);
      LocalSensors.i2c_co2sensor.valid = true;
    }
  }
  scd4x.powerDown();
#endif
}