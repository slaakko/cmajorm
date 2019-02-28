// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/InitDone.hpp>
#include <cmajor/cmdoclib/Constraint.hpp>
#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <cmajor/xpath/InitDone.hpp>

namespace cmajor { namespace cmdoclib {

void Init()
{
    SetConstraintSymbolBinder();
    SourceCodePrinterInit();
    GlobalInit();
    xpath::Init();
}

void Done()
{
    xpath::Done();
    GlobalDone();
    SourceCodePrinterDone();
}

} } // namespace cmajor::cmdoclib
