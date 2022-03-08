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
const uint8_t trap_exec = 19;
const uint8_t trap_wait = 20;
const uint8_t trap_getpid = 21;
const uint8_t trap_create = 22;
const uint8_t trap_open = 23;
const uint8_t trap_close = 24;
const uint8_t trap_read = 25;
const uint8_t trap_write = 26;
const uint8_t trap_ioctl = 27;
const uint8_t trap_unlink = 28;
const uint8_t trap_seek = 29;
const uint8_t trap_tell = 30;
const uint8_t trap_stat = 31;
const uint8_t trap_getcwd = 32;
const uint8_t trap_chdir = 33;
const uint8_t trap_mkdir = 34;
const uint8_t trap_opendir = 35;
const uint8_t trap_closedir = 36;
const uint8_t trap_readdir = 37;
const uint8_t trap_utime = 38;
const uint8_t trap_mcpy = 39;
const uint8_t trap_open_resource = 40;
const uint8_t trap_close_resource = 41;
const uint8_t trap_get_resource_size = 42;
const uint8_t trap_read_resource = 43;
const uint8_t trap_decompress = 44;
const uint8_t trap_get_decompressed_data_size = 45;
const uint8_t trap_get_decompressed_data = 46;
const uint8_t trap_close_decompression = 47;
const uint8_t trap_get_host_name = 48;
const uint8_t trap_get_user_name = 49;
const uint8_t trap_pipe = 50;
const uint8_t trap_dup = 51;
const uint8_t trap_setuid = 52;
const uint8_t trap_setgid = 53;
const uint8_t trap_getuid = 54;
const uint8_t trap_getgid = 55;
const uint8_t trap_umask = 56;
const uint8_t trap_chmod = 57;
const uint8_t trap_chown = 58;
const uint8_t trap_rename = 59;
const uint8_t trap_add_dir_change_notification = 60;
const uint8_t trap_remove_dir_change_notifications = 61;
const uint8_t trap_directories_changed = 62;
const uint8_t clear_directories_changed = 63;

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
