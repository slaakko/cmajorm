// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
#define CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <sngcm/ast/AstWriter.hpp>

namespace cmajor { namespace symbols {

using namespace sngcm::ast;

class Symbol;

class SYMBOLS_API SymbolWriter
{
public:   
    SymbolWriter(const std::string& fileName_);
    AstWriter& GetAstWriter() { return astWriter; }
    BinaryWriter& GetBinaryWriter() { return astWriter.GetBinaryWriter(); }
    void Write(Symbol* symbol);
    void SetLexers(std::vector<soulng::lexer::Lexer*>* lexers_);
    void SetSpanConversionModuleId(const boost::uuids::uuid& spanConversionModuleId_);
private:
    AstWriter astWriter;
    boost::uuids::uuid spanConversionModuleId;
    std::vector<soulng::lexer::Lexer*>* lexers;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
