// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_AST_AST_READER_INCLUDED
#define CMAJOR_AST_AST_READER_INCLUDED
#include <cmajor/ast/Specifier.hpp>
#include <cmajor/util/BinaryReader.hpp>
#include <cmajor/parsing/Scanner.hpp>

namespace cmajor { namespace ast {

using namespace cmajor::util;
using cmajor::parsing::Span;
class Node;
class IdentifierNode;
class LabelNode;
class StatementNode;
class DefaultStatementNode;
class CompoundStatementNode;
class ConstraintNode;
class WhereConstraintNode;
class ConceptIdNode;
class ConceptNode;
class ConditionalCompilationExpressionNode;
class ConditionalCompilationPartNode;

inline int32_t MakeFileIndex(int16_t moduleId, int16_t fileId)
{
    return static_cast<int32_t>(moduleId << 16) | fileId;
}

inline int16_t GetModuleId(int32_t fileIndex)
{
    return static_cast<int16_t>((fileIndex >> 16) & 0xFFFF);
}

inline int16_t GetFileId(int32_t fileIndex)
{
    return static_cast<int16_t>(fileIndex & 0xFFFF);
}

class AstReader
{
public:
    AstReader(const std::string& fileName_);
    BinaryReader& GetBinaryReader() { return binaryReader; }
    Node* ReadNode();
    IdentifierNode* ReadIdentifierNode();
    LabelNode* ReadLabelNode();
    StatementNode* ReadStatementNode();
    DefaultStatementNode* ReadDefaultStatementNode();
    CompoundStatementNode* ReadCompoundStatementNode();
    ConstraintNode* ReadConstraintNode();
    WhereConstraintNode* ReadWhereConstraintNode();
    ConceptIdNode* ReadConceptIdNode();
    ConceptNode* ReadConceptNode();
    ConditionalCompilationExpressionNode* ReadConditionalCompilationExpressionNode();
    ConditionalCompilationPartNode* ReadConditionalCompilationPartNode();
    Specifiers ReadSpecifiers();
    Span ReadSpan();
    void SetModuleId(int16_t moduleId_) { moduleId = moduleId_; }
private:
    BinaryReader binaryReader;
    int16_t moduleId;
};

} } // namespace cmajor::ast

#endif // CMAJOR_AST_AST_READER_INCLUDED
