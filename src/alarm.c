#include "koster-common/alarm.h"

#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/zbus/zbus.h>

#include "koster-common/koster-zbus.h"

LOG_MODULE_DECLARE(koster_common);

#define ALARM_MAX_ALARMS 20

static atomic_t active_alarms_[ALARM_MAX_ALARMS];
static atomic_t active_alarm_times_[ALARM_MAX_ALARMS];
static atomic_t number_of_active_alarms_;

void AlarmInit() {
    number_of_active_alarms_ = ATOMIC_INIT(0);
    for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
        active_alarms_[i] = ATOMIC_INIT(0);
        active_alarm_times_[i] = ATOMIC_INIT(0);
    }
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
        case 0xF001:
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
        case 0xF102:
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
    const uint32_t epoch = k_uptime_seconds();
    for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
        const uint16_t alarm_id_i = (uint16_t)(atomic_get(&active_alarms_[i]) & 0xFFFF);
        if (alarm_id_i == 0 && first_free_index == UINT32_MAX) {
            first_free_index = i;
        }
        if (alarm_id_i == alarm_id) {
            if (!active) {
                LOG_INF("[alarm] Clearing alarm ID 0x%X", alarm_id);
                atomic_inc(&number_of_active_alarms_);
                atomic_clear(&active_alarms_[i]);
                atomic_clear(&active_alarm_times_[i]);
                notify_alarm(active, alarm_id, epoch);
                rc = 0;
                break;
            }
        }
    }

    if (first_free_index < ALARM_MAX_ALARMS) {
        LOG_ERR("[alarm] Setting alarm ID 0x%X", alarm_id);
        atomic_dec(&number_of_active_alarms_);
        atomic_set(&active_alarms_[first_free_index], alarm_id);
        atomic_set(&active_alarm_times_[first_free_index], epoch);
        notify_alarm(active, alarm_id, epoch);
        rc = 0;
    }

    return rc;
}

bool AlarmActiveTypeAAlarms() {
    for (int i = 0; i < ALARM_MAX_ALARMS; ++i) {
        if (AlarmGetType(atomic_get(&active_alarms_[i])) == 'A') {
            return true;
        }
    }
    return false;
}

int AlarmWalk(alarm_walk_cb_t cb, void *arg) {
    int rc = 0;
    for (int i = 0; i < ALARM_MAX_ALARMS && rc == 0; ++i) {
        struct alarm_t alarm;
        alarm.id = (uint16_t)(atomic_get(&active_alarms_[i]) & 0xFFFF);
        if (alarm.id == 0) {
            continue;
        }
        alarm.epoch = (uint16_t)(atomic_get(&active_alarm_times_[i]) & 0xFFFF);

        rc = cb(alarm, arg);
    }
    return rc;
}
