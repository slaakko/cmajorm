// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGCM_AST_SYNC_NODE_INCLUDED
#define SNGCM_AST_SYNC_NODE_INCLUDED
#include <sngcm/ast/Node.hpp>

namespace sngcm { namespace ast {

class SNGCM_AST_API SyncNode : public Node
{
public:
    SyncNode(const Span& span_, const boost::uuids::uuid& moduleId_);
    Node* Clone(CloneContext& cloneContext) const override;
    void Accept(Visitor& visitor) override;
};

} } // namespace sngcm::ast

#endif // SNGCM_AST_SYNC_NODE_INCLUDED
