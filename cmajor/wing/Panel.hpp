// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_PANEL_INCLUDED
#define CMAJOR_WING_PANEL_INCLUDED
#include <cmajor/wing/ContainerControl.hpp>

namespace cmajor { namespace wing {

class WING_API Panel : public ContainerControl
{
public:
    Panel(ControlCreateParams& createParams);
protected:
    void OnPaint(PaintEventArgs& args) override;
};

} } // cmajor::wing

#endif // CMAJOR_WING_PANEL_INCLUDED
