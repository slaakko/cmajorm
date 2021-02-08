// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_ACCESS_INCLUDED
#define CMAJOR_BINDER_ACCESS_INCLUDED
#include <cmajor/binder/BinderApi.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

BINDER_API void CheckAccess(FunctionSymbol* fromFunction, Symbol* toSymbol);
BINDER_API bool HasAccess(FunctionSymbol* fromFunction, Symbol* toSymbol);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_ACCESS_INCLUDED
