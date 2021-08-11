// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/ResourceFileEditor.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace view {

using namespace soulng::unicode;

ResourceFileEditorCreateParams::ResourceFileEditorCreateParams(const std::string& filePath_) : editorCreateParams(), textViewCreateParams(), filePath(filePath_)
{
    editorCreateParams.WindowClassName("cmajor.view.ResourceFileEditor");
    editorCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    editorCreateParams.BackgroundColor(Color::White);
    editorCreateParams.SetDock(Dock::fill);
}

ResourceFileEditorCreateParams& ResourceFileEditorCreateParams::Defaults()
{
    return *this;
}

ResourceFileEditor::ResourceFileEditor(ResourceFileEditorCreateParams& createParams) : Editor(createParams.editorCreateParams), textView(nullptr)
{
    std::unique_ptr<TextView> textViewPtr(new TextView(createParams.textViewCreateParams.Defaults()));
    textView = textViewPtr.get();
    ScrollableControl* scrollableTextView(new ScrollableControl(ScrollableControlCreateParams(textViewPtr.release()).SetDock(Dock::fill)));
    AddChild(scrollableTextView);
    textView->SetFilePath(createParams.filePath);
    std::u32string content = ToUtf32(soulng::util::ReadFile(createParams.filePath));
    textView->SetTextContent(content);
    textView->SetIndentSize(4);
    textView->SetDoubleBuffered();
}

const std::string& ResourceFileEditor::FilePath() const
{
    return textView->FilePath();
}

bool ResourceFileEditor::IsDirty() const
{
    return textView->IsDirty();
}

void ResourceFileEditor::ResetDirty()
{
    textView->ResetDirty();
}

void ResourceFileEditor::Save()
{
    textView->SaveText();
}

void ResourceFileEditor::Select()
{
    textView->Select();
}

void ResourceFileEditor::Undo()
{
    textView->Undo();
}

void ResourceFileEditor::Redo()
{
    textView->Redo();
}

TextView* ResourceFileEditor::GetTextView() const
{
    return textView;
}

} } // namespace cmajor::view
