// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/SourceCodeView.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

TokenLine DefaultTokenizeLine(const std::u32string& line, int lineNumber, int startState)
{
    Token token;
    TokenLine tokenLine;
    const char32_t* begin = line.c_str();
    const char32_t* end = line.c_str() + line.length();
    token.match.begin = begin;
    token.match.end = end;
    token.line = lineNumber;
    tokenLine.tokens.push_back(token);
    tokenLine.endState = 0;
    return tokenLine;
}

SourceCodeFont::SourceCodeFont(const std::string& fontFamilyName_, float fontSize_, FontStyle fontStyle_) : fontFamilyName(fontFamilyName_), fontSize(fontSize_), fontStyle(fontStyle_)
{
}

bool operator<(const SourceCodeFont& left, const SourceCodeFont& right)
{
    if (left.fontFamilyName < right.fontFamilyName) return true;
    if (left.fontFamilyName > right.fontFamilyName) return false;
    if (left.fontStyle < right.fontStyle) return true;
    if (left.fontStyle > right.fontStyle) return false;
    return left.fontSize < right.fontSize;
}

SourceCodeViewCreateParams::SourceCodeViewCreateParams()
{
    textViewCreateParams.WindowClassName("wing.SourceCodeView");
    SetSourceCodeTokenStyle(SourceCodeTokenKind::plain, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::space, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::keyword, TokenStyle(Color::Blue, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::identifier, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::string, TokenStyle(Color(163, 21, 21), FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::character, TokenStyle(Color(163, 21, 21), FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::number, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::comment, TokenStyle(Color(0, 128, 0), FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::lineNumber, TokenStyle(Color(43, 145, 175), FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::beginBlock, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
    SetSourceCodeTokenStyle(SourceCodeTokenKind::endBlock, TokenStyle(Color::Black, FontStyle::FontStyleRegular));
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::Defaults()
{
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    textViewCreateParams.WindowClassName(windowClassName_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    textViewCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::WindowStyle(int windowStyle_)
{
    textViewCreateParams.WindowStyle(windowStyle_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    textViewCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    textViewCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::Text(const std::string& text_)
{
    textViewCreateParams.Text(text_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::Location(const Point& location_)
{
    textViewCreateParams.Location(location_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::SetSize(const Size& size_)
{
    textViewCreateParams.SetSize(size_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::SetAnchors(Anchors anchors_)
{
    textViewCreateParams.SetAnchors(anchors_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::SetDock(Dock dock_)
{
    textViewCreateParams.SetDock(dock_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::TextColor(const Color& textColor_)
{
    textViewCreateParams.TextColor(textColor_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::SelectionBackgroundColor(const Color& selectionBackgroundColor_)
{
    textViewCreateParams.SelectionBackgroundColor(selectionBackgroundColor_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    textViewCreateParams.FontFamilyName(fontFamilyName_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::FontSize(float fontSize_)
{
    textViewCreateParams.FontSize(fontSize_);
    return *this;
}

SourceCodeViewCreateParams& SourceCodeViewCreateParams::SetSourceCodeTokenStyle(SourceCodeTokenKind tokenKind, TokenStyle tokenStyle)
{
    tokenStyleMap[tokenKind] = tokenStyle;
    return *this;
}

SourceCodeView::SourceCodeView(SourceCodeViewCreateParams& createParams) : 
    TextView(createParams.textViewCreateParams), tokenStyleMap(createParams.tokenStyleMap), numLineNumberDigits(0)
{
}

TokenLine SourceCodeView::TokenizeLine(const std::u32string& line, int lineNumber, int startState)
{
    return DefaultTokenizeLine(line, lineNumber, startState);
}

SourceCodeTokenKind SourceCodeView::GetTokenKind(const Token& token) const
{
    return SourceCodeTokenKind::plain;
}

void SourceCodeView::DrawHilites(Graphics& graphics, int lineIndex, const PointF& origin)
{
}

void SourceCodeView::DrawLine(Graphics& graphics, int lineIndex, const PointF& origin)
{
    int lineNumber = lineIndex + 1;
    std::string lineNumberStr = std::to_string(lineNumber);
    std::string lineNumberText = Format(lineNumberStr, numLineNumberDigits, FormatJustify::right);
    PointF pt(origin);
    SourceCodeTextStyle& lineNumberTextStyle = GetOrInsertTextStyle(SourceCodeTokenKind::lineNumber);
    DrawString(graphics, lineNumberText, *lineNumberTextStyle.font, pt, *lineNumberTextStyle.brush);
    pt.X = pt.X + CharWidth() * (numLineNumberDigits + 1);
    const TokenLine& tokenLine = tokenLines[lineIndex];
    int startState = tokenLine.startState;
    for (const Token& token : tokenLine.tokens)
    {
        SourceCodeTokenKind tokenKind = GetTokenKind(token);
        SourceCodeTextStyle& tokenTextStyle = GetOrInsertTextStyle(tokenKind);
        std::u32string tokenStr = token.match.ToString();
        std::string s(ToUtf8(tokenStr));
        DrawString(graphics, s, *tokenTextStyle.font, pt, *tokenTextStyle.brush);
        pt.X = pt.X + CharWidth() * tokenStr.length();
        startState = -1;
    }
    PointF hiliteOrigin(origin);
    hiliteOrigin.X = hiliteOrigin.X + CharWidth() * LineNumberFieldLength();
    DrawHilites(graphics, lineIndex, hiliteOrigin);
}

bool SourceCodeView::IsBeginBlockLine(int lineIndex) const
{
    if (lineIndex >= 0 && lineIndex < tokenLines.size())
    {
        const TokenLine& tokenLine = tokenLines[lineIndex];
        for (const Token& token : tokenLine.tokens)
        {
            if (GetTokenKind(token) == SourceCodeTokenKind::beginBlock)
            {
                return true;
            }
        }
    }
    return false;
}

bool SourceCodeView::IsEndBlockLine(int lineIndex) const
{
    if (lineIndex >= 0 && lineIndex < tokenLines.size())
    {
        const TokenLine& tokenLine = tokenLines[lineIndex];
        for (const Token& token : tokenLine.tokens)
        {
            if (GetTokenKind(token) == SourceCodeTokenKind::endBlock)
            {
                return true;
            }
        }
    }
    return false;
}

int SourceCodeView::RemoveIndent(int lineIndex) const 
{
    if (IsEndBlockLine(lineIndex))
    {
        return IndentSize();
    }
    else
    {
        return 0;
    }
}

int SourceCodeView::GetIndent(const std::u32string& line, int lineIndex)
{
    for (int i = 0; i < line.length(); ++i)
    {
        if (line[i] != ' ')
        {
            if (IsBeginBlockLine(lineIndex))
            {
                return i + IndentSize();
            }
            else
            {
                return i;
            }
        }
    }
    return 0;
}

void SourceCodeView::OnLinesChanged()
{
    TextView::OnLinesChanged();
    tokenLines.clear();
    int state = 0;
    int n = static_cast<int>(Lines().size());
    numLineNumberDigits = Log10(n + 1);
    for (int i = 0; i < n; ++i)
    {
        const std::u32string& line = *Lines()[i];
        TokenLine tokenLine = TokenizeLine(line, i + 1, state);
        state = tokenLine.endState;
        tokenLines.push_back(std::move(tokenLine));
    }
}

void SourceCodeView::OnLineChanged(LineEventArgs& args)
{
    TextView::OnLineChanged(args);
    int lineIndex = args.lineIndex;
    const std::u32string& line = *Lines()[lineIndex];
    int state = 0;
    if (lineIndex > 0 && !tokenLines.empty())
    {
        state = tokenLines[lineIndex - 1].endState;
    }
    while (lineIndex >= tokenLines.size())
    {
        tokenLines.push_back(TokenLine());
    }
    TokenLine tokenLine = TokenizeLine(line, lineIndex + 1, state);
    tokenLines[lineIndex] = std::move(tokenLine);
}

void SourceCodeView::OnLineDeleted(LineEventArgs& args)
{
    TextView::OnLineDeleted(args);
    int lineIndex = args.lineIndex;
    tokenLines.erase(tokenLines.begin() + lineIndex);
}

void SourceCodeView::OnLineInserted(LineEventArgs& args)
{
    TextView::OnLineInserted(args);
    int lineIndex = args.lineIndex;
    int state = 0;
    while (lineIndex >= tokenLines.size())
    {
        tokenLines.push_back(TokenLine());
    }
    std::u32string& line = *Lines()[lineIndex];
    if (lineIndex > 0)
    {
        state = tokenLines[lineIndex - 1].endState;
    }
    TokenLine tokenLine = TokenizeLine(line, lineIndex + 1, state);
    tokenLines.insert(tokenLines.begin() + lineIndex, std::move(tokenLine));
    Invalidate();
}

TokenStyle SourceCodeView::GetTokenStyle(SourceCodeTokenKind tokenKind) const
{
    auto it = tokenStyleMap.find(tokenKind);
    if (it != tokenStyleMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("SourceCodeView: token style not found");
    }
}

Font* SourceCodeView::GetOrInsertFont(const std::string& fontFamilyName, float fontSize, FontStyle fontStyle)
{
    SourceCodeFont font(fontFamilyName, fontSize, fontStyle);
    auto it = fontMap.find(font);
    if (it != fontMap.cend())
    {
        return it->second;
    }
    std::u16string familyName = ToUtf16(fontFamilyName);
    std::unique_ptr<Font> fontPtr(new Font(FontFamily((const WCHAR*)familyName.c_str()), fontSize, fontStyle, Unit::UnitPoint));
    Font* fnt = fontPtr.get();
    fontMap[font] = fnt;
    fonts.push_back(std::move(fontPtr));
    return fnt;
}

SourceCodeTextStyle& SourceCodeView::GetOrInsertTextStyle(SourceCodeTokenKind tokenKind)
{
    auto it = textStyleMap.find(tokenKind);
    if (it != textStyleMap.cend())
    {
        return it->second;
    }
    TokenStyle tokenStyle = GetTokenStyle(tokenKind);
    SourceCodeTextStyle& textStyle = textStyleMap[tokenKind];
    textStyle.font = GetOrInsertFont(FontFamilyName(), FontSize(), tokenStyle.fontStyle);
    textStyle.brush = GetOrInsertBrush(tokenStyle.color);
    return textStyle;
}

std::u32string SourceCodeView::GetText(const SourceSpan& sourceSpan) const
{
    if (sourceSpan.line >= 1 && sourceSpan.line <= Lines().size())
    {
        const std::u32string& line = *Lines()[sourceSpan.line - 1];
        if (sourceSpan.scol >= 1 && sourceSpan.scol <= line.length())
        {
            int n = sourceSpan.ecol - sourceSpan.scol;
            if (n > 0)
            {
                return line.substr(sourceSpan.scol - 1, n);
            }
        }
    }
    return std::u32string();
}

} } // cmajor::wing
