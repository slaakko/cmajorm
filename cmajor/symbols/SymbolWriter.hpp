// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
#define CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
#include <cmajor/ast/AstWriter.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::ast;

class Symbol;

class SymbolWriter 
{
public:   
    SymbolWriter(const std::string& fileName_);
    AstWriter& GetAstWriter() { return astWriter; }
    BinaryWriter& GetBinaryWriter() { return astWriter.GetBinaryWriter(); }
    void Write(Symbol* symbol);
private:
    AstWriter astWriter;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_SYMBOL_WRITER_INCLUDED
