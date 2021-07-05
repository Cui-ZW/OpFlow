#include "DataStructures/Index/MDIndex.hpp"
#include "DataStructures/Index/RangedIndex.hpp"
#include <gmock/gmock.h>

using namespace OpFlow;

template struct DS::MDIndex<3>;

class MDIndexTest : public testing::Test {
protected:
    void SetUp() override {}

    DS::MDIndex<3> i3;
};

TEST_F(MDIndexTest, InitZeroTest) {
    ASSERT_EQ(i3[0], 0);
    ASSERT_EQ(i3[1], 0);
    ASSERT_EQ(i3[2], 0);
}

TEST_F(MDIndexTest, InitFromIntList) {
    i3 = DS::MDIndex<3>(1, 2, 3);
    ASSERT_EQ(i3[0], 1);
    ASSERT_EQ(i3[1], 2);
    ASSERT_EQ(i3[2], 3);
}