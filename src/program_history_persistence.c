#include "koster-common/program_history_persistence.h"

#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(koster_common);
static struct program_history_t* history_;

// FAKE STUFF. Should be removed when implemented
int fake_history_counter_;
program_history_walk_cb walk_cb_;
#define N_FAKE_HISTORIES 50
// END FAKE STUFF.

void ProgramHistoryPersistenceSave(struct program_history_t* history) { history_ = history; }

int ProgramHistoryPersistenceWalk(program_history_walk_cb cb) {
    fake_history_counter_ = 0;
    int rc = -1;
    do {
        rc = cb(&history_->header);
        history_->header.v1.start_time -= fake_history_counter_++;
    } while (fake_history_counter_ < N_FAKE_HISTORIES && rc == 0);
    return rc;
}

int ProgramHistoryPersistenceRead(struct program_history_t** history, uint32_t run_id) {
    *history = history_;
    return 0;
}
