#pragma once

#include "fff/fff.h"
#include "zephyr/kernel.h"

struct zbus_channel {};

#define ZBUS_CHAN_DECLARE(_name, ...) struct zbus_channel _name

DECLARE_FAKE_VALUE_FUNC(int, zbus_chan_pub, const struct zbus_channel *, const void *, k_timeout_t);
