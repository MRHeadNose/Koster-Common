#ifndef KOSTER_COMMON_PARAMETERS_PRIVATE_H
#define KOSTER_COMMON_PARAMETERS_PRIVATE_H

struct param_t {
    int id;
    char name[PARAM_NAME_MAX_LEN];
    int type;
    int access;
    char description[PARAM_DESC_MAX_LEN];
    int32_t value;
    int32_t min;
    int32_t max;
};

struct param_category_t {
    int id;
    char name[PARAM_CATEGORY_NAME_MAX_LEN];
    int n_params;
    struct param_t* params[PARAM_MAX_NUM_PARAMS_IN_CATEGORY];
};

#endif
