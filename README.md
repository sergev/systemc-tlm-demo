Demos of SystemC TLM2 applications.

Build:
~~~~~~
    mkdir build
    cd build
    cmake ..
    make

Prerequisites:
~~~~~~~~~~~~~~

    On Mac, a SystemC 2.3.3 version should be installed for c++17 compiler:

        brew install --build-bottle systemc-2-3-3.rb

Available demos:
~~~~~~~~~~~~~~~~
 * test_build.cpp - make sure the SystemC library links correctly. Run an empty simulation.
 * test_run3.cpp - advance simulation by three steps, one nanosecond per step..
 * test_thread.cpp - create an active module and let it make three steps.
 * test_mem.cpp - create two modules: processor and memory. The processor writes and reads a few bytes to/from memory.
