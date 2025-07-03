#ifndef KOSTER_COMMON_ALARM_H
#define KOSTER_COMMON_ALARM_H

#include <stdbool.h>
#include <stdint.h>

#define ALARM_ORIGIN_MASK 0xFF00
#define ALARM_ERROR_ID_MASK 0x00FF

typedef enum {
    kAlarmOriginUnknown = 0,
    kAlarmOriginKoster = 0x1000,
    kAlarmOriginVinga1 = 0x2000,
    kAlarmOriginVinga2 = 0x2100,
    kAlarmOriginVinga3 = 0x2200,
    kAlarmOriginVinga4 = 0x2300,
    kAlarmOriginVinga5 = 0x2400,
    kAlarmOriginTest1 = 0xF000,
    kAlarmOriginTest2 = 0xF100,
} alarm_origin_t;

struct alarm_t {
    uint16_t id;
    uint32_t epoch;  // milliseconds
};

/**
 * @brief Initialize alarm handler
 */
void AlarmInit();

/**
 * @brief Get type for alarm.
 *
 * @param alarm_id the alarm ID (origin | error_id)
 * @return the alarm type 'A', 'B', or 'U' (Unknown).
 */
char AlarmGetType(uint16_t alarm_id);

/**
 * @brief Set or clear an alarm alarm.
 *
 * @param active  true to activate the alarm, false to clear
 * @param error_id  the error ID (alarm ID sans origin)
 * @param origin  the origin (alarm ID sans error ID)
 * @return 0 on success, -1 on error
 */
int AlarmSet(const bool active, const uint8_t error_id, const alarm_origin_t origin);

/**
 * @brief Check if there are active type A alarms (critical)
 *
 * @return true if there are type A alarms active, false if not
 */
bool AlarmActiveTypeAAlarms();

/**
 * @brief Callback function type for active alarm entries.
 *
 * This function is called for each alarm entry during iteration in AlarmWalk.
 *
 * @param entry Pointer to the current alarm id.
 * @param arg   User-defined argument passed from input to AlarmWalk
 *
 * @return 0 to continue iteration, non-zero to stop.
 */
typedef int (*alarm_walk_cb_t)(const struct alarm_t alarm, void *arg);

/**
 * @brief Iterates over each active alarm entry and invokes the provided callback function.
 *
 * @param cb Pointer to a function to be called for each entry
 * @param arg Pointer to user-defined data to be passed to the callback function.
 *
 * @details The function processes all entries, calling the callback for each one if provided.
 *          If the callback returns a non-zero value, the iteration stops early.
 *
 * @return 0 on success, -1 if the walk failed or was stopped by callback.
 */
int AlarmWalk(alarm_walk_cb_t cb, void *arg);

#endif
