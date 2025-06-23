/*
 * Copyright (c) 2025 Hedson Technologies AB
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "koster-common/program_logger.h"

#include <stdint.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/storage/flash_map.h>

#define LOG_MAGIC 0xAFFECAFE

LOG_MODULE_DECLARE(koster_common);

// Written to flash as a header for each log entry
struct log_entry_header {
    uint32_t magic;
    uint16_t sequence;
    uint16_t length;
};

typedef struct {
    const struct flash_area *fap;
    off_t offset;
    ssize_t data_length;
} callback_ctx_t;

#if DT_HAS_CHOSEN(zephyr_logger_partition)
#define LOGGER_PARTITION DT_FIXED_PARTITION_ID(DT_CHOSEN(zephyr_logger_partition))
#else
#define LOGGER_PARTITION FIXED_PARTITION_ID(history_partition)
#endif

struct circular_log {
    const struct flash_area *fap;
    uint16_t aligned_entry_size;
    uint16_t max_entries;
    uint16_t head;  // next write index
    uint16_t next_sequence;
};

static struct circular_log _clog;

static int scan_entries(struct circular_log *clog) {
    struct log_entry_header hdr;

    clog->next_sequence = 0;
    clog->head = 0;

    for (uint32_t i = 0; i < clog->max_entries; i++) {
        off_t offset = i * clog->aligned_entry_size;

        if (flash_area_read(clog->fap, offset, &hdr, sizeof(hdr)) != 0 || hdr.magic != LOG_MAGIC) {
            continue;
        }

        if (hdr.sequence >= clog->next_sequence) {
            clog->next_sequence = hdr.sequence + 1;
            clog->head = (i + 1) % clog->max_entries;
        }
    }
    return 0;
}

int ProgramLoggerWrite(const void *data, size_t len) {
    int rc;
    struct circular_log *clog = &_clog;
    if (len + sizeof(struct log_entry_header) > clog->aligned_entry_size) {
        return -EINVAL;
    }

    off_t offset = clog->aligned_entry_size * clog->head;
    rc = flash_area_erase(clog->fap, offset, clog->aligned_entry_size);
    if (rc != 0) {
        return rc;
    }
    struct log_entry_header hdr = {
            .magic = LOG_MAGIC,
            .sequence = clog->next_sequence++,
            .length = len,
    };
    clog->head = (clog->head + 1) % clog->max_entries;

    rc = flash_area_write(clog->fap, offset, &hdr, sizeof(hdr));
    if (rc != 0) {
        return rc;
    }
    rc = flash_area_write(clog->fap, offset + sizeof(hdr), data, len);
    if (rc != 0) {
        return rc;
    }
    return 0;
}

int ProgramLoggerRead(const program_log_entry_t *entry, void *data, size_t len) {
    if (!entry || !data) {
        return -EINVAL;
    }
    const callback_ctx_t *ctx = (const callback_ctx_t *)entry;
    size_t read_len = MIN(len, ctx->data_length);
    off_t data_offset = ctx->offset + sizeof(struct log_entry_header);

    int rc = flash_area_read(ctx->fap, data_offset, data, read_len);
    if (rc != 0) {
        LOG_ERR("Failed to read log entry");
        return rc;
    }

    return read_len;
}

int ProgramLoggerEmit(program_entry_lookup_cb_t cb, void *arg) {
    int emitted = 0;
    struct circular_log *clog = &_clog;
    for (int i = 0; i < clog->max_entries; i++) {
        int index = (clog->head + i) % clog->max_entries;
        off_t offset = clog->aligned_entry_size * index;
        struct log_entry_header hdr;
        // Read header and check header for magic number
        if (flash_area_read(clog->fap, offset, &hdr, sizeof(hdr)) != 0 || hdr.magic != LOG_MAGIC) {
            continue;
        }

        callback_ctx_t ctx = {.fap = clog->fap, .offset = offset, .data_length = hdr.length};

        if (cb && cb((program_log_entry_t *)&ctx, arg) != 0) {
            break;
        }

        emitted++;
    }

    return emitted;
}

int ProgramLoggerInit(size_t entry_size) {
    struct circular_log *clog = &_clog;
    struct flash_sector flash_sector;
    size_t entry_size_with_hdr = entry_size + sizeof(struct log_entry_header);
    uint32_t cnt;

    int rc = flash_area_open(LOGGER_PARTITION, &clog->fap);
    if (rc) {
        LOG_ERR("Failed to open flash area");
        return rc;
    }

    // Get information of the first sector and assume all sectors are the same size
    cnt = 1;
    rc = flash_area_get_sectors(LOGGER_PARTITION, &cnt, &flash_sector);
    if (rc != 0 && rc != -ENOMEM) {
        LOG_ERR("Failed to get sector for logger partition (%d)", rc);
        return rc;
    }

    if (entry_size_with_hdr > flash_sector.fs_size) {
        // Align up to sector size
        clog->aligned_entry_size = ROUND_UP(entry_size_with_hdr, flash_sector.fs_size);
    } else {
        clog->aligned_entry_size = ROUND_UP(entry_size_with_hdr, 4);  // ensure alignment
    }

    clog->max_entries = clog->fap->fa_size / clog->aligned_entry_size;

    return scan_entries(clog);
}
