#include "koster-common/alarm.h"

char AlarmGetType(uint16_t alarm_id) {
    switch (alarm_id & ALARM_ERROR_ID_MASK) {
        default:
            return 'X';
    }
    return 'X';
}
