// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/AstWriter.hpp>
#include <sngcm/ast/AstReader.hpp>
#include <sngcm/ast/Node.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngcm { namespace ast {

AstWriter::AstWriter(const std::string& fileName_) : binaryWriter(fileName_), lexers(nullptr), spanConversionModuleId(boost::uuids::nil_uuid())
{
}

void AstWriter::Write(Node* node)
{
    binaryWriter.Write(static_cast<uint8_t>(node->GetNodeType()));
    if (node->ModuleId() == spanConversionModuleId)
    {
        Write(node->GetSpan(), true);
    }
    else
    {
        Write(node->GetSpan(), false);
    }
    binaryWriter.Write(node->ModuleId());
    node->Write(*this);
}

void AstWriter::Write(Specifiers specifiers)
{
    binaryWriter.Write(static_cast<uint32_t>(specifiers));
}

void AstWriter::Write(const Span& span, bool convertExternal)
{
    if (!span.Valid())
    {
        binaryWriter.Write(false);
    }
    else
    {
        Span s = span;
        if (convertExternal)
        {
            if (s.fileIndex >= 0 && s.fileIndex < lexers->size())
            {
                soulng::lexer::Lexer* lexer = (*lexers)[s.fileIndex];
                lexer->ConvertExternal(s);
            }
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

void AstWriter::SetSpanConversionModuleId(const boost::uuids::uuid& spanConversionModuleId_)
{
    spanConversionModuleId = spanConversionModuleId_;
}

} } // namespace sngcm::ast
