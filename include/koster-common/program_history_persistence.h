#ifndef KOSTER_UI_CONTROLLER_PROGRAM_HISTORY_PERSISTENCE_H
#define KOSTER_UI_CONTROLLER_PROGRAM_HISTORY_PERSISTENCE_H

#include <stdint.h>

#include "program_history.h"

/**
 * Collect and save history to persistent storage
 */
void ProgramHistoryPersistenceSave();

/**
 * ProgramHistoryPersistenceWalk callback type
 *
 * Callback type supplied to @ref ProgramHistoryPersistenceWalk
 *
 * @param history  pointer to the program_header_t read from storage
 * @return 0 continue walking, non-zero stop walking.
 */
typedef int (*program_history_walk_cb)(struct program_header_t* header);

/**
 * Trigger walk through of program history entries
 *
 * The history will be walked in backwards chronological order with respect to to when they were written.
 *
 * @param cb  callback invoked while walking the entries
 * @return 0 on success, non-zero on failure.
 */
int ProgramHistoryPersistenceWalk(program_history_walk_cb cb);

/**
 * Read a complete history entry from persistent storage
 *
 * @param history  pointer to history, read destination
 * @param run_id   the ID of the run to read
 * @return 0 on success, non-zero on failure.
 */
int ProgramHistoryPersistenceRead(struct program_history_t** history, uint32_t run_id);

#endif
