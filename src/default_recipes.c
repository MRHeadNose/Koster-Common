#include "default_recipes.h"

#include "default_recipes_generated.h"
#include "koster-common/parameters.h"
#include "recipe_types.h"
#include "zephyr/logging/log.h"

LOG_MODULE_DECLARE(koster_common);

void DefaultRecipesGet(struct recipes_t* recipes) {
    int lang = ParamGetLanguage();

    switch (lang) {
        case kParamSwedish:
            DefaultRecipesSetSE();
            break;
        case kParamEnglish:
            DefaultRecipesSetEN();
            break;
        default:
            LOG_ERR("[default_recipes] Unkown language (%i). Loading English recipes.", lang);
            DefaultRecipesSetEN();
            break;
    }

    DefaultRecipesFill(recipes);
}
