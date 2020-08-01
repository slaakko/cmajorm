// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

AstWriter::AstWriter(const std::string& fileName_) : binaryWriter(fileName_), lexers(nullptr)
{
}

void AstWriter::Write(Node* node)
{
    binaryWriter.Write(static_cast<uint8_t>(node->GetNodeType()));
    Write(node->GetSpan());
    node->Write(*this);
}

void AstWriter::Write(Specifiers specifiers)
{
    binaryWriter.Write(static_cast<uint32_t>(specifiers));
}

void AstWriter::Write(const Span& span)
{
    if (!span.Valid())
    {
        binaryWriter.Write(false);
    }
    else
    {
        Span s = span;
        if (span.fileIndex >= 0 && span.fileIndex < lexers->size())
        {
            soulng::lexer::Lexer* lexer = (*lexers)[span.fileIndex];
            lexer->ConvertExternal(s);
        }
        binaryWriter.Write(true);
        binaryWriter.WriteULEB128UInt(static_cast<uint32_t>(s.fileIndex));
        binaryWriter.WriteULEB128UInt(static_cast<uint32_t>(s.line));
        binaryWriter.WriteULEB128UInt(static_cast<uint32_t>(s.start));
        binaryWriter.WriteULEB128UInt(static_cast<uint32_t>(s.end));
    }
}

void AstWriter::SetLexers(std::vector<soulng::lexer::Lexer*>* lexers_)
{
    lexers = lexers_;
}

} } // namespace sngcm::ast
