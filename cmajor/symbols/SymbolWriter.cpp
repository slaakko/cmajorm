// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/Symbol.hpp>

namespace cmajor { namespace symbols {

SymbolWriter::SymbolWriter(const std::string& fileName_) : astWriter(fileName_), lexers(nullptr)
{
}

void SymbolWriter::Write(Symbol* symbol)
{
    astWriter.GetBinaryWriter().Write(static_cast<uint8_t>(symbol->GetSymbolType()));
    astWriter.Write(symbol->GetSpan());
    astWriter.GetBinaryWriter().Write(symbol->Name());
    symbol->Write(*this);
}

void SymbolWriter::SetLexers(std::vector<soulng::lexer::Lexer*>* lexers_)
{
    lexers = lexers_;
    astWriter.SetLexers(lexers);
}

} } // namespace cmajor::symbols
