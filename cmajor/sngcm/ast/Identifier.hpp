// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_IDENTIFIER_INCLUDED
#define SNGCM_AST_IDENTIFIER_INCLUDED
#include <soulng/lexer/Token.hpp>
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API IdentifierNode : public Node
{
public:
    IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, NodeType nodeType_);
    IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& identifier_);
    IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, NodeType nodeType_, const std::u32string& identifier_);
    IdentifierNode(const Span& span_, const boost::uuids::uuid& moduleId_, const soulng::lexer::Token& token);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const std::u32string& Str() const { return identifier; }
    void SetStr(const std::u32string& str) { identifier = str; }
    std::string ToString() const override;
    bool IsInternal() const;
private:
    std::u32string identifier;
};

class SNGCM_AST_API CursorIdNode : public IdentifierNode
{
public:
    CursorIdNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    CursorIdNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& identifier_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_IDENTIFIER_INCLUDED
