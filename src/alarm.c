#include "koster-common/alarm.h"

#include <memory.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "koster-common/koster-zbus.h"
#include "koster-common/rtc.h"

LOG_MODULE_DECLARE(koster_common);

#define ALARM_MAX_ALARMS 20

struct k_mutex alarm_mutex_;
static uint16_t active_alarms_[ALARM_MAX_ALARMS];
static uint32_t active_alarm_times_[ALARM_MAX_ALARMS];

extern const struct zbus_channel kzbus_alarm_chan;

void AlarmInit() {
    k_mutex_init(&alarm_mutex_);
    memset(active_alarms_, 0, sizeof(active_alarms_));
    memset(active_alarm_times_, 0, sizeof(active_alarm_times_));
}

char AlarmGetType(uint16_t alarm_id) {
    switch (alarm_id) {
        case 0x1002:
        case 0x1003:
        case 0x1008:
        case 0x1009:
        case 0x100A:
        case 0x100B:
        case 0x100C:
        case 0x100D:
        case 0x100F:
        case 0x1011:
        case 0x1012:
        case 0x1013:
        case 0x1014:
            return 'A';
        case 0x1001:
        case 0x1004:
        case 0x1005:
        case 0x1006:
        case 0x1007:
        case 0x100E:
        case 0x1010:
        case 0x1015:
        case 0x1016:
        case 0x1017:
            return 'B';
        default:
            return 'U';
    }
    return 'U';
}

static void notify_alarm(const bool active, const uint16_t alarm_id, const uint32_t epoch) {
    struct kzbus_msg_t alarm_msg;
    alarm_msg.msg_type = kMsgAlarm;
    alarm_msg.epoch_time = epoch;
    alarm_msg.alarm_msg.alarm_active = active;
    alarm_msg.alarm_msg.alarm_id = alarm_id;
    zbus_chan_pub(&kzbus_alarm_chan, &alarm_msg, K_NO_WAIT);
}

int AlarmSet(const bool active, const uint8_t error_id, const alarm_origin_t origin) {
    const uint16_t alarm_id = (((uint16_t)error_id) & ALARM_ERROR_ID_MASK) | (origin & ALARM_ORIGIN_MASK);
    int rc = -1;
    uint32_t first_free_index = UINT32_MAX;
    bool notify = false;

    const uint32_t epoch = RtcGetEpoch();

    if (k_mutex_lock(&alarm_mutex_, K_FOREVER) == 0) {
        for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
            const uint16_t alarm_id_i = active_alarms_[i];
            if (alarm_id_i == 0 && first_free_index == UINT32_MAX) {
                first_free_index = i;
            }
            if (alarm_id_i == alarm_id) {
                if (!active) {
                    LOG_INF("[alarm] Clearing alarm ID 0x%X", alarm_id);
                    active_alarms_[i] = 0;
                    active_alarm_times_[i] = 0;
                    notify = true;
                    rc = 0;
                    break;
                }
            }
        }

        if (first_free_index < ALARM_MAX_ALARMS && active) {
            LOG_ERR("[alarm] Setting alarm ID 0x%X", alarm_id);
            active_alarms_[first_free_index] = alarm_id;
            active_alarm_times_[first_free_index] = epoch;
            notify = true;
            rc = 0;
        }

        k_mutex_unlock(&alarm_mutex_);
    }

    if (notify) {
        notify_alarm(active, alarm_id, epoch);
    }

    return rc;
}

bool AlarmActiveTypeAAlarms() {
    bool ret = false;
    if (k_mutex_lock(&alarm_mutex_, K_FOREVER) == 0) {
        for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
            if (AlarmGetType(active_alarms_[i]) == 'A') {
                ret = true;
                break;
            }
        }
        k_mutex_unlock(&alarm_mutex_);
    }
    return ret;
}

int AlarmWalk(alarm_walk_cb_t cb, void *arg) {
    int rc = 0;
    if (k_mutex_lock(&alarm_mutex_, K_FOREVER) == 0) {
        for (int i = 0; i < ALARM_MAX_ALARMS && rc == 0; ++i) {
            struct alarm_t alarm;
            alarm.id = active_alarms_[i];
            if (alarm.id == 0) {
                continue;
            }
            alarm.epoch = active_alarm_times_[i];

            rc = cb(alarm, arg);
        }
        k_mutex_unlock(&alarm_mutex_);
    }
    return rc;
}

bool AlarmIsActive(const uint8_t error_id, alarm_origin_t *origin) {
    bool is_active = false;
    if (k_mutex_lock(&alarm_mutex_, K_FOREVER) == 0) {
        for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
            if ((active_alarms_[i] & ALARM_ERROR_ID_MASK) == error_id) {
                is_active = true;
                if (origin != NULL) {
                    *origin = active_alarms_[i] & ALARM_ORIGIN_MASK;
                }
                break;
            }
        }
        k_mutex_unlock(&alarm_mutex_);
    }
    return is_active;
}
