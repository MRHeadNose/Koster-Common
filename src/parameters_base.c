#include "koster-common/parameters_base.h"

#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>

#include "koster-common/parameters.h"
#include "parameters_private.h"

struct k_mutex param_mutex;

int ParamGetName(const struct param_t* param, char* buf) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            strncpy(buf, param->name, PARAM_NAME_MAX_LEN);
            rc = 0;
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

int32_t ParamGetValue(const struct param_t* param) {
    int32_t ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            ret_val = *param->value;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int32_t ParamGetMinValue(const struct param_t* param) {
    int32_t ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            ret_val = param->min;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int32_t ParamGetMaxValue(const struct param_t* param) {
    int32_t ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            ret_val = param->max;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int ParamGetExponent(const struct param_t* param) {
    int ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            ret_val = param->exponent;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int ParamIncreaseValue(const struct param_t* param) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            if (*param->value == param->max) {
                *param->value = param->min;
            } else {
                ++(*param->value);
            }
            rc = 0;
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

int ParamDecreaseValue(const struct param_t* param) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            if (*param->value == param->min) {
                *param->value = param->max;
            } else {
                --(*param->value);
            }
            rc = 0;
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

int ParamSetValue(const struct param_t* param, const uint32_t value) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL && value >= param->min && value <= param->max) {
            *param->value = value;
            rc = 0;
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

bool ParamIsEnum(const struct param_t* param) {
    bool is_enum = false;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            is_enum = param->type == kParamTypeEnum;
        }
        k_mutex_unlock(&param_mutex);
    }
    return is_enum;
}

unsigned int ParamCategoryGetNParams(const struct param_category_t* category) {
    unsigned int ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            ret_val = category->n_params;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int ParamCategoryGetParam(const struct param_category_t* category,
                          const struct param_t** param,
                          const unsigned int index) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            if (index < category->n_params) {
                *param = category->params[index];
                rc = 0;
            }
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

int ParamGetCategoryName(const struct param_category_t* category, char* buf) {
    int rc = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            strncpy(buf, category->name, PARAM_CATEGORY_NAME_MAX_LEN);
            rc = 0;
        }
        k_mutex_unlock(&param_mutex);
    }
    return rc;
}

int ParamGetCurrentValueString(const struct param_t* param, char* buf) {
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        return ParamGetValueString(param, buf, *param->value);
        k_mutex_unlock(&param_mutex);
    }
    return -1;
}
