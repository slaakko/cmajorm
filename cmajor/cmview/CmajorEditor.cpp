// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/CmajorEditor.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace view {

using namespace soulng::unicode;

CmajorEditorCreateParams::CmajorEditorCreateParams(const std::string& filePath_, CmajorSourceCodeViewCreateParams& sourceCodeViewCreateParams_, DebugStripCreateParams& debugStripCreateParams_) :
    editorCreateParams(), sourceCodeViewCreateParams(sourceCodeViewCreateParams_), debugStripCreateParams(debugStripCreateParams_), filePath(filePath_)
{
    editorCreateParams.WindowClassName("cmajor.view.CmajorEditor");
    editorCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    editorCreateParams.BackgroundColor(Color::White);
    editorCreateParams.SetDock(Dock::fill);
}

CmajorEditorCreateParams& CmajorEditorCreateParams::Defaults()
{
    return *this;
}

CmajorEditor::CmajorEditor(CmajorEditorCreateParams& createParams) : Editor(createParams.editorCreateParams), sourceCodeView(nullptr), debugStrip(nullptr)
{
    std::unique_ptr<CmajorSourceCodeView> sourceCodeViewPtr(new CmajorSourceCodeView(createParams.sourceCodeViewCreateParams.Defaults()));
    sourceCodeView = sourceCodeViewPtr.get();
    sourceCodeView->SetFlag(ControlFlags::scrollSubject);
    std::unique_ptr<DebugStrip> debugStripPtr(new DebugStrip(createParams.debugStripCreateParams.SetTextView(sourceCodeView)));
    debugStrip = debugStripPtr.get();
    AddChild(debugStripPtr.release());
    ScrollableControl* scrollableSourceCodeView(new ScrollableControl(ScrollableControlCreateParams(sourceCodeViewPtr.release()).SetDock(Dock::fill)));
    AddChild(scrollableSourceCodeView);
    sourceCodeView->SetEditor(this);
    sourceCodeView->SetDebugStrip(debugStrip);
    sourceCodeView->SetFilePath(createParams.filePath);
    std::u32string content = ToUtf32(soulng::util::ReadFile(createParams.filePath));
    sourceCodeView->SetTextContent(content);
    sourceCodeView->SetIndentSize(4);
    sourceCodeView->SetDoubleBuffered();
}

const std::string& CmajorEditor::FilePath() const
{
    return sourceCodeView->FilePath();
}

bool CmajorEditor::IsDirty() const
{
    return sourceCodeView->IsDirty();
}

void CmajorEditor::ResetDirty()
{
    sourceCodeView->ResetDirty();
}

bool CmajorEditor::IsCCDirty() const
{
    return sourceCodeView->IsCCDirty();
}

void CmajorEditor::ResetCCDirty()
{
    sourceCodeView->ResetCCDirty();
}

void CmajorEditor::Save()
{
    sourceCodeView->SaveText();
}

void CmajorEditor::Select()
{
    sourceCodeView->Select();
}

void CmajorEditor::Undo()
{
    sourceCodeView->Undo();
}

void CmajorEditor::Redo()
{
    sourceCodeView->Redo();
}

TextView* CmajorEditor::GetTextView() const
{
    return sourceCodeView;
}

} } // namespace cmajor::view
