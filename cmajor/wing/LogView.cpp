// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/LogView.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;
    
TextViewCreateParams& MakeTextViewCreateParams(TextViewCreateParams& createParams)
{
    createParams.controlCreateParams.WindowClassName("wing.LogView");
    createParams.FontSize(9.0f);
    return createParams;
}

LogView::LogView(TextViewCreateParams& createParams) : TextView(MakeTextViewCreateParams(createParams))
{
    SetReadOnly();
}

void LogView::WriteLine(const std::string& text)
{
    int startLineNumber = static_cast<int>(Lines().size() - 1);
    std::vector<std::u32string> lines = SplitTextIntoLines(ToUtf32(text));
    for (const std::u32string& line : lines)
    {
        AddLine(line);
    }
    SetCaretLineCol(static_cast<int>(Lines().size()), 1);
    SetTextExtent();
    ScrollToCaret();
    SetChanged();
    InvalidateLines(startLineNumber, static_cast<int>(startLineNumber + lines.size()));
}

} } // cmajor::wing
