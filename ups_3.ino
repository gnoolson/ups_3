#include <Arduino.h>
#include "config.h"

extern "C" {
#include <gnl_sob.h>
#include "src/activity/activity_manager.h"
#include "src/bms/bms.h"
#include "src/buzzer/buzzer.h"
#include "src/charger/charger.h"
#include "src/context/context.h"
#include "src/control_center/control_center.h"
#include "src/display/display.h"
#include "src/grid/grid.h"
#include "src/inverter/inverter.h"
#include "src/load/load.h"
#include "src/power_source_switch/power_source_switch.h"
#include "src/sensor/sensor_220v.h"
#include "src/settings/settings.h"
}

buzzer_t* p_buzzer = NULL;
gnl_sob_t* p_sob = NULL;
display_t* p_display = NULL;
context_t* p_context = NULL;
activity_manager_t* p_actmgr = NULL;
sensor_220v_t* p_grid_voltage_sensor = NULL;
sensor_220v_t* p_inverter_voltage_sensor = NULL;
charger_t* p_charger = NULL;
grid_t* p_grid = NULL;
settings_t* p_settings = NULL;
power_source_switch_t* p_power_source_switch = NULL;
inverter_t* p_inverter = NULL;
load_t* p_load = NULL;
control_center_t* p_cc = NULL;

bool recommendation_to_be_prepared(void* arg) {
    load_t* p_load = (load_t*)arg;
    return load_exists(p_load);
}

void setup() {
    Serial.begin(9600);

    {
        p_grid_voltage_sensor = sensor_220v_new_and_setup(CONFIG_SENSOR_220V_GRID_ALERT_PIN);
        sensor_220v_begin(p_grid_voltage_sensor);
        p_inverter_voltage_sensor = sensor_220v_new_and_setup(CONFIG_SENSOR_220V_INVERTER_ALERT_PIN);
        sensor_220v_begin(p_inverter_voltage_sensor);
    }
    {
        p_buzzer = buzzer_new_and_setup(CONFIG_BUZZER_PIN);
        buzzer_begin(p_buzzer);
    }

    {
        p_settings = settings_new();
        settings_begin(p_settings);
        settings_load(p_settings);
    }

    {  //
        bms_begin();
    }

    {  //
        p_context = context_new_and_setup(CONFIG_CONTEXT_LENGTH);
    }

    {
        p_sob = gnl_sob_new_and_setup(CONFIG_BUTTON_LENGTH, HIGH);
        gnl_sob_add_button(p_sob, CONFIG_BUTTON_UP_ID, CONFIG_BUTTON_UP_PIN);
        gnl_sob_add_button(p_sob, CONFIG_BUTTON_DOWN_ID, CONFIG_BUTTON_DOWN_PIN);
        gnl_sob_add_button(p_sob, CONFIG_BUTTON_ENTER_ID, CONFIG_BUTTON_ENTER_PIN);
        gnl_sob_add_button(p_sob, CONFIG_BUTTON_BACK_ID, CONFIG_BUTTON_BACK_PIN);
        gnl_sob_begin(p_sob, false);
    }

    {
        p_display = display_new_and_setup(0x27);
        display_begin(p_display);
    }

    {
        p_charger = charger_new_and_setup(CONFIG_CHARGER_LOAD_EXISTS_PIN, CONFIG_CHARGER_POWER_ON_PIN);
        charger_begin(p_charger);
    }

    {  //
        p_grid = grid_new_and_setup(p_settings, p_grid_voltage_sensor);
    }

    {
        p_power_source_switch = pss_new_and_setup(p_settings, p_buzzer, CONFIG_POWER_SOURCE_SWITCH_PIN);
        pss_begin(p_power_source_switch);
    }

    {
        p_load = load_new_and_setup(p_settings, CONFIG_LOAD_EXISTS_SENSOR_PIN);
        load_begin(p_load);
    }

    {
        p_inverter = inverter_new_and_setup(p_settings, p_inverter_voltage_sensor, CONFIG_INVERTER_POWER_ON_PIN, &recommendation_to_be_prepared, p_load);
        inverter_begin(p_inverter);
    }

    {  //
        p_cc = cc_new_and_setup(p_context);
    }

    context_set_object(p_context, CONFIG_CONTEXT_OBJ_DISPLAY, p_display);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_SETTINGS, p_settings);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_GRID, p_grid);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_CHARGER, p_charger);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_POWER_SOURCE_SWITCH, p_power_source_switch);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_CONTROL_CENTER, p_cc);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_INVERTER, p_inverter);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_LOAD, p_load);
    context_set_object(p_context, CONFIG_CONTEXT_OBJ_BUZZER, p_buzzer);

    {
        p_actmgr = actmgr_new_and_setup(p_context);
        actmgr_begin(p_actmgr);
    }

    Serial.println("UPS_3");
}

void callback(gnl_button_info_t** p_infos, uint8_t length, void* p_value) {
    activity_manager_t* p_actmgr = (activity_manager_t*)p_value;
    actmgr_process_buttons(p_actmgr, p_infos, length);
}

void loop() {
    bms_update();
    buzzer_upload(p_buzzer);
    gnl_sob_update(p_sob, &callback, p_actmgr);
    display_update(p_display);
    actmgr_update(p_actmgr);
    sensor_220v_update(p_grid_voltage_sensor);
    sensor_220v_update(p_inverter_voltage_sensor);
    charger_update(p_charger);
    load_update(p_load);
    pss_update(p_power_source_switch);
    inverter_update(p_inverter);
    cc_update(p_cc);
}