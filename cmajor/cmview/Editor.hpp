// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_EDITOR_INCLUDED
#define CMAJOR_VIEW_EDITOR_INCLUDED
#include <cmajor/cmview/ViewApi.hpp>
#include <cmajor/wing/ContainerControl.hpp>
#include <cmajor/wing/TextView.hpp>

namespace cmajor { namespace view {

using namespace cmajor::wing;

class CMVIEW_API Editor : public ContainerControl
{
public:
    Editor(ControlCreateParams& createParams);
    virtual bool IsCmajorEditor() const { return false; }
    virtual const std::string& FilePath() const = 0;
    virtual bool IsDirty() const = 0;
    virtual void ResetDirty() = 0;
    virtual bool IsCCDirty() const;
    virtual void ResetCCDirty();
    virtual void Save() = 0;
    virtual void Select() = 0;
    virtual void Undo() = 0;
    virtual void Redo() = 0;
    virtual TextView* GetTextView() const = 0;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_EDITOR_INCLUDED
