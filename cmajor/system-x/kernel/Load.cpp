// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Load.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/ProcessManager.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Scheduler.hpp>
#include <system-x/kernel/Fs.hpp>
#include <system-x/kernel/Mount.hpp>
#include <system-x/machine/Registers.hpp>
#include <soulng/util/Util.hpp>

namespace cmsx::kernel {

TextSegmentWriteProtectionGuard::TextSegmentWriteProtectionGuard(uint64_t rv_, cmsx::machine::Memory& mem_) : rv(rv_), mem(mem_)
{
    mem.SetTextSegmentReadOnly(rv, false);
}

TextSegmentWriteProtectionGuard::~TextSegmentWriteProtectionGuard()
{
    mem.SetTextSegmentReadOnly(rv, true);
}

void SetupCode(cmsx::object::ExecutableFile* executable, cmsx::machine::Memory& memory, uint64_t rv)
{
    cmsx::object::CodeSection* codeSection = executable->GetCodeSection();
    int64_t codeSectionBaseAddress = codeSection->BaseAddress();
    for (int64_t i = 0; i < codeSection->Length(); ++i)
    {
        uint8_t value = codeSection->GetByte(i);
        memory.WriteByte(rv, static_cast<uint64_t>(codeSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
}

void SetupData(cmsx::object::ExecutableFile* executable, cmsx::machine::Memory& memory, uint64_t rv)
{
    cmsx::object::DataSection* dataSection = executable->GetDataSection();
    int64_t dataSectionBaseAddress = dataSection->BaseAddress();
    for (int64_t i = 0; i < dataSection->Length(); ++i)
    {
        uint8_t value = dataSection->GetByte(i);
        memory.WriteByte(rv, static_cast<uint64_t>(dataSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
}

void SetupStack(cmsx::machine::Memory& memory, uint64_t rv)
{
    int64_t stackStart = cmsx::machine::stackSegmentBaseAddress;
    memory.WriteOcta(rv, static_cast<uint64_t>(stackStart), 0, cmsx::machine::Protection::write);
}

int64_t SetupArgs(Process* process, int64_t address, const std::vector<std::string>& args, cmsx::machine::Memory& memory)
{
    uint64_t rv = process->RV();
    int64_t start = address;
    for (int32_t i = 0; i < args.size(); ++i)
    {
        memory.WriteOcta(rv, static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
        address = address + 8;
    }
    memory.WriteOcta(rv, static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
    address = address + 8;
    std::vector<uint64_t> argAddresses;
    for (int32_t i = 0; i < args.size(); ++i)
    {
        argAddresses.push_back(static_cast<uint64_t>(address));
        const std::string& arg = args[i];
        address = WriteString(process, arg, address, memory);
    }
    int64_t end = address;
    address = start;
    for (int32_t i = 0; i < argAddresses.size(); ++i)
    {
        memory.WriteOcta(rv, static_cast<uint64_t>(address), argAddresses[i], cmsx::machine::Protection::write);
        address = address + 8;
    }
    return end;
}

int64_t SetupEnv(Process* process, int64_t address, const std::vector<std::string>& env, cmsx::machine::Memory& memory)
{
    uint64_t rv = process->RV();
    int64_t start = address;
    for (int i = 0; i < env.size(); ++i)
    {
        memory.WriteOcta(rv, static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
        address = address + 8;
    }
    memory.WriteOcta(rv, static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
    address = address + 8;
    std::vector<uint64_t> envAddresses;
    for (int i = 0; i < env.size(); ++i)
    {
        envAddresses.push_back(static_cast<uint64_t>(address));
        const std::string& e = env[i];
        address = WriteString(process, e, address, memory);
    }
    int64_t end = address;
    address = start;
    for (int64_t i = 0; i < envAddresses.size(); ++i)
    {
        memory.WriteOcta(rv, static_cast<uint64_t>(address), envAddresses[i], cmsx::machine::Protection::write);
        address = address + 8;
    }
    return end;
}

void Load(Process* process, const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine)
{
    uint64_t rv = machine.Mem().AllocateTranslationMap();
    std::unique_ptr<cmsx::object::BinaryFile> binaryFile(cmsx::object::ReadBinaryFile(process->FilePath()));
    process->SetFilePath(MapPath(process->FilePath()));
    Load(process, binaryFile.get(), args, env, machine, rv, true);
}

void Load(Process* process, cmsx::object::BinaryFile* binaryFile,
    const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine, uint64_t rv, bool addRunnable)
{
    TextSegmentWriteProtectionGuard guard(rv, machine.Mem());
    cmsx::machine::Registers regs;
    process->SetRV(rv);
    int argCount = args.size();
    if (binaryFile->Kind() == cmsx::object::BinaryFileKind::executableFile)
    {
        cmsx::object::ExecutableFile* executable = static_cast<cmsx::object::ExecutableFile*>(binaryFile);
        regs.Set(cmsx::machine::regSP, cmsx::machine::stackSegmentBaseAddress);
        SetupCode(executable, machine.Mem(), rv);
        Region textRegion(RegionId::text, executable->GetCodeSection()->BaseAddress(), executable->GetCodeSection()->Length());
        process->GetRegionTable().AddRegion(textRegion);
        Region dataRegion(RegionId::data, executable->GetDataSection()->BaseAddress(), executable->GetDataSection()->Length());
        SetupData(executable, machine.Mem(), rv);
        process->GetRegionTable().AddRegion(dataRegion);
        SetupStack(machine.Mem(), rv);
        Region stackRegion(RegionId::stack, cmsx::machine::stackSegmentBaseAddress, 8);
        process->GetRegionTable().AddRegion(stackRegion);
        int64_t poolSegmentBaseAddress = cmsx::machine::poolSegmentBaseAddress;
        int64_t address = poolSegmentBaseAddress;
        int64_t argsAddress = soulng::util::Align(address, 8);
        process->SetArgumentsStartAddress(argsAddress);
        address = SetupArgs(process, argsAddress, args, machine.Mem());
        process->SetArgumentsLength(address - argsAddress);
        int64_t envAddress = soulng::util::Align(address, 8);
        process->SetEnvironmentStartAddress(envAddress);
        address = SetupEnv(process, envAddress, env, machine.Mem());
        process->SetEnvironmentLength(address - envAddress);
        address = soulng::util::Align(address, 4096);
        Region poolRegion(RegionId::pool, poolSegmentBaseAddress, address - poolSegmentBaseAddress);
        process->GetRegionTable().AddRegion(poolRegion);
        process->SetHeapStartAddress(address);
        process->SetHeapLength(0);
        cmsx::object::Symbol* main = executable->GetSymbolTable().GetSymbol("Main");
        if (main)
        {
            int64_t entryPoint = main->Start();
            process->SetEntryPoint(entryPoint);
            regs.SetPC(static_cast<uint64_t>(entryPoint));
            int64_t mainFrame = cmsx::machine::stackSegmentBaseAddress;
            int64_t mainArgAddr = mainFrame + 8;
            machine.Mem().WriteOcta(rv, static_cast<uint64_t>(mainArgAddr), static_cast<uint32_t>(argCount), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(rv, static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(argsAddress), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(rv, static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(envAddress), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            std::shared_ptr<cmsx::object::SymbolTable> symbolTable(executable->ReleaseSymbolTable());
            process->SetSymbolTable(symbolTable);
            AddTrapsToSymbolTable(*process->GetSymbolTable());
            regs.Set(cmsx::machine::regSP, cmsx::machine::stackSegmentBaseAddress);
            process->SaveContext(machine, regs);
            if (addRunnable)
            {
                ProcessManager::Instance().IncrementRunnableProcesses();
                Scheduler::Instance().AddRunnableProcess(process, cmsx::machine::ProcessState::runnableInUser);
            }
        }
        else
        {
            throw SystemError(ENOTFOUND, "error loading file '" + process->FilePath() + "': 'Main' entry point not found");
        }
    }
    else
    {
        throw SystemError(EFAIL, "error loading file '" + process->FilePath() + "': executable file expected");
    }
}

} // namespace cmsx::kernel
