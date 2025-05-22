#ifndef KOSTER_COMMON_DEFAULT_RECIPES_H
#define KOSTER_COMMON_DEFAULT_RECIPES_H

#include <stddef.h>
#include <stdint.h>

struct recipes_t;

/**
 * Get default recipes
 *
 * @param recipes the recipes struct to fill with default recipes
 * @return 0 on success, -1 on failure.
 */
int DefaultRecipesGet(struct recipes_t* recipes);

#endif
