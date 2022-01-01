// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_NAMESPACE_INCLUDED
#define CMAJOR_BINDER_BOUND_NAMESPACE_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <sngcm/ast/Namespace.hpp>

namespace cmajor { namespace binder {

using namespace sngcm::ast;

class BINDER_API BoundNamespace : public BoundNode
{
public:
    BoundNamespace(NamespaceNode& namespaceNode_);
    BoundNamespace(const BoundNamespace&) = delete;
    BoundNamespace& operator=(const BoundNamespace&) = delete;
    void Accept(BoundNodeVisitor& visitor) override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    NamespaceNode& GetNamespaceNode() { return namespaceNode; }
    void AddMember(std::unique_ptr<BoundNode>&& member);
    const std::vector<std::unique_ptr<BoundNode>>& Members() const { return members; }
private:
    NamespaceNode& namespaceNode;
    std::vector<std::unique_ptr<BoundNode>> members;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_NAMESPACE_INCLUDED
