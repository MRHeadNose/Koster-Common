#include "koster-common/rtc.h"

#include <errno.h>
#include <stdbool.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/timeutil.h>

#include "koster-common/parameters.h"

#define TM_YEAR_REF 1900

LOG_MODULE_DECLARE(koster_common);

static const struct device* const rtc = DEVICE_DT_GET(DT_ALIAS(rtc));

int RtcSetFromParameters() {
    struct rtc_time time;
    time.tm_year = (ParamGetYear() - TM_YEAR_REF);
    time.tm_mon = ParamGetMonth() - 1;  // from [1,12] to [0,11]
    time.tm_mday = ParamGetDay();
    time.tm_hour = ParamGetHour();
    time.tm_min = ParamGetMinute();
    time.tm_sec = ParamGetSecond();
    time.tm_wday = ParamGetWeekday();
    time.tm_yday = -1;  // Unknown yearday

    return rtc_set_time(rtc, &time);
}

int RtcToParameters() {
    struct rtc_time time;
    int rc = rtc_get_time(rtc, &time);

    if (rc != 0) {
        return rc;
    }

    rc += ParamSetYear(time.tm_year + TM_YEAR_REF);
    rc += ParamSetMonth(time.tm_mon + 1);  // from [0,11] to [1,12]
    rc += ParamSetDay(time.tm_mday);
    rc += ParamSetHour(time.tm_hour);
    rc += ParamSetMinute(time.tm_min);
    rc += ParamSetSecond(time.tm_sec);
    rc += ParamSetWeekday(time.tm_wday);

    return rc;
}

uint32_t RtcGetEpoch() {
    struct rtc_time rtc_time;
    int rc = rtc_get_time(rtc, &rtc_time);

    if (rc == -ENODATA) {
        LOG_WRN_ONCE("[Rtc] No RTC data. Setting RTC from parameters.");
        rc = RtcSetFromParameters();
        if (rc != 0) {
            LOG_WRN_ONCE("[Rtc] Failed to set time: rc = %i", rc);
        }

    } else if (rc != 0) {
        LOG_WRN_ONCE("[Rtc] Failed to get time: rc = %i", rc);
        return 0;
    }

    struct tm* utc_time = rtc_time_to_tm(&rtc_time);
    if (utc_time != NULL) {
        return mktime(utc_time);
    }

    return 0;
}
