#include <gtest/gtest.h>
#include <systemc.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/simple_initiator_socket.h>

//
// Set the clock frequency to 1 GHz.
//
const sc_core::sc_time clock_period(1, sc_core::SC_NS);

//
// Define a passive module: generic memory.
//
class Memory : public sc_core::sc_module {
private:
    // Memory storage.
    uint8_t *mem_buf;

    // Size of allocated memory in bytes.
    uint64_t mem_size;

    // Latency of memory access.
    const sc_core::sc_time mem_latency;

public:
    // Socket for receiving TLM2 transactions.
    tlm_utils::simple_target_socket<Memory> socket;

    //
    // Allocate a memory of specified size, with specified latency.
    //
    Memory(sc_core::sc_module_name name, uint64_t size, sc_core::sc_time lat)
        : sc_module(name), mem_size(size), mem_latency(lat), socket("socket")
    {
        // Allocate the memory and fill it with zeroes.
	mem_buf = (uint8_t*) calloc(mem_size, sizeof(uint8_t));
	if (mem_buf == nullptr)
	    throw std::runtime_error("Cannot allocate memory");

        // Connect methods to the socket.
	socket.register_b_transport(this, &Memory::b_transport);
	socket.register_get_direct_mem_ptr(this, &Memory::get_direct_mem_ptr);
	socket.register_transport_dbg(this, &Memory::transport_dbg);
    }

    ~Memory()
    {
        free(mem_buf);
    }

private:
    //
    // TLM2 method: blocking transport interface.
    // Process the given transaction.
    // Return the delay.
    //
    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& delay)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    addr = trans.get_address();
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned char*   byt = trans.get_byte_enable_ptr();

        if (addr + len > sc_dt::uint64(mem_size)) {
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            SC_REPORT_FATAL("Memory", "Unsupported access\n");
            return;
        }
        if (byt != 0) {
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            SC_REPORT_FATAL("Memory", "Unsupported access\n");
            return;
        }

        switch (cmd) {
        case tlm::TLM_READ_COMMAND:
            memcpy(ptr, &mem_buf[addr], len);
            std::cout << sc_core::sc_time_stamp()
                      << ": read [0x" << std::hex << addr
                      << "] -> 0x" << (unsigned)*ptr << std::dec << std::endl;
            break;
        case tlm::TLM_WRITE_COMMAND:
            std::cout << sc_core::sc_time_stamp()
                      << ": write [0x" << std::hex << addr
                      << "] <- 0x" << (unsigned)*ptr << std::dec << std::endl;
            memcpy(&mem_buf[addr], ptr, len);
            break;
        case tlm::TLM_IGNORE_COMMAND:
            break;
        }
        delay += mem_latency;

        trans.set_dmi_allowed(true);
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    //
    // TLM2 method: fill the dmi_data structure with a direct pointer to the memory buffer.
    //
    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi& dmi_data)
    {
        dmi_data.allow_read_write();

        dmi_data.set_dmi_ptr(reinterpret_cast<unsigned char*>(&mem_buf[0]));
        dmi_data.set_start_address(0);
        dmi_data.set_end_address(mem_size - 1);
        dmi_data.set_read_latency(mem_latency);
        dmi_data.set_write_latency(mem_latency);
        return true;
    }

    //
    // TLM2 method: debug transport interface.
    // A backdoor to the memory contents.
    // Takes zero time.
    //
    virtual unsigned int transport_dbg(tlm::tlm_generic_payload& trans)
    {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64    addr = trans.get_address();
        unsigned char*   ptr = trans.get_data_ptr();
        unsigned int     len = trans.get_data_length();
        unsigned int     num_bytes = (len + addr < mem_size) ? len : (mem_size - addr);

        switch (cmd) {
        case tlm::TLM_READ_COMMAND:
            memcpy(ptr, &mem_buf[addr], num_bytes);
            break;
        case tlm::TLM_WRITE_COMMAND:
            memcpy(&mem_buf[addr], ptr, num_bytes);
            break;
        case tlm::TLM_IGNORE_COMMAND:
            break;
        }
        return num_bytes;
    }
};

//
// Active module.
//
class Master : public sc_core::sc_module {
public:
    // Socket for sending TLM2 transactions.
    tlm_utils::simple_initiator_socket<Master> socket;

    // This module is active: it has a thread attached.
    SC_HAS_PROCESS(Master);

    //
    // Allocate a master object with given name.
    //
    Master(sc_core::sc_module_name name)
        : sc_core::sc_module(name),
          socket("socket")
    {
        SC_THREAD(main_thread);
    }

    virtual ~Master() {}

private:
    //
    // Main thread of the master module.
    //
    void main_thread()
    {
        // Wait for SystemC initialization.
        sc_core::wait(sc_core::SC_ZERO_TIME);

        // Cycle 0: idle.
        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        // Cycles 1-3: write memory.
        write8(0x123, 0xa5);

        // Cycle 4: idle.
        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        // Cycles 5-7: read memory.
        auto val = read8(0x456);
        EXPECT_EQ(val, 0x00);

        // Cycle 8: idle.
        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        // Cycles 9-11: read memory.
        val = read8(0x123);
        EXPECT_EQ(val, 0xa5);

        // Cycle 12: idle.
        std::cout << sc_core::sc_time_stamp() << ": step" << std::endl;
        wait(clock_period * 1);

        // Stop the SystemC simulation.
        sc_core::sc_stop();
        wait(sc_core::SC_ZERO_TIME);

        EXPECT_EQ(sc_core::sc_time_stamp(), clock_period * 13);
    }

    //
    // Template: read N bytes from memory.
    // Return true on success.
    //
    template <typename TYPE>
    bool read(uint64_t address, TYPE* data)
    {
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_command(tlm::TLM_READ_COMMAND);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(data));
        trans.set_data_length(sizeof(TYPE));
        trans.set_streaming_width(sizeof(TYPE));

        sc_core::sc_time latency{ sc_core::SC_ZERO_TIME };
        socket->b_transport(trans, latency);
        wait(latency);

        if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
            SC_REPORT_FATAL("Read", "Bad response\n");
            return false;
        }
        return true;
    }

    //
    // Template: write N bytes to memory.
    // Return true on success.
    //
    template <typename TYPE>
    bool write(uint64_t address, TYPE* data)
    {
        tlm::tlm_generic_payload trans;
        trans.set_address(address);
        trans.set_command(tlm::TLM_WRITE_COMMAND);
        trans.set_data_ptr(reinterpret_cast<unsigned char*>(data));
        trans.set_data_length(sizeof(TYPE));
        trans.set_streaming_width(sizeof(TYPE));

        sc_core::sc_time latency{ sc_core::SC_ZERO_TIME };
        socket->b_transport(trans, latency);
        wait(latency);

        if (trans.get_response_status() != tlm::TLM_OK_RESPONSE) {
            SC_REPORT_FATAL("Write", "Bad response\n");
            return false;
        }
        return true;
    }

    //
    // Read one byte from memory.
    //
    uint8_t read8(uint64_t address) { uint8_t data{}; read(address, &data); return data; }

    //
    // Write one byte to memory.
    //
    void write8(uint64_t address, uint8_t data) { write(address, &data); }
};

//
// Test the Memory module.
//
TEST(tlm, mem)
{
    // Instantiate modules.
    Memory mem("memory", 1024 * 1024, clock_period * 3);
    Master master("master");

    // Connect sockets.
    master.socket(mem.socket);

    // Start simulation.
    sc_core::sc_start();
}
