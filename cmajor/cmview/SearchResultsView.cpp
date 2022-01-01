// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/SearchResultsView.hpp>
#include <cmajor/wing/PaddedControl.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <wing/Theme.hpp>

namespace cmajor { namespace view {

SearchResult::SearchResult(const std::string& fileName_, int lineNumber_, const std::string& line_) : fileName(fileName_), lineNumber(lineNumber_), line(line_)
{
}

ViewSearchResultEventArgs::ViewSearchResultEventArgs(SearchResult* searchResult_) : searchResult(searchResult_)
{
}

SearchResultsView::SearchResultsView() : 
    Control(ControlCreateParams().WindowClassName("cmajor.view.SearchResultsView").WindowClassBackgroundColor(COLOR_WINDOW).
    BackgroundColor(GetColor("search.results.background")).SetDock(Dock::fill)),
    container(this), treeView(nullptr), child(nullptr)
{
    std::unique_ptr<TreeView> treeViewPtr(new TreeView(TreeViewCreateParams().BackgroundColor(GetColor("search.results.background")).TextColor(GetColor("search.results.text")).
        SelectedNodeColor(GetColor("search.results.node.selected")).StateIndicatorColor(GetColor("search.results.state.indicator"))));
    treeView = treeViewPtr.get();
    treeView->SetBackgroundItemName("search.results.background");
    treeView->SetTextItemName("search.results.text");
    treeView->SetStateIndicatorItemName("search.results.state.indicator");
    treeView->SetNodeSelectedItemName("search.results.node.selected");
    treeView->SetFlag(ControlFlags::scrollSubject);
    treeView->SetDoubleBuffered();
    treeView->NodeDoubleClick().AddHandler(this, &SearchResultsView::TreeViewNodeDoubleClick);
    std::unique_ptr<Control> paddedTreeView(new PaddedControl(PaddedControlCreateParams(treeViewPtr.release()).Defaults()));
    std::unique_ptr<Control> scrollableTreeView(new ScrollableControl(ScrollableControlCreateParams(paddedTreeView.release()).SetDock(Dock::fill)));
    child = scrollableTreeView.get();
    container.AddChild(scrollableTreeView.release());
    Clear();
}

void SearchResultsView::Clear()
{
    treeView->SetRoot(new TreeViewNode("Search Results"));
    child->Invalidate();
}

void SearchResultsView::Add(SearchResult* searchResult)
{
    searchResults.push_back(std::unique_ptr<SearchResult>(searchResult));
    std::string text = searchResult->fileName;
    text.append(1, ':').append(std::to_string(searchResult->lineNumber));
    TreeViewNode* resultNode = new TreeViewNode(text);
    resultNode->SetData(searchResult);
    resultNode->AddChild(new TreeViewNode(searchResult->line));
    treeView->Root()->AddChild(resultNode);
}

void SearchResultsView::TreeViewNodeDoubleClick(TreeViewNodeClickEventArgs& args)
{
    SearchResult* searchResult = static_cast<SearchResult*>(args.node->Data());
    ViewSearchResultEventArgs viewSearchResultArgs(searchResult);
    OnViewSearchResult(viewSearchResultArgs);
}

void SearchResultsView::SetChildPos()
{
    Point loc;
    Size size = GetSize();
    child->SetLocation(loc);
    child->SetSize(size);
}

void SearchResultsView::OnLocationChanged()
{
    Control::OnLocationChanged();
    SetChildPos();
}

void SearchResultsView::OnSizeChanged()
{
    Control::OnSizeChanged();
    SetChildPos();
}

void SearchResultsView::OnPaint(PaintEventArgs& args)
{
    treeView->Root()->ExpandAll();
    treeView->Invalidate();
    child->Invalidate();
    Control::OnPaint(args);
}

void SearchResultsView::OnViewSearchResult(ViewSearchResultEventArgs& args)
{
    viewSearchResult.Fire(args);
}

} } // namespace cmajor::view