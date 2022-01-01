// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_SEARCH_RESULTS_VIEW_INCLUDED
#define CMAJOR_VIEW_SEARCH_RESULTS_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

struct CMVIEW_API SearchResult
{
    SearchResult(const std::string& fileName_, int lineNumber_, const std::string& line_);
    std::string fileName;
    int lineNumber;
    std::string line;
};

struct CMVIEW_API ViewSearchResultEventArgs
{
    ViewSearchResultEventArgs(SearchResult* searchResult_);
    SearchResult* searchResult;
};

using ViewSearchResultEvent = EventWithArgs<ViewSearchResultEventArgs>;

class CMVIEW_API SearchResultsView : public Control
{
public:
    SearchResultsView();
    ViewSearchResultEvent& ViewSearchResult() { return viewSearchResult; }
    void Clear();
    void Add(SearchResult* searchResult);
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
    virtual void OnViewSearchResult(ViewSearchResultEventArgs& args);
private:
    void TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args);
    void SetChildPos();
    Container container;
    TreeView* treeView;
    Control* child;
    std::vector<std::unique_ptr<SearchResult>> searchResults;
    ViewSearchResultEvent viewSearchResult;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_SEARCH_RESULTS_VIEW_INCLUDED
