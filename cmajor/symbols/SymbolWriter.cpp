// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <cmajor/symbols/Symbol.hpp>

namespace cmajor { namespace symbols {

SymbolWriter::SymbolWriter(const std::string& fileName_) : astWriter(fileName_), spanConversionModuleId(boost::uuids::nil_uuid()), lexers(nullptr)
{
}

void SymbolWriter::Write(Symbol* symbol)
{
    astWriter.GetBinaryWriter().Write(static_cast<uint8_t>(symbol->GetSymbolType()));
    Module* sourceModule = GetModuleById(symbol->SourceModuleId());
    if (sourceModule && !spanConversionModuleId.is_nil() && sourceModule->Id() == spanConversionModuleId && sourceModule->GetFlag(ModuleFlags::compiling))
    {
        astWriter.Write(symbol->GetSpan(), true);
    }
    else
    {
        astWriter.Write(symbol->GetSpan(), false);
    }
    astWriter.GetBinaryWriter().Write(symbol->SourceModuleId());
    astWriter.GetBinaryWriter().Write(symbol->Name());
    symbol->Write(*this);
}

void SymbolWriter::SetLexers(std::vector<soulng::lexer::Lexer*>* lexers_)
{
    lexers = lexers_;
    astWriter.SetLexers(lexers);
}

void SymbolWriter::SetSpanConversionModuleId(const boost::uuids::uuid& spanConversionModuleId_)
{
    spanConversionModuleId = spanConversionModuleId_;
    astWriter.SetSpanConversionModuleId(spanConversionModuleId);
}

} } // namespace cmajor::symbols
