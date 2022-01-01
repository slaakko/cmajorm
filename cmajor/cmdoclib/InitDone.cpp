// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/InitDone.hpp>
#include <cmajor/cmdoclib/Constraint.hpp>
#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/Global.hpp>
#include <sngxml/xpath/InitDone.hpp>

namespace cmajor { namespace cmdoclib {

void Init()
{
    SetConstraintSymbolBinder();
    SourceCodePrinterInit();
    GlobalInit();
    sngxml::xpath::Init();
}

void Done()
{
    sngxml::xpath::Done();
    GlobalDone();
    SourceCodePrinterDone();
}

} } // namespace cmajor::cmdoclib
