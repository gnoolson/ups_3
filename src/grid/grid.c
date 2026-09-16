#include "grid.h"
#include "../serial/serial.h"

grid_t* grid_new_and_setup(settings_t* p_settings, sensor_220v_t* p_sensor) {
    grid_t* p_grid = (grid_t*)malloc(sizeof(grid_t));
    p_grid->p_sensor = p_sensor;
    p_grid->p_settings = p_settings;

    return p_grid;
}

void grid_delete(grid_t* p_grid) {
    free(p_grid);
}

bool grid_has_voltage(grid_t* p_grid) {
    return sensor_220v_has_voltage(p_grid->p_sensor);
}
