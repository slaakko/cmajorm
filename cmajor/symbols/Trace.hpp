// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_TRACE_INCLUDED
#define CMAJOR_SYMBOLS_TRACE_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <string>
#include <stdint.h>

namespace cmajor { namespace symbols {

SYMBOLS_API int32_t GetTraceFunctionId(const std::string& functionFullName);
SYMBOLS_API void ReadTraceTable();
SYMBOLS_API void WriteTraceTable();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_TRACE_INCLUDED
