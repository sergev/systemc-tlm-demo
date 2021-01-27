#include <systemc.h>
#include <gtest/gtest.h>

int sc_main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();

    return status;
}
