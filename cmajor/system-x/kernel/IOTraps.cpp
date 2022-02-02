// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/IOTraps.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/kernel/Trap.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/machine/Processor.hpp>

namespace cmsx::kernel {

class TrapCreateHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_create"; }
};

uint64_t TrapCreateHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        int32_t mode = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regBX));
        return Create(process, pathAddr, mode);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapOpenHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_open"; }
};

uint64_t TrapOpenHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        int32_t flags = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regBX));
        int32_t mode = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regCX));
        return Open(process, pathAddr, flags, mode);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapCloseHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_close"; }
};

uint64_t TrapCloseHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        Close(process, fd);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapReadHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_read"; }
};

uint64_t TrapReadHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t bufferAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        int64_t count = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regCX));
        return Read(process, fd, bufferAddr, count);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapWriteHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_write"; }
};

uint64_t TrapWriteHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t bufferAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        int64_t count = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regCX));
        return Write(process, fd, bufferAddr, count);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapIOCtlHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_ioctl"; }
};


uint64_t TrapIOCtlHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int32_t item = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regBX));
        return IOCtl(process, fd, item);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapUnlinkHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_unlink"; }
};

uint64_t TrapUnlinkHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        Unlink(process, pathAddr);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapSeekHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_seek"; }
};

uint64_t TrapSeekHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t offset = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        int32_t whence = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regCX));
        return Seek(process, fd, offset, whence);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapTellHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_tell"; }
};

uint64_t TrapTellHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int32_t fd = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regAX));
        return Tell(process, fd);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapStatHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_stat"; }
};

uint64_t TrapStatHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t statBufAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        int32_t statBufSize = static_cast<int32_t>(processor.Regs().Get(cmsx::machine::regCX));
        Stat(process, pathAddr, statBufAddr, statBufSize);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapDirStatHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_dir_stat"; }
};

uint64_t TrapDirStatHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        return DirStat(process, pathAddr);
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapGetCWDHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_getcwd"; }
};

uint64_t TrapGetCWDHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t bufAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        int64_t bufSize = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regBX));
        GetCWD(process, bufAddr, bufSize);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

class TrapChDirHandler : public TrapHandler
{
public:
    uint64_t HandleTrap(cmsx::machine::Processor& processor) override;
    std::string TrapName() const { return "trap_chdir"; }
};

uint64_t TrapChDirHandler::HandleTrap(cmsx::machine::Processor& processor)
{
    Process* process = static_cast<Process*>(processor.CurrentProcess());
    try
    {
        int64_t pathAddr = static_cast<int64_t>(processor.Regs().Get(cmsx::machine::regAX));
        ChDir(process, pathAddr);
        return 0;
    }
    catch (const SystemError& error)
    {
        process->SetError(error);
        return static_cast<uint64_t>(-1);
    }
}

void InitIOTraps()
{
    SetTrapHandler(trap_create, new TrapCreateHandler());
    SetTrapHandler(trap_open, new TrapOpenHandler());
    SetTrapHandler(trap_close, new TrapCloseHandler());
    SetTrapHandler(trap_read, new TrapReadHandler());
    SetTrapHandler(trap_write, new TrapWriteHandler());
    SetTrapHandler(trap_ioctl, new TrapIOCtlHandler());
    SetTrapHandler(trap_unlink, new TrapUnlinkHandler());
    SetTrapHandler(trap_seek, new TrapSeekHandler());
    SetTrapHandler(trap_tell, new TrapTellHandler());
    SetTrapHandler(trap_stat, new TrapStatHandler());
    SetTrapHandler(trap_dir_stat, new TrapDirStatHandler());
    SetTrapHandler(trap_getcwd, new TrapGetCWDHandler());
    SetTrapHandler(trap_chdir, new TrapChDirHandler());
}

void DoneIOTraps()
{
    SetTrapHandler(trap_chdir, nullptr);
    SetTrapHandler(trap_getcwd, nullptr);
    SetTrapHandler(trap_dir_stat, nullptr);
    SetTrapHandler(trap_stat, nullptr);
    SetTrapHandler(trap_tell, nullptr);
    SetTrapHandler(trap_seek, nullptr);
    SetTrapHandler(trap_unlink, nullptr);
    SetTrapHandler(trap_ioctl, nullptr);
    SetTrapHandler(trap_write, nullptr);
    SetTrapHandler(trap_read, nullptr);
    SetTrapHandler(trap_close, nullptr);
    SetTrapHandler(trap_open, nullptr);
    SetTrapHandler(trap_create, nullptr);
}

} // namespace cmsx::kernel
