// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Load.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <soulng/util/Util.hpp>

namespace cmsx::kernel {

void SetupCode(cmsx::object::ExecutableFile* executable, cmsx::machine::Memory& memory)
{
    cmsx::object::CodeSection* codeSection = executable->GetCodeSection();
    int64_t codeSectionBaseAddress = codeSection->BaseAddress();
    for (int64_t i = 0; i < codeSection->Length(); ++i)
    {
        uint8_t value = codeSection->GetByte(i);
        memory.WriteByte(static_cast<uint64_t>(codeSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
}

void SetupData(cmsx::object::ExecutableFile* executable, cmsx::machine::Memory& memory)
{
    cmsx::object::DataSection* dataSection = executable->GetDataSection();
    int64_t dataSectionBaseAddress = dataSection->BaseAddress();
    for (int64_t i = 0; i < dataSection->Length(); ++i)
    {
        uint8_t value = dataSection->GetByte(i);
        memory.WriteByte(static_cast<uint64_t>(dataSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
}

void SetupStack(cmsx::machine::Memory& memory)
{
    int64_t stackStart = cmsx::machine::stackSegmentBaseAddress;
    memory.WriteOcta(static_cast<uint64_t>(stackStart), 0, cmsx::machine::Protection::write);
}

int64_t WriteString(const std::string& s, int64_t address, cmsx::machine::Memory& memory)
{
    for (char c : s)
    {
        memory.WriteByte(static_cast<uint64_t>(address), static_cast<uint8_t>(c), cmsx::machine::Protection::write);
        ++address;
    }
    memory.WriteByte(static_cast<uint64_t>(address), static_cast<uint8_t>(0), cmsx::machine::Protection::write);
    ++address;
    return address;
}

int64_t SetupArgs(int64_t address, const std::vector<std::string>& args, cmsx::machine::Memory& memory)
{
    int64_t start = address;
    for (int32_t i = 0; i < args.size(); ++i)
    {
        memory.WriteOcta(static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
        address = address + 8;
    }
    memory.WriteOcta(static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
    address = address + 8;
    std::vector<uint64_t> argAddresses;
    for (int32_t i = 0; i < args.size(); ++i)
    {
        argAddresses.push_back(static_cast<uint64_t>(address));
        const std::string& arg = args[i];
        address = WriteString(arg, address, memory);
    }
    int64_t end = address;
    address = start;
    for (int32_t i = 0; i < argAddresses.size(); ++i)
    {
        memory.WriteOcta(static_cast<uint64_t>(address), argAddresses[i], cmsx::machine::Protection::write);
        address = address + 8;
    }
    return end;
}

int64_t SetupEnv(int64_t address, const std::vector<std::string>& env, cmsx::machine::Memory& memory)
{
    int64_t start = address;
    for (int i = 0; i < env.size(); ++i)
    {
        memory.WriteOcta(static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
        address = address + 8;
    }
    memory.WriteOcta(static_cast<uint64_t>(address), 0, cmsx::machine::Protection::write);
    address = address + 8;
    std::vector<uint64_t> envAddresses;
    for (int i = 0; i < env.size(); ++i)
    {
        envAddresses.push_back(static_cast<uint64_t>(address));
        const std::string& e = env[i];
        address = WriteString(e, address, memory);
    }
    int64_t end = address;
    address = start;
    for (int64_t i = 0; i < envAddresses.size(); ++i)
    {
        memory.WriteOcta(static_cast<uint64_t>(address), envAddresses[i], cmsx::machine::Protection::write);
        address = address + 8;
    }
    return end;
}

void Load(Process* process, const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine)
{
    uint64_t rv = machine.Mem().AllocateTranslationMap();
    process->SetRV(rv);
    int argCount = args.size();
    std::unique_ptr<cmsx::object::BinaryFile> binaryFile(cmsx::object::ReadBinaryFile(process->FilePath()));
    if (binaryFile->Kind() == cmsx::object::BinaryFileKind::executableFile)
    {
        cmsx::object::ExecutableFile* executable = static_cast<cmsx::object::ExecutableFile*>(binaryFile.get());
        process->SetCodeStartAddress(executable->GetCodeSection()->BaseAddress());
        process->SetCodeLength(executable->GetCodeSection()->Length());
        machine.Regs().SetSpecial(cmsx::machine::rV, process->RV());
        machine.Regs().Set(cmsx::machine::regSP, cmsx::machine::stackSegmentBaseAddress);
        SetupCode(executable, machine.Mem());
        process->SetDataStartAddress(executable->GetDataSection()->BaseAddress());
        process->SetDataLength(executable->GetDataSection()->Length());
        SetupData(executable, machine.Mem());
        SetupStack(machine.Mem());
        process->SetStackStartAddress(cmsx::machine::stackSegmentBaseAddress);
        int64_t poolSegmentBaseAddress = cmsx::machine::poolSegmentBaseAddress;
        int64_t address = poolSegmentBaseAddress;
        int64_t argsAddress = soulng::util::Align(address, 8);
        process->SetArgumentsStartAddress(argsAddress);
        address = SetupArgs(argsAddress, args, machine.Mem());
        process->SetArgumentsLength(address - argsAddress);
        int64_t envAddress = soulng::util::Align(address, 8);
        process->SetEnvironmentStartAddress(envAddress);
        address = SetupEnv(envAddress, env, machine.Mem());
        process->SetEnvironmentLength(address - envAddress);
        address = soulng::util::Align(address, 4096);
        process->SetHeapStartAddress(address);
        process->SetHeapLength(0);
        cmsx::object::Symbol* main = executable->GetSymbolTable().GetSymbol("Main");
        if (main)
        {
            int64_t entryPoint = main->Start();
            process->SetEntryPoint(entryPoint);
            machine.Regs().SetPC(static_cast<uint64_t>(entryPoint));
            int64_t mainFrame = cmsx::machine::stackSegmentBaseAddress;
            int64_t mainArgAddr = mainFrame + 8;
            machine.Mem().WriteOcta(static_cast<uint64_t>(mainArgAddr), static_cast<uint32_t>(argCount), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(argsAddress), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(envAddress), cmsx::machine::Protection::write);
            process->SetSymbolTable(executable->ReleaseSymbolTable());
            AddTrapsToSymbolTable(*process->GetSymbolTable());
            machine.GetProcessor().EnableInterrupts();
        }
        else
        {
            throw std::runtime_error("error loading file '" + process->FilePath() + "': 'Main' entry point not found");
        }
    }
    else
    {
        throw std::runtime_error("error loading file '" + process->FilePath() + "': executable file expected");
    }
}

} // namespace cmsx::kernel
