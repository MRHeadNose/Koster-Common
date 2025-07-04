#include <vector>

#include "gtest/gtest.h"

extern "C" {
#include "fff/fff.h"
#include "koster-common/alarm.h"
#include "zephyr/zbus/zbus.h"
}

DEFINE_FFF_GLOBALS;

const struct zbus_channel kzbus_alarm_chan;

constexpr uint8_t kTypeAAlarmId{1};
constexpr alarm_origin_t kTypeAAlarmOrigin{kAlarmOriginTest1};
constexpr int kAlarmMaxAlarms{20};

std::vector<alarm_t> alarm_walk_entries_;
int walk_callback(const alarm_t alarm, void*) {
    alarm_walk_entries_.push_back(alarm);
    return 0;
}

class AlarmTests : public testing::Test {
  protected:
    void SetUp() override {
        AlarmInit();
        alarm_walk_entries_.clear();
    };
};

TEST_F(AlarmTests, TypeAAlarmNotActive_AlarmActiveTypeAAlarmsReturnsFalse) { ASSERT_FALSE(AlarmActiveTypeAAlarms()); }

TEST_F(AlarmTests, TypeAAlarmActive_AlarmActiveTypeAAlarmsReturnsTrue) {
    ASSERT_EQ(AlarmSet(true, kTypeAAlarmId, kTypeAAlarmOrigin), 0);
    ASSERT_TRUE(AlarmActiveTypeAAlarms());
}

TEST_F(AlarmTests, AlarmSet_ReturnNegativeWhenMaxIsReached) {
    for (int i = 0; i < kAlarmMaxAlarms; ++i) {
        ASSERT_EQ(AlarmSet(true, i, kAlarmOriginTest1), 0);
    }
    ASSERT_LT(AlarmSet(true, kAlarmMaxAlarms, kAlarmOriginTest1), 0);
}

TEST_F(AlarmTests, AlarmSetAFewEntriesAndWalk) {
    ASSERT_EQ(AlarmSet(true, 1, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 3, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmWalk(walk_callback, NULL), 0);
    ASSERT_EQ(alarm_walk_entries_.size(), 3);
    ASSERT_EQ(alarm_walk_entries_.at(0).id, 1 | kAlarmOriginTest1);
    ASSERT_EQ(alarm_walk_entries_.at(1).id, 2 | kAlarmOriginTest1);
    ASSERT_EQ(alarm_walk_entries_.at(2).id, 3 | kAlarmOriginTest1);
}

TEST_F(AlarmTests, AlarmSetAndClearAFewEntriesAndWalk) {
    ASSERT_EQ(AlarmSet(true, 1, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 3, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 4, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 5, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(false, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(false, 4, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmWalk(walk_callback, NULL), 0);
    ASSERT_EQ(alarm_walk_entries_.size(), 3);
    ASSERT_EQ(alarm_walk_entries_.at(0).id, 1 | kAlarmOriginTest1);
    ASSERT_EQ(alarm_walk_entries_.at(1).id, 3 | kAlarmOriginTest1);
    ASSERT_EQ(alarm_walk_entries_.at(2).id, 5 | kAlarmOriginTest1);
}

TEST_F(AlarmTests, AlarmSet_AlarmIsActiveReturnsTrue) {
    ASSERT_EQ(AlarmSet(true, 1, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 3, kAlarmOriginTest2), 0);

    alarm_origin_t origin;
    ASSERT_TRUE(AlarmIsActive(1, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest1);

    ASSERT_TRUE(AlarmIsActive(2, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest1);

    ASSERT_TRUE(AlarmIsActive(3, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest2);

    ASSERT_FALSE(AlarmIsActive(4, NULL));
}

TEST_F(AlarmTests, AlarmSetAndClearAFewEntries_AlarmIsActiveReturnsCorrect) {
    ASSERT_EQ(AlarmSet(true, 1, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 3, kAlarmOriginTest2), 0);
    ASSERT_EQ(AlarmSet(true, 4, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(true, 5, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(false, 2, kAlarmOriginTest1), 0);
    ASSERT_EQ(AlarmSet(false, 4, kAlarmOriginTest1), 0);

    alarm_origin_t origin;
    ASSERT_TRUE(AlarmIsActive(1, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest1);

    ASSERT_FALSE(AlarmIsActive(2, &origin));

    ASSERT_TRUE(AlarmIsActive(3, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest2);

    ASSERT_FALSE(AlarmIsActive(4, &origin));

    ASSERT_TRUE(AlarmIsActive(5, &origin));
    ASSERT_EQ(origin, kAlarmOriginTest1);
}
