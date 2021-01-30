# Demos of SystemC TLM applications

 * test_build.cpp - make sure the SystemC library links correctly. Run an empty simulation.
 * test_run3.cpp - advance simulation by three steps, one nanosecond per step..
 * test_thread.cpp - create an active module and let it make three steps.
 * test_mem.cpp - create two modules: processor and memory. The processor writes and reads a few bytes to/from memory.

## Build

    git clone https://github.com/sergev/systemc-tlm-demo.git
    cd systemc-tlm-demo
    mkdir build
    cd build
    cmake ..
    make

## Tests

The tests can be run via 'make test' or directly with ctest:

    $ cd build
    $ make test
    Running tests...
    Test project /Users/vak/systemc-tlm-demo/build
        Start 1: test_build
    1/4 Test #1: test_build .......................   Passed    0.00 sec
        Start 2: test_run3
    2/4 Test #2: test_run3 ........................   Passed    0.00 sec
        Start 3: test_thread
    3/4 Test #3: test_thread ......................   Passed    0.00 sec
        Start 4: test_mem
    4/4 Test #4: test_mem .........................   Passed    0.00 sec

    100% tests passed, 0 tests failed out of 4

    Total Test time (real) =   0.02 sec

## Prerequisites

To build the SystemC library from sources for C++17 compiler, use:

    wget https://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.3.tar.gz
    tar xzf systemc-2.3.3.tar.gz
    cd systemc-2.3.3
    mkdir build
    cd build
    cmake .. -DCMAKE_CXX_STANDARD=17 -DCMAKE_INSTALL_PREFIX=/usr/local
    make
    make install
