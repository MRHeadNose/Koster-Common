#pragma once

#include "fff/fff.h"

typedef ssize_t (*settings_read_cb)(void *cb_arg, void *data, size_t len);
typedef struct {
} sys_snode_t;
struct settings_handler {
    const char *name;
    int (*h_get)(const char *key, char *val, int val_len_max);
    int (*h_set)(const char *key, size_t len, settings_read_cb read_cb, void *cb_arg);
    int (*h_commit)(void);
    int (*h_export)(int (*export_func)(const char *name, const void *val, size_t val_len));
    sys_snode_t node;
};

FAKE_VALUE_FUNC(int, settings_subsys_init);
FAKE_VALUE_FUNC(int, settings_load);
FAKE_VALUE_FUNC(int, settings_register, struct settings_handler *);
FAKE_VALUE_FUNC(int, settings_save_one, const char *, const void *, size_t);
