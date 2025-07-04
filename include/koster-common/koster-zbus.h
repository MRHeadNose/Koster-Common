#ifndef KOSTER_COMMON_KOSTER_ZBUS_H
#define KOSTER_COMMON_KOSTER_ZBUS_H

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/zbus/zbus.h>

#include "koster-common/alarm.h"

#define ZBUS_SENDER_NAME_MAX_LEN 16

/**
 * Sent to the Runner to request a program to start. Sent on channel kzbus_control_chan.
 */
struct kzbus_req_start_msg_t {
    /** the program to start */
    uint8_t program_index;
    /** use temperature control (pyrometer) */
    bool temperature_on;
    /** a bitmap representing which cassettes should be used */
    uint8_t cassettes;
};

/**
 * Sent to the Runner to request a program to stop. Sent on channel kzbus_control_chan.
 */
struct kzbus_req_stop_msg_t {};

/**
 * Sent to the Runner to request the laser to be turned on. Sent on channel kzbus_control_chan.
 */
struct kzbus_req_laser_on_msg_t {};

/**
 * Sent to the Runner to request the laser to be turned off. Sent on channel kzbus_control_chan.
 */
struct kzbus_req_laser_off_msg_t {};

typedef enum {
    /** No program is running */
    kProgramStateIdle,
    /** A program is running */
    kProgramStateRunning,
    /** An error occured */
    kProgramStateError
} program_state_t;

/**
 * Sent from the Runner. Sent on channel kzbus_program_chan.
 */
struct kzbus_program_msg_t {
    /** the current state of the program */
    program_state_t program_state;
    /** how many seconds the running program has left in total */
    uint16_t program_time_left;
    /** the program step that is currently running, index starting at 0  */
    uint8_t step;
    /** how many seconds the running program has left in the current step */
    uint16_t step_time_left;
    /** the current uv/ir power output in percent */
    uint8_t power;
    /** total accumulated energy so far over the run of the program, in Ws */
    double total_energy;
    /** bitmap representing which cassettes are in use */
    uint8_t cassettes;
    /** true if fans are on, false if not */
    bool fan_on;
    /** true if laser is on, false if not */
    bool laser_on;
    /** the current target (set) temperature in degrees C */
    int16_t target_temperature;
};

/**
 * Sent from the Runner. Sent on channel kzbus_temperature_chan.
 */
struct kzbus_temperature_msg_t {
    /** temperature measurement in degrees C */
    int16_t temperature;
    /** the ID of the Vinga node the measurement originated from  */
    uint8_t vinga_id;
};

/**
 * Sent from the Runner. Sent on channel kzbus_distance_chan.
 */
struct kzbus_distance_msg_t {
    /** distance measurement in millimeters */
    uint16_t distance;
    /** the ID of the Vinga node the measurement originated from  */
    uint8_t vinga_id;
};

/**
 * Sent from the Runner. Sent on channel kzbus_alarm_chan.
 */
struct kzbus_alarm_msg_t {
    bool alarm_active;
    uint16_t alarm_id;
};

/**
 * Sent from the Runner. Sent on channel kzbus_ircam_chan.
 */
struct kzbus_ircam_msg_t {
    int16_t* img_buf;
    int16_t min_temperature;
    int16_t max_temperature;
    /** the ID of the Vinga node the IR image originated from  */
    uint8_t vinga_id;
};

/**
 * Encodes the message type of kzbus_msg_t
 */
typedef enum {
    kMsgReqStart,
    kMsgReqStop,
    kMsgReqLaserOn,
    kMsgReqLaserOff,
    kMsgProgram,
    kMsgTemperature,
    kMsgDistance,
    kMsgIRCamera,
    kMsgAlarm
} kzbus_msg_type_t;

/**
 * Generic message type.
 *
 * This is the message sent over ZBus on all channels.
 */
struct kzbus_msg_t {
    /** The type of message being sent */
    kzbus_msg_type_t msg_type;
    char sender[ZBUS_SENDER_NAME_MAX_LEN];
    uint16_t epoch_time;
    union {
        struct kzbus_req_start_msg_t req_start_msg;
        struct kzbus_req_stop_msg_t req_stop_msg;
        struct kzbus_req_laser_on_msg_t req_laser_on_msg;
        struct kzbus_req_laser_on_msg_t req_laser_off_msg;
        struct kzbus_program_msg_t program_msg;
        struct kzbus_temperature_msg_t temperature_msg;
        struct kzbus_distance_msg_t distance_msg;
        struct kzbus_alarm_msg_t alarm_msg;
        struct kzbus_ircam_msg_t ircam_msg;
    };
};

/**
 * ZBus channels
 *
 * Add a subscriber using ZBUS_SUBSCRIBER_DEFINE, e.g.  ZBUS_SUBSCRIBER_DEFINE(my_subscriber, 1);
 * Then, register as observer using ZBUS_CHAN_ADD_OBS, e.g. ZBUS_CHAN_ADD_OBS(kzbus_control_chan, my_subscriber, 4)
 */
ZBUS_CHAN_DECLARE(kzbus_program_chan,      // Channel for program state messages (from program runner)
                  kzbus_temperature_chan,  // Channel for temperature messages (from program runner)
                  kzbus_distance_chan,     // Channel for distance messages (from program runner)
                  kzbus_alarm_chan,        // Channel for alarm messages (from program runner)
                  kzbus_control_chan,      // Channel for control messages (to program runner)
                  kzbus_ircam_chan         // Channel for IR camera (from program runner)
);

#endif
