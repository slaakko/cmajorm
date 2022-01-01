// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_MACHINE_DEBUGGER_INCLUDED
#define CMSX_MACHINE_DEBUGGER_INCLUDED
#include <system-x/machine/Api.hpp>

namespace cmsx::machine {

class Machine;

class CMSX_MACHINE_API Debugger
{
public:
    virtual ~Debugger();
    virtual void Intercept() = 0;
    virtual void ProcessExit() = 0;
};

} // cmsx::machine

#endif // CMSX_MACHINE_DEBUGGER_INCLUDED
