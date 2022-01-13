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
const uint8_t trap_write = 7;
const uint8_t trap_get_current_time_point = 8;
const uint8_t trap_sleep = 9;
const uint8_t trap_get_current_date = 10;
const uint8_t trap_get_current_date_time = 11;
const uint8_t trap_pow = 12;

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
