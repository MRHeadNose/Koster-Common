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

struct recipes_t {
    size_t n_recipes;
    struct recipe_t recipes[RECIPE_MAX_RECIPES];
};

static const struct recipe_t kDefaultRecipe0 = {
        .id = 0,
        .name = "recipe 0",
        .type = kRecipeIR,
        .flash_off_t1_time = 240,
        .flash_off_t1_power = 20,
        .flash_off_t2_time = 300,
        .flash_off_t2_power = 70,
        .hardening_t1_time = 240,
        .hardening_t1_rise = 15,
        .hardening_t1_temp = 80,
        .hardening_t2_time = 300,
        .hardening_t2_rise = 25,
        .hardening_t2_temp = 180,
        .hardening_t3_time = 0,
        .hardening_t3_rise = 0,
        .hardening_t3_temp = 0,
        .uv_time = 0,
};
static const struct recipe_t kDefaultRecipe1 = {
        .id = 1,
        .name = "recipe 1",
        .type = kRecipeUV,
        .flash_off_t1_time = 0,
        .flash_off_t1_power = 0,
        .flash_off_t2_time = 0,
        .flash_off_t2_power = 0,
        .hardening_t1_time = 0,
        .hardening_t1_rise = 0,
        .hardening_t1_temp = 0,
        .hardening_t2_time = 0,
        .hardening_t2_rise = 0,
        .hardening_t2_temp = 0,
        .hardening_t3_time = 0,
        .hardening_t3_rise = 0,
        .hardening_t3_temp = 0,
        .uv_time = 200,
};

static struct recipes_t recipes_;

/* static int handle_get(const char *name, char *val, int val_len_max) { */
/*     LOG_INF("[recipe] handle_get: name = %s, val_len_max = %i", name, val_len_max); */
/*     const char *next; */

/*     if (settings_name_steq(name, "recipes", &next) && !next) { */
/*         LOG_INF("[recipe] handle_get: next = %s", next); */
/*         val_len_max = MIN(val_len_max, sizeof(recipes_)); */
/*         memcpy(val, &recipes_, val_len_max); */
/*         return val_len_max; */
/*     } */

/*     return -1; */
/* } */

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
