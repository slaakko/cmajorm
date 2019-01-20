// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/AstWriter.hpp>
#include <cmajor/ast/Node.hpp>

namespace cmajor { namespace ast {

AstWriter::AstWriter(const std::string& fileName_) : binaryWriter(fileName_)
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
        binaryWriter.Write(true);
        binaryWriter.WriteULEB128UInt(span.FileIndex());
        binaryWriter.WriteULEB128UInt(span.LineNumber());
        binaryWriter.WriteULEB128UInt(span.Start());
        binaryWriter.WriteULEB128UInt(span.End());
    }
}

} } // namespace cmajor::ast
