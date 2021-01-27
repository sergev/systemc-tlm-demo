#include <gtest/gtest.h>
#include <systemc.h>

TEST(tlm, build)
{
    sc_core::sc_start();
    EXPECT_EQ(sc_core::sc_time_stamp(), sc_core::SC_ZERO_TIME);
}
