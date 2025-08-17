#ifndef KOSTER_COMMON_RECIPE_H
#define KOSTER_COMMON_RECIPE_H

#include <stddef.h>
#include <stdint.h>

// Support at least 31 UTF-8 characters
#define RECIPE_NAME_MAX_SIZE 127
// Maximum number of recipes stored
#define RECIPE_MAX_RECIPES 32
// Maximum number of pyro-on timers
#define RECIPE_MAX_PYRO_ON_TIMERS 3
// Maximum number of pyro-off timers
#define RECIPE_MAX_PYRO_OFF_TIMERS 2

typedef enum {
    kRecipeNone,
    kRecipeIR,
    kRecipeUVIR,
    kRecipe3StepIR,
    kRecipeUV,
    kRecipeUVLED,
    kRecipeNTypes
} recipe_type_t;
// Ordered recipe type string for use in LVGL rollers
#define RECIPE_TYPES_STRING "Select type\nIR\nIR & UV\n3 Step IR\nUV\nUV LED"

struct program_step {
    uint16_t time;         // Time in seconds to hold power or temperature
    uint16_t uv_time;      // UV time in seconds, 0 if unused
    uint16_t target_temp;  // Target for ramping or holding temperature, 0 means fixed power is used
    union {
        uint8_t power;      // Power level in percent when using fixed power
        uint8_t temp_rise;  // Temperature rise in Celsius per minute used for ramping
    };
};

struct recipe_t;

/**
 * Initialize Recipe and read from persistent storage
 *
 * @return 0 on success, -1 on failure.
 */
int RecipeInit();

/**
 * Create a new recipe
 *
 * @param recipe pointer to the recipe pointer that will be filled when creating a new recipe.
 * @return -1 on failure, 0 on success
 */
int RecipeNew(struct recipe_t **recipe);

/**
 * Delete a recipe
 *
 * @param recipe pointer to the recipe to delete
 * @return -1 on failure, 0 on success
 */
int RecipeDelete(struct recipe_t *recipe);

/**
 * Get a recipe by id
 *
 * @param recipe pointer to the recipe pointer that will be filled when loading.
 * @param id     the ID of the recipe to load;
 * @return -1 on failure, 0 on success
 */
int RecipeGet(struct recipe_t **recipe, uint8_t id);

/**
 * Get the number of existing recipes.
 *
 * @return The number of existing recipes.
 */
uint8_t RecipeGetNumRecipes();

/**
 * Save all recipes to persistent storage
 *
 * @return -1 on failure, 0 on success
 */
int RecipePersistAll();

/**
 * Set recipe name
 *
 * @param recipe pointer to the recipe to modify
 * @param name null-terminated string with the new name. The size can be most RECIPE_NAME_MAX_SIZE including the null
 * character.
 * @return -1 on failure, 0 on success
 */
int RecipeSetName(struct recipe_t *recipe, const char *name);

/**
 * Set recipe type
 *
 * @param recipe pointer to the recipe to modify
 * @param type the new type
 * @return -1 on failure, 0 on success
 */
int RecipeSetType(struct recipe_t *recipe, recipe_type_t type);

/**
 * Set pyro-off time
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_PYRO_OFF_TIMERS
 * @param seconds      the new time in seconds
 * @return -1 on failure, 0 on success
 */
int RecipeSetPyroOffTime(struct recipe_t *recipe, uint8_t timer_number, uint16_t seconds);

/**
 * Set pyro-off power
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_PYRO_OFF_TIMERS
 * @param pct          the new power in percent
 * @return -1 on failure, 0 on success
 */
int RecipeSetPyroOffPower(struct recipe_t *recipe, uint8_t timer_number, uint8_t pct);

/**
 * Set pyro-on time
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @param seconds      the new time in seconds
 * @return -1 on failure, 0 on success
 */
int RecipeSetPyroOnTime(struct recipe_t *recipe, uint8_t timer_number, uint16_t seconds);

/**
 * Set pyro-on temperature rise
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @param c_per_min     the new rise time in celsius per minute
 * @return -1 on failure, 0 on success
 */
int RecipeSetPyroOnRise(struct recipe_t *recipe, uint8_t timer_number, uint8_t c_per_min);

/**
 * Set pyro-on end temperature
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @param celsius      the new end temperature in celsius
 * @return -1 on failure, 0 on success
 */
int RecipeSetPyroOnTemp(struct recipe_t *recipe, uint8_t timer_number, uint16_t celsius);

/**
 * Set UV timer
 *
 * @param recipe  pointer to the recipe to modify
 * @param seconds the new time in seconds
 * @return -1 on failure, 0 on success
 */
int RecipeSetUVTime(struct recipe_t *recipe, uint16_t seconds);

/**
 * Get recipe name
 *
 * @param recipe pointer to the recipe
 * @param buf    the name will be copied to this
 * @param bufsz  the size of the buffer
 * @return -1 on failure, 0 on success
 */
int RecipeGetName(const struct recipe_t *recipe, char *buf, size_t bufsz);

/**
 * Get recipe type
 *
 * @param recipe pointer to the recipe
 * @return the recipe type. Will return kRecipeNtypes if recipe is NULL.
 */
recipe_type_t RecipeGetType(const struct recipe_t *recipe);

/**
 * Get pyro-off time
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_PYRO_OFF_TIMERS
 * @return the time in seconds
 */
uint16_t RecipeGetPyroOffTime(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get pyro-off power
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_PYRO_OFF_TIMERS
 * @return the power in percent
 */
uint8_t RecipeGetPyroOffPower(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get pyro-on time
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @return the time in seconds
 */
uint16_t RecipeGetPyroOnTime(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get pyro-on temperature rise
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @return the rise time in celsius per minute
 */
uint8_t RecipeGetPyroOnRise(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get pyro-on end temperature
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_PYRO_ON_TIMERS
 * @return the end temperature in celsius
 */
uint16_t RecipeGetPyroOnTemp(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get UV timer
 *
 * @param recipe  pointer to the recipe
 * @return the time in seconds
 */
uint16_t RecipeGetUVTime(const struct recipe_t *recipe);


void RecipePrintAll();

#endif
