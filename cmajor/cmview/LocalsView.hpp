// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
#define CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmmsg/CmdbMessage.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

class CMVIEW_API LocalsView: public Control
{
public:
    LocalsView();
    void Clear();
protected:
    void OnLocationChanged() override;
    void OnSizeChanged() override;
    void OnPaint(PaintEventArgs& args) override;
private:
    void SetChildPos();
    Container container;
    TreeView* treeView;
    Control* child;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_LOCALS_VIEW_INCLUDED
