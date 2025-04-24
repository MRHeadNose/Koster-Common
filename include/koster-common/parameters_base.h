#ifndef KOSTER_COMMON_PARAMETERS_BASE_H
#define KOSTER_COMMON_PARAMETERS_BASE_H

#include <stdint.h>

struct param_t;
struct param_category_t;

/**
 * Initialize Parameters and read from persistent storage
 *
 * @return 0 on success, -1 on failure.
 */
int ParamInit();

/**
 * Get a parameter by id
 *
 * @return -1 on failure, 0 on success
 * @param param  pointer to the parameter pointer that will be filled when loading.
 * @param id     the ID of the parameter to load;
 */
int ParamGet(struct param_t** param, const unsigned int id);

/**
 * Get parameter value
 *
 * @return the value of the parameter
 * @param param  pointer to the parameter
 */
int32_t ParamGetValue(const struct param_t* param);

/**
 * Get name of parameter
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 * @param buf    will be filled with the parameter name
 */
int ParamGetName(const struct param_t* param, char* buf);

/**
 * Get a parameter category by alphabetical index
 *
 * @return -1 on failure, 0 on success
 * @param param  pointer to the category pointer that will be filled when loading.
 * @param index  the alphabetical index
 */
int ParamGetCategory(struct param_category_t** category, const unsigned int index);

/**
 * Get name of category
 *
 * @return 0 on success, -1 on failure
 * @param category  pointer to the category
 * @param buf       will be filled with the parameter name
 */
int ParamGetCategoryName(const struct param_category_t* category, char* buf);

/**
 * Get number of parameters in category
 *
 * @return the number of parameters in category
 * @param category  pointer to the category
 */
unsigned int ParamCategoryGetNParams(const struct param_category_t* category);

/**
 * Get a parameter from a category by alphabetical index
 *
 * @return -1 on failure, 0 on success
 * @param category  pointer to the category
 * @param param     pointer to the parameter pointer that will be filled.
 * @param index     the alphabetical index
 */
int ParamCategoryGetParam(const struct param_category_t* category, struct param_t** param, const unsigned int index);

/**
 * Get string representation of the current parameter value
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 * @param buf    will be filled with the string representation of the current parameter value
 */
int ParamGetCurrentValueString(struct param_t* param, char* buf);

#endif
