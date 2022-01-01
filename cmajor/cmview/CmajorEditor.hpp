// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_CMAJOR_EDITOR_INCLUDED
#define CMAJOR_VIEW_CMAJOR_EDITOR_INCLUDED
#include <cmajor/cmview/Editor.hpp>
#include <cmajor/cmview/CmajorSourceCodeView.hpp>
#include <cmajor/cmview/DebugStrip.hpp>

namespace cmajor { namespace view {

struct CMVIEW_API CmajorEditorCreateParams
{
    CmajorEditorCreateParams(const std::string& filePath_, CmajorSourceCodeViewCreateParams& sourceCodeViewCreateParams_, DebugStripCreateParams& debugStripCreateParams_);
    CmajorEditorCreateParams& Defaults();
    ControlCreateParams editorCreateParams;
    CmajorSourceCodeViewCreateParams sourceCodeViewCreateParams;
    DebugStripCreateParams debugStripCreateParams;
    std::string filePath;
};

class CMVIEW_API CmajorEditor : public Editor
{
public:
    CmajorEditor(CmajorEditorCreateParams& createParams);
    bool IsCmajorEditor() const override { return true; }
    const std::string& FilePath() const override;
    bool IsDirty() const override;
    void ResetDirty() override;
    bool IsCCDirty() const override;
    void ResetCCDirty() override;
    void Save() override;
    void Select() override;
    void Undo() override;
    void Redo() override;
    TextView* GetTextView() const override;
    CmajorSourceCodeView* SourceCodeView() { return sourceCodeView; }
    DebugStrip* GetDebugStrip() const { return debugStrip; }
private:
    CmajorSourceCodeView* sourceCodeView;
    DebugStrip* debugStrip;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_CMAJOR_EDITOR_INCLUDED
