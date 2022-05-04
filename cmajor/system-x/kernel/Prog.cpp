// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/kernel/Prog.hpp>
#include <system-x/kernel/Error.hpp>
#include <system-x/kernel/Process.hpp>
#include <system-x/kernel/IO.hpp>
#include <system-x/machine/Processor.hpp>
#include <soulng/util/Process.hpp>
#include <soulng/util/Socket.hpp>

namespace cmsx::kernel {

class Prog
{
public:
    Prog(soulng::util::Process* process_, const std::string& name_, int portNumber_);
    virtual ~Prog();
    virtual bool Stop(std::string& error) = 0;
    soulng::util::Process* GetProcess() const { return process.get(); }
    int PortNumber() const { return portNumber; }
    const std::string& Name() const { return name; }
private:
    std::unique_ptr<soulng::util::Process> process;
    int portNumber;
    std::string name;
};

Prog::Prog(soulng::util::Process* process_, const std::string& name_, int portNumber_) : process(process_), name(name_), portNumber(portNumber_)
{
}

Prog::~Prog()
{
    try
    {
        process->Terminate();
        process.reset();
    }
    catch (...)
    {
    }
}

class SxbsProg : public Prog
{
public:
    SxbsProg(soulng::util::Process* process);
    bool Stop(std::string& error) override;
};

SxbsProg::SxbsProg(soulng::util::Process* process) : Prog(process, "sxbs", sxbsPortNumber)
{
}

bool SxbsProg::Stop(std::string& error)
{
    soulng::util::TcpSocket socket;
    try
    {
        socket = TcpSocket("127.0.0.1", std::to_string(PortNumber()));
    }
    catch (const std::exception& ex)
    {
        error = ex.what();
        return false;
    }
    catch (...)
    {
        error = "unknown error";
    }
    try
    {
        soulng::util::Write(socket, "stop server request");
        std::string replyStr = soulng::util::ReadStr(socket);
        if (replyStr == "stop server reply")
        {
            socket.Close();
            return true;
        }
    }
    catch (const std::exception& ex)
    {
        error = ex.what();
        return false;
    }
    catch (...)
    {
        error = "unknown error";
    }
    if (error.empty())
    {
        error = "unknown error";
    }
    return false;
}

class ProgTable
{
public:
    static void Init();
    static void Done();
    static ProgTable& Instance() { return *instance; }
    int32_t AddProg(Prog* prog);
    void RemoveProg(int32_t prog);
private:
    static std::unique_ptr<ProgTable> instance;
    std::vector<std::unique_ptr<Prog>> progs;
};

std::unique_ptr<ProgTable> ProgTable::instance;

void ProgTable::Init()
{
    instance.reset(new ProgTable());
}

void ProgTable::Done()
{
    instance.reset();
}

int32_t ProgTable::AddProg(Prog* prog)
{
    int32_t progId = progs.size();
    progs.push_back(std::unique_ptr<Prog>(prog));
    return progId;
}

void ProgTable::RemoveProg(int32_t progId)
{
    if (progId >= 0 && progId < progs.size())
    {
        std::string errorMsg;
        if (!progs[progId]->Stop(errorMsg))
        {
            progs[progId].reset();
            throw SystemError(EHOST, "prog id " + std::to_string(progId) + " (" + progs[progId]->Name() + ") failed to stop : " + errorMsg, __FUNCTION__);
        }
        progs[progId].reset();
    }
    else
    {
        throw SystemError(EPARAM, "bad prog id " + std::to_string(progId), __FUNCTION__);
    }
}

int32_t Start(Process* process, int64_t progAddr)
{
    if (progAddr == 0)
    {
        throw SystemError(EPARAM, "prog is null", __FUNCTION__);
    }
    cmsx::machine::Memory& mem = process->GetProcessor()->GetMachine()->Mem();
    std::string progName = ReadString(process, progAddr, mem);
    if (progName == "sx.bs")
    {
        Prog* prog = new SxbsProg(new soulng::util::Process("sxbs",
            soulng::util::Process::Redirections::processStdIn |
            soulng::util::Process::Redirections::processStdOut |
            soulng::util::Process::Redirections::processStdErr));
        std::string line = prog->GetProcess()->ReadLine(soulng::util::Process::StdHandle::stdOut);
        if (line == "sx.bs.ready")
        {
            return ProgTable::Instance().AddProg(prog);
        }
        else if (line == "sx.bs.error")
        {
            std::string line = prog->GetProcess()->ReadLine(soulng::util::Process::StdHandle::stdOut);
            throw SystemError(EPARAM, "error starting '" + progName + "': " + line, __FUNCTION__);
        }
        else
        {
            throw SystemError(EPARAM, "error starting '" + progName + "': process exited prematurely", __FUNCTION__);
        }
    }
    else
    {
        throw SystemError(EPARAM, "unknown prog name '" + progName + "'", __FUNCTION__);
    }
}

void Stop(int32_t prog)
{
    ProgTable::Instance().RemoveProg(prog);
}

void InitProg()
{
    ProgTable::Init();
}

void DoneProg()
{
    ProgTable::Done();
}

} // namespace cmsx::kernel
