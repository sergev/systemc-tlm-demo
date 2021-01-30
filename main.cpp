//
// Main routine for C++.
// Please, include this file into your code.
//
int sc_main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    int status = RUN_ALL_TESTS();

    return status;
}
