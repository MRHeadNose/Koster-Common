#include "default_recipes.h"

#include "default_recipes_generated.h"
#include "parameters.h"
#include "recipe_types.h"

LOG_MODULE_DECLARE(koster_common);

extern struct recipes_t default_recipes;

int DefaultRecipesGet(struct recipes_t* recipes) {
    int lang = ParamGetLanguage();

    switch (lang) {
        case kSwedish:
            return DefaultRecipesGetSE(recipes);
        case kEnglish:
            return DefaultRecipesGetEN(recipes);
        default:
            LOG_ERR("[default_recipes] Uknown language (%i). Loading English recipes.", lang);
            return DefaultRecipesGetEN(recipes);
    }

    recipes->n_recipes = default_recipes.n_recipes;
    for (int i = 0; i < default_recipes.n_recipes; ++i) {
        recipes->recipes[i].id = default_recipes.recipes[i].id;
        recipes->recipes[i].type = default_recipes.recipes[i].type;
        for (int k; k < RECIPE_MAX_PYRO_ON_TIMERS; ++k) {
            recipes->recipes[i].pyro_on_time[k] = default_recipes.recipes[i].pyro_on_time[k];
            recipes->recipes[i].pyro_on_rise[k] = default_recipes.recipes[i].pyro_on_rise[k];
            recipes->recipes[i].pyro_on_temp[k] = default_recipes.recipes[i].pyro_on_temp[k];
        }
        for (int k; k < RECIPE_MAX_PYRO_OFF_TIMERS; ++k) {
            recipes->recipes[i].pyro_off_time[k] = default_recipes.recipes[i].pyro_off_time[k];
            recipes->recipes[i].pyro_off_power[k] = default_recipes.recipes[i].pyro_off_power[k];
        }
        recipes->recipes[i].uv_time = default_recipes.recipes[i].uv_time;
    }

    return 0;
}
