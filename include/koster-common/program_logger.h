#ifndef KOSTER_COMMON_PROGRAM_LOGGER_H
#define KOSTER_COMMON_PROGRAM_LOGGER_H

#include <stddef.h>
#include <stdint.h>

typedef struct program_log_entry_t program_log_entry_t;

typedef int (*program_entry_lookup_cb_t)(const program_log_entry_t *entry, void *arg);

int ProgramLoggerInit(void);

// Write entry to the log.
int ProgramLoggerWrite(const void *data, size_t len);

// Return number of entries after calling cb for each entry if not NULL.
int ProgramLoggerEmit(program_entry_lookup_cb_t cb, void *arg);

// Read data from the entry into data buffer, returns number of bytes read.
// Entry is given by callback from emit function
int ProgramLoggerRead(program_log_entry_t *entry, void *data, size_t len);

#endif
