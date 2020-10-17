// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/AstReader.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/ast/Concept.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/ast/Class.hpp>
#include <boost/uuid/nil_generator.hpp>

namespace sngcm { namespace ast {

AstReader::AstReader(const std::string& fileName_) :
    binaryReader(fileName_), rootModuleId(boost::uuids::nil_uuid()), moduleNameTable(nullptr), moduleIdMap(nullptr)
{
}

Node* AstReader::ReadNode()
{
    NodeType nodeType = static_cast<NodeType>(binaryReader.ReadByte());
    Span span = ReadSpan();
    Node* node = NodeFactory::Instance().CreateNode(nodeType, span);
    node->SetRootModuleId(rootModuleId);
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
        int16_t moduleId = GetModuleId(fileIndex);
        auto it = moduleNameTable->find(moduleId);
        if (it != moduleNameTable->cend())
        {
            auto it2 = moduleIdMap->find(it->second);
            if (it2 != moduleIdMap->cend())
            {
                moduleId = it2->second;
            }
            else
            {
                //throw std::runtime_error("module id for module name '" + it->second + "' not found");
                uint32_t line = binaryReader.ReadULEB128UInt();
                uint32_t start = binaryReader.ReadULEB128UInt();
                uint32_t end = binaryReader.ReadULEB128UInt();
                return Span();
            }
        }
        else
        {
            //throw std::runtime_error("module name for module id " + std::to_string(moduleId) + " not found");
            uint32_t line = binaryReader.ReadULEB128UInt();
            uint32_t start = binaryReader.ReadULEB128UInt();
            uint32_t end = binaryReader.ReadULEB128UInt();
            return Span();
        }
        int16_t fileId = GetFileId(static_cast<int32_t>(fileIndex));
        fileIndex = static_cast<uint32_t>(MakeFileIndex(moduleId, fileId));
        uint32_t line = binaryReader.ReadULEB128UInt();
        uint32_t start = binaryReader.ReadULEB128UInt();
        uint32_t end = binaryReader.ReadULEB128UInt();
        return Span(static_cast<int32_t>(fileIndex), static_cast<int32_t>(line), static_cast<int32_t>(start), static_cast<int32_t>(end));
    }
}

void AstReader::SetModuleMaps(const boost::uuids::uuid& rootModuleId_, std::unordered_map<int16_t, std::string>* moduleNameTable_, std::unordered_map<std::string, int16_t>* moduleIdMap_)
{
    rootModuleId = rootModuleId_;
    moduleNameTable = moduleNameTable_;
    moduleIdMap = moduleIdMap_;
}

} } // namespace sngcm::ast
