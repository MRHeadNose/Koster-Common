#ifndef KOSTER_COMMON_RECIPE_H
#define KOSTER_COMMON_RECIPE_H

#include <stddef.h>
#include <stdint.h>

// Support at least 32 UTF-8 characters
#define RECIPE_NAME_MAX_SIZE 128
// Maximum number of recipes stored
#define RECIPE_MAX_RECIPES 32
// Maximum number of hardening (full bake) timers
#define RECIPE_MAX_HARDENING_TIMERS 3
// Maximum number of flash-off timers
#define RECIPE_MAX_FLASHOFF_TIMERS 2

typedef enum { kRecipeIR, kRecipeUVIR, kRecipe3StepIR, kRecipeUV, kRecipeNTypes } recipe_type_t;

struct recipe_t;

/**
 * Initialize Recipe and read from persistent storage
 *
 * @return 0 on success, -1 on failure.
 */
int RecipeInit();

/**
 * Get a recipe by id
 *
 * @param recipe pointer to the recipe struct that will be filled when loading.
 * @param id     the ID of the recipe to load;
 * @return -1 on failure, 0 on success
 */
int RecipeGet(struct recipe_t* recipe, uint8_t id);

/**
 * Get the number of existing recipes.
 *
 * Note: Also check that the return value is less than RECIPE_MAX_RECIPES before using it to save a new recipe.
 *
 * @return The number of existing recipes. This is also the next available recipe_t.id
 */
uint8_t RecipeGetNRecipes();

/**
 * Save a recipe. If recipe.id exists, the recipe will be overwritten. If recipe.id is the next unused value, a new
 * recipe will be created. In other cases, the function will return error.
 *
 * @param recipe pointer to the recipe struct that will be filled when loading
 * @param id     the ID of the recipe to load
 * @return -1 on failure, 0 on success
 */
int RecipeSave(const struct recipe_t* recipe);

/**
 * Save all recipes to persistent storage
 *
 * @return -1 on failure, 0 on success
 */
int RecipePersist();

/**
 * Set recipe name
 *
 * @param recipe pointer to the recipe to modify
 * @param name the new name
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
 * Set flash-off time
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_FLASHOFF_TIMERS
 * @param seconds      the new time in seconds
 * @return -1 on failure, 0 on success
 */
int RecipeSetFlashOffTime(struct recipe_t *recipe, uint8_t timer_number, uint16_t seconds);

/**
 * Set flash-off power
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_FLASHOFF_TIMERS
 * @param pct          the new power in percent
 * @return -1 on failure, 0 on success
 */
int RecipeSetFlashOffPower(struct recipe_t *recipe, uint8_t timer_number, uint8_t pct);

/**
 * Set hardening (full-bake) time
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @param seconds      the new time in seconds
 * @return -1 on failure, 0 on success
 */
int RecipeSetHardeningTime(struct recipe_t *recipe, uint8_t timer_number, uint16_t seconds);

/**
 * Set hardening (full-bake) temperature rise
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @param c_per_min     the new rise time in celsius per minute
 * @return -1 on failure, 0 on success
 */
int RecipeSetHardeningRise(struct recipe_t *recipe, uint8_t timer_number, uint8_t c_per_min);

/**
 * Set hardening (full-bake) end temperature
 *
 * @param recipe       pointer to the recipe to modify
 * @param timer_number the timer to modify. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @param celsius      the new end temperature in celsius
 * @return -1 on failure, 0 on success
 */
int RecipeSetHardeningTemp(struct recipe_t *recipe, uint8_t timer_number, uint16_t celsius);

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
 * Get flash-off time
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_FLASHOFF_TIMERS
 * @return the time in seconds
 */
uint16_t RecipeGetFlashOffTime(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get flash-off power
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_FLASHOFF_TIMERS
 * @return the power in percent
 */
uint8_t RecipeGetFlashOffPower(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get hardening (full-bake) time
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @return the time in seconds
 */
uint16_t RecipeGetHardeningTime(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get hardening (full-bake) temperature rise
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @return the rise time in celsius per minute
 */
uint8_t RecipeGetHardeningRise(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get hardening (full-bake) end temperature
 *
 * @param recipe       pointer to the recipe
 * @param timer_number the timer to get. Must be less than RECIPE_MAX_HARDENING_TIMERS
 * @return the end temperature in celsius
 */
uint16_t RecipeGetHardeningTemp(const struct recipe_t *recipe, uint8_t timer_number);

/**
 * Get UV timer
 *
 * @param recipe  pointer to the recipe
 * @return the time in seconds
 */
uint16_t RecipeGetUVTime(const struct recipe_t *recipe);

#endif
