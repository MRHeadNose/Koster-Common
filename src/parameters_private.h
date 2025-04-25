#ifndef KOSTER_COMMON_PARAMETERS_PRIVATE_H
#define KOSTER_COMMON_PARAMETERS_PRIVATE_H

#include <stdint.h>

typedef enum { kParamTypeEnum, kParamTypeNumeric } param_type_t;

struct param_t {
    int id;
    char name[PARAM_NAME_MAX_LEN];
    param_type_t type;
    int access;
    char description[PARAM_DESC_MAX_LEN];
    int32_t value;
    int32_t min;
    int32_t max;
    int exponent;
};

struct param_category_t {
    int id;
    char name[PARAM_CATEGORY_NAME_MAX_LEN];
    int n_params;
    struct param_t* params[PARAM_MAX_NUM_PARAMS_IN_CATEGORY];
};

#endif
