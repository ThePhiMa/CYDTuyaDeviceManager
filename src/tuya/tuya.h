#ifndef TUYA_H
#define TUYA_H

#include <HTTPClient.h>
#include <EEPROM.h>
#include "ArduinoJson-v7.1.0.h"
#include "ui/ui.h"

// Tuya API details
extern const char *baseURL;
extern const char *clientId;
extern const char *clientSecret;
extern const char *emptyStringSHA256;

// EEPROM addresses for storing tokens
const int ACCESS_TOKEN_ADDR = 0;
const int REFRESH_TOKEN_ADDR = 100;
const int EXPIRES_IN_ADDR = 200;

extern String accessToken;
extern String refreshToken;
extern unsigned long expiresIn;

// Function declarations
String toUpperCase(String str);
void loadTokens();
void saveTokens();
void getAccessToken();
void refreshAccessToken();
void devicesUpdate();
JsonDocument getDevicesList();
void handleDevices(JsonDocument &devices);
void getDeviceProperties(String deviceId, JsonVariant device);
String getTimestamp();
String calculateSignature(const String &clientId, const String &accessToken, const String &timestamp, const String &nonce, const String &signUrl, const String &body, const String &clientSecret);
String calculateHash(const String &payload);
String calculateHmacSha256(const String &message, const String &key);

#endif