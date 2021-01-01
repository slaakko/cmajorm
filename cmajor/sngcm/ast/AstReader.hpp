// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_AST_READER_INCLUDED
#define SNGCM_AST_AST_READER_INCLUDED
#include <sngcm/ast/AstApi.hpp>
#include <sngcm/ast/Specifier.hpp>
#include <soulng/util/BinaryReader.hpp>
#include <soulng/lexer/Span.hpp>
#include <unordered_map>

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
    void SetModuleMaps(const boost::uuids::uuid& rootModuleId, std::unordered_map<int16_t, std::string>* moduleNameTable_, std::unordered_map<std::string, int16_t>* moduleIdMap_);
private:
    BinaryReader binaryReader;
    boost::uuids::uuid rootModuleId;
    std::unordered_map<int16_t, std::string>* moduleNameTable;
    std::unordered_map<std::string, int16_t>* moduleIdMap;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_AST_READER_INCLUDED
