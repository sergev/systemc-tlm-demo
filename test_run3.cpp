#include <gtest/gtest.h>
#include <systemc.h>

//
// Run a fixed number of steps.
//
TEST(tlm, run3)
{
    // Set the clock frequency to 1 GHz.
    sc_core::sc_time const clock_period(1, sc_core::SC_NS);

    // Wait for SystemC initialization.
    sc_core::sc_start(sc_core::SC_ZERO_TIME);

    // Step the model forward.
    std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
    sc_core::sc_start(clock_period * 1);

    std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
    sc_core::sc_start(clock_period * 1);

    std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
    sc_core::sc_start(clock_period * 1);

    // Stop the SystemC simulation.
    sc_core::sc_stop();

    EXPECT_EQ(sc_core::sc_time_stamp(), clock_period * 3);
}

#include "main.cpp"
