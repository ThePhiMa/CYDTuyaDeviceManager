#include "tuya.h"

const char *baseURL = "https://openapi.tuyaeu.com/";

const char *emptyStringSHA256 = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

String accessToken;
String refreshToken;
unsigned long expiresIn;