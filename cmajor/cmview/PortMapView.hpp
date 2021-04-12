// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_PORT_MAP_VIEW_INCLUDED
#define CMAJOR_VIEW_PORT_MAP_VIEW_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/cmpm/PortMapMessage.hpp>
#include <cmajor/wing/Container.hpp>
#include <cmajor/wing/TreeView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

class CMVIEW_API PortMapView : public Control
{
public:
    PortMapView();
    void Clear();
    void Update(const std::vector<PortLease>& portLeases);
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnLocationChanged() override;
    void OnSizeChanged() override;
private:
    void SetChildPos();
    Container container;
    TreeView* treeView;
    Control* child;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_PORT_MAP_VIEW_INCLUDED
