# Demos of SystemC TLM applications

 * test_build.cpp - make sure the SystemC library links correctly. Run an empty simulation.
 * test_run3.cpp - advance simulation by three steps, one nanosecond per step..
 * test_thread.cpp - create an active module and let it make three steps.
 * test_mem.cpp - create two modules: processor and memory. The processor writes and reads a few bytes to/from memory.

## Build

    mkdir build
    cd build
    cmake ..
    make

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
