#ifndef KOSTER_COMMON_RTC_H
#define KOSTER_COMMON_RTC_H

#include <stdint.h>

/**
 * @brief Set RTC from Parameters
 *
 * @return 0 on success, negative on failure
 */
int RtcSetFromParameters();

/**
 * @brief Set Date and Time Parameters from RTC
 *
 * @return 0 on success, negative on failure
 */
int RtcToParameters();

uint32_t RtcGetEpoch();

#endif
