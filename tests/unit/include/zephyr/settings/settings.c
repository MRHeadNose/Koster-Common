#include "settings.h"

DEFINE_FAKE_VALUE_FUNC(int, settings_subsys_init);
DEFINE_FAKE_VALUE_FUNC(int, settings_load);
DEFINE_FAKE_VALUE_FUNC(int, settings_register, struct settings_handler *);
DEFINE_FAKE_VALUE_FUNC(int, settings_save_one, const char *, const void *, size_t);
