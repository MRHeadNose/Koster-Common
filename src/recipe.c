#include "koster-common/recipe.h"

#include <memory.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#if defined(CONFIG_SETTINGS_FILE)
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#endif

LOG_MODULE_REGISTER(koster_common);

#define STORAGE_PARTITION storage_partition
#define STORAGE_PARTITION_ID FIXED_PARTITION_ID(STORAGE_PARTITION)
#define RECIPIES_SETTING "koster_common/recipes"

static struct settings_handler handler_;

struct recipe_t {
    uint8_t id;
    char name[RECIPE_NAME_MAX_SIZE];
    recipe_type_t type;
    uint16_t flash_off_time[RECIPE_MAX_FLASHOFF_TIMERS];   // Flash-off time (seconds)
    uint8_t flash_off_power[RECIPE_MAX_FLASHOFF_TIMERS];   // Flash-off power (percent)
    uint16_t hardening_time[RECIPE_MAX_HARDENING_TIMERS];  // Flash-on time (seconds)
    uint8_t hardening_rise[RECIPE_MAX_HARDENING_TIMERS];   // Flash-on temperature rise (celsius per minute)
    uint16_t hardening_temp[RECIPE_MAX_HARDENING_TIMERS];  // Flash-on end temperature (celsius)
    uint16_t uv_time;                                      // UV time (seconds)
};

struct recipes_t {
    size_t n_recipes;
    struct recipe_t recipes[RECIPE_MAX_RECIPES];
};

static const struct recipe_t kDefaultRecipe0 = {
        .id = 0,
        .name = "recipe 0",
        .type = kRecipeIR,
        .flash_off_time = {240, 300},
        .flash_off_power = {20, 70},
        .hardening_time = {240, 300, 0},
        .hardening_rise = {15, 25, 0},
        .hardening_temp = {80, 180, 0},
        .uv_time = 0,
};
static const struct recipe_t kDefaultRecipe1 = {
        .id = 1,
        .name = "recipe 1",
        .type = kRecipeUV,
        .flash_off_time = {0},
        .flash_off_power = {0},
        .hardening_time = {0},
        .hardening_rise = {0},
        .hardening_temp = {0},
        .uv_time = 200,
};

static struct recipes_t recipes_;

static int handle_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;
    size_t name_len;

    name_len = settings_name_next(name, &next);

    if (!strncmp(name, "recipes", name_len)) {
        if (len != sizeof(recipes_)) {
            LOG_ERR("[recipe] handle_set: read size different from size of recipes");
            return -EINVAL;
        }

        int rc = read_cb(cb_arg, &recipes_, sizeof(recipes_));
        if (rc >= 0) {
            return 0;
        }

        return rc;
    }

    return -ENOENT;
}

static int handle_export(int (*storage_func)(const char *name, const void *value, size_t val_len)) {
    return storage_func(RECIPIES_SETTING, &recipes_, sizeof(recipes_));
}

int RecipeInit() {
    int rc;

#if defined(CONFIG_SETTINGS_FILE)
    FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(cstorage);

    static struct fs_mount_t littlefs_mnt = {.type = FS_LITTLEFS,
                                             .fs_data = &cstorage,
                                             .storage_dev = (void *)STORAGE_PARTITION_ID,
                                             .mnt_point = "/lfs1"};

    rc = fs_mount(&littlefs_mnt);
    if (rc != 0) {
        LOG_ERR("[recipe] error mounting littlefs: [%d]\n", rc);
    }
#endif

    rc = settings_subsys_init();
    if (rc) {
        LOG_ERR("[recipe] settings_subsys_init failed (err %d)", rc);
        return -1;
    }

    handler_.name = "koster_common";
    handler_.h_get = NULL;
    handler_.h_set = handle_set;
    handler_.h_commit = NULL;
    handler_.h_export = handle_export;

    rc = settings_register(&handler_);
    if (rc != 0) {
        LOG_ERR("[recipe] settings_register failed (err %d)", rc);
        return -1;
    }

    rc = settings_load();
    if (rc != 0) {
        LOG_ERR("[recipe] settings_load failed (err %d)", rc);
        return -1;
    }

    if (recipes_.n_recipes == 0) {
        LOG_INF("[recipe] No recipes found. Adding default recipes.");
        recipes_.n_recipes = 2;
        recipes_.recipes[0] = kDefaultRecipe0;
        recipes_.recipes[1] = kDefaultRecipe1;
    } else {
        LOG_INF("[recipe] Loaded %i recipes.", recipes_.n_recipes);
    }

    return 0;
}

int RecipeGet(struct recipe_t *recipe, const uint8_t id) {
    if (id < recipes_.n_recipes) {
        (*recipe) = recipes_.recipes[id];
        return 0;
    }
    return -1;
}

int RecipeSave(const struct recipe_t *recipe) {
    if (recipe->id < recipes_.n_recipes) {
        recipes_.recipes[recipe->id] = *recipe;
        return 0;
    }
    if (recipe->id == recipes_.n_recipes) {
        ++recipes_.n_recipes;
        recipes_.recipes[recipe->id] = *recipe;
        return 0;
    } else {
        LOG_ERR("[recipe] RecipeSave: Recipe ID does not exist and is not the next available.");
        return -1;
    }
}

int RecipePersist() {
    int rc = settings_save_one(RECIPIES_SETTING, &recipes_, sizeof(recipes_));
    if (rc != 0) {
        LOG_ERR("[recipe] settings_save_one failed (err %d)", rc);
    }
    return rc;
}

int RecipeSetName(struct recipe_t *recipe, const char *name) {
    if (recipe != NULL) {
        strncpy(recipe->name, name, RECIPE_NAME_MAX_SIZE);
        return 0;
    }
    return -1;
}

int RecipeSetType(struct recipe_t *recipe, const recipe_type_t type) {
    if (recipe != NULL && type > 0 && type < kRecipeNTypes) {
        recipe->type = type;
        return 0;
    }
    return -1;
}

int RecipeSetFlashOffTime(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t seconds) {
    if (recipe != NULL && timer_number < RECIPE_MAX_FLASHOFF_TIMERS) {
        recipe->flash_off_time[timer_number] = seconds;
        return 0;
    }
    return -1;
}

int RecipeSetFlashOffPower(struct recipe_t *recipe, const uint8_t timer_number, const uint8_t pct) {
    if (recipe != NULL && timer_number < RECIPE_MAX_FLASHOFF_TIMERS && pct <= 100) {
        recipe->flash_off_power[timer_number] = pct;
        return 0;
    }
    return -1;
}

int RecipeSetHardeningTime(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t seconds) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        recipe->hardening_time[timer_number] = seconds;
        return 0;
    }
    return -1;
}

int RecipeSetHardeningRise(struct recipe_t *recipe, const uint8_t timer_number, const uint8_t c_per_min) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        recipe->hardening_rise[timer_number] = c_per_min;
        return 0;
    }
    return -1;
}

int RecipeSetHardeningTemp(struct recipe_t *recipe, const uint8_t timer_number, const uint16_t celsius) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        recipe->hardening_temp[timer_number] = celsius;
        return 0;
    }
    return -1;
}

int RecipeSetUVTime(struct recipe_t *recipe, const uint16_t seconds) {
    if (recipe != NULL) {
        recipe->uv_time = seconds;
        return 0;
    }
    return -1;
}

int RecipeGetName(const struct recipe_t *recipe, char *buf, size_t bufsz) {
    if (recipe != NULL) {
        bufsz = MIN(bufsz, RECIPE_NAME_MAX_SIZE);
        strncpy(buf, recipe->name, bufsz);
        return 0;
    }
    return -1;
}

recipe_type_t RecipeGetType(const struct recipe_t *recipe) {
    if (recipe != NULL) {
        return recipe->type;
    }
    return kRecipeNTypes;
}

uint16_t RecipeGetFlashOffTime(const struct recipe_t *recipe, const uint8_t timer_number) {
    if (recipe != NULL && timer_number < RECIPE_MAX_FLASHOFF_TIMERS) {
        return recipe->flash_off_time[timer_number];
    }
    return 0;
}

uint8_t RecipeGetFlashOffPower(const struct recipe_t *recipe, const uint8_t timer_number) {
    if (recipe != NULL && timer_number < RECIPE_MAX_FLASHOFF_TIMERS) {
        return recipe->flash_off_power[timer_number];
    }
    return 0;
}

uint16_t RecipeGetHardeningTime(const struct recipe_t *recipe, const uint8_t timer_number) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        return recipe->hardening_time[timer_number];
    }
    return 0;
}

uint8_t RecipeGetHardeningRise(const struct recipe_t *recipe, const uint8_t timer_number) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        return recipe->hardening_rise[timer_number];
    }
    return 0;
}

uint16_t RecipeGetHardeningTemp(const struct recipe_t *recipe, const uint8_t timer_number) {
    if (recipe != NULL && timer_number < RECIPE_MAX_HARDENING_TIMERS) {
        return recipe->hardening_temp[timer_number];
    }
    return 0;
}

uint16_t RecipeGetUVTime(const struct recipe_t *recipe) {
    if (recipe != NULL) {
        return recipe->uv_time;
    }
    return 0;
}
