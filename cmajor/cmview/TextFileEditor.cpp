// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmview/TextFileEditor.hpp>
#include <cmajor/wing/ScrollableControl.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace view {

using namespace soulng::unicode;    

TextFileEditorCreateParams::TextFileEditorCreateParams(const std::string& filePath_) : editorCreateParams(), textViewCreateParams(), filePath(filePath_)
{
    editorCreateParams.WindowClassName("cmajor.view.TextFileEditor");
    editorCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    editorCreateParams.BackgroundColor(Color::White);
    editorCreateParams.SetDock(Dock::fill);
}

TextFileEditorCreateParams& TextFileEditorCreateParams::Defaults()
{
    return *this;
}

TextFileEditor::TextFileEditor(TextFileEditorCreateParams& createParams) : Editor(createParams.editorCreateParams), textView(nullptr)
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

const std::string& TextFileEditor::FilePath() const
{
    return textView->FilePath();
}

bool TextFileEditor::IsDirty() const
{
    return textView->IsDirty();
}

void TextFileEditor::ResetDirty()
{
    textView->ResetDirty();
}

void TextFileEditor::Save() 
{
    textView->SaveText();
}

void TextFileEditor::Select()
{
    textView->Select();
}

void TextFileEditor::Undo()
{
    textView->Undo();
}

void TextFileEditor::Redo()
{
    textView->Redo();
}

TextView* TextFileEditor::GetTextView() const
{
    return textView;
}

} } // namespace cmajor::view
