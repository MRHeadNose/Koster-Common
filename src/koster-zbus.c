#include "koster-common/koster-zbus.h"

/**
 * Channel for program state messages (from program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_program_chan, /* Name */
                 struct kzbus_msg_t, /* Message type */
                 NULL,               /* Validator */
                 NULL,               /* User data */
                 ZBUS_OBSERVERS(),   /* Observers */
                 ZBUS_MSG_INIT(0));

/**
 * Channel for temperature messages (from program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_temperature_chan, /* Name */
                 struct kzbus_msg_t,     /* Message type */
                 NULL,                   /* Validator */
                 NULL,                   /* User data */
                 ZBUS_OBSERVERS(),       /* Observers */
                 ZBUS_MSG_INIT(0));

/**
 * Channel for distance messages (from program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_distance_chan, /* Name */
                 struct kzbus_msg_t,  /* Message type */
                 NULL,                /* Validator */
                 NULL,                /* User data */
                 ZBUS_OBSERVERS(),    /* Observers */
                 ZBUS_MSG_INIT(0));

/**
 * Channel for error messages (from program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_error_chan,              /* Name */
                 struct kzbus_msg_t,            /* Message type */
                 NULL,                          /* Validator */
                 NULL,                          /* User data */
                 ZBUS_OBSERVERS(),              /* Observers */
                 ZBUS_MSG_INIT(0));

/**
 * Channel for IR camera image buffer (from program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_ircam_chan,              /* Name */
                 struct kzbus_msg_t,            /* Message type */
                 NULL,                          /* Validator */
                 NULL,                          /* User data */
                 ZBUS_OBSERVERS(),              /* Observers */
                 ZBUS_MSG_INIT(0));

/**
 * Channel for control messages (to program runner)
 */
ZBUS_CHAN_DEFINE(kzbus_control_chan, /* Name */
                 struct kzbus_msg_t, /* Message type */
                 NULL,               /* Validator */
                 NULL,               /* User data */
                 ZBUS_OBSERVERS(),   /* Observers */
                 ZBUS_MSG_INIT(0));
