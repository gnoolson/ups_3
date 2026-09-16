#ifndef GRID_H
#define GRID_H

#include "../sensor/sensor_220v.h"
#include "../settings/settings.h"

typedef struct {
	settings_t* p_settings;
	sensor_220v_t* p_sensor;
} grid_t;

grid_t* grid_new_and_setup(settings_t* p_settings, sensor_220v_t* p_sensor);
void grid_delete(grid_t* p_grid);
bool grid_has_voltage(grid_t* p_grid);

#endif