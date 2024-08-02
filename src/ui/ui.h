#ifndef UI_H
#define UI_H

#include <lvgl.h>
#include <lv_conf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ArduinoJson-v7.1.0.h"
#include "globals.h"

// Tuya cloud sensor names
extern const char *tuyaTempSensorName;
extern const char *tuyaDoorSensorName;
extern const char *teckingSmartPlugName;

static lv_obj_t *main_screen;
static lv_obj_t *time_label;
static lv_obj_t *weather_label;
static lv_obj_t *device_title[9];
static lv_obj_t *device_labels[9];
static lv_obj_t *device_status_indicator[9];

#if LV_COLOR_DEPTH != 16
#error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP != 1
#error "LV_COLOR_16_SWAP should be 1 to match SquareLine Studio's settings"
#endif

void setup_ui();
void update_time(const char *time_str);
void update_weather(const char *temp, const char *humidity);
void update_device(int device_index, bool is_online, const char *name, const char *attr1, const char *attr2);

void updateTimeDisplay();
void updateDevicesDisplay(JsonVariant device, JsonVariant properties);
void updateWeatherDisplay(String currentTemp, String currentHumidity);

#endif