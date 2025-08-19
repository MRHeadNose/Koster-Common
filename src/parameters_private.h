#ifndef KOSTER_COMMON_PARAMETERS_PRIVATE_H
#define KOSTER_COMMON_PARAMETERS_PRIVATE_H

#include <stdint.h>

typedef enum { kParamTypeEnum, kParamTypeNumeric } param_type_t;

struct param_t {
    int id;
    const char* name;
    param_type_t type;
    int access;
    const char* description;
    int32_t* value;
    int32_t min;
    int32_t max;
    int exponent;
};

struct param_category_t {
    int id;
    const char* name;
    unsigned int n_params[PARAM_ACCESS_LEVELS];  // number of parameter per access level
    const struct param_t* params[PARAM_MAX_NUM_PARAMS_IN_CATEGORY];
};

#endif
