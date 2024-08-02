// Links
// LVGL:
// ESP32 smartdisplay demo by Rzeldent: https://github.com/rzeldent/esp32-smartdisplay-demo
// LVGL examples: https://docs.lvgl.io/master/examples.html
// Tuya :
// API explorer: https://eu.platform.tuya.com/cloud/explorer
// Get access token: https://developer.tuya.com/en/docs/cloud/6c1636a9bd?id=Ka7kjumkoa53v
// Sign request for cloud auth: https://developer.tuya.com/en/docs/iot/new-singnature?id=Kbw0q34cs2e5g
// Verify signature result: https://developer.tuya.com/en/docs/iot/check-postman-sign?id=Kavfn3820sxg4
// API calls samples: https://developer.tuya.com/en/docs/iot/set-up-postman-environment?id=Ka7o385w1svns
// Limits on  api request frequency: https://developer.tuya.com/en/docs/iot/frequency-control?id=Kcojz2r2dg1f6
// Set postman env for api calls: https://developer.tuya.com/en/docs/iot/set-up-postman-environment?id=Ka7o385w1svns

#include <main.h>
#include "config/config.h" // Include the configuration file

void setup()
{
#ifdef ARDUINO_USB_CDC_ON_BOOT
    delay(5000);
#endif
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    log_i("Board: %s", BOARD_NAME);
    log_i("CPU: %s rev%d, CPU Freq: %d Mhz, %d core(s)", ESP.getChipModel(), ESP.getChipRevision(), getCpuFrequencyMhz(), ESP.getChipCores());
    log_i("Free heap: %d bytes", ESP.getFreeHeap());
    log_i("Free PSRAM: %d bytes", ESP.getPsramSize());
    log_i("SDK version: %s", ESP.getSdkVersion());

    smartdisplay_init();

    __attribute__((unused)) auto disp = lv_disp_get_default();
    // lv_disp_set_rotation(disp, LV_DISP_ROT_90);
    // lv_disp_set_rotation(disp, LV_DISP_ROT_180);
    lv_disp_set_rotation(disp, LV_DISP_ROT_270);

    setup_ui();

    if (!setupWiFi())
    {
        Serial.println("Restarting due to WiFi connection failure");
        ESP.restart();
    }

    setupTime();
    updateTimeOffset();

    // Load tokens from EEPROM
    loadTokens();

    // Get new access token if needed
    if (accessToken.length() == 0 || millis() > expiresIn)
    {
        getAccessToken();
        // devicesUpdate();
    }
}

void loop()
{
    lv_timer_handler();

    // Refresh token 30s before expiration if needed
    if (millis() > expiresIn - 30000)
    {
        refreshAccessToken();
    }

    if (millis() - lastDeviceUpdateTime > deviceUpdateFreq)
    {
        // Get and display device list
        devicesUpdate();
        lastDeviceUpdateTime = millis();
    }

    updateTimeDisplay();

    if (millis() - lastTimeUpdateTime > timeUpdateFreq)
    {
        // Check every hour
        updateTimeOffset();
        Serial.print("Current timestamp: ");
        Serial.println(getTimestamp());
        lastTimeUpdateTime = millis();
    }

    // Update once a second only
    delay(1000);
}

void setupTime()
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    Serial.println("Waiting for NTP time sync: ");
    time_t now = time(nullptr);
    int retry = 0;
    while (now < 8 * 3600 * 2)
    {
        // Wait for a reasonable time value
        delay(500);
        Serial.print(".");
        now = time(nullptr);
        if (++retry > 20)
        {
            // Try for 10 seconds max
            Serial.println("\nFailed to sync with NTP. Check your internet connection.");
            return;
        }
    }
    Serial.println("\nTime synchronized");

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "Current time: %A, %B %d %Y %H:%M:%S");
}

bool isCentralEuropeanSummerTime(time_t now)
{
    struct tm *timeinfo;
    timeinfo = localtime(&now);

    // DST starts last Sunday of March and ends last Sunday of October
    if (timeinfo->tm_mon < 2 || timeinfo->tm_mon > 9)
    {
        return false;
    }
    if (timeinfo->tm_mon > 2 && timeinfo->tm_mon < 9)
    {
        return true;
    }

    int previousSunday = timeinfo->tm_mday - timeinfo->tm_wday;

    if (timeinfo->tm_mon == 2)
    { // March
        return previousSunday >= 25;
    }
    else
    { // October
        return previousSunday < 25;
    }
}

void updateTimeOffset()
{
    time_t now;
    time(&now);

    if (isCentralEuropeanSummerTime(now))
    {
        configTime(gmtOffset_sec + daylightOffset_sec, 0, ntpServer);
        Serial.println("Adjusted for summer time");
    }
    else
    {
        configTime(gmtOffset_sec, 0, ntpServer);
        Serial.println("Adjusted for standard time");
    }

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "Updated time: %A, %B %d %Y %H:%M:%S");
}

bool setupWiFi()
{
    int retries = 0;

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (retries < MAX_WIFI_RETRIES)
    {
        Serial.println("Attempting to connect to WiFi...");

        unsigned long startAttemptTime = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT)
        {
            delay(500);
            Serial.print(".");
        }

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("\nConnected to WiFi");
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());
            return true;
        }
        else
        {
            Serial.println("\nFailed to connect to WiFi");
            retries++;
            if (retries < MAX_WIFI_RETRIES)
            {
                Serial.println("Retrying...");
                WiFi.disconnect(true, true);
                delay(1000);
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            }
        }
    }

    Serial.println("WiFi connection failed after maximum retries");
    return false;
}