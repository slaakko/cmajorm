// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/AstReader.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/ast/Concept.hpp>
#include <sngcm/ast/Statement.hpp>

namespace sngcm { namespace ast {

AstReader::AstReader(const std::string& fileName_) : binaryReader(fileName_), moduleId(-1)
{
}

Node* AstReader::ReadNode()
{
    NodeType nodeType = static_cast<NodeType>(binaryReader.ReadByte());
    Span span = ReadSpan();
    Node* node = NodeFactory::Instance().CreateNode(nodeType, span);
    node->Read(*this);
    return node;
}

IdentifierNode* AstReader::ReadIdentifierNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::identifierNode)
    {
        return static_cast<IdentifierNode*>(node);
    }
    else
    {
        throw std::runtime_error("identifier node expected");
    }
}

LabelNode* AstReader::ReadLabelNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::labelNode)
    {
        return static_cast<LabelNode*>(node);
    }
    else
    {
        throw std::runtime_error("label node expected");
    }
}

StatementNode* AstReader::ReadStatementNode()
{
    Node* node = ReadNode();
    if (node->IsStatementNode())
    {
        return static_cast<StatementNode*>(node);
    }
    else
    {
        throw std::runtime_error("statement node expected");
    }
}

DefaultStatementNode* AstReader::ReadDefaultStatementNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::defaultStatementNode)
    {
        return static_cast<DefaultStatementNode*>(node);
    }
    else
    {
        throw std::runtime_error("default statement node expected");
    }
}

CompoundStatementNode* AstReader::ReadCompoundStatementNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::compoundStatementNode)
    {
        return static_cast<CompoundStatementNode*>(node);
    }
    else
    {
        throw std::runtime_error("compound statement node expected");
    }
}

ConstraintNode* AstReader::ReadConstraintNode()
{
    Node* node = ReadNode();
    if (node->NodeIsConstraintNode())
    {
        return static_cast<ConstraintNode*>(node);
    }
    else
    {
        throw std::runtime_error("constraint node expected");
    }
}

WhereConstraintNode* AstReader::ReadWhereConstraintNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::whereConstraintNode)
    {
        return static_cast<WhereConstraintNode*>(node);
    }
    else
    {
        throw std::runtime_error("where constraint node expected");
    }
}

ConceptIdNode* AstReader::ReadConceptIdNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::conceptIdNode)
    {
        return static_cast<ConceptIdNode*>(node);
    }
    else
    {
        throw std::runtime_error("concept id node expected");
    }
}

ConceptNode* AstReader::ReadConceptNode()
{
    Node* node = ReadNode();
    if (node->IsConceptNode())
    {
        return static_cast<ConceptNode*>(node);
    }
    else
    {
        throw std::runtime_error("concept node expected");
    }
}

ConditionalCompilationExpressionNode* AstReader::ReadConditionalCompilationExpressionNode()
{
    Node* node = ReadNode();
    if (node->IsConditionalCompilationExpressionNode())
    {
        return static_cast<ConditionalCompilationExpressionNode*>(node);
    }
    else
    {
        throw std::runtime_error("conditional compilation expression node expected");
    }
}

ConditionalCompilationPartNode* AstReader::ReadConditionalCompilationPartNode()
{
    Node* node = ReadNode();
    if (node->GetNodeType() == NodeType::conditionalCompilationPartNode)
    {
        return static_cast<ConditionalCompilationPartNode*>(node);
    }
    else
    {
        throw std::runtime_error("conditional compilation part node expected");
    }
}

Specifiers AstReader::ReadSpecifiers()
{
    return static_cast<Specifiers>(binaryReader.ReadUInt());
}

Span AstReader::ReadSpan()
{
    bool valid = binaryReader.ReadBool();
    if (!valid)
    {
        return Span();
    }
    else
    {
        uint32_t fileIndex = binaryReader.ReadULEB128UInt();
        if (moduleId != -1)
        {
            int16_t fileId = GetFileId(static_cast<int32_t>(fileIndex));
            fileIndex = static_cast<uint32_t>(MakeFileIndex(moduleId, fileId));
        }
        uint32_t line = binaryReader.ReadULEB128UInt();
        uint32_t start = binaryReader.ReadULEB128UInt();
        uint32_t end = binaryReader.ReadULEB128UInt();
        return Span(static_cast<int32_t>(fileIndex), static_cast<int32_t>(line), static_cast<int32_t>(start), static_cast<int32_t>(end));
    }
}

} } // namespace sngcm::ast
