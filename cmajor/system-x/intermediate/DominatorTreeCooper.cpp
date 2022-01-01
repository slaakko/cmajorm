// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/DominatorTreeCooper.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Util.hpp>
#include <system-x/intermediate/Visit.hpp>
#include <sngxml/dom/Document.hpp>
#include <soulng/util/Unicode.hpp>
#include <set>
#include <fstream>

namespace cmsx::intermediate {

using namespace soulng::unicode;

DominatorTreeNode::DominatorTreeNode(DominatorTreeCooper* tree_, int number_, BasicBlock* basicBlock_) : tree(tree_), number(number_), basicBlock(basicBlock_), children(this)
{
}

DominatorTreeNode* DominatorTreeNode::Parent() const
{ 
    if (tree->Root() == this)
    {
        return nullptr;
    }
    else
    {
        return static_cast<DominatorTreeNode*>(children.Parent());
    }
}

void DominatorTreeNode::AddChild(DominatorTreeNode* child)
{
    children.AddChild(child);
}

void DominatorTreeNode::AddToDominatorFrontierSet(BasicBlock* block)
{
    AddPtrToSet(block, dominanceFrontierSet);
}

DominatorTreeCooper::DominatorTreeCooper(Function& function_) : function(function_), numRounds(0), nodes(this), startNodeNumber(undefinedBlockNumber)
{
}

int DominatorTreeCooper::GetBlockNumber(BasicBlock* block) const
{
    auto it = basicBlockNumberMap.find(block);
    if (it != basicBlockNumberMap.cend())
    {
        return it->second;
    }
    else
    {
        return undefinedBlockNumber;
    }
}

DominatorTreeNode* DominatorTreeCooper::Root() const
{
    return static_cast<DominatorTreeNode*>(nodes.FirstChild());
}

sngxml::dom::Element* ToXmlElement(DominatorTreeNode* node)
{
    sngxml::dom::Element* element = new sngxml::dom::Element(U"node");
    element->SetAttribute(U"id", ToUtf32(node->GetBasicBlock()->Name()));
    element->SetAttribute(U"number", ToUtf32(std::to_string(node->Number())));
    for (BasicBlock* dominanceFrontierNode : node->DominanceFrontierSet())
    {
        sngxml::dom::Element* frontier_element = new sngxml::dom::Element(U"dominance_frontier_node");
        frontier_element->SetAttribute(U"id", ToUtf32(dominanceFrontierNode->Name()));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(frontier_element));
    }
    DominatorTreeNode* child = node->FirstChild();
    while (child)
    {
        sngxml::dom::Element* childElement = ToXmlElement(child);
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(childElement));
        child = child->Next();
    }
    return element;
}

sngxml::dom::Element* DominatorTreeCooper::ToXml()
{
    sngxml::dom::Element* treeElement = new sngxml::dom::Element(U"dominator_tree");
    treeElement->SetAttribute(U"numRounds", ToUtf32(std::to_string(numRounds)));
    DominatorTreeNode* root = Root();
    sngxml::dom::Element* rootElement = ToXmlElement(root);
    treeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rootElement));
    return treeElement;
}

void DominatorTreeCooper::WriteXmlDocument(const std::string& filePath)
{
    sngxml::dom::Element* element = ToXml();
    sngxml::dom::Document document;
    document.AppendChild(std::unique_ptr<sngxml::dom::Node>(element));
    std::ofstream file(filePath);
    CodeFormatter formatter(file);
    formatter.SetIndentSize(1);
    document.Write(formatter);
}

int DominatorTreeCooper::Intersect(int b1, int b2) const
{
    int finger1 = b1;
    int finger2 = b2;
    while (finger1 != finger2)
    {
        while (finger1 < finger2)
        {
            finger1 = doms[finger1];
        }
        while (finger2 < finger1)
        {
            finger2 = doms[finger2];
        }
    }
    return finger1;
}

BasicBlock* DominatorTreeCooper::ImmediateDominator(BasicBlock* block) const
{
    if (block == function.FirstBasicBlock())
    {
        return nullptr;
    }
    else
    {
        DominatorTreeNode* node = GetNode(block);
        if (node)
        {
            return node->Parent()->GetBasicBlock();
        }
        else
        {
            throw std::runtime_error("node for basic block '" + std::to_string(block->Id()) + "' not found");
        }
    }
}

const std::vector<BasicBlock*>& DominatorTreeCooper::DominanceFrontier(BasicBlock* block) const
{
    DominatorTreeNode* node = GetNode(block);
    if (node)
    {
        return node->DominanceFrontierSet();
    }
    else
    {
        throw std::runtime_error("node for basic block '" + std::to_string(block->Id()) + "' not found");
    }
}

void DominatorTreeCooper::Init(const std::vector<BasicBlock*>& reversePostorder)
{
    int n = reversePostorder.size();
    basicBlocks.resize(n + 1);
    for (int i = 0; i < n; ++i)
    {
        BasicBlock* block = reversePostorder[i];
        int blockNumber = n - i;
        basicBlocks[blockNumber] = block;
        basicBlockNumberMap[block] = blockNumber;
    }
    doms.resize(n + 1);
    for (int i = 0; i <= n; ++i)
    {
        doms[i] = undefinedBlockNumber;
    }
    startNodeNumber = GetBlockNumber(function.FirstBasicBlock());
    doms[startNodeNumber] = startNodeNumber;
}

void DominatorTreeCooper::Run(const std::vector<BasicBlock*>& reversePostorder)
{
    bool changed = true;
    while (changed)
    {
        ++numRounds;
        changed = false;
        for (BasicBlock* block : reversePostorder)
        {
            if (!block->Predecessors().empty())
            {
                int b = GetBlockNumber(block);
                BasicBlock* firstPredecessor = block->Predecessors()[0];
                int newIdom = GetBlockNumber(firstPredecessor);
                int pn = block->Predecessors().size();
                for (int i = 1; i < pn; ++i)
                {
                    BasicBlock* predecessor = block->Predecessors()[i];
                    int p = GetBlockNumber(predecessor);
                    if (doms[p] != undefinedBlockNumber)
                    {
                        newIdom = Intersect(p, newIdom);
                    }
                }
                if (doms[b] != newIdom)
                {
                    doms[b] = newIdom;
                    changed = true;
                }
            }
        }
    }
}

void DominatorTreeCooper::Compute()
{
    ComputeDominatorTree();
    ComputeDominanceFrontier();
}

void DominatorTreeCooper::ComputeDominatorTree()
{
    std::vector<BasicBlock*> reversePostorder = ReversePostorder(function);
    Init(reversePostorder);
    Run(reversePostorder);
    MakeTree();
}

void DominatorTreeCooper::MakeTree()
{
    nodeVec.resize(doms.size());
    BasicBlock* block = function.FirstBasicBlock();
    int blockNumber = GetBlockNumber(block);
    DominatorTreeNode* root = new DominatorTreeNode(this, blockNumber, block);
    nodeVec[blockNumber] = root;
    nodes.AddChild(root);
    nodeMap[block] = root;
    block = block->Next();
    while (block)
    {
        int blockNumber = GetBlockNumber(block);
        DominatorTreeNode* node = new DominatorTreeNode(this, blockNumber, block);
        nodeMap[block] = node;
        nodeVec[blockNumber] = node;
        block = block->Next();
    }
    for (int blockNumber = 1; blockNumber < doms.size(); ++blockNumber)
    {
        if (blockNumber != startNodeNumber)
        {
            DominatorTreeNode* node = nodeVec[blockNumber];
            DominatorTreeNode* parent = nodeVec[doms[blockNumber]];
            parent->AddChild(node);
        }
    }
}

void DominatorTreeCooper::ComputeDominanceFrontier()
{
    BasicBlock* block = function.FirstBasicBlock();
    while (block)
    {
        if (block->Predecessors().size() > 1)
        {
            int b = GetBlockNumber(block);
            for (BasicBlock* predecessor : block->Predecessors())
            {
                int runner = GetBlockNumber(predecessor);
                while (runner != doms[b])
                {
                    DominatorTreeNode* runnerNode = nodeVec[runner];
                    runnerNode->AddToDominatorFrontierSet(block);
                    runner = doms[runner];
                }
            }
        }
        block = block->Next();
    }
}

DominatorTreeNode* DominatorTreeCooper::GetNode(BasicBlock* block) const
{
    auto it = nodeMap.find(block);
    if (it != nodeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

bool DominatorTreeCooper::Dominates(BasicBlock* x, BasicBlock* y) const
{
    if (x == y)
    {
        return true;
    }
    DominatorTreeNode* y_node = GetNode(y);
    if (y_node)
    {
        DominatorTreeNode* x_node = GetNode(x);
        if (x_node)
        {
            return IsSameParentOrAncestor(x_node, y_node);
        }
        else
        {
            throw std::runtime_error("node for basic block '" + std::to_string(x->Id()) + "' not found");
        }
    }
    else
    {
        throw std::runtime_error("node for basic block '" + std::to_string(y->Id()) + "' not found");
    }
}

bool DominatorTreeCooper::IsSameParentOrAncestor(DominatorTreeNode* x, DominatorTreeNode* y) const
{
    if (x)
    {
        if (x == y)
        {
            return true;
        }
        else 
        {
            return IsSameParentOrAncestor(x->Parent(), y);
        }
    }
    else
    {
        return false;
    }
}

} // cmsx::intermediate
