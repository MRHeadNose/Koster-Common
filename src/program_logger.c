/*
 * Copyright (c) 2025 Hedson Technologies AB
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "koster-common/program_logger.h"

#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <zephyr/fs/fcb.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(koster_common);

#if DT_HAS_CHOSEN(zephyr_logger_partition)
#define LOGGER_PARTITION DT_FIXED_PARTITION_ID(DT_CHOSEN(zephyr_logger_partition))
#else
#define LOGGER_PARTITION FIXED_PARTITION_ID(history_partition)
#endif

static struct fcb _fcb;

int ProgramLoggerWrite(const void *data, size_t len) {
    int rc;
    struct fcb_entry loc;
    rc = fcb_append(&_fcb, len, &loc);
    /* If no space left */
    if (rc == -ENOSPC) {
        LOG_INF("No space left in FCB, trying to rotate");
        /* Try erase oldest sector */
        rc = fcb_rotate(&_fcb);
        if (rc == 0) {
            rc = fcb_append(&_fcb, len, &loc);
        }
    }
    if (rc) {
        return rc;
    }
    rc = flash_area_write(_fcb.fap, FCB_ENTRY_FA_DATA_OFF(loc), data, len);
    if (rc) {
        LOG_ERR("Failed to write data to FCB (%d)", rc);
        return rc;
    }
    rc = fcb_append_finish(&_fcb, &loc);
    if (rc) {
        LOG_ERR("Failed to finish fcb_append (%d)", rc);
    }
    return rc;
}

int ProgramLoggerRead(program_log_entry_t *entry, void *data, size_t len) {
    int rc;
    size_t read_len = len;
    struct fcb_entry_ctx *ctx = (struct fcb_entry_ctx *)entry;
    if (ctx == NULL || data == NULL) {
        return -EINVAL;
    }
    if (read_len > ctx->loc.fe_data_len) {
        read_len = ctx->loc.fe_data_len;
    }
    rc = flash_area_read(ctx->fap, FCB_ENTRY_FA_DATA_OFF(ctx->loc), data, len);
    if (rc) {
        LOG_ERR("Failed to read data from FCB (%d)", rc);
        return rc;
    }
    return read_len;
}

int ProgramLoggerEmit(program_entry_lookup_cb_t cb, void *arg) {
    int rc = 0;
    int count = 0;
    struct fcb_entry_ctx entry;

    (void)memset(&entry, 0, sizeof(entry));
    entry.fap = _fcb.fap;
    while (!fcb_getnext(&_fcb, &entry.loc) && rc == 0) {
        if (cb != NULL) {
            rc = cb((program_log_entry_t *)&entry, arg);
        }
        count++;
    }
    return count;
}

int ProgramLoggerInit(void) { return fcb_init(LOGGER_PARTITION, &_fcb); }
