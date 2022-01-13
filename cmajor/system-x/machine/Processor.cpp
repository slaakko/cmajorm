// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Processor.hpp>
#include <system-x/machine/Machine.hpp>
#include <system-x/machine/Instruction.hpp>
#include <system-x/machine/Interrupt.hpp>
#include <system-x/machine/Debugger.hpp>

namespace cmsx::machine {

ProcessObserver::~ProcessObserver()
{
}

Process::~Process()
{
}

Scheduler::~Scheduler()
{
}

Processor::Processor() : id(0), machine(nullptr), currentProcess(nullptr)
{
}

void RunProcessor(Processor* processor)
{
    processor->Run();
}

void Processor::Start()
{
    thread = std::thread(RunProcessor, this);
}

void Processor::Stop()
{
    thread.join();
}

void Processor::Run()
{
    try
    {
        while (!machine->Exiting())
        {
            Scheduler* scheduler = machine->GetScheduler();
            currentProcess = scheduler->GetRunnableProcess();
            if (!currentProcess)
            {
                break;
            }
            start = std::chrono::steady_clock::now();
            currentProcess->RestoreContext(*machine, registers);
            currentProcess->SetRunning(this);
            uint64_t pc = registers.GetPC();
            while (currentProcess && currentProcess->State() == ProcessState::running)
            {
                Debugger* debugger = currentProcess->GetDebugger();
                if (debugger)
                {
                    debugger->Intercept();
                }
                if (machine->Exiting()) break;
                uint64_t prevPC = pc;
                uint8_t x = 0;
                uint8_t y = 0;
                uint8_t z = 0;
                Instruction* inst = FetchInstruction(pc, x, y, z);
                inst->Execute(*this, x, y, z);
                SetPC(inst, pc, prevPC);
                CheckInterrupts();
                pc = registers.GetPC();
            }
        }
    }
    catch (const std::exception&)
    {
        exception = std::current_exception();
    }
}

Instruction* Processor::FetchInstruction(uint64_t& pc, uint8_t& x, uint8_t& y, uint8_t& z)
{
    Memory& mem = machine->Mem();
    uint64_t rv = registers.GetSpecial(rV);
    uint8_t opCode = mem.ReadByte(rv, pc, Protection::execute);
    ++pc;
    x = mem.ReadByte(rv, pc, Protection::execute);
    ++pc;
    y = mem.ReadByte(rv, pc, Protection::execute);
    ++pc;
    z = mem.ReadByte(rv, pc, Protection::execute);
    ++pc;
    Instruction* inst = machine->GetInstruction(opCode);
    return inst;
}

void Processor::SetPC(Instruction* inst, uint64_t pc, uint64_t prevPC)
{
    uint64_t regsPC = registers.GetPC();
    if (!inst->IsJumpInstruction() && regsPC == prevPC)
    {
        registers.SetPC(pc);
    }
    registers.SetSpecial(rW, prevPC);
}

void Processor::CheckInterrupts()
{
    uint64_t interruptBits = registers.GetInterruptBits();
    if (interruptBits)
    {
        for (int irq = 0; irq < 64; ++irq)
        {
            uint64_t irqBit = static_cast<uint64_t>(1) << irq;
            if (interruptBits & irqBit)
            {
                InterruptHandler* handler = GetInterruptHandler(irq);
                if (handler)
                {
                    stop = std::chrono::steady_clock::now();
                    if (currentProcess)
                    {
                        currentProcess->AddUserTime(stop - start);
                    }
                    handler->HandleInterrupt(*this);
                    if (currentProcess)
                    {
                        start = std::chrono::steady_clock::now();
                        currentProcess->AddSystemTime(start - stop);
                    }
                }
                else
                {
                    throw std::runtime_error("no interrupt handler for IRQ " + std::to_string(irq));
                }
            }
        }
    }
}

void Processor::EnableInterrupts()
{
    registers.SetSpecial(rK, ALL_INTERRUPT_BITS);
}

void Processor::ResetCurrentProcess()
{
    start = std::chrono::steady_clock::now();
    currentProcess->AddSystemTime(start - stop);
    currentProcess->SaveContext(*machine, registers);
    currentProcess->ResetProcessor();
    currentProcess = nullptr;
}

void Processor::CheckException()
{
    if (exception)
    {
        std::rethrow_exception(exception);
    }
}

} // cmsx::machine
