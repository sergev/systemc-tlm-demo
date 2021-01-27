#include <gtest/gtest.h>
#include <systemc.h>
#include <tlm_utils/simple_target_socket.h>

//
// Set the clock frequency to 1 GHz.
//
const sc_core::sc_time clock_period(1, sc_core::SC_NS);

//
// Define a passive module: generic memory.
//
class Memory : public sc_core::sc_module {
private:
    uint8_t *mem_buf;
    uint64_t mem_size;

public:
    // Socket for target TLM2 transactions.
    tlm_utils::simple_target_socket<Memory> socket;

    const sc_time latency;

    Memory(sc_core::sc_module_name name, sc_time lat, uint64_t size)
        : sc_module(name), mem_size(size), socket("socket"), latency(lat)
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

    //
    // TLM2 methods.
    //
    virtual void b_transport(tlm::tlm_generic_payload& trans, sc_time& delay)
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
            break;
        case tlm::TLM_WRITE_COMMAND:
            memcpy(&mem_buf[addr], ptr, len);
            break;
        case tlm::TLM_IGNORE_COMMAND:
            break;
        }
        delay += latency;

        trans.set_dmi_allowed(true);
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }

    virtual bool get_direct_mem_ptr(tlm::tlm_generic_payload& trans,
                                    tlm::tlm_dmi& dmi_data)
    {
        dmi_data.allow_read_write();

        dmi_data.set_dmi_ptr(reinterpret_cast<unsigned char*>(&mem_buf[0]));
        dmi_data.set_start_address(0);
        dmi_data.set_end_address(mem_size - 1);
        dmi_data.set_read_latency(latency);
        dmi_data.set_write_latency(latency);
        return true;
    }

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
// Test the Memory module.
//
TEST(tlm, mem)
{
    Memory mem("memory", clock_period * 3, 1024 * 1024);

    sc_core::sc_start();
    EXPECT_EQ(sc_core::sc_time_stamp(), clock_period * 3);
}
