// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Process.hpp>

namespace cmsx::kernel {

Process::Process(int32_t id_) : 
    id(id_), rv(static_cast<uint64_t>(-1)), state(ProcessState::idle), 
    entryPoint(-1), codeStartAddress(-1), codeLength(0), dataStartAddress(-1), dataLength(0), 
    argumentsStartAddress(-1), argumentsLength(0), environmentStartAddress(-1), environmentLength(0), heapStartAddress(-1), heapLength(0), stackStartAddress(-1),
    userTime(0), systemTime(0)
{
}

void Process::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
}

void Process::SetSymbolTable(cmsx::object::SymbolTable* symbolTable_)
{
    symbolTable.reset(symbolTable_);
}

void Process::RemoveFromParent()
{
    if (Parent())
    {
        Parent()->RemoveChild(this);
    }
}

void Process::AddUserTime(std::chrono::steady_clock::duration duration)
{
    userTime = userTime + duration;
}

void Process::AddSystemTime(std::chrono::steady_clock::duration duration)
{
    systemTime = systemTime + duration;
}

} // namespace cmsx::kernel
