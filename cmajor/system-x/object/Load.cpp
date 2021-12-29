// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/object/Load.hpp>
#include <system-x/object/BinaryFile.hpp>
#include <system-x/machine/Memory.hpp>
#include <soulng/util/Util.hpp>
#include <memory>
#include <stdexcept>

namespace cmsx::object {

void SetupImage(ExecutableFile* executable, cmsx::machine::Memory& memory)
{
    CodeSection* codeSection = executable->GetCodeSection();
    int64_t codeSectionBaseAddress = codeSection->BaseAddress();
    for (int64_t i = 0; i < codeSection->Length(); ++i)
    {
        uint8_t value = codeSection->GetByte(i);
        memory.WriteByte(static_cast<uint64_t>(codeSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
    DataSection* dataSection = executable->GetDataSection();
    int64_t dataSectionBaseAddress = dataSection->BaseAddress();
    for (int64_t i = 0; i < dataSection->Length(); ++i)
    {
        uint8_t value = dataSection->GetByte(i);
        memory.WriteByte(static_cast<uint64_t>(dataSectionBaseAddress + i), value, cmsx::machine::Protection::write);
    }
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

void Load(const std::string& executableFilePath, uint64_t rv, const std::vector<std::string>& args, const std::vector<std::string>& env, cmsx::machine::Machine& machine)
{
    int argCount = args.size();
    std::unique_ptr<BinaryFile> binaryFile(ReadBinaryFile(executableFilePath));
    if (binaryFile->Kind() == BinaryFileKind::executableFile)
    {
        ExecutableFile* executable = static_cast<ExecutableFile*>(binaryFile.get());
        machine.Regs().SetSpecial(cmsx::machine::rV, rv);
        machine.Regs().Set(cmsx::machine::regSP, cmsx::machine::stackSegmentBaseAddress);
        SetupImage(executable, machine.Mem());
        int64_t poolSegmentBaseAddress = cmsx::machine::poolSegmentBaseAddress;
        int64_t address = poolSegmentBaseAddress;
        int64_t argsAddress = soulng::util::Align(address, 8);
        address = SetupArgs(argsAddress, args, machine.Mem());
        int64_t envAddress = soulng::util::Align(address, 8);
        address = SetupEnv(envAddress, env, machine.Mem());
        Symbol* main = executable->GetSymbolTable().GetSymbol("main");
        if (main)
        {
            int64_t entryPoint = main->Start();
            machine.Regs().SetPC(static_cast<uint64_t>(entryPoint));
            int64_t mainFrame = cmsx::machine::stackSegmentBaseAddress;
            int64_t mainArgAddr = mainFrame;
            machine.Mem().WriteTetra(static_cast<uint64_t>(mainArgAddr), static_cast<uint32_t>(argCount), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(argsAddress), cmsx::machine::Protection::write);
            mainArgAddr = mainArgAddr + 8;
            machine.Mem().WriteOcta(static_cast<uint64_t>(mainArgAddr), static_cast<uint64_t>(envAddress), cmsx::machine::Protection::write);
        }
        else
        {
            throw std::runtime_error("error loading file '" + executableFilePath + "': 'main' entry point not found");
        }
    }
    else
    {
        throw std::runtime_error("error loading file '" + executableFilePath + "': executable file expected");
    }
}

} // namespace cmsx::object
