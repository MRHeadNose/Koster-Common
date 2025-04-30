#ifndef KOSTER_COMMON_PARAMETERS_PRIVATE_H
#define KOSTER_COMMON_PARAMETERS_PRIVATE_H

#include <stdint.h>

typedef enum { kParamTypeEnum, kParamTypeNumeric } param_type_t;

struct param_t {
    const int id;
    const char* const name;
    const param_type_t type;
    const int access;
    const char* const description;
    int32_t* const value;
    const int32_t min;
    const int32_t max;
    const int exponent;
};

struct param_category_t {
    const int id;
    const char* const name;
    const int n_params;
    const struct param_t* const params[PARAM_MAX_NUM_PARAMS_IN_CATEGORY];
};

#endif
