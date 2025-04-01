#ifndef KOSTER_COMMON_RECIPE_H
#define KOSTER_COMMON_RECIPE_H

#include <stdint.h>

// Support at least 32 UTF-8 characters
#define RECIPE_NAME_MAX_SIZE 128
// Maximum number of recipes stored
#define RECIPE_MAX_RECIPES 32

typedef enum { kRecipeIR, kRecipeUVIR, kRecipe3StepIR, kRecipeUV, kRecipeNTypes } recipe_type_t;

struct recipe_t {
    uint8_t id;
    char name[RECIPE_NAME_MAX_SIZE];
    recipe_type_t type;
    uint16_t flash_off_t1_time;  // Flash-off timer 1 time (seconds)
    uint8_t flash_off_t1_power;  // Flash-off timer 1 power (percent)
    uint16_t flash_off_t2_time;  // Flash-off timer 2 time (seconds)
    uint8_t flash_off_t2_power;  // Flash-off timer 2 power (percent)
    uint16_t hardening_t1_time;  // Flash-on timer 1 time (seconds)
    uint16_t hardening_t1_rise;  // Flash-on timer 1 temperature rise (tenths of celsius per minute)
    uint16_t hardening_t1_temp;  // Flash-on timer 1 end temperature (tenths of celsius)
    uint16_t hardening_t2_time;  // Flash-on timer 2 time (seconds)
    uint16_t hardening_t2_rise;  // Flash-on timer 2 temperature rise (tenths of celsius per minute)
    uint16_t hardening_t2_temp;  // Flash-on timer 2 end temperature (tenths of celsius)
    uint16_t hardening_t3_time;  // Flash-on timer 3 time (seconds)
    uint16_t hardening_t3_rise;  // Flash-on timer 3 temperature rise (tenths of celsius per minute)
    uint16_t hardening_t3_temp;  // Flash-on timer 3 end temperature (tenths of celsius)
    uint16_t uv_time;            // UV time (seconds)
};

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

#endif
