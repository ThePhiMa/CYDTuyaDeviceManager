#include "ui.h"

// UI initialisation
void setup_ui()
{
    main_screen = lv_scr_act();
    // Set the background color to black
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Create a container for the grid
    lv_obj_t *cont = lv_obj_create(main_screen);
    lv_obj_set_size(cont, lv_pct(100), lv_pct(100));
    lv_obj_set_style_bg_color(cont, lv_color_make(10, 10, 10), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    // Create Time widget
    lv_obj_t *time_box = lv_obj_create(cont);
    lv_obj_set_size(time_box, lv_pct(48), lv_pct(30));
    lv_obj_set_style_border_width(time_box, 2, 0);
    lv_obj_set_style_bg_color(time_box, lv_color_make(32, 32, 32), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(time_box, LV_ALIGN_TOP_LEFT, 5, 5);
    lv_obj_set_scrollbar_mode(time_box, LV_SCROLLBAR_MODE_OFF);
    time_label = lv_label_create(time_box);

    lv_label_set_text(time_label, "00:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(time_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_center(time_label);

    // Create Weather widget
    lv_obj_t *weather_box = lv_obj_create(cont);
    lv_obj_set_size(weather_box, lv_pct(48), lv_pct(30));
    lv_obj_set_style_border_width(weather_box, 2, 0);
    lv_obj_set_style_pad_top(weather_box, 8, 0);
    lv_obj_set_style_bg_color(weather_box, lv_color_make(32, 32, 32), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(weather_box, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_set_scrollbar_mode(weather_box, LV_SCROLLBAR_MODE_OFF);

    weather_label = lv_label_create(weather_box);
    lv_label_set_text(weather_label, "25°C\n50%");
    lv_obj_set_style_text_font(weather_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(weather_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_align(weather_label, LV_ALIGN_TOP_MID, 0, 0);

    // Create Device widgets
    for (int i = 0; i < 9; i++)
    {
        lv_obj_t *device_box = lv_obj_create(cont);
        lv_obj_set_size(device_box, lv_pct(31), lv_pct(20));
        lv_obj_set_style_border_width(device_box, 2, 0);
        lv_obj_set_style_bg_color(device_box, lv_color_make(32, 32, 32), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_all(device_box, 2, 0);
        lv_obj_set_scrollbar_mode(device_box, LV_SCROLLBAR_MODE_OFF);

        device_title[i] = lv_label_create(device_box);
        lv_obj_set_style_text_font(device_title[i], &lv_font_montserrat_8, 0);
        lv_obj_set_style_text_color(device_title[i], lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(device_title[i], "Device %d", i + 1);
        lv_obj_align(device_title[i], LV_ALIGN_TOP_LEFT, 10, 0);

        device_status_indicator[i] = lv_obj_create(device_box);
        lv_obj_set_size(device_status_indicator[i], 8, 33);
        lv_obj_set_style_radius(device_status_indicator[i], LV_RADIUS_CIRCLE, 0);

        device_labels[i] = lv_label_create(device_box);
        lv_obj_set_style_text_font(device_labels[i], &lv_font_montserrat_8, 0);
        lv_obj_set_style_text_color(device_labels[i], lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(device_labels[i], "Attribute 1\nAttribute 2");
        lv_obj_align(device_labels[i], LV_ALIGN_TOP_LEFT, 10, 12);
    }
}

void update_time(const char *time_str)
{
    lv_label_set_text(time_label, time_str);
}

void update_weather(const char *temp, const char *humidity)
{
    float temperature;
    char formatted_temp[10]; // Buffer for formatted temperature

    // Check if the temperature already contains a decimal point
    if (strchr(temp, '.') != NULL)
    {
        temperature = atof(temp);
    }
    else
    {
        temperature = atof(temp) / 10.0;
    }

    snprintf(formatted_temp, sizeof(formatted_temp), "%.1f°C", temperature);

    lv_label_set_text_fmt(weather_label, "%s\n%s%%", formatted_temp, humidity);
}

void update_device(int device_index, bool is_online, const char *name, const char *attr1, const char *attr2)
{
    if (device_index >= 0 && device_index < 9)
    {
        lv_label_set_text_fmt(device_title[device_index], "%s", name);
        lv_label_set_text_fmt(device_labels[device_index], "%s\n%s", attr1, attr2);
        lv_obj_set_style_bg_color(device_status_indicator[device_index], is_online ? lv_palette_main(LV_PALETTE_GREEN) : lv_palette_main(LV_PALETTE_RED), 0);
    }
}

void updateWeatherDisplay(String currentTemp, String currentHumidity)
{
    Serial.println("Weather updated.");
    update_weather(currentTemp.c_str(), currentHumidity.c_str());
}

void updateTimeDisplay()
{
    struct tm timeinfo;
    char time_str[9]; // HH:MM:SS\0

    getLocalTime(&timeinfo);

    strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);

    update_time(time_str);
}

void updateDevicesDisplay(JsonVariant device, JsonVariant properties)
{
    // I could do this dynamically, but doing this half-manually is good enough for my use-case for now. ;-)

    String deviceModel = device["model"].as<String>();

    String name;
    String property1;
    String property2;

    if (deviceModel.equals(tuyaTempSensorName))
    {
        // 1: temp_current, 2: humidity_value
        updateWeatherDisplay(properties[0]["value"].as<String>(), properties[1]["value"].as<String>());
    }
    else if (deviceModel.equals(tuyaDoorSensorName))
    {
        // 1: isOnline, 2: battery_percentage
        bool isOnline = device["isOnline"].as<bool>();
        property1 = isOnline ? String("Online") : String("Offline");
        property2 = String("Battery: ") + properties[1]["value"].as<String>() + String("%");
        name = device["customName"].as<String>();
        if (name.equals(""))
            name = device["name"].as<String>();

        update_device(deviceIndex++,
                      !(properties[0]["value"].as<bool>()), // Attr: doorcontact_state
                      name.c_str(),
                      property1.c_str(),
                      property2.c_str());
    }
    else if (deviceModel.equals(teckingSmartPlugName))
    {
        // 1: isOnline, 2: curr_power
        bool isOnline = device["isOnline"].as<bool>();
        property1 = isOnline ? String("Online") : String("Offline");
        property2 = properties[4]["value"].as<String>() + String("W");
        name = device["customName"].as<String>();
        if (name.equals(""))
            name = device["name"].as<String>();

        update_device(deviceIndex++,
                      properties[0]["value"].as<bool>(), // Attr: swtich_1
                      name.c_str(),
                      property1.c_str(),
                      property2.c_str());
    }
}