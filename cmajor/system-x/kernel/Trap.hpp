// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_TRAP_INCLUDED
#define CMSX_KERNEL_TRAP_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/object/Symbol.hpp>

namespace cmsx::kernel {

const uint8_t trap_exit = 0;
const uint8_t trap_memory_page_size = 1;
const uint8_t trap_heap_start = 2;
const uint8_t trap_heap_length = 3;
const uint8_t trap_allocate_memory_pages = 4;
const uint8_t trap_dump_heap = 5;
const uint8_t trap_random_seed = 6;
const uint8_t trap_current_time_point = 7;
const uint8_t trap_sleep = 8;
const uint8_t trap_current_date = 9;
const uint8_t trap_current_date_time = 10;
const uint8_t trap_times = 11;
const uint8_t trap_pow = 12;
const uint8_t trap_throw = 13;
const uint8_t trap_catch = 14;
const uint8_t trap_resume = 15;
const uint8_t trap_stack_trace = 16;
const uint8_t trap_get_system_error = 17;
const uint8_t trap_fork = 18;
const uint8_t trap_wait = 19;
const uint8_t trap_getpid = 20;
const uint8_t trap_create = 21;
const uint8_t trap_open = 22;
const uint8_t trap_close = 23;
const uint8_t trap_read = 24;
const uint8_t trap_write = 25;
const uint8_t trap_ioctl = 26;
const uint8_t trap_unlink = 27;
const uint8_t trap_seek = 28;
const uint8_t trap_tell = 29;
const uint8_t trap_stat = 30;
const uint8_t trap_getcwd = 31;
const uint8_t trap_chdir = 32;
const uint8_t trap_mkdir = 33;
const uint8_t trap_opendir = 34;
const uint8_t trap_closedir = 35;
const uint8_t trap_readdir = 36;
const uint8_t trap_utime = 37;
const uint8_t trap_mcpy = 38;

class CMSX_KERNEL_API TrapHandler
{
public:
    virtual ~TrapHandler();
    virtual uint64_t HandleTrap(cmsx::machine::Processor& processor) = 0;
    virtual std::string TrapName() const = 0;
};

CMSX_KERNEL_API void SetTrapHandler(uint8_t trap, TrapHandler* handler);
CMSX_KERNEL_API void AddTrapsToSymbolTable(cmsx::object::SymbolTable& symbolTable);
CMSX_KERNEL_API void InitTrap();
CMSX_KERNEL_API void DoneTrap();

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_TRAP_INCLUDED
