#include "tuya.h"
#include "config/config.h"

void loadTokens()
{
    // Serial.println("Load tokens");
    char buffer[100];

    // Load access token
    for (int i = 0; i < 100; i++)
    {
        buffer[i] = EEPROM.read(ACCESS_TOKEN_ADDR + i);
        if (buffer[i] == '\0')
            break;
    }
    accessToken = String(buffer);

    // Load refresh token
    for (int i = 0; i < 100; i++)
    {
        buffer[i] = EEPROM.read(REFRESH_TOKEN_ADDR + i);
        if (buffer[i] == '\0')
            break;
    }
    refreshToken = String(buffer);

    // Load expiration time
    EEPROM.get(EXPIRES_IN_ADDR, expiresIn);
    // Serial.println("Tokens loaded");
}

void saveTokens()
{
    // Save access token
    for (unsigned int i = 0; i < accessToken.length(); i++)
    {
        EEPROM.write(ACCESS_TOKEN_ADDR + i, accessToken[i]);
    }
    EEPROM.write(ACCESS_TOKEN_ADDR + accessToken.length(), '\0');

    // Save refresh token
    for (unsigned int i = 0; i < refreshToken.length(); i++)
    {
        EEPROM.write(REFRESH_TOKEN_ADDR + i, refreshToken[i]);
    }
    EEPROM.write(REFRESH_TOKEN_ADDR + refreshToken.length(), '\0');

    // Save expiration time
    EEPROM.put(EXPIRES_IN_ADDR, expiresIn);

    EEPROM.commit();
}

void getAccessToken()
{
    Serial.println("Getting access token...");

    String url = String(baseURL) + "v1.0/token?grant_type=1";
    String timestamp = String(getTimestamp());
    String nonce = String(random(0xffff), HEX);

    String signature = calculateSignature(TUYA_CLIENT_ID, "", timestamp, nonce, "/v1.0/token?grant_type=1", "", TUYA_CLIENT_SECRET);

    HTTPClient http;
    http.begin(url);

    http.addHeader("client_id", TUYA_CLIENT_ID);
    http.addHeader("sign", signature);
    http.addHeader("t", timestamp);
    http.addHeader("sign_method", "HMAC-SHA256");
    http.addHeader("nonce", nonce);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        // Serial.println("Response: " + response);

        JsonDocument doc;
        deserializeJson(doc, response);

        if (doc["success"].as<bool>())
        {
            accessToken = doc["result"]["access_token"].as<String>();
            refreshToken = doc["result"]["refresh_token"].as<String>();
            expiresIn = millis() + doc["result"]["expire_time"].as<unsigned long>() * 1000;

            // Serial.println("Access token obtained successfully");
            // Serial.println("Access Token: " + accessToken);
            // Serial.println("Refresh Token: " + refreshToken);
            // Serial.println("Expires In: " + String(expiresIn));
            // Serial.println("Current ms: " + String(millis()));
        }
        else
        {
            Serial.println("Failed to obtain access token");
            Serial.println("Error: " + doc["msg"].as<String>());
        }
    }
    else
    {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

String getTimestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t time_ms = (uint64_t)(tv.tv_sec) * 1000LL + (uint64_t)(tv.tv_usec) / 1000LL;
    char timestamp[14];
    snprintf(timestamp, sizeof(timestamp), "%013llu", time_ms);
    return String(timestamp);
}

String toUpperCase(String str)
{
    String result = str;
    for (unsigned int i = 0; i < result.length(); i++)
    {
        result[i] = toupper(result[i]);
    }
    return result;
}

String calculateSignature(const String &clientId, const String &accessToken, const String &timestamp, const String &nonce, const String &signUrl, const String &body, const String &clientSecret)
{
    String stringToSign = "GET\n";
    // stringToSign += calculateHash(body) + "\n\n";
    stringToSign += emptyStringSHA256;
    stringToSign += "\n\n";
    stringToSign += signUrl;

    String message = clientId + accessToken + timestamp + nonce + stringToSign;
    // Serial.println("Message to hash: " + message);
    String signature = calculateHmacSha256(message, clientSecret);
    return toUpperCase(signature);
}

String calculateHash(const String &payload)
{
    if (payload.length() == 0)
    {
        return "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    }
    uint8_t shaResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, (const unsigned char *)payload.c_str(), payload.length());
    mbedtls_md_finish(&ctx, shaResult);
    mbedtls_md_free(&ctx);

    String hashHex = "";
    for (int i = 0; i < sizeof(shaResult); i++)
    {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", shaResult[i]);
        hashHex += hex;
    }

    return hashHex;
}

String calculateHmacSha256(const String &message, const String &key)
{
    uint8_t hmacResult[32];
    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 1);
    mbedtls_md_hmac_starts(&ctx, (const unsigned char *)key.c_str(), key.length());
    mbedtls_md_hmac_update(&ctx, (const unsigned char *)message.c_str(), message.length());
    mbedtls_md_hmac_finish(&ctx, hmacResult);
    mbedtls_md_free(&ctx);

    String hmacHex = "";
    for (int i = 0; i < sizeof(hmacResult); i++)
    {
        char hex[3];
        snprintf(hex, sizeof(hex), "%02x", hmacResult[i]);
        hmacHex += hex;
    }

    return hmacHex;
}

void refreshAccessToken()
{
    Serial.println("Refreshing access token...");

    String subURL = "v1.0/token/" + refreshToken;
    String url = String(baseURL) + subURL;
    String timestamp = String(getTimestamp());
    String nonce = String(random(0xffff), HEX);

    String signature = calculateSignature(TUYA_CLIENT_ID, "", timestamp, nonce, "/" + subURL, "", TUYA_CLIENT_SECRET);

    HTTPClient http;
    http.begin(url);

    http.addHeader("client_id", TUYA_CLIENT_ID);
    http.addHeader("sign", signature);
    http.addHeader("t", timestamp);
    http.addHeader("sign_method", "HMAC-SHA256");
    http.addHeader("nonce", nonce);

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        // Serial.println("Response: " + response);

        JsonDocument doc;
        deserializeJson(doc, response);

        if (doc["success"].as<bool>())
        {
            accessToken = doc["result"]["access_token"].as<String>();
            refreshToken = doc["result"]["refresh_token"].as<String>();
            expiresIn = millis() + doc["result"]["expire_time"].as<unsigned long>() * 1000;

            // Serial.println("Access token obtained successfully");
            // Serial.println("Access Token: " + accessToken);
            // Serial.println("Refresh Token: " + refreshToken);
            // Serial.println("Expires In: " + String(expiresIn));
        }
        else
        {
            Serial.println("Failed to refresh access token");
            Serial.println("Error: " + doc["msg"].as<String>());
        }
    }
    else
    {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}

void devicesUpdate()
{
    // JsonArray devices = doc.to<JsonArray>();
    JsonDocument devices = getDevicesList();
    String devicesString = devices.as<String>();
    // Serial.println("DevicesString: " + devicesString);
    handleDevices(devices);
}

JsonDocument getDevicesList()
{
    String timestamp = String(getTimestamp());
    String nonce = String(random(0xffff), HEX);

    String signature = calculateSignature(TUYA_CLIENT_ID, accessToken, timestamp, nonce, "/v2.0/cloud/thing/device?page_size=20", "", TUYA_CLIENT_SECRET);

    HTTPClient http;
    http.begin(String(baseURL) + "v2.0/cloud/thing/device?page_size=20");

    http.addHeader("client_id", TUYA_CLIENT_ID);
    http.addHeader("access_token", accessToken);
    http.addHeader("sign", signature);
    http.addHeader("t", timestamp);
    http.addHeader("sign_method", "HMAC-SHA256");
    http.addHeader("nonce", nonce);

    int httpResponseCode = http.GET();

    String response = "";
    JsonDocument doc;
    if (httpResponseCode > 0)
    {
        response = http.getString();

        http.end();

        deserializeJson(doc, response);

        return doc;
    }
    else
    {
        http.end();
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);

        return doc;
    }
}

void handleDevices(JsonDocument &devices)
{
    deviceIndex = 0;

    JsonArray deviceList = devices["result"].as<JsonArray>();
    if (!deviceList.isNull() && deviceList.size() > 0)
    {
        for (JsonVariant device : deviceList)
        {
            // String deviceName = device["name"].as<String>();
            // String deviceModel = device["model"].as<String>();
            // String deviceCategory = device["category"].as<String>();
            String deviceId = device["id"].as<String>();
            // bool isOnline = device["isOnline"].as<bool>();

            // Serial.println("Device: " + deviceName);
            // Serial.println("Model: " + deviceModel);
            // Serial.println("Category: " + deviceCategory);
            // Serial.println("Status: " + String(isOnline ? "Online" : "Offline"));
            // Serial.println("ID: " + deviceId);
            // Serial.println("---");

            // Get device properties
            getDeviceProperties(deviceId, device);
        }
    }

    Serial.println("Updated all devices.");
}

void getDeviceProperties(String deviceId, JsonVariant device)
{
    String timestamp = String(getTimestamp());
    // String nonce = String(random(0xffff), HEX);

    String signature = calculateSignature(TUYA_CLIENT_ID, accessToken, timestamp, "", "/v2.0/cloud/thing/" + deviceId + "/shadow/properties", "", TUYA_CLIENT_SECRET);

    HTTPClient http;
    http.begin(String(baseURL) + "v2.0/cloud/thing/" + deviceId + "/shadow/properties");

    http.addHeader("client_id", TUYA_CLIENT_ID);
    http.addHeader("sign", signature);
    http.addHeader("t", timestamp);
    http.addHeader("access_token", accessToken);
    http.addHeader("sign_method", "HMAC-SHA256");

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String response = http.getString();
        // Serial.println("Response: " + response);

        JsonDocument doc;
        deserializeJson(doc, response);

        JsonArray properties = doc["result"]["properties"];
        for (JsonVariant prop : properties)
        {
            String code = prop["code"].as<String>();
            String value = prop["value"].as<String>();

            // Serial.print(code + ": ");
            // Serial.println(value);
        }

        updateDevicesDisplay(device, properties);

        Serial.println("Updated device " + deviceId + " successfully.");
    }
    else
    {
        Serial.print("Error on HTTP request: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}