#ifndef KOSTER_COMMON_PROGRAM_HISTORY_H
#define KOSTER_COMMON_PROGRAM_HISTORY_H

#include <stdint.h>

#define PROGRAM_HISTORY_GRAPH_MAX_PTS_V1 1200
#define PROGRAM_HISTORY_TYPE_LEN_V1 10
#define PROGRAM_HISTORY_PYRO_ON_TIMERS_V1 2
#define PROGRAM_HISTORY_PYRO_OFF_TIMERS_V1 3
#define PROGRAM_HISTORY_IMG_DATA_SIZE_V1 32 * 24
#define PROGRAM_HISTORY_MAX_PARAMS_V1 128
#define PROGRAM_HISTORY_MAX_ALARMS_V1 16
#define PROGRAM_HISTORY_RECIPE_NAME_LEN_V1 128
#define PROGRAM_HISTORY_USER_ID_LEN_V1 64

typedef enum {
    kProgramRecipeIR = 0,
    kProgramRecipeUV = 1,
    kProgramRecipeIRUV = 2,
    kProgramRecipe3StepIR = 3,
    kProgramRecipeUVLED = 4,
} program_recipe_type_t;

struct program_recipe_v1_t {
    program_recipe_type_t type;
    uint16_t pyro_off_time[PROGRAM_HISTORY_PYRO_ON_TIMERS_V1];  // pyro-off time (seconds)
    uint8_t pyro_off_power[PROGRAM_HISTORY_PYRO_ON_TIMERS_V1];  // pyro-off power (percent)
    uint16_t pyro_on_time[PROGRAM_HISTORY_PYRO_OFF_TIMERS_V1];  // pyro-on time (seconds)
    uint8_t pyro_on_rise[PROGRAM_HISTORY_PYRO_OFF_TIMERS_V1];   // pyro-on temperature rise (celsius per minute)
    uint16_t pyro_on_temp[PROGRAM_HISTORY_PYRO_OFF_TIMERS_V1];  // pyro-on end temperature (celsius)
    uint16_t uv_time;                                           // UV time (seconds)
};

struct program_graphs_v1_t {
    uint16_t n_pts;                                         // Number of points in the graph
    uint16_t time[PROGRAM_HISTORY_GRAPH_MAX_PTS_V1];        // Time since start (65536 s = 18.h h max)
    int16_t temperature[PROGRAM_HISTORY_GRAPH_MAX_PTS_V1];  // Temperature in C
    uint8_t power[PROGRAM_HISTORY_GRAPH_MAX_PTS_V1];        // Power in %
    uint8_t distance[PROGRAM_HISTORY_GRAPH_MAX_PTS_V1];     // Distance in cm
};

struct program_header_v1_t {
    uint32_t run_id;
    uint32_t start_time;  // Unix epoch in seconds
    char recipe_name[PROGRAM_HISTORY_RECIPE_NAME_LEN_V1];
};

typedef enum {
    kTerminationUnknown = 0,
    kTerminationFinished = 1,
    kTerminationUser = 2,
    kTerminationAlarm = 3
} program_termination_t;

struct program_data_v1_t {
    uint32_t energy;                 // Energy in Watt-seconds
    uint8_t cassettes;               // Bitmap off enabled cassettes (1 = enabled, 0 = disabled)
    uint8_t temperature_control_on;  // 0 = off, otherwise on
    program_termination_t termination;
    char user_id[PROGRAM_HISTORY_USER_ID_LEN_V1];
    struct program_recipe_v1_t recipe;
    struct program_graphs_v1_t graphs;
    int16_t img[PROGRAM_HISTORY_IMG_DATA_SIZE_V1];
    uint8_t param_ids[PROGRAM_HISTORY_MAX_PARAMS_V1];
    int32_t param_vals[PROGRAM_HISTORY_MAX_PARAMS_V1];
    uint8_t alarm_ids[PROGRAM_HISTORY_MAX_ALARMS_V1];
};

struct program_header_t {
    uint8_t version;
    union {
        struct program_header_v1_t v1;
    };
};

struct program_data_t {
    uint8_t version;
    union {
        struct program_data_v1_t v1;
    };
};

struct program_history_t {
    struct program_header_t header;
    struct program_data_t data;
};

#endif
