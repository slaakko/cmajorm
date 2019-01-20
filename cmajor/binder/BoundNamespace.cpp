// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/symbols/Exception.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

BoundNamespace::BoundNamespace(Module* module_, NamespaceNode& namespaceNode_) : BoundNode(module_, namespaceNode_.GetSpan(), BoundNodeType::boundNamespace), namespaceNode(namespaceNode_)
{
}

void BoundNamespace::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundNamespace::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot load a namespace", GetSpan());
}

void BoundNamespace::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception(GetModule(), "cannot store a namespace", GetSpan());
}

void BoundNamespace::AddMember(std::unique_ptr<BoundNode>&& member)
{
    members.push_back(std::move(member));
}

} } // namespace cmajor::binder
