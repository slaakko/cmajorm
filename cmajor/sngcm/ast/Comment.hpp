// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_COMMENT_INCLUDED
#define SNGCM_AST_COMMENT_INCLUDED
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API CommentNode : public Node
{
public:
    CommentNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    CommentNode(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& comment_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
    void Write(AstWriter& writer) override;
    void Read(AstReader& reader) override;
    const std::u32string& Comment() const { return comment; }
private:
    std::u32string comment;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_COMMENT_INCLUDED
