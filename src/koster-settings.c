#include "koster-common/koster-settings.h"

#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#if defined(CONFIG_SETTINGS_FILE)
#include <zephyr/fs/fs.h>
#include <zephyr/fs/littlefs.h>
#endif

LOG_MODULE_DECLARE(koster_common);

#define STORAGE_PARTITION storage_partition
#define STORAGE_PARTITION_ID FIXED_PARTITION_ID(STORAGE_PARTITION)

int KosterSettingsInit() {
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
        return -1;
    }
#endif

    rc = settings_subsys_init();
    if (rc) {
        LOG_ERR("[recipe] settings_subsys_init failed (err %d)", rc);
        return -1;
    }

    return rc;
}
