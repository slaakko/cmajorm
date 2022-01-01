// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_VIEW_RESOURCE_FILE_EDITOR_INCLUDED
#define CMAJOR_VIEW_RESOURCE_FILE_EDITOR_INCLUDED
#include <cmajor/cmview/Editor.hpp>

namespace cmajor { namespace view {

struct CMVIEW_API ResourceFileEditorCreateParams
{
    ResourceFileEditorCreateParams(const std::string& filePath_);
    ResourceFileEditorCreateParams& Defaults();
    ControlCreateParams editorCreateParams;
    TextViewCreateParams textViewCreateParams;
    std::string filePath;
};

class CMVIEW_API ResourceFileEditor : public Editor
{
public:
    ResourceFileEditor(ResourceFileEditorCreateParams& createParams);
    const std::string& FilePath() const override;
    bool IsDirty() const override;
    void ResetDirty() override;
    void Save() override;
    void Select() override;
    void Undo() override;
    void Redo() override;
    TextView* GetTextView() const override;
private:
    TextView* textView;
};

} } // namespace cmajor::view

#endif // CMAJOR_VIEW_RESOURCE_FILE_EDITOR_INCLUDED
