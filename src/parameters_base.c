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

int ParamGetId(const struct param_t* param) {
    int ret_val = 0;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            ret_val = param->id;
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

int ParamSetValue(const struct param_t* param, const int32_t value) {
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

int ParamCategoryGetNParams(const struct param_category_t* category, const unsigned int access_level) {
    if (access_level >= PARAM_ACCESS_LEVELS) {
        return -EINVAL;
    }
    int ret_val = -ENOENT;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            ret_val = category->n_params[access_level];
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int ParamCategoryGetTotalNParams(const struct param_category_t* category) {
    int ret_val = -ENOENT;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            // Highest access level contains all parameters
            ret_val = category->n_params[PARAM_ACCESS_LEVELS - 1];
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
}

int ParamCategoryGetParam(const struct param_category_t* category,
                          const struct param_t** param,
                          const unsigned int index) {
    int rc = -EINVAL;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (category != NULL) {
            // Highest access level contains all parameters
            if (index < category->n_params[PARAM_ACCESS_LEVELS - 1]) {
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

int ParamGetAccess(const struct param_t* param) {
    int access = -1;
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        if (param != NULL) {
            access = param->access;
        }
        k_mutex_unlock(&param_mutex);
    }
    return access;
}

int ParamGetCurrentValueString(const struct param_t* param, char* buf) {
    if (k_mutex_lock(&param_mutex, K_FOREVER) == 0) {
        return ParamGetValueString(param, buf, *param->value);
        k_mutex_unlock(&param_mutex);
    }
    return -1;
}

int ParamCategoryWalk(param_category_walk_cb_t cb, const unsigned int access_level, void* arg) {
    if (access_level >= PARAM_ACCESS_LEVELS || cb == NULL) {
        return -EINVAL;
    }

    const struct param_category_t* category;
    for (unsigned int cat_i = 0; cat_i < PARAM_NUM_CATEGORIES; ++cat_i) {
        if (ParamGetCategory(&category, cat_i) != 0) {
            return -ENOENT;
        }

        if (ParamCategoryGetNParams(category, access_level) > 0) {
            if (cb(category, arg) < 0) {
                // Walk interrupted by callback return code
                return 0;
            }
        }
    }

    return 0;
}

int ParamWalk(param_walk_cb_t cb, const struct param_category_t* category, const unsigned int access_level, void* arg) {
    if (access_level >= PARAM_ACCESS_LEVELS || cb == NULL || category == NULL) {
        return -EINVAL;
    }

    const struct param_t* parameter;
    for (unsigned int par_i = 0; par_i < ParamCategoryGetNParams(category, access_level); ++par_i) {
        if (ParamCategoryGetParam(category, &parameter, par_i) != 0) {
            return -ENOENT;
        }

        if (cb(parameter, arg) < 0) {
            // Walk interrupted by callback return code
            return 0;
        }
    }

    return 0;
}
