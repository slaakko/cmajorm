// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_AST_WRITER_INCLUDED
#define SNGCM_AST_AST_WRITER_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/util/BinaryWriter.hpp>
#include <soulng/lexer/Span.hpp>
#include <soulng/lexer/Lexer.hpp>

namespace sngcm { namespace ast {

using namespace soulng::util;
using soulng::lexer::Span;
class Node;

class SNGCM_AST_API AstWriter
{
public:
    AstWriter(const std::string& fileName_);
    BinaryWriter& GetBinaryWriter() { return binaryWriter; }
    void Write(Node* node);
    void Write(Specifiers specifiers);
    void Write(const Span& span, bool convertExternal);
    void SetLexers(std::vector<soulng::lexer::Lexer*>* lexers_);
    void SetSpanConversionModuleId(const boost::uuids::uuid& spanConversionModuleId_);
    const boost::uuids::uuid& SpanConversionModuleId() const { return spanConversionModuleId; }
private:
    BinaryWriter binaryWriter;
    std::vector<soulng::lexer::Lexer*>* lexers;
    boost::uuids::uuid spanConversionModuleId;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_AST_WRITER_INCLUDED
