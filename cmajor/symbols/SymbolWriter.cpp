// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/Symbol.hpp>

namespace cmajor { namespace symbols {

SymbolWriter::SymbolWriter(const std::string& fileName_) : astWriter(fileName_)
{
}

void SymbolWriter::Write(Symbol* symbol)
{
    astWriter.GetBinaryWriter().Write(static_cast<uint8_t>(symbol->GetSymbolType()));
    astWriter.Write(symbol->GetSpan());
    astWriter.GetBinaryWriter().Write(symbol->Name());
    symbol->Write(*this);
}

} } // namespace cmajor::symbols
