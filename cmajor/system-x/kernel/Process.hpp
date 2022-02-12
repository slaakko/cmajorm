// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_KERNEL_PROCESS_INCLUDED
#define CMSX_KERNEL_PROCESS_INCLUDED
#include <system-x/kernel/Api.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/File.hpp>
#include <system-x/kernel/Region.hpp>
#include <system-x/object/Symbol.hpp>
#include <system-x/object/FunctionTable.hpp>
#include <system-x/machine/Debugger.hpp>
#include <system-x/machine/Process.hpp>
#include <soulng/util/IntrusiveList.hpp>
#include <chrono>
#include <stdint.h>
#include <string>
#include <list>

namespace cmsx::kernel {

class CMSX_KERNEL_API Process : public soulng::util::IntrusiveListNode<Process>, public cmsx::machine::UserProcess
{
public:
    Process(int32_t id_);
    int32_t Id() const override { return id; }
    uint64_t RV() const override { return rv; }
    void SetRV(uint64_t rv_) { rv = rv_; }
    uint64_t AXAddress() const { return axAddress; }
    uint64_t BXAddress() const { return bxAddress; }
    uint64_t CXAddress() const { return cxAddress; }
    const std::string& FilePath() const { return filePath; }
    void SetFilePath(const std::string& filePath_);
    cmsx::machine::ProcessState State() const override { return state; }
    void SetState(cmsx::machine::ProcessState state_);
    int64_t EntryPoint() const { return entryPoint; }
    void SetEntryPoint(int64_t entryPoint_) { entryPoint = entryPoint_; }
    int64_t ArgumentsStartAddress() const { return argumentsStartAddress; }
    void SetArgumentsStartAddress(int64_t argumentsStartAddress_) { argumentsStartAddress = argumentsStartAddress_; }
    int64_t ArgumentsLength() const { return argumentsLength; }
    void SetArgumentsLength(int64_t argumentsLength_) { argumentsLength = argumentsLength_; }
    int64_t EnvironmentStartAddress() const { return environmentStartAddress; }
    void SetEnvironmentStartAddress(int64_t environmentStartAddress_) { environmentStartAddress = environmentStartAddress_; }
    int64_t EnvironmentLength() const { return environmentLength; }
    void SetEnvironmentLength(int64_t environmentLength_) { environmentLength = environmentLength_; }
    int64_t HeapStartAddress() const override { return heapStartAddress; }
    void SetHeapStartAddress(int64_t heapStartAddress_) { heapStartAddress = heapStartAddress_; }
    int64_t HeapLength() const override { return heapLength; }
    void SetHeapLength(int64_t heapLength_) override;
    void SetAddressesFrom(Process* parent);
    cmsx::object::SymbolTable* GetSymbolTable() const { return symbolTable.get(); }
    const std::shared_ptr<cmsx::object::SymbolTable>& GetSymbolTablePtr() const { return symbolTable; }
    void SetSymbolTable(const std::shared_ptr<cmsx::object::SymbolTable>& symbolTable_);
    cmsx::object::FunctionTable* GetFunctionTable();
    void RemoveFromParent();
    uint8_t ExitCode() const { return exitCode; }
    void SetExitCode(uint8_t exitCode_) { exitCode = exitCode_; }
    void Exit(uint8_t exitCode_) override;
    void SaveContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs) override;
    void RestoreContext(cmsx::machine::Machine& machine, cmsx::machine::Registers& regs) override;
    void SetRunning(cmsx::machine::Processor* processor_) override;
    void ResetProcessor() override;
    cmsx::machine::Debugger* GetDebugger() const override;
    void SetDebugger(cmsx::machine::Debugger* debugger_) override;
    cmsx::machine::Processor* GetProcessor() const override { return processor; }
    void SetProcessor(cmsx::machine::Processor* processor_) { processor = processor_; }
    void SetStartUserTime() override;
    void SetStartSleepTime() override;
    void SetStartSystemTime() override;
    void AddUserTime() override;
    void AddSleepTime() override;
    void AddSystemTime() override;
    const std::chrono::steady_clock::duration& UserTime() const { return userTime; }
    const std::chrono::steady_clock::duration& SleepTime() const { return sleepTime; }
    const std::chrono::steady_clock::duration& SystemTime() const { return systemTime; }
    RegionTable& GetRegionTable() { return regionTable; }
    ProcessFileTable& GetFileTable() { return fileTable; }
    void SetError(const SystemError& error_);
    SystemError GetError() const { return error; }
    uint64_t CurrentExceptionAddress() const { return currentExceptionAddress; }
    void SetCurrentExceptionAddress(uint64_t exceptionAddress) { currentExceptionAddress = exceptionAddress; }
    uint64_t CurrentExceptionClassId() const { return currentExceptionClassId; }
    void SetCurrentExceptionClassId(uint64_t exceptionClassId) { currentExceptionClassId = exceptionClassId; }
    cmsx::object::TryRecord* CurrentTryRecord() const { return currentTryRecord; }
    void SetCurrentTryRecord(cmsx::object::TryRecord* tryRecord) { currentTryRecord = tryRecord; }
    void SetKernelFiber(void* kernelFiber_) override { kernelFiber = kernelFiber_; }
    void* KernelFiber() const override { return kernelFiber; }
    void DeleteKernelFiber();
    uint64_t GetINodeKeyOfWorkingDir() const override { return inodeKeyOfWorkingDirAsULong; }
    void SetINodeKeyOfWorkingDir(uint64_t inodeKeyOfWorkingDirAsULong_) override { inodeKeyOfWorkingDirAsULong = inodeKeyOfWorkingDirAsULong_; }
private:
    int32_t id;
    uint64_t rv;
    uint64_t kernelSP;
    uint64_t axAddress;
    uint64_t bxAddress;
    uint64_t cxAddress;
    std::string filePath;
    cmsx::machine::ProcessState state;
    int64_t entryPoint;
    int64_t argumentsStartAddress;
    int64_t argumentsLength;
    int64_t environmentStartAddress;
    int64_t environmentLength;
    int64_t heapStartAddress;
    int64_t heapLength;
    int64_t stackStartAddress;
    uint8_t exitCode;
    std::chrono::steady_clock::time_point startUserTime;
    std::chrono::steady_clock::time_point startSleepTime;
    std::chrono::steady_clock::time_point startSystemTime;
    std::chrono::steady_clock::duration userTime;
    std::chrono::steady_clock::duration sleepTime;
    std::chrono::steady_clock::duration systemTime;
    std::shared_ptr<cmsx::object::SymbolTable> symbolTable;
    cmsx::machine::Debugger* debugger;
    cmsx::machine::Processor* processor;
    RegionTable regionTable;
    ProcessFileTable fileTable;
    std::unique_ptr<cmsx::object::FunctionTable> functionTable;
    SystemError error;
    uint64_t currentExceptionAddress;
    uint64_t currentExceptionClassId;
    cmsx::object::TryRecord* currentTryRecord;
    void* userFiber;
    void* kernelFiber;
    uint64_t inodeKeyOfWorkingDirAsULong;
};

CMSX_KERNEL_API int32_t Fork(Process* parent);
CMSX_KERNEL_API int32_t Wait(Process* parent, int64_t childExitCodeAddress);
CMSX_KERNEL_API void Exec(Process* process, int64_t filePathAddress, int64_t argvAddress, int64_t envpAddress);

} // namespace cmsx::kernel

#endif // CMSX_KERNEL_PROCESS_INCLUDED
