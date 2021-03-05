// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMCODE_SOLUTION_INCLUDED
#define CMCODE_SOLUTION_INCLUDED
#include <sngcm/ast/Project.hpp>
#include <sngcm/ast/Solution.hpp>
#include <cmajor/wing/TreeView.hpp>
#include <map>

namespace cmcode {

using namespace cmajor::wing;

std::unique_ptr<sngcm::ast::Project> ReadProject(const std::string& projectFilePath, std::vector<std::unique_ptr<std::u32string>>& contents);
std::unique_ptr<sngcm::ast::Solution> ReadSolution(const std::string& solutionFilePath, std::vector<std::unique_ptr<std::u32string>>& contents);

enum class SolutionTreeViewNodeDataKind : int
{
    solution, project, file
};

struct SolutionTreeViewNodeData
{
    SolutionTreeViewNodeData(SolutionTreeViewNodeDataKind kind_, sngcm::ast::Solution* solution_, sngcm::ast::Project* project_, const std::string& filePath_, const std::string& fileName_);
    SolutionTreeViewNodeDataKind kind;
    sngcm::ast::Solution* solution;
    sngcm::ast::Project* project;
    std::string filePath;
    std::string fileName;
    std::string key;
};

class SolutionData
{
public:
    SolutionData(std::unique_ptr<sngcm::ast::Solution>&& solution_, TreeView* solutionTreeView_);
    sngcm::ast::Solution* GetSolution() const { return solution.get(); }
    TreeView* SolutionTreeView() const { return solutionTreeView; }
    TreeViewNode* ActiveProjectNode() const { return activeProjectNode; }
    void SetActiveProjectNode(TreeViewNode* activeProjectNode_) { activeProjectNode = activeProjectNode_; }
    SolutionTreeViewNodeData* GetSolutionTreeViewNodeDataByKey(const std::string& key) const;
private:
    void SetActiveProject();
    TreeView* solutionTreeView;
    std::unique_ptr<sngcm::ast::Solution> solution;
    std::vector<std::unique_ptr<SolutionTreeViewNodeData>> treeViewData;
    std::map<std::string, SolutionTreeViewNodeData*> treeViewDataMap;
    TreeViewNode* activeProjectNode;
};

} // namespace cmcode

#endif // CMCODE_SOLUTION_INCLUDED
