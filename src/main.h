#include <Arduino.h>
#include <esp32_smartdisplay.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <HTTPClient.h>
#include <mbedtls/md.h>
#include <time.h>
#include <cctype>
#include <esp_system.h>
#include <esp_spiram.h>
#include <esp_himem.h>

#include "ArduinoJson-v7.1.0.h"
#include "ui/ui.h"
#include "tuya/tuya.h"

const long WIFI_TIMEOUT = 20000; // 20 seconds timeout for Wi-Fi connection
const int MAX_WIFI_RETRIES = 3;  // Maximum number of Wi-Fi connection attempts

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;     // GMT+1 (3600 seconds = 1 hour)
const int daylightOffset_sec = 3600; // 1 hour for DST

unsigned long lastTimeUpdateTime = 0;
const unsigned int timeUpdateFreq = 3600000;
unsigned long lastTokenUpdateTime = 0;
unsigned long lastDeviceUpdateTime = 0;
const unsigned int deviceUpdateFreq = 30000; // Update every 30s

// Function prototypes
// General functions
bool setupWiFi();

// Time related logic
void setupTime();
bool isCentralEuropeanSummerTime(time_t now);
void updateTimeOffset();