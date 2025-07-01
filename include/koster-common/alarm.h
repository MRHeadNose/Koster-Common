#ifndef KOSTER_COMMON_ALARM_H
#define KOSTER_COMMON_ALARM_H

#include <stdint.h>

#define ALARM_ORIGIN_MASK 0xF000
#define ALARM_BOARD_NUMBER_MASK 0x0F00
#define ALARM_ERROR_ID_MASK 0x00FF

typedef enum { kAlarmOriginUnknown = 0, kAlarmOriginKoster = 0x1000, kAlarmOriginVinga = 0x2000 } alarm_origin_t;

char AlarmGetType(uint16_t alarm_id);

#endif
