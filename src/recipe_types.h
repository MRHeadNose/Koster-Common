#ifndef KOSTER_COMMON_RECIPE_TYPES_H
#define KOSTER_COMMON_RECIPE_TYPES_H

#include <stdint.h>

#include "koster-common/recipe.h"

#define RECIPE_NAME_MAX_STORAGE_SIZE RECIPE_NAME_MAX_SIZE + 1

struct recipe_t {
    uint8_t id;
    char name[RECIPE_NAME_MAX_STORAGE_SIZE];
    recipe_type_t type;
    uint16_t pyro_off_time[RECIPE_MAX_PYRO_OFF_TIMERS];  // pyro-off time (seconds)
    uint8_t pyro_off_power[RECIPE_MAX_PYRO_OFF_TIMERS];  // pyro-off power (percent)
    uint16_t pyro_on_time[RECIPE_MAX_PYRO_ON_TIMERS];    // pyro-on time (seconds)
    uint8_t pyro_on_rise[RECIPE_MAX_PYRO_ON_TIMERS];     // pyro-on temperature rise (celsius per minute)
    uint16_t pyro_on_temp[RECIPE_MAX_PYRO_ON_TIMERS];    // pyro-on end temperature (celsius)
    uint16_t uv_time;                                    // UV time (seconds)
};

struct recipes_t {
    uint8_t n_recipes;
    struct recipe_t recipes[RECIPE_MAX_RECIPES];
};

#endif
