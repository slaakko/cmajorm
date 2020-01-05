// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_AST_READER_INCLUDED
#define SNGCM_AST_AST_READER_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/lexer/Span.hpp>

namespace sngcm { namespace ast {

using namespace soulng::util;
using soulng::lexer::Span;
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
    return (static_cast<int32_t>(moduleId) << 16) | fileId;
}

inline int16_t GetModuleId(int32_t fileIndex)
{
    return static_cast<int16_t>((fileIndex >> 16) & 0xFFFF);
}

inline int16_t GetFileId(int32_t fileIndex)
{
    return static_cast<int16_t>(fileIndex & 0xFFFF);
}

class SNGCM_AST_API AstReader
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

} } // namespace sngcm::ast

#endif // SNGCM_AST_AST_READER_INCLUDED
