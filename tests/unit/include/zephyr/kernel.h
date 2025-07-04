#pragma once

#include <errno.h>
#include <stdint.h>

#include "fff/fff.h"

#define K_MSEC(X) X
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define K_FOREVER 0
#define K_NO_WAIT 0

struct k_mutex {
    int foo;
};
typedef int k_timeout_t;

DECLARE_FAKE_VALUE_FUNC(int, k_mutex_lock, struct k_mutex *, k_timeout_t);
DECLARE_FAKE_VOID_FUNC(k_mutex_unlock, struct k_mutex *);
DECLARE_FAKE_VALUE_FUNC(int, k_mutex_init, struct k_mutex *);

DECLARE_FAKE_VALUE_FUNC(int, settings_name_next, const char *, const char **);
DECLARE_FAKE_VOID_FUNC(z_log_minimal_printk);
DECLARE_FAKE_VOID_FUNC(log_const_app);

DECLARE_FAKE_VALUE_FUNC(uint32_t, k_uptime_seconds);
