// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_AST_WRITER_INCLUDED
#define CMAJOR_AST_AST_WRITER_INCLUDED
#include <cmajor/ast/AstApi.hpp>
#include <cmajor/ast/Specifier.hpp>
#include <cmajor/util/BinaryWriter.hpp>
#include <cmajor/parsing/Scanner.hpp>

namespace cmajor { namespace ast {

using namespace cmajor::util;
using cmajor::parsing::Span;
class Node;

class AST_API AstWriter
{
public:
    AstWriter(const std::string& fileName_);
    BinaryWriter& GetBinaryWriter() { return binaryWriter; }
    void Write(Node* node);
    void Write(Specifiers specifiers);
    void Write(const Span& span);
private:
    BinaryWriter binaryWriter;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_AST_WRITER_INCLUDED
