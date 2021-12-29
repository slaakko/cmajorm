// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_DOMINATOR_TREE_COOPER_INCLUDED
#define CMSX_INTERMEDIATE_DOMINATOR_TREE_COOPER_INCLUDED
#include <system-x/intermediate/Code.hpp>
#include <sngxml/dom/Element.hpp>

namespace cmsx::intermediate {

const int undefinedBlockNumber = 0;

class DominatorTreeCooper;

class CMSX_INTERMEDIATE_API DominatorTreeNode : public soulng::util::Component
{
public:
    DominatorTreeNode(DominatorTreeCooper* tree_, int number_, BasicBlock* basicBlock_);
    int Number() const { return number; }
    void AddChild(DominatorTreeNode* child);
    DominatorTreeNode* FirstChild() const { return static_cast<DominatorTreeNode*>(children.FirstChild()); }
    DominatorTreeNode* LastChild() const { return static_cast<DominatorTreeNode*>(children.LastChild()); }
    DominatorTreeNode* Parent() const;
    DominatorTreeNode* Next() const { return static_cast<DominatorTreeNode*>(NextSibling()); }
    DominatorTreeNode* Prev() const { return static_cast<DominatorTreeNode*>(PrevSibling()); }
    BasicBlock* GetBasicBlock() const { return basicBlock; }
    const std::vector<BasicBlock*>& DominanceFrontierSet() const { return dominanceFrontierSet; }
    void AddToDominatorFrontierSet(BasicBlock* block);
private:
    DominatorTreeCooper* tree;
    int number;
    BasicBlock* basicBlock;
    soulng::util::Container children;
    std::vector<BasicBlock*> dominanceFrontierSet;
};

class CMSX_INTERMEDIATE_API DominatorTreeCooper : public soulng::util::Component
{
public:
    DominatorTreeCooper(Function& function_);
    Function* GetFunction() const { return &function; }
    void Compute();
    void ComputeDominatorTree();
    void ComputeDominanceFrontier();
    BasicBlock* ImmediateDominator(BasicBlock* block) const;
    const std::vector<BasicBlock*>& DominanceFrontier(BasicBlock* block) const;
    int GetBlockNumber(BasicBlock* block) const;
    BasicBlock* GetBlock(int blockNumber) const { return basicBlocks[blockNumber]; }
    sngxml::dom::Element* ToXml();
    void WriteXmlDocument(const std::string& filePath);
    DominatorTreeNode* Root() const;
    DominatorTreeNode* GetNode(BasicBlock* block) const;
    bool Dominates(BasicBlock* x, BasicBlock* y) const;
private:
    int Intersect(int b1, int b2) const;
    void Init(const std::vector<BasicBlock*>& reversePostorder);
    void Run(const std::vector<BasicBlock*>& reversePostorder);
    void MakeTree();
    bool IsSameParentOrAncestor(DominatorTreeNode* x, DominatorTreeNode* y) const;
    Function& function;
    std::map<BasicBlock*, int> basicBlockNumberMap;
    std::vector<BasicBlock*> basicBlocks;
    std::vector<int> doms;
    std::vector<DominatorTreeNode*> nodeVec;
    int numRounds;
    soulng::util::Container nodes;
    std::map<BasicBlock*, DominatorTreeNode*> nodeMap;
    int startNodeNumber;
};

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_DOMINATOR_TREE_COOPER_INCLUDED
