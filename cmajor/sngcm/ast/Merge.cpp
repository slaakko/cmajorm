// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/Merge.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Visitor.hpp>

namespace sngcm { namespace ast {

class NodeSelectorVisitor : public Visitor
{
public:
    NodeSelectorVisitor(const std::u32string& nodeName_, NodeType nodeType_);
    Node* GetSelectedNode() const { return selectedNode; }
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(ClassNode& classNode) override;
private:
    std::u32string nodeName;
    NodeType nodeType;
    Node* selectedNode;
};


NodeSelectorVisitor::NodeSelectorVisitor(const std::u32string& nodeName_, NodeType nodeType_) : nodeName(nodeName_), nodeType(nodeType_), selectedNode(nullptr)
{
}

void NodeSelectorVisitor::Visit(NamespaceNode& namespaceNode)
{
    if (nodeType == NodeType::namespaceNode)
    {
        if (namespaceNode.Id()->Str() == nodeName)
        {
            selectedNode = &namespaceNode;
        }
    }
    if (!selectedNode)
    {
        int n = namespaceNode.Members().Count();
        for (int i = 0; i < n; ++i)
        {
            namespaceNode.Members()[i]->Accept(*this);
            if (selectedNode) return;
        }
    }
}

void NodeSelectorVisitor::Visit(ClassNode& classNode)
{
    if (nodeType == NodeType::classNode)
    {
        if (classNode.Id()->Str() == nodeName)
        {
            selectedNode = &classNode;
        }
        if (!selectedNode)
        {
            int n = classNode.Members().Count();
            for (int i = 0; i < n; ++i)
            {
                classNode.Members()[i]->Accept(*this);
                if (selectedNode) return;
            }
        }
    }
}

class MergeVisitor : public Visitor
{
public:
    MergeVisitor(Node* targetContainer_);
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(ClassNode& classNode) override;
private:
    Node* targetContainer;
};

MergeVisitor::MergeVisitor(Node* targetContainer_) : targetContainer(targetContainer_)
{
}

void MergeVisitor::Visit(NamespaceNode& namespaceNode)
{
    bool added = false;
    if (!namespaceNode.Id()->Str().empty())
    {
        NodeSelectorVisitor visitor(namespaceNode.Id()->Str(), NodeType::namespaceNode);
        targetContainer->Accept(visitor);
        Node* selectedNode = visitor.GetSelectedNode();
        if (selectedNode)
        {
            targetContainer = selectedNode;
        }
        else
        {
            if (targetContainer->GetNodeType() == NodeType::namespaceNode)
            {
                NamespaceNode* targetNamespace = static_cast<NamespaceNode*>(targetContainer);
                CloneContext cloneContext;
                Node* clonedSource = namespaceNode.Clone(cloneContext);
                targetNamespace->AddMember(clonedSource);
                added = true;
            }
        }
    }
    if (!added && targetContainer->GetNodeType() == NodeType::namespaceNode)
    {
        NamespaceNode* targetNamespace = static_cast<NamespaceNode*>(targetContainer);
        int n = namespaceNode.Members().Count();
        for (int i = 0; i < n; ++i)
        {
            Node* member = namespaceNode.Members()[i];
            if (member->GetNodeType() == NodeType::namespaceNode)
            {
                member->Accept(*this);
            }
            else if (member->GetNodeType() == NodeType::classNode)
            {
                member->Accept(*this);
            }
            else
            {
                CloneContext cloneContext;
                Node* clonedSource = member->Clone(cloneContext);
                targetNamespace->AddMember(clonedSource);
            }
        }
    }
}

void MergeVisitor::Visit(ClassNode& classNode)
{
    bool added = false;
    NodeSelectorVisitor visitor(classNode.Id()->Str(), NodeType::classNode);
    targetContainer->Accept(visitor);
    Node* selectedNode = visitor.GetSelectedNode();
    if (selectedNode)
    {
        targetContainer = selectedNode;
    }
    else
    {
        if (targetContainer->GetNodeType() == NodeType::namespaceNode)
        {
            NamespaceNode* targetNamespace = static_cast<NamespaceNode*>(targetContainer);
            CloneContext cloneContext;
            Node* clonedSource = classNode.Clone(cloneContext);
            targetNamespace->AddMember(clonedSource);
            added = true;
        }
        else if (targetContainer->GetNodeType() == NodeType::classNode)
        {
            ClassNode* targetClass = static_cast<ClassNode*>(targetContainer);
            CloneContext cloneContext;
            Node* clonedSource = classNode.Clone(cloneContext);
            targetClass->AddMember(clonedSource);
            added = true;
        }
    }
    if (!added && targetContainer->GetNodeType() == NodeType::classNode)
    {
        ClassNode* targetClass = static_cast<ClassNode*>(targetContainer);
        int n = classNode.Members().Count();
        for (int i = 0; i < n; ++i)
        {
            Node* member = classNode.Members()[i];
            if (member->GetNodeType() == NodeType::classNode)
            {
                member->Accept(*this);
            }
            else
            {
                CloneContext cloneContext;
                Node* clonedSource = member->Clone(cloneContext);
                targetClass->AddMember(clonedSource);
            }
        }
    }
}

void Merge(CompileUnitNode& source, CompileUnitNode& target)
{
    MergeVisitor visitor(target.GlobalNs());
    source.GlobalNs()->Accept(visitor);
}

} } // namespace sngcm::ast
