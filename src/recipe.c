#include "koster-common/recipe.h"

#include <memory.h>
#include <stdbool.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>
#if defined(CONFIG_SETTINGS_FILE)
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#endif

LOG_MODULE_DECLARE(koster_common);

#define RECIPES_SETTING "recipe/recipes"
#define RECIPE_NAME_MAX_STORAGE_SIZE RECIPE_NAME_MAX_SIZE + 1

static struct settings_handler handler_;

struct recipe_t {
    uint8_t id;
    char name[RECIPE_NAME_MAX_STORAGE_SIZE];
    recipe_type_t type;
    uint16_t pyro_off_time[RECIPE_MAX_PYRO_OFF_TIMERS];  // pyro-off time (seconds)
    uint8_t pyro_off_power[RECIPE_MAX_PYRO_OFF_TIMERS];  // pyro-off power (percent)
    uint16_t pyro_on_time[RECIPE_MAX_PYRO_ON_TIMERS];    // pyro-on time (seconds)
    uint8_t pyro_on_rise[RECIPE_MAX_PYRO_ON_TIMERS];     // pyro-on temperature rise (celsius per minute)
    uint16_t pyro_on_temp[RECIPE_MAX_PYRO_ON_TIMERS];    // pyro-on end temperature (celsius)
    uint16_t uv_time;                                    // UV time (seconds)
};

struct recipes_t {
    uint8_t n_recipes;
    struct recipe_t recipes[RECIPE_MAX_RECIPES];
};

static const struct recipe_t kDefaultRecipe0 = {
        .id = 0,
        .name = "IR recipe",
        .type = kRecipeIR,
        .pyro_off_time = {240, 300},
        .pyro_off_power = {20, 70},
        .pyro_on_time = {240, 300, 0},
        .pyro_on_rise = {15, 25, 0},
        .pyro_on_temp = {80, 180, 0},
        .uv_time = 0,
};
static const struct recipe_t kDefaultRecipe1 = {
        .id = 1,
        .name = "3 step IR",
        .type = kRecipe3StepIR,
        .pyro_off_time = {0, 0},
        .pyro_off_power = {0, 0},
        .pyro_on_time = {240, 300, 120},
        .pyro_on_rise = {15, 25, 20},
        .pyro_on_temp = {80, 180, 220},
        .uv_time = 0,
};

struct k_mutex recipes_mutex;

static struct recipes_t recipes_;

static int handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;
    size_t name_len;

    name_len = settings_name_next(name, &next);

    if (!strncmp(name, "recipes", name_len)) {
        if (len != sizeof(struct recipes_t)) {
            LOG_ERR("[recipe] handle_set: read size different from size of recipes");
            return -EINVAL;
        }

        int rc = -EBUSY;
        if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
            rc = read_cb(cb_arg, &recipes_, sizeof(recipes_));
            k_mutex_unlock(&recipes_mutex);
            if (rc >= 0) {
                return 0;
            }
        }

        return rc;
    }

    return -ENOENT;
}

static int handle_export(int (*storage_func)(const char *name, const void *value, size_t val_len)) {
    int ret = -EBUSY;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        ret = storage_func(RECIPES_SETTING, &recipes_, sizeof(recipes_));
        k_mutex_unlock(&recipes_mutex);
    }
    return ret;
}

int RecipeInit() {
    k_mutex_init(&recipes_mutex);

    recipes_.n_recipes = 0;
    for (int i = 0; i < RECIPE_MAX_RECIPES; ++i) {
        memset(&recipes_.recipes[i], 0, sizeof(struct recipe_t));
    }

    handler_.name = "recipe";
    handler_.h_get = NULL;
    handler_.h_set = handle_set;
    handler_.h_commit = NULL;
    handler_.h_export = handle_export;

    int rc = settings_register(&handler_);
    if (rc != 0) {
        LOG_ERR("[recipe] settings_register failed (err %d)", rc);
        return -1;
    }

    rc = settings_load();
    if (rc != 0) {
        LOG_ERR("[recipe] settings_load failed (err %d)", rc);
        return -1;
    }

    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipes_.n_recipes == 0) {
            LOG_INF("[recipe] No recipes found. Adding default recipes.");
            recipes_.n_recipes = 2;
            recipes_.recipes[0] = kDefaultRecipe0;
            recipes_.recipes[1] = kDefaultRecipe1;
            RecipePersistAll();
        } else {
            LOG_INF("[recipe] Loaded %d recipes.", recipes_.n_recipes);
        }
        k_mutex_unlock(&recipes_mutex);
    }

    return 0;
}

int RecipeGet(struct recipe_t **recipe, const uint8_t id) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (id < recipes_.n_recipes) {
            *recipe = &recipes_.recipes[id];
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeNew(struct recipe_t **recipe) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipes_.n_recipes < RECIPE_MAX_RECIPES) {
            memset(&recipes_.recipes[recipes_.n_recipes], 0, sizeof(struct recipe_t));
            recipes_.recipes[recipes_.n_recipes].type = kRecipeIR;
            recipes_.recipes[recipes_.n_recipes].id = recipes_.n_recipes;

            *recipe = &recipes_.recipes[recipes_.n_recipes];
            ++recipes_.n_recipes;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeDelete(struct recipe_t *recipe) {
    int rc = -1;
    if (recipe == NULL) {
        return rc;
    }

    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        // zero all fields
        const uint8_t id = recipe->id;
        memset(&recipes_.recipes[id], 0, sizeof(struct recipe_t));

        // repack the struct
        for (int i = id; i < recipes_.n_recipes; ++i) {
            memmove(&recipes_.recipes[i], &recipes_.recipes[i + 1], sizeof(struct recipe_t));
            recipes_.recipes[i].id = i;
        }
        --recipes_.n_recipes;

        k_mutex_unlock(&recipes_mutex);
        rc = 0;
    }
    return rc;
}

uint8_t RecipeGetNumRecipes() {
    uint8_t n_recipes = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        n_recipes = recipes_.n_recipes;
        k_mutex_unlock(&recipes_mutex);
    }
    return n_recipes;
}

int RecipePersistAll() {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        rc = settings_save_one(RECIPES_SETTING, &recipes_, sizeof(recipes_));
        if (rc != 0) {
            LOG_ERR("[recipe] settings_save_one failed (err %d)", rc);
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetName(struct recipe_t *recipe, const char *name) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            strncpy(recipe->name, name, RECIPE_NAME_MAX_SIZE);
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetType(struct recipe_t *recipe, const recipe_type_t type) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && type >= 0 && type < kRecipeNTypes) {
            recipe->type = type;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetPyroOffTime(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t seconds) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_OFF_TIMERS) {
            recipe->pyro_off_time[timer_number] = seconds;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetPyroOffPower(struct recipe_t *recipe, const uint8_t timer_number, const uint8_t pct) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_OFF_TIMERS && pct <= 100) {
            recipe->pyro_off_power[timer_number] = pct;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetPyroOnTime(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t seconds) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            recipe->pyro_on_time[timer_number] = seconds;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetPyroOnRise(struct recipe_t *recipe, const uint8_t timer_number, const uint8_t c_per_min) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            recipe->pyro_on_rise[timer_number] = c_per_min;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetPyroOnTemp(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t celsius) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            recipe->pyro_on_temp[timer_number] = celsius;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeSetUVTime(struct recipe_t *recipe, const uint16_t seconds) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            recipe->uv_time = seconds;
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

int RecipeGetName(const struct recipe_t *recipe, char *buf, size_t bufsz) {
    int rc = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            bufsz = MIN(bufsz, RECIPE_NAME_MAX_SIZE);
            strncpy(buf, recipe->name, bufsz);
            rc = 0;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return rc;
}

recipe_type_t RecipeGetType(const struct recipe_t *recipe) {
    recipe_type_t ret_val = kRecipeNTypes;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            ret_val = recipe->type;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint16_t RecipeGetPyroOffTime(const struct recipe_t *recipe, const uint8_t timer_number) {
    uint16_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_OFF_TIMERS) {
            ret_val = recipe->pyro_off_time[timer_number];
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint8_t RecipeGetPyroOffPower(const struct recipe_t *recipe, const uint8_t timer_number) {
    uint8_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_OFF_TIMERS) {
            ret_val = recipe->pyro_off_power[timer_number];
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint16_t RecipeGetPyroOnTime(const struct recipe_t *recipe, const uint8_t timer_number) {
    uint16_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            ret_val = recipe->pyro_on_time[timer_number];
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint8_t RecipeGetPyroOnRise(const struct recipe_t *recipe, const uint8_t timer_number) {
    uint8_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            ret_val = recipe->pyro_on_rise[timer_number];
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint16_t RecipeGetPyroOnTemp(const struct recipe_t *recipe, const uint8_t timer_number) {
    uint16_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL && timer_number < RECIPE_MAX_PYRO_ON_TIMERS) {
            ret_val = recipe->pyro_on_temp[timer_number];
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

uint16_t RecipeGetUVTime(const struct recipe_t *recipe) {
    uint16_t ret_val = 0;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            ret_val = recipe->uv_time;
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return ret_val;
}

int32_t RecipeTotalTime(const struct recipe_t *recipe, const bool pyro_on) {
    int32_t time = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            switch (recipe->type) {
                case kRecipeIR:
                    if (pyro_on) {
                        time = recipe->pyro_on_time[0] + recipe->pyro_on_time[1];
                    } else {
                        time = recipe->pyro_off_time[0] + recipe->pyro_off_time[1];
                    }
                    break;
                case kRecipeUVIR:
                    if (pyro_on) {
                        time = recipe->pyro_on_time[0] + recipe->pyro_on_time[1] + recipe->uv_time;
                    } else {
                        time = recipe->pyro_off_time[0] + recipe->pyro_off_time[1] + recipe->uv_time;
                    }
                    break;
                case kRecipe3StepIR:
                    time = recipe->pyro_on_time[0] + recipe->pyro_on_time[1] + recipe->pyro_on_time[2];
                    break;
                case kRecipeUV:
                    time = recipe->uv_time;
                    break;
                default:
                    break;
            }
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return time;
}

int32_t RecipeMaxTemperature(const struct recipe_t *recipe, const bool pyro_on) {
    int32_t time = -1;
    if (k_mutex_lock(&recipes_mutex, K_FOREVER) == 0) {
        if (recipe != NULL) {
            switch (recipe->type) {
                case kRecipeIR:
                case kRecipeUVIR:
                    if (pyro_on) {
                        time = MAX(recipe->pyro_on_temp[0], recipe->pyro_on_temp[1]);
                    }
                    break;
                case kRecipe3StepIR:
                    time = MAX(recipe->pyro_on_temp[0], MAX(recipe->pyro_on_temp[1], recipe->pyro_on_temp[2]));
                    break;
                case kRecipeUV:
                    break;
                default:
                    break;
            }
        }
        k_mutex_unlock(&recipes_mutex);
    }
    return time;
}

void RecipePrintAll() {
    for (int i = 0; i < recipes_.n_recipes; ++i) {
        LOG_INF("[recipe] %s, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i, %i",
                recipes_.recipes[i].name,
                recipes_.recipes[i].type,
                recipes_.recipes[i].pyro_off_power[0],
                recipes_.recipes[i].pyro_off_time[0],
                recipes_.recipes[i].pyro_off_power[1],
                recipes_.recipes[i].pyro_off_time[1],
                recipes_.recipes[i].pyro_on_time[0],
                recipes_.recipes[i].pyro_on_rise[0],
                recipes_.recipes[i].pyro_on_temp[0],
                recipes_.recipes[i].pyro_on_time[1],
                recipes_.recipes[i].pyro_on_rise[1],
                recipes_.recipes[i].pyro_on_temp[1],
                recipes_.recipes[i].pyro_on_time[2],
                recipes_.recipes[i].pyro_on_rise[2],
                recipes_.recipes[i].pyro_on_temp[2],
                recipes_.recipes[i].uv_time);
    }
}
