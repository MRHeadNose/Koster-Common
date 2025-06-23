#ifndef KOSTER_COMMON_PROGRAM_LOGGER_H
#define KOSTER_COMMON_PROGRAM_LOGGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct program_log_entry_t program_log_entry_t;

/**
 * @brief Callback function type for processing program log entries.
 *
 * This function is called for each program log entry during iteration in ProgramLoggerEmit.
 *
 * @param entry Pointer to the current program log entry.
 * @param arg   User-defined argument passed from input to ProgramLoggerEmit
 *
 * @return 0 to continue iteration, non-zero to stop.
 */
typedef int (*program_entry_lookup_cb_t)(const program_log_entry_t *entry, void *arg);

/**
 * @brief Initializes the program logger.
 *
 * This function initializes the program logger with the specified maximum
 * log item entry size.
 * It must be called before any other logger functions are used.
 *
 * @param entry_size The maximum size of each log entry.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int ProgramLoggerInit(size_t entry_size);

/**
 * @brief Write an entry to the log.
 *
 * This function writes a log entry. The length of the entry (`len`) must be less than
 * the `entry_size` specified during the initialization of the logger.
 *
 * @param entry Pointer to the data to be logged.
 * @param len Length of the data to be logged. Must be less than `entry_size` from init.
 *
 * @return 0 on success, or a negative error code on failure.
 */
int ProgramLoggerWrite(const void *data, size_t len);

/**
 * @brief Iterates over each entry and invokes the provided callback function if it is not NULL.
 *
 * @param cb Pointer to a function to be called for each entry
 * @param arg Pointer to user-defined data to be passed to the callback function.
 *
 * @details The function processes all entries, calling the callback for each one if provided.
 *          After processing, it returns the total number of entries processed.
 *          If the callback returns a non-zero value, the iteration stops early.
 *
 * @return The total number of entries processed.
 */
int ProgramLoggerEmit(program_entry_lookup_cb_t cb, void *arg);

/**
 * @brief Reads data from a program log entry into a buffer.
 *
 * This function reads up to @p len bytes of data from the specified
 * program log entry into the provided @p data buffer.
 *
 * @param[in]  entry Pointer to the program log entry to read from.
 * @param[out] data  Pointer to the buffer where the read data will be stored.
 * @param[in]  len   Maximum number of bytes to read into the buffer.
 *
 * @return Number of bytes actually read from the entry or negative error code on failure.
 */
int ProgramLoggerRead(const program_log_entry_t *entry, void *data, size_t len);

#endif
