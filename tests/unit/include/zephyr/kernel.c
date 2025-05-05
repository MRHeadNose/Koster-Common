#include "kernel.h"

DEFINE_FAKE_VALUE_FUNC(int, k_mutex_lock, struct k_mutex *, k_timeout_t);
DEFINE_FAKE_VOID_FUNC(k_mutex_unlock, struct k_mutex *);
DEFINE_FAKE_VALUE_FUNC(int, k_mutex_init, struct k_mutex *);

DEFINE_FAKE_VALUE_FUNC(int, settings_name_next, const char *, const char **);
DEFINE_FAKE_VOID_FUNC(z_log_minimal_printk);
DEFINE_FAKE_VOID_FUNC(log_const_app);
