#include <gtest/gtest.h>
#include <systemc.h>

//
// Set the clock frequency to 1 GHz.
//
const sc_core::sc_time clock_period(1, sc_core::SC_NS);

//
// Define a module which has a thread.
//
class Processor : public sc_core::sc_module {

public:
    SC_HAS_PROCESS(Processor);

    Processor(sc_core::sc_module_name name) : sc_core::sc_module(name)
    {
        SC_THREAD(main_thread);
    }

    virtual ~Processor() {}

private:
    void main_thread()
    {
        // Wait for SystemC initialization.
        sc_core::wait(sc_core::SC_ZERO_TIME);

        // Step the model forward a few cycles.
        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        // Stop the SystemC simulation.
        sc_core::sc_stop();
        wait(sc_core::SC_ZERO_TIME);
    }
};

//
// Run a module which has a thread.
//
TEST(tlm, thread)
{
    Processor cpu("cpu");

    sc_core::sc_start();
    EXPECT_EQ(sc_core::sc_time_stamp(), clock_period * 3);
}
