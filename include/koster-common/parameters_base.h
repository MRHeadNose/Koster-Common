#ifndef KOSTER_COMMON_PARAMETERS_BASE_H
#define KOSTER_COMMON_PARAMETERS_BASE_H

#include <stdbool.h>
#include <stdint.h>

struct param_t;
struct param_category_t;

// NOTE: Some of these functions are defined in generated code.

/**
 * Initialize Parameters and read from persistent storage
 *
 * @return 0 on success, -1 on failure.
 */
int ParamInit();

/**
 * Get parameter value
 *
 * @return the value of the parameter
 * @param param  pointer to the parameter
 */
int32_t ParamGetValue(const struct param_t* param);

/**
 * Get minimum parameter value
 *
 * @return the minimum value of the parameter
 * @param param  pointer to the parameter
 */
int32_t ParamGetMinValue(const struct param_t* param);

/**
 * Get maximum parameter value
 *
 * @return the maximum value of the parameter
 * @param param  pointer to the parameter
 */
int32_t ParamGetMaxValue(const struct param_t* param);

/**
 * Increase value of parameter
 *
 * The value will wrap-around when reaching the maximum value for the parameter
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 */
int ParamIncreaseValue(struct param_t* param);

/**
 * Decrease value of parameter
 *
 * The value will wrap-around when reaching the minium value for the parameter
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 */
int ParamDecreaseValue(struct param_t* param);

/**
 * Set parameter value
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 * @param value  the value to set
 */
int ParamSetValue(struct param_t* param, const uint32_t value);

/**
 * Check if parameter is an enum
 *
 * @return true if parameter is an enum, false otherwise
 * @param param  pointer to the parameter
 */
bool ParamIsEnum(struct param_t* param);

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

/**
 * Get string representation of the a parameter value
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 * @param buf    will be filled with the string representation of the current parameter value
 * @param value  the value to get the string represention of
 */
int ParamGetValueString(struct param_t* param, char* buf, const int32_t value);

/**
 * Get parameter exponent
 *
 * The display value of a parameter is value * 10^exponent
 *
 * @return the exponent
 * @param param  pointer to the parameter
 */
int ParamGetExponent(const struct param_t* param);

/**
 * Save a parameter to persistent storage
 *
 * @return 0 on success, -1 on failure
 * @param param  pointer to the parameter
 */
int ParamSave(struct param_t* param);

#endif
