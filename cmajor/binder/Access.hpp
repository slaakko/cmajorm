// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_ACCESS_INCLUDED
#define CMAJOR_BINDER_ACCESS_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

void CheckAccess(FunctionSymbol* fromFunction, Symbol* toSymbol);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_ACCESS_INCLUDED
