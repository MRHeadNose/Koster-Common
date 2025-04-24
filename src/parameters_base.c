#include <string.h>
#include <zephyr/kernel.h>

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
            ret_val = param->value;
        }
        k_mutex_unlock(&param_mutex);
    }
    return ret_val;
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

int ParamCategoryGetParam(const struct param_category_t* category, struct param_t** param, unsigned int index) {
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
