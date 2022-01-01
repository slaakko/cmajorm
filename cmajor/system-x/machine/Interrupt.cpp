// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/machine/Interrupt.hpp>
#include <vector>
#include <memory>

namespace cmsx::machine {

InterruptHandler::~InterruptHandler()
{
}

class InterruptHandlerTable
{
public:
    static void Init();
    static void Done();
    static InterruptHandlerTable& Instance() { return *instance; }
    void SetInterruptHandler(uint8_t irq, InterruptHandler* handler);
    InterruptHandler* GetInterruptHandler(uint8_t irq) const;
private:
    InterruptHandlerTable();
    static std::unique_ptr<InterruptHandlerTable> instance;
    std::vector<std::unique_ptr<InterruptHandler>> handlers;
};

std::unique_ptr<InterruptHandlerTable> InterruptHandlerTable::instance;

void InterruptHandlerTable::Init()
{
    instance.reset(new InterruptHandlerTable());
}

void InterruptHandlerTable::Done()
{
    instance.reset();
}

InterruptHandlerTable::InterruptHandlerTable()
{
    handlers.resize(64);
}

void InterruptHandlerTable::SetInterruptHandler(uint8_t irq, InterruptHandler* handler)
{
    handlers[irq].reset(handler);
}

InterruptHandler* InterruptHandlerTable::GetInterruptHandler(uint8_t irq) const
{
    return handlers[irq].get();
}

void SetInterruptHandler(uint8_t irq, InterruptHandler* handler)
{
    InterruptHandlerTable::Instance().SetInterruptHandler(irq, handler);
}

InterruptHandler* GetInterruptHandler(uint8_t irq)
{
    return InterruptHandlerTable::Instance().GetInterruptHandler(irq);
}

void InitInterrupt()
{
    InterruptHandlerTable::Init();
}

void DoneInterrupt()
{
    InterruptHandlerTable::Done();
}

} // cmsx::machine
