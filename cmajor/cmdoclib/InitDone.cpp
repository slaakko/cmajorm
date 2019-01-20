// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/InitDone.hpp>
#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/Global.hpp>

namespace cmajor { namespace cmdoclib {

void Init()
{
    SourceCodePrinterInit();
    GlobalInit();
}

void Done()
{
    GlobalDone();
    SourceCodePrinterDone();
}

} } // namespace cmajor::cmdoclib
