// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PROCESS_INCLUDED
#define CMSX_KERNEL_PROCESS_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/object/Symbol.hpp>
#include <soulng/util/IntrusiveList.hpp>
#include <chrono>
#include <stdint.h>
#include <string>

namespace cmsx::kernel {

enum class ProcessState
{
    idle, running
};

class CMSX_KERNEL_API Process : soulng::util::IntrusiveListNode<Process>
{
public:
    Process(int32_t id_);
    int32_t Id() const { return id; }
    uint64_t RV() const { return rv; }
    void SetRV(uint64_t rv_) { rv = rv_; }
    const std::string& FilePath() const { return filePath; }
    void SetFilePath(const std::string& filePath_);
    ProcessState State() const { return state; }
    void SetState(ProcessState state_) { state = state_; }
    int64_t EntryPoint() const { return entryPoint; }
    void SetEntryPoint(int64_t entryPoint_) { entryPoint = entryPoint_; }
    int64_t CodeStartAddress() const { return codeStartAddress; }
    void SetCodeStartAddress(int64_t codeStartAddress_) { codeStartAddress = codeStartAddress_; }
    int64_t CodeLength() const { return codeLength; }
    void SetCodeLength(int64_t codeLength_) { codeLength = codeLength_; }
    int64_t DataStartAddress() const { return dataStartAddress; }
    void SetDataStartAddress(int64_t dataStartAddress_) { dataStartAddress = dataStartAddress_; }
    int64_t DataLength() const { return dataLength; }
    void SetDataLength(int64_t dataLength_) { dataLength = dataLength_; }
    int64_t ArgumentsStartAddress() const { return argumentsStartAddress; }
    void SetArgumentsStartAddress(int64_t argumentsStartAddress_) { argumentsStartAddress = argumentsStartAddress_; }
    int64_t ArgumentsLength() const { return argumentsLength; }
    void SetArgumentsLength(int64_t argumentsLength_) { argumentsLength = argumentsLength_; }
    int64_t EnvironmentStartAddress() const { return environmentStartAddress; }
    void SetEnvironmentStartAddress(int64_t environmentStartAddress_) { environmentStartAddress = environmentStartAddress_; }
    int64_t EnvironmentLength() const { return environmentLength; }
    void SetEnvironmentLength(int64_t environmentLength_) { environmentLength = environmentLength_; }
    int64_t HeapStartAddress() const { return heapStartAddress; }
    void SetHeapStartAddress(int64_t heapStartAddress_) { heapStartAddress = heapStartAddress_; }
    int64_t HeapLength() const { return heapLength; }
    void SetHeapLength(int64_t heapLength_) { heapLength = heapLength_; }
    int64_t StackStartAddress() const { return stackStartAddress; }
    void SetStackStartAddress(int64_t stackStackAddress_) { stackStartAddress = stackStackAddress_; }
    cmsx::object::SymbolTable* GetSymbolTable() const { return symbolTable.get(); }
    void SetSymbolTable(cmsx::object::SymbolTable* symbolTable_);
    void RemoveFromParent();
    std::chrono::steady_clock::duration UserTime() const { return userTime; }
    void AddUserTime(std::chrono::steady_clock::duration duration);
    std::chrono::steady_clock::duration SystemTime() const { return systemTime; }
    void AddSystemTime(std::chrono::steady_clock::duration duration);
    uint8_t ExitCode() const { return exitCode; }
    void SetExitCode(uint8_t exitCode_) { exitCode = exitCode_; }
private:
    int32_t id;
    uint64_t rv;
    std::string filePath;
    ProcessState state;
    int64_t entryPoint;
    int64_t codeStartAddress;
    int64_t codeLength;
    int64_t dataStartAddress;
    int64_t dataLength;
    int64_t argumentsStartAddress;
    int64_t argumentsLength;
    int64_t environmentStartAddress;
    int64_t environmentLength;
    int64_t heapStartAddress;
    int64_t heapLength;
    int64_t stackStartAddress;
    uint8_t exitCode;
    std::chrono::steady_clock::duration userTime;
    std::chrono::steady_clock::duration systemTime;
    std::unique_ptr<cmsx::object::SymbolTable> symbolTable;
};

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PROCESS_INCLUDED
