// Change to your WiFi credentials
const char* ssid0     = "your_ssid0";
const char* password0 = "your_password0";
const char* ssid1     = "your_ssid1";
const char* password1 = "your_password1";
const char* ssid2     = "your_ssid2";
const char* password2 = "your_password2";


// Domain Name Server - separate bytes by comma!
#define MY_DNS 192,168,0,1

// Weather display's hostname
const char *Hostname = "your_hostname";

//!< List of known BLE sensors' MAC addresses (separate multiple entries by comma)
//#define KNOWN_BLE_ADDRESSES {"a4:c1:38:b8:1f:7f"}  
#define KNOWN_BLE_ADDRESSES {"49:22:05:17:0c:1f"}

// MQTT connection for subscribing (remote sensor data)
const int   MQTT_PORT = 1883;
const char *MQTT_HOST = "your_broker";
const char *MQTT_USER = "your_user";   // leave blank if no credentials used
const char *MQTT_PASS = "your_passwd"; // leave blank if no credentials used
const char *MQTT_SUB_IN = "your/subscribe/topic";

// TOPICS_NEW: BresserWeatherSensorLW
#define TOPICS_NEW

#ifdef TOPICS_NEW
  #define WS_TEMP_C "ws_temp_c"
  #define WS_HUMIDITY "ws_humidity"
  #define TH1_TEMP_C "th1_temp_c"
  #define TH1_HUMIDITY "th1_humidity"
  #define A0_VOLTAGE_MV "a0_voltage_mv"
  #define WS_RAIN_DAILY_MM "ws_rain_daily_mm"
  #define WS_RAIN_HOURLY_MM "ws_rain_hourly_mm"
  #define WS_RAIN_MM "ws_rain_mm"
  #define WS_RAIN_MONTHLY_MM "ws_rain_monthly_mm"
  #define WS_RAIN_WEEKLY_MM "ws_rain_weekly_mm"
  #define SOIL1_MOISTURE "soil1_moisture"
  #define SOIL1_TEMP_C "soil1_temp_c"
  #define OW0_TEMP_C "ow0_temp_c"
  #define WS_WIND_AVG_MS "ws_wind_avg_ms"
  #define WS_WIND_DIR_DEG "ws_wind_dir_deg"
  #define WS_WIND_GUST_MS "ws_wind_gust_ms"
#else
  #define WS_TEMP_C "air_temp_c"
  #define WS_HUMIDITY "humidity"
  #define TH1_TEMP_C "indoor_temp_c"
  #define TH1_HUMIDITY "indoor_humidity"
  #define A0_VOLTAGE_MV "battery_v"
  #define WS_RAIN_DAILY_MM "rain_day"
  #define WS_RAIN_HOURLY_MM "rain_hr"
  #define WS_RAIN_MM "rain_mm"
  #define WS_RAIN_MONTHLY_MM "rain_mon"
  #define WS_RAIN_WEEKLY_MM "rain_week"
  #define SOIL1_MOISTURE "soil_moisture"
  #define SOIL1_TEMP_C "soil_temp_c"
  #define OW0_TEMP_C "water_temp_c"
  #define WS_WIND_AVG_MS "wind_avg_meter_sec"
  #define WS_WIND_DIR_DEG "wind_direction_deg"
  #define WS_WIND_GUST_MS "wind_gust_meter_sec"
#endif

// MQTT connection for publishing (local sensor data)
const int   MQTT_PORT_P = 1883;
const char *MQTT_HOST_P = "your_broker_pub";
const char *MQTT_USER_P = "your_user_pub";
const char *MQTT_PASS_P = "your passwd_pub";

// Use your own API key by signing up for a free developer account at https://openweathermap.org/
String apikey       = "your_API_key";                      // See: https://openweathermap.org/  // It's free to get an API key, but don't take more than 60 readings/minute!
const char server[] = "api.openweathermap.org";
//http://api.openweathermap.org/data/2.5/weather?q=Melksham,UK&APPID=your_OWM_API_key&mode=json&units=metric&cnt=1   // Example API call for weather data
//http://api.openweathermap.org/data/2.5/forecast?q=Melksham,UK&APPID=your_OWM_API_key&mode=json&units=metric&cnt=40 // Example API call for forecast data
//Set your location according to OWM locations

String City             = "MELKSHAM";                      // Your home city See: http://bulk.openweathermap.org/sample/
String Country          = "GB";                            // Your _ISO-3166-1_two-letter_country_code country code, on OWM find your nearest city and the country code is displayed
                                                           // https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
String Language         = "EN";                            // NOTE: Only the weather description is translated by OWM
                                                           // Examples: Arabic (AR) Czech (CZ) English (EN) Greek (EL) Persian(Farsi) (FA) Galician (GL) Hungarian (HU) Japanese (JA)
                                                           // Korean (KR) Latvian (LA) Lithuanian (LT) Macedonian (MK) Slovak (SK) Slovenian (SL) Vietnamese (VI)
String Hemisphere       = "north";                         // or "south"  
String Units            = "M";                             // Use 'M' for Metric or I for Imperial 
const char* Timezone    = "GMT0BST,M3.5.0/01,M10.5.0/02";  // Choose your time zone from: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
                                                           // See below for examples
const char* ntpServer   = "0.uk.pool.ntp.org";             // Or, choose a time server close to you, but in most cases it's best to use pool.ntp.org to find an NTP server
                                                           // then the NTP system decides e.g. 0.pool.ntp.org, 1.pool.ntp.org as the NTP syem tries to find  the closest available servers
                                                           // EU "0.europe.pool.ntp.org"
                                                           // US "0.north-america.pool.ntp.org"
                                                           // See: https://www.ntppool.org/en/                                                           
int   gmtOffset_sec     = 0;    // UK normal time is GMT, so GMT Offset is 0, for US (-5Hrs) is typically -18000, AU is typically (+8hrs) 28800
int  daylightOffset_sec = 3600; // In the UK DST is +1hr or 3600-secs, other countries may use 2hrs 7200 or 30-mins 1800 or 5.5hrs 19800 Ahead of GMT use + offset behind - offset

// Example time zones
//const char* Timezone = "MET-1METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "CET-1CEST,M3.5.0,M10.5.0/3";       // Central Europe
//const char* Timezone = "EST-2METDST,M3.5.0/01,M10.5.0/02"; // Most of Europe
//const char* Timezone = "EST5EDT,M3.2.0,M11.1.0";           // EST USA  
//const char* Timezone = "CST6CDT,M3.2.0,M11.1.0";           // CST USA
//const char* Timezone = "MST7MDT,M4.1.0,M10.5.0";           // MST USA
//const char* Timezone = "NZST-12NZDT,M9.5.0,M4.1.0/3";      // Auckland
//const char* Timezone = "EET-2EEST,M3.5.5/0,M10.5.5/0";     // Asia
//const char* Timezone = "ACST-9:30ACDT,M10.1.0,M4.1.0/3":   // Australia

// Personalization Options

// Screen definitions
#define ScreenOWM   0
#define ScreenLocal 1
#define ScreenMQTT  2
#define ScreenStart 3

#define TXT_START "Your Weather Station"
#define START_SCREEN ScreenStart
#define LAST_SCREEN  ScreenMQTT

// Locations / Screen Titles
#define LOCATIONS_TXT {"Forecast", "Local", "Remote", "Start"}

#include "bitmap_local.h"             // Picture shown on ScreenLocal - replace by your own
#include "bitmap_remote.h"            // Picture shown on ScreenMQTT  - replace by your own
