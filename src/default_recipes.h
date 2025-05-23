#ifndef KOSTER_COMMON_DEFAULT_RECIPES_H
#define KOSTER_COMMON_DEFAULT_RECIPES_H

#include <stddef.h>
#include <stdint.h>

#include "recipe_types.h"

/**
 * Get default recipes
 *
 * @param recipes the recipes struct to fill with default recipes
 */
void DefaultRecipesGet(struct recipes_t* recipes);

#endif
