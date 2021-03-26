// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/TextView.hpp>
#include <cmajor/wing/Application.hpp>
#include <cmajor/wing/EditCommand.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <fstream>
#include <algorithm>
#include <limits>

#undef min
#undef max

namespace cmajor { namespace wing {

using namespace soulng::unicode;

const std::u32string ccStopChars = U" ,()[]{};\"'<=|&^!+/%~:";

bool IsCCStopChar(char32_t c)
{
    return ccStopChars.find(c) != std::u32string::npos;
}

const std::u32string wordSeparators = U"`~!@#$%^&*()-=+[{]}\\|;:'\",.<>/?";

bool IsWordSeparator(char32_t c)
{
    return wordSeparators.find(c) != std::u32string::npos;
}

bool IsEmptyOrSpaceLine(const std::u32string& line)
{
    if (line.empty()) return true;
    for (char32_t c : line)
    {
        if (c != ' ') return false;
    }
    return true;
}

int ReadCaretTimeoutFromRegistry()
{
    int caretTimeout = -1;
    HKEY currentUserKey = nullptr;
    LSTATUS status = RegOpenCurrentUser(KEY_READ, &currentUserKey);
    if (status == ERROR_SUCCESS)
    {
        std::u16string subkeyStr = ToUtf16("Control Panel\\Desktop");
        std::u16string valueName = ToUtf16("CaretTimeout");
        uint32_t flags = RRF_RT_REG_DWORD;
        uint32_t type = 0u;
        uint32_t data = 0u;
        uint32_t dataSize = 4u;
        LSTATUS status = RegGetValueW(currentUserKey, (LPCWSTR)subkeyStr.c_str(), (LPCWSTR)valueName.c_str(), flags, (LPDWORD)&type, &data, (LPDWORD)&dataSize);
        if (status == ERROR_SUCCESS)
        {
            caretTimeout = data;
        }
        RegCloseKey(currentUserKey);
    }
    return caretTimeout;
}

Color DefaultSelectionBackgroundColor()
{
    return Color(153, 201, 239);
}

SourcePos::SourcePos() : line(0), column(0)
{
}

SourcePos::SourcePos(int line_, int column_) : line(line_), column(column_)
{
}

Selection::Selection() : fixed(SelectionFixed::none), start(), end()
{
}

SelectionData::SelectionData() : selectedText(), indent(0), numTrailingSpaces(0)
{
}

SelectionData::SelectionData(const std::u32string& selectedText_, int indent_, int numTrailingSpaces_) : selectedText(selectedText_), indent(indent_), numTrailingSpaces(numTrailingSpaces_)
{
}

LineEventArgs::LineEventArgs(int lineIndex_, int indentLineIndex_) : lineIndex(lineIndex_), indentLineIndex(indentLineIndex_)
{
}

TextViewCreateParams::TextViewCreateParams()
{
    controlCreateParams.WindowClassName("wing.TextView");
    controlCreateParams.WindowClassStyle(DoubleClickWindowClassStyle());
    controlCreateParams.WindowStyle(DefaultChildWindowStyle() | WS_TABSTOP);
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.BackgroundColor(Color::White);
    textColor = Color::Black;
    selectionBackgroundColor = DefaultSelectionBackgroundColor();
    fontFamilyName = "Consolas";
    fontSize = 10.0f;
}

TextViewCreateParams& TextViewCreateParams::Defaults()
{
    return *this;
}

TextViewCreateParams& TextViewCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return* this;
}

TextViewCreateParams& TextViewCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::SetSize(const Size& size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

TextViewCreateParams& TextViewCreateParams::SelectionBackgroundColor(const Color& selectionBackgroundColor_)
{
    selectionBackgroundColor = selectionBackgroundColor_;
    return *this;
}

TextViewCreateParams& TextViewCreateParams::TextColor(const Color& textColor_)
{
    textColor = textColor_;
    return *this;
}

TextViewCreateParams& TextViewCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

TextViewCreateParams& TextViewCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return* this;
}

TextView::TextView(TextViewCreateParams& createParams) : 
    Control(createParams.controlCreateParams), 
    flags(TextViewFlags::none), textColor(createParams.textColor), selectionBackgroundColor(createParams.selectionBackgroundColor), 
    fontFamilyName(createParams.fontFamilyName), fontSize(createParams.fontSize), cursor(LoadStandardCursor(StandardCursorId::ibeam)),
    caretTimerPeriod(defaultCaretTimerPeriod), caretColumn(1), caretLine(1), charWidth(0), charHeight(0), textWidth(0), textHeight(0), topLine(1), topLineDiff(0.0f), leftCol(1), leftColDiff(0.0f),
    maxLineLength(0), maxLineIndex(0), indentSize(1), selection(), padding(), lines(), editCommandList(this), update(false), stringFormat()
{
    int timeout = ReadCaretTimeoutFromRegistry();
    if (timeout != -1)
    {
        caretTimerPeriod = timeout;
    }
    std::u16string familyName = ToUtf16(fontFamilyName);
    SetFont(Font(FontFamily((const WCHAR*)familyName.c_str()), fontSize, FontStyle::FontStyleRegular, Unit::UnitPoint));
    SetChanged();
    measureString = "// this file has been semiautomatically generated from 'D:/work/soulng-project/sngcm/ast/AstReader.hpp' using cpp2cm version 1.0.0";
    stringFormat.SetAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
    stringFormat.SetLineAlignment(Gdiplus::StringAlignment::StringAlignmentNear);
}

void TextView::SetReadOnly() 
{ 
    if (!IsReadOnly())
    {
        flags = flags | TextViewFlags::readOnly;
        OnReadOnlyChanged();
    }
}

void TextView::SetReadWrite() 
{ 
    if (IsReadOnly())
    {
        flags = flags & ~TextViewFlags::readOnly;
        OnReadOnlyChanged();
    }
}


void TextView::SetDirty()
{
    if (!IsDirty())
    {
        flags = flags | TextViewFlags::dirty;
        OnDirtyChanged();
    }
}

void TextView::ResetDirty()
{
    if (IsDirty())
    {
        flags = flags & ~TextViewFlags::dirty;
        OnDirtyChanged();
    }
}

void TextView::SetCCDirty()
{
    if (!IsCCDirty())
    {
        flags = flags | TextViewFlags::ccdirty;
        OnCCDirtyChanged();
    }
}

void TextView::ResetCCDirty()
{
    if (IsCCDirty())
    {
        flags = flags & ~TextViewFlags::ccdirty;
        OnCCDirtyChanged();
    }
}

void TextView::AddLine(const std::u32string& line)
{
    lines.push_back(std::unique_ptr<std::u32string>(new std::u32string(line)));
    SetMaxLineLength();
    SetTextExtent();
}

void TextView::SetUndoRedoMenuItems(MenuItem* undoMenuItem, MenuItem* redoMenuItem)
{
    editCommandList.SetMenuItems(undoMenuItem, redoMenuItem);
}

void TextView::Select()
{
    editCommandList.UpdateMenuItems();
}

void TextView::Undo()
{
    editCommandList.Undo();
}

void TextView::Redo()
{
    editCommandList.Redo();
}

void TextView::SetMaxLineLength()
{
    maxLineLength = 0;
    maxLineIndex = 0;
    int n = lines.size();
    for (int i = 0; i < n; ++i)
    {
        const std::u32string& line = *lines[i];
        if (line.length() > maxLineLength)
        {
            maxLineLength = line.length();
            maxLineIndex = i;
        }
    }
}

void TextView::SetPadding(const Padding& padding_)
{
    if (padding != padding_)
    {
        padding = padding_;
        Invalidate();
    }
}

void TextView::SetCaretLineCol(int line, int column)
{
    if (caretLine != line || caretColumn != column)
    {
        caretLine = line;
        caretColumn = column;
        SetCaretLocation();
    }
}

void TextView::SetTopLineCol(int line, int column)
{
    Point newOrigin(static_cast<int>((column - 1) * charWidth), static_cast<int>((line - 1) * charHeight));
    topLine = static_cast<int>(newOrigin.Y / charHeight + 1.5f);
    topLineDiff = (topLine - 1) * charHeight - newOrigin.Y;
    leftCol = static_cast<int>(newOrigin.X / charWidth + 1.5f);
    leftColDiff = (leftCol - 1) * charWidth - newOrigin.X;
    SetContentLocation(newOrigin);
    Invalidate();
}

int TextView::GetVisibleLineCount() const
{
    Size size = GetSize();
    return static_cast<int>(size.Height / charHeight);
}

int TextView::GetVisibleColumnCount() const
{
    Size size = GetSize();
    return static_cast<int>(size.Width / charWidth);
}

bool TextView::IsLinePartiallyVisible(int line) const
{
    return line >= (topLine - 1) && line < (topLine + GetVisibleLineCount() + 1);
}

bool TextView::IsLineVisible(int line) const
{
    return line >= topLine && line < topLine + GetVisibleLineCount();
}

void TextView::GetLineColumn(const Point& contentLocation, int& line, int& column) const
{
    line = static_cast<int>(contentLocation.Y / charHeight + 1);
    column = static_cast<int>(contentLocation.X / charWidth + 1) - LineNumberFieldLength();
}

void TextView::EnsureLineVisible(int line)
{
    Invalidate();
    Application::ProcessMessages(); 
    if (IsLineVisible(line))
    {
        return;
    }
    int tl = std::max(1, line - GetVisibleLineCount() / 2);
    SetTopLineCol(tl, 1);
}

void TextView::Clear()
{
    std::u32string emptyContent;
    SetTextContent(emptyContent);
}

void TextView::SetTextContent(const std::u32string& textContent)
{
    maxLineLength = 0;
    maxLineIndex = 0;
    lines.clear();
    std::vector<std::u32string> splitLines = SplitTextIntoLines(textContent);
    for (std::u32string& line : splitLines)
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string(std::move(line))));
    }
    lineStartIndeces = CalculateLineStartIndeces(textContent);
    SetMaxLineLength();
    OnLinesChanged();
    SetContentChanged();
    SetChanged();
    SetCaretLineCol(1, 1 + LineNumberFieldLength());
    OnContentLocationChanged();
    Invalidate();
    update = true;
}

char32_t TextView::GetCharAt(int line, int col) const
{
    if (line < 1)
    {
        line = 1;
    }
    else if (line > lines.size())
    {
        line = static_cast<int>(lines.size());
    }
    int lineLength = GetLineLength(line);
    if (col < 1)
    {
        col = 1;
    }
    else
    {
        if (lineLength > 0 && col > lineLength)
        {
            col = lineLength;
        }
    }
    if (line >= 1 && line <= lines.size() && col >= 1 && col <= lineLength)
    {
        return (*lines[line - 1])[col - 1];
    }
    return '\0';
}

bool TextView::Prev(int& line, int& col)
{
    if (col > 1)
    {
        --col;
        return true;
    }
    else
    {
        if (line > 1)
        {
            --line;
            int lineLength = GetLineLength(line);
            if (lineLength > 0)
            {
                col = lineLength;
            }
            else
            {
                col = 1;
            }
            return true;
        }
    }
    return false;
}

bool TextView::Next(int& line, int& col)
{
    int lineLength = GetLineLength(line);
    if (col < lineLength)
    {
        ++col;
        return true;
    }
    else
    {
        if (line < lines.size())
        {
            ++line;
            col = 1;
            return true;
        }
    }
    return false;
}

void TextView::PrevWord(int& line, int& col)
{
    if (Prev(line, col))
    {
        char32_t c = GetCharAt(line, col);
        while (IsWhiteSpace(c) || IsWordSeparator(c))
        {
            if (col == 1 || !Prev(line, col))
            {
                return;
            }
            c = GetCharAt(line, col);
        }
        c = GetCharAt(line, col);
        while (!IsWhiteSpace(c) && !IsWordSeparator(c))
        {
            if (col == 1 || !Prev(line, col))
            {
                return;
            }
            c = GetCharAt(line, col);
        }
        c = GetCharAt(line, col);
        if (IsWhiteSpace(c) || IsWordSeparator(c))
        {
            Next(line, col);
        }
    }
}

void TextView::NextWord(int& line, int& col)
{
    if (Next(line, col))
    {
        char32_t c = GetCharAt(line, col);
        while (!IsWhiteSpace(c) && !IsWordSeparator(c))
        {
            if (col == 1 || !Next(line, col))
            {
                return;
            }
            c = GetCharAt(line, col);
        }
        c = GetCharAt(line, col);
        while (IsWhiteSpace(c))
        {
            if (col == 1 || !Next(line, col))
            {
                return;
            }
            c = GetCharAt(line, col);
        }
    }
}

void TextView::SetCaretPosByCharIndex(int charIndex)
{
    std::vector<int>::const_iterator it = std::lower_bound(lineStartIndeces.cbegin(), lineStartIndeces.cend(), charIndex);
    if (it != lineStartIndeces.cbegin() && it == lineStartIndeces.cend())
    {
        --it;
    }
    if (it >= lineStartIndeces.cbegin() && it != lineStartIndeces.cend())
    {
        if (*it > charIndex)
        {
            --it;
        }
    }
    int lineNumber = static_cast<int>(it - lineStartIndeces.cbegin() + 1);
    int columnNumber = 1 + LineNumberFieldLength();
    if (it >= lineStartIndeces.cbegin() && it != lineStartIndeces.cend())
    {
        int lineStartCharIndex = *it;
        columnNumber = charIndex - lineStartCharIndex + 1 + LineNumberFieldLength();
    }
    SetCaretLineCol(lineNumber, columnNumber);
    ScrollToCaret();
}

void TextView::ScrollToCaret()
{
    if (lines.empty() || charHeight == 0) return;
    Size size = GetSize();
    int windowLines = static_cast<int>(size.Height / charHeight);
    int windowCols = static_cast<int>(size.Width / charWidth);
    Point currentOrigin = ContentLocation();
    int topLineNumber = static_cast<int>(currentOrigin.Y / charHeight + 1);
    int leftColNumber = static_cast<int>(currentOrigin.X / charWidth + 1);
    int oldTopLineNumber = topLineNumber;
    if (caretLine > topLineNumber)
    {
        while (caretLine - topLineNumber + 1 >= windowLines)
        {
            ++topLineNumber;
        }
    }
    else if (caretLine < topLineNumber)
    {
        topLineNumber = caretLine;
    }
    else
    {
        topLineNumber = std::max(1, caretLine - 1);
    }
    int oldLeftColNumber = leftColNumber;
    if (caretColumn >= leftColNumber)
    {
        while (caretColumn - leftColNumber + 2 >= windowCols)
        {
            ++leftColNumber;
        }
    }
    else
    {
        if (caretColumn == 1 + LineNumberFieldLength())
        {
            leftColNumber = 1;
        }
        else
        {
            leftColNumber = caretColumn;
        }
    }
    if (update || caretLine == 1 || topLineNumber != oldTopLineNumber || leftColNumber != oldLeftColNumber)
    {
        update = false;
        Point newOrigin(static_cast<int>((leftColNumber - 1) * charWidth), static_cast<int>((topLineNumber - 1) * charHeight));
        topLine = topLineNumber;
        topLineDiff = 0.0f;
        leftCol = leftColNumber;
        leftColDiff = 0.0f;
        SetContentLocation(newOrigin);
        SetCaretLocation();
        Invalidate();
    }
}

void TextView::SaveText()
{
    std::ofstream file(filePath);
    for (const std::unique_ptr<std::u32string>& line : lines)
    {
        file << ToUtf8(*line) << "\n";
    }
    ResetDirty();
}

void TextView::SetTextExtent()
{
    textWidth = static_cast<int>(maxLineLength * charWidth);
    textHeight = static_cast<int>(lines.size() * charHeight);
    SetContentSize(Size(textWidth, textHeight));
}

Point TextView::CaretPos()
{
    int x = static_cast<int>(padding.left + (caretColumn - 1) * charWidth);
    int y = static_cast<int>(padding.top + (caretLine - 1) * charHeight);
    Point loc(x, y);
    TranslateContentLocation(loc);
    return loc;
}

Point TextView::CCPos()
{
    Point ccPos = CaretPos();
    ccPos.Y = ccPos.Y + static_cast<int>(charHeight + 0.5);
    return ccPos;
}

void TextView::SetCCText(const std::u32string& line, int columnIndex)
{
    cctext.clear();
    if (line.empty()) return;
    while (columnIndex >= line.length())
    {
        --columnIndex;
    }
    while (columnIndex >= 0)
    {
        if (IsCCStopChar(line[columnIndex]))
        {
            break;
        }
        cctext.append(1, line[columnIndex]);
        --columnIndex;
    }
    std::reverse(cctext.begin(), cctext.end());
    OnCCTextChanged();
}

void TextView::ResetCCText()
{
    cctext.clear();
    OnCCTextChanged();
}

void TextView::ReplaceCCText(const std::u32string& replacement)
{
    int caretLineIndex = std::max(0, caretLine - 1);
    int caretColumnIndex = std::max(0, caretColumn - 1);
    if (caretLineIndex < lines.size())
    {
        std::u32string& line = *lines[caretLineIndex];
        while (caretColumnIndex > line.length())
        {
            --caretColumnIndex;
        }
        int endColumnIndex = std::max(0, caretColumnIndex);
        while (caretColumnIndex > 0)
        {
            if (caretColumnIndex < line.length())
            {
                if (IsCCStopChar(line[caretColumnIndex]))
                {
                    ++caretColumnIndex;
                    break;
                }
            }
            --caretColumnIndex;
        }
        int startColumndIndex = std::max(0, caretColumnIndex);
        line = line.substr(0, startColumndIndex) + replacement + line.substr(endColumnIndex);
        int newEndColumnIndex = startColumndIndex + static_cast<int>(replacement.length());
        LineEventArgs args(caretLineIndex, -1);
        OnLineChanged(args);
        SetCaretLineCol(caretLineIndex + 1, newEndColumnIndex + 1 + LineNumberFieldLength());
        ScrollToCaret();
        Invalidate();
        SetDirty();
        SetCCDirty();
    }
}

std::u32string TextView::GetCursorText() const
{
    std::u32string cursorText;
    int caretIndex = std::max(0, caretColumn - 1);
    long n = lines.size();
    for (long i = 0; i < n; ++i)
    {
        const std::u32string& line = *lines[i];
        int lineNumber = static_cast<int>(i + 1);
        if (lineNumber == caretLine)
        {
            int startIndex = caretIndex;
            while (startIndex > 0)
            {
                if (startIndex < line.length())
                {
                    bool exit = false;
                    switch (line[startIndex])
                    {
                        case ';':
                        case '{':
                        case '}':
                        {
                            ++startIndex;
                            exit = true;
                            break;
                        }
                    }
                    if (exit)
                    {
                        break;
                    }
                }
                --startIndex;
            }
            cursorText.append(line.substr(0, startIndex)).append(1, '$').append(line.substr(caretIndex)).append(1, '\n');
        }
        else
        {
            cursorText.append(line).append(1, '\n');
        }
    }
    return cursorText;
}

void TextView::InsertChar(int lineIndex, int columnIndex, char32_t c)
{
    while (lineIndex >= lines.size())
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
    }
    std::u32string& line = *lines[lineIndex];
    line.insert(columnIndex, 1, c);
    if (c == '}' && StartsWith(line, std::u32string(IndentSize(), ' ')))
    {
        line.erase(0, IndentSize());
        columnIndex = columnIndex - IndentSize();
    }
    if (line.length() > maxLineLength)
    {
        maxLineLength = line.length();
        maxLineIndex = lineIndex;
        SetTextExtent();
    }
    LineEventArgs args(lineIndex, -1);
    OnLineChanged(args);
    SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength());
    KeyEventArgs rightArgs(Keys::right);
    OnKeyDown(rightArgs);
    SetCCText(line, columnIndex);
    SetDirty();
    SetCCDirty();
}

void TextView::InsertText(int lineIndex, int columnIndex, const std::u32string& text)
{
    if (text.empty()) return;
    editCommandList.BeginGroup();
    if (!selection.IsEmpty())
    {
        lineIndex = selection.start.line - 1;
        columnIndex = selection.start.column - 1;
        AddRemoveSelectionCommand();
        RemoveSelection();
    }
    std::vector<std::u32string> linesToInsert = SplitTextIntoLines(text);
    AddInsertLinesCommand(lineIndex, columnIndex, linesToInsert);
    InsertLines(lineIndex, columnIndex, linesToInsert);
    editCommandList.EndGroup();
}

void TextView::InsertLines(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToInsert)
{
    if (linesToInsert.size() == 1 || linesToInsert.size() == 2 && IsEmptyOrSpaceLine(linesToInsert[1]))
    {
        while (lineIndex >= lines.size())
        {
            lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
        }
        std::u32string& line = *lines[lineIndex];
        line.insert(columnIndex, linesToInsert.front());
        LineEventArgs args(lineIndex, -1);
        OnLineChanged(args);
        SetCaretLineCol(lineIndex + 1, static_cast<int>(columnIndex + linesToInsert.front().length() + 1 + LineNumberFieldLength()));
        ScrollToCaret();
        InvalidateLineCol(lineIndex + 1, 1 + LineNumberFieldLength());
        SetDirty();
        SetCCDirty();
    }
    else
    {
        int indent = 0;
        if (lineIndex > 0 && lineIndex < lines.size())
        {
            indent = GetIndent(*lines[lineIndex - 1], lineIndex - 1);
        }
        int indentLineIndex = lineIndex - 1;
        for (const std::u32string& lineToInsert : linesToInsert)
        {
            lines.insert(lines.begin() + lineIndex, std::unique_ptr<std::u32string>(new std::u32string(lineToInsert)));
            SetLineNumberFieldLength(static_cast<int>(lines.size()));
            LineEventArgs args(lineIndex, indentLineIndex);
            OnLineInserted(args);
            ++lineIndex;
        }
        SetTextExtent();
        SetCaretLineCol(lineIndex, 1 + LineNumberFieldLength() + indent);
        ScrollToCaret();
        Invalidate();
        InvalidateLines(lineIndex + 1, static_cast<int>(lineIndex + linesToInsert.size() + 1));
        SetDirty();
        SetCCDirty();
    }
}

void TextView::NewLine(int lineIndex, int columnIndex)
{
    while (lineIndex >= lines.size())
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
    }
    std::u32string& line = *lines[lineIndex];
    std::u32string toInsert = TrimEnd(line.substr(columnIndex));
    bool lineChanged = false;
    if (!toInsert.empty())
    {
        line.erase(columnIndex, line.length() - columnIndex);
        lineChanged = true;
    }
    std::u32string trimmedLine = TrimEnd(line);
    if (trimmedLine.length() != line.length())
    {
        std::swap(line, trimmedLine);
        lineChanged = true;
    }
    if (lineChanged)
    {
        LineEventArgs changedArgs(lineIndex, -1);
        OnLineChanged(changedArgs);
    }
    lines.insert(lines.begin() + lineIndex + 1, std::unique_ptr<std::u32string>(new std::u32string(toInsert)));
    SetLineNumberFieldLength(static_cast<int>(lines.size()));
    int indentLineIndex = lineIndex;
    while (indentLineIndex >= 0 && IsEmptyOrSpaceLine(*lines[indentLineIndex]))
    {
        --indentLineIndex;
    }
    if (indentLineIndex < 0)
    {
        indentLineIndex = 0;
    }
    LineEventArgs insertedArgs(lineIndex + 1, indentLineIndex);
    OnLineInserted(insertedArgs);
    SetTextExtent();
    SetCaretLineCol(lineIndex + 1 + 1, 1 + LineNumberFieldLength() + GetIndent(*lines[indentLineIndex], indentLineIndex));
    ScrollToCaret();
    Invalidate();
    SetDirty();
    SetCCDirty();
}

void TextView::Tab(int lineIndex, int columnIndex)
{
    while (lineIndex >= lines.size())
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
    }
    std::u32string& line = *lines[lineIndex];
    line.insert(columnIndex, std::u32string(IndentSize(), ' '));
    if (line.length() > maxLineLength)
    {
        maxLineLength = static_cast<int>(line.length());
        maxLineIndex = lineIndex;
        SetTextExtent();
    }
    LineEventArgs args(lineIndex, -1);
    OnLineChanged(args);
    SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength() + IndentSize());
    ScrollToCaret();
    InvalidateLineCol(lineIndex + 1, columnIndex + 1);
    SetDirty();
    SetCCDirty();
}

void TextView::Backtab(int lineIndex, int columnIndex)
{
    if (lineIndex >= lines.size()) return;
    std::u32string& line = *lines[lineIndex];
    int targetCol = std::max(1, columnIndex - IndentSize() + 1);
    int numSpaces = 0;
    int col = 0;
    for (int i = columnIndex; i >= targetCol; --i)
    {
        if (line[i - 1] == ' ')
        {
            col = i - 1;
            ++numSpaces;
        }
        else
        {
            break;
        }
    }
    if (numSpaces > 0)
    {
        line.erase(col, numSpaces);
        LineEventArgs args(lineIndex, -1);
        OnLineChanged(args);
        SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength() - numSpaces);
        ScrollToCaret();
        InvalidateLineCol(lineIndex + 1, 1 + col + LineNumberFieldLength());
        SetDirty();
        SetCCDirty();
    }
}

void TextView::AddSpaces(int lineIndex, int columnIndex, int numSpaces)
{
    std::u32string& line = *lines[lineIndex];
    line.insert(columnIndex - numSpaces, std::u32string(numSpaces, ' '));
    LineEventArgs args(lineIndex, -1);
    OnLineChanged(args);
    SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength() - numSpaces);
    ScrollToCaret();
    InvalidateLineCol(lineIndex + 1, 1 + LineNumberFieldLength());
    SetDirty();
    SetCCDirty();
}

void TextView::RemoveSpaces(int lineIndex, int columnIndex, int numSpaces)
{
    std::u32string& line = *lines[lineIndex];
    line.erase(columnIndex - numSpaces, numSpaces);
    LineEventArgs args(lineIndex, -1);
    OnLineChanged(args);
    SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength() - numSpaces);
    ScrollToCaret();
    InvalidateLineCol(lineIndex + 1, 1 + LineNumberFieldLength());
    SetDirty();
    SetCCDirty();
}

void TextView::DeleteChar(int lineIndex, int columnIndex, int indent, int numSpaces, bool removeIndent)
{
    if (lineIndex >= lines.size()) return;
    std::u32string& line = *lines[lineIndex];
    if (removeIndent)
    {
        line.insert(0, std::u32string(IndentSize(), ' '));
    }
    if (columnIndex < line.length())
    {
        line.erase(columnIndex, 1);
        LineEventArgs args(lineIndex, -1);
        OnLineChanged(args);
        SetCCText(line, columnIndex);
    }
    else
    {
        if (lines.size() > lineIndex + 1)
        {
            if (numSpaces > 0)
            {
                line.append(std::u32string(numSpaces, ' '));
            }
            std::u32string nextLine = *lines[lineIndex + 1];
            line.append(nextLine.substr(indent));
            LineEventArgs changedArgs(lineIndex, -1);
            OnLineChanged(changedArgs);
            lines.erase(lines.begin() + lineIndex + 1);
            LineEventArgs deletedArgs(lineIndex + 1, -1);
            OnLineDeleted(deletedArgs);
            SetTextExtent();
            ResetCCText();
        }
    }
    SetCaretLineCol(lineIndex + 1, columnIndex + 1 + LineNumberFieldLength());
    SetDirty();
    SetCCDirty();
}

void TextView::Backspace(int lineIndex, int columnIndex)
{
    if (lineIndex != 0 || columnIndex != 0)
    {
        KeyEventArgs leftArgs(Keys::left);
        OnKeyDown(leftArgs);
        KeyEventArgs deleteArgs(Keys::delete_);
        OnKeyDown(deleteArgs);
    }
}

void TextView::DeleteLines(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToDelete)
{
    if (linesToDelete.size() == 1)
    {
        std::u32string& line = *lines[lineIndex];
        line.erase(columnIndex, linesToDelete.front().length());
        LineEventArgs args(lineIndex, -1);
        OnLineChanged(args);
    }
    else
    {
        for (int i = 0; i < linesToDelete.size(); ++i)
        {
            lines.erase(lines.begin() + lineIndex);
            LineEventArgs args(lineIndex, -1);
            OnLineDeleted(args);
        }
    }
    SetTextExtent();
    SetLineNumberFieldLength(static_cast<int>(lines.size()));
    SetCaretLineCol(lineIndex, 1 + LineNumberFieldLength());
    ScrollToCaret();
    Invalidate();
    SetDirty();
    SetCCDirty();
}

void TextView::SetSelection(const Selection& selection_)
{
    selection = selection_;
}

void TextView::ResetSelection()
{
    if (!selection.IsEmpty())
    {
        selection = Selection();
        OnSelectionChanged();
        Invalidate();
    }
}

bool TextView::IsSelectionEmpty() const
{
    return selection.IsEmpty();
}

SelectionData TextView::GetSelection() const
{
    if (selection.IsEmpty()) return SelectionData();
    if (selection.start.line == selection.end.line)
    {
        std::u32string s = (*lines[selection.start.line - 1]).substr(selection.start.column - 1, selection.end.column - selection.start.column);
        int indent = MinIndent(s);
        SelectionData selectionData(Unindent(s, indent), indent, 0);
        return selectionData;
    }
    else
    {
        int numTrailingSpaces = 0;
        std::u32string s = (*lines[selection.start.line - 1]).substr(selection.start.column - 1).append(1, '\n');
        for (int i = selection.start.line; i < selection.end.line - 1; ++i)
        {
            s.append(*lines[i]).append(1, '\n');
        }
        if (selection.end.line - 1 < lines.size())
        {
            if (!(*lines[selection.end.line - 1]).substr(0, selection.end.column - 1).empty())
            {
                std::u32string lastLine = (*lines[selection.end.line - 1]).substr(0, selection.end.column - 1);
                numTrailingSpaces = GetNumTrailingSpaces(lastLine);
                s.append(lastLine).append(1, '\n');
            }
        }
        else
        {
            int i = selection.end.line - 1;
            while (i >= lines.size())
            {
                s.append(1, '\n');
                --i;
            }
        }
        int indent = MinIndent(s);
        SelectionData selectionData(Unindent(s, indent), indent, numTrailingSpaces);
        return selectionData;
    }
}

void TextView::InsertSelection(const Selection& selectionToInsert, const SelectionData& selectionData, bool wholeLine)
{
    if (selectionToInsert.IsEmpty()) return;
    if (selectionToInsert.start.line == selectionToInsert.end.line)
    {
        if (wholeLine)
        {
            int lineIndex = selectionToInsert.start.line - 1;
            std::u32string selectedText = selectionData.selectedText;
            if (selectionData.indent > 0)
            {
                selectedText.insert(0, std::u32string(selectionData.indent, ' '));
            }
            lines.insert(lines.begin() + lineIndex, std::unique_ptr<std::u32string>(new std::u32string(selectedText)));
            LineEventArgs args(lineIndex, -1);
            OnLineInserted(args);
            SetCaretLineCol(lineIndex + 1, 1 + LineNumberFieldLength());
        }
        else
        {
            int lineIndex = selectionToInsert.start.line - 1;
            std::u32string selectedText = selectionData.selectedText;
            if (selectionData.indent > 0)
            {
                selectedText.insert(0, std::u32string(selectionData.indent, ' '));
            }
            (*lines[lineIndex]).insert(selectionToInsert.start.column - 1, selectedText);
            LineEventArgs args(lineIndex, -1);
            OnLineChanged(args);
            SetCaretLineCol(lineIndex + 1, selectionToInsert.start.column + LineNumberFieldLength());
        }
    }
    else
    {
        std::vector<std::u32string> selectionLines = SplitTextIntoLines(selectionData.selectedText);
        int startLineIndex = selectionToInsert.start.line - 1;
        int endLineIndex = selectionToInsert.end.line - 1;
        int lineCount = endLineIndex - startLineIndex + 1;
        if (selectionToInsert.start.column > 1)
        {
            std::u32string firstSelectionLine = selectionLines[0];
            if (selectionData.indent > 0)
            {
                firstSelectionLine.insert(0, selectionData.indent, ' ');
            }
            (*lines[startLineIndex]).insert(selectionToInsert.start.column - 1, firstSelectionLine);
            LineEventArgs changeArgs(startLineIndex, -1);
            OnLineChanged(changeArgs);
            ++startLineIndex;
        }
        else
        {
            std::u32string firstSelectionLine = selectionLines[0];
            if (selectionData.indent > 0)
            {
                firstSelectionLine.insert(0, selectionData.indent, ' ');
            }
            lines.insert(lines.begin() + startLineIndex, std::unique_ptr<std::u32string>(new std::u32string(firstSelectionLine)));
            LineEventArgs args(startLineIndex, -1);
            OnLineInserted(args);
            ++startLineIndex;
        }
        int n = endLineIndex;
        for (int i = startLineIndex; i < n; ++i)
        {
            std::u32string selectionLine = selectionLines[i - startLineIndex + 1];
            if (selectionData.indent > 0)
            {
                selectionLine.insert(0, selectionData.indent, ' ');
            }
            lines.insert(lines.begin() + i, std::unique_ptr<std::u32string>(new std::u32string(std::move(selectionLine))));
            LineEventArgs args(i, -1);
            OnLineInserted(args);
        }
        std::u32string lastLine;
        if (selectionLines.size() >= lineCount)
        {
            lastLine = selectionLines.back();
            if (selectionData.indent > 0)
            {
                lastLine.insert(0, selectionData.indent, ' ');
            }
            lastLine.append(selectionData.numTrailingSpaces, ' ');
        }
        if (wholeLine)
        {
            lines.insert(lines.begin() + endLineIndex, std::unique_ptr<std::u32string>(new std::u32string(std::move(lastLine))));
            LineEventArgs args(endLineIndex, -1);
            OnLineInserted(args);
        }
        else
        {
            (*lines[endLineIndex]).insert(0, lastLine);
            LineEventArgs args(endLineIndex, -1);
            OnLineChanged(args);
        }
    }
    selection = selectionToInsert;
    SetCaretLineCol(selection.end.line, selection.end.column + LineNumberFieldLength());
    Invalidate();
    SetDirty();
    SetCCDirty();
}

void TextView::RemoveSelection()
{
    if (selection.IsEmpty()) return;
    if (selection.start.line == selection.end.line)
    {
        if (selection.end.column - selection.start.column >= GetLineLength(selection.start.line))
        {
            int lineIndex = selection.start.line - 1;
            lines.erase(lines.begin() + lineIndex);
            LineEventArgs args(lineIndex, -1);
            OnLineDeleted(args);
            SetCaretLineCol(selection.start.line, 1 + LineNumberFieldLength());
        }
        else
        {
            int lineIndex = selection.start.line - 1;
            (*lines[lineIndex]).erase(selection.start.column - 1, selection.end.column - selection.start.column);
            LineEventArgs args(lineIndex, -1);
            OnLineChanged(args);
            SetCaretLineCol(lineIndex + 1, selection.start.column + LineNumberFieldLength());
        }
    }
    else
    {
        int startLineIndex = selection.start.line - 1;
        int endLineIndex = selection.end.line - 1;
        if (selection.start.column > 1)
        {
            (*lines[startLineIndex]).erase(selection.start.column - 1, GetLineLength(startLineIndex + 1) - selection.start.column + 1);
            LineEventArgs args(startLineIndex, -1);
            OnLineChanged(args);
            ++startLineIndex;
        }
        else
        {
            lines.erase(lines.begin() + startLineIndex);
            --endLineIndex;
            LineEventArgs args(startLineIndex, -1);
            OnLineDeleted(args);
        }
        int n = endLineIndex;
        for (int i = startLineIndex; i < n; ++i)
        {
            lines.erase(lines.begin() + startLineIndex);
            --endLineIndex;
            LineEventArgs args(startLineIndex, -1);
            OnLineDeleted(args);
        }
        if (endLineIndex >= 0 && endLineIndex < lines.size())
        {
            if (selection.end.column > GetLineLength(endLineIndex + 1))
            {
                lines.erase(lines.begin() + endLineIndex);
                LineEventArgs args(endLineIndex, -1);
                OnLineDeleted(args);
            }
            else
            {
                (*lines[endLineIndex]).erase(0, selection.end.column - 1);
                LineEventArgs args(endLineIndex, -1);
                OnLineChanged(args);
            }
        }
        SetCaretLineCol(selection.start.line, selection.start.column + LineNumberFieldLength());
    }
    ResetSelection();
    Invalidate();
    SetDirty();
    SetCCDirty();
    SetFocus();
}

void TextView::IndentSelection()
{
    if (selection.IsEmpty()) return;
    int startLineIndex = selection.start.line - 1;
    int endLineIndex = selection.end.line - 1;
    std::u32string indentString(IndentSize(), ' ');
    for (int i = startLineIndex; i <= endLineIndex; ++i)
    {
        if ((*lines[i]).empty()) continue;
        if (i == selection.end.line - 1 && (*lines[i]).substr(0, selection.end.column - 1).empty()) continue;
        (*lines[i]).insert(0, indentString);
        LineEventArgs args(i, -1);
        OnLineChanged(args);
    }
    InvalidateLines(selection.start.line, selection.end.line);
    SetDirty();
    SetCCDirty();
}

void TextView::UnindentSelection()
{
    if (selection.IsEmpty()) return;
    int startLineIndex = selection.start.line - 1;
    int endLineIndex = selection.end.line - 1;
    std::u32string indentString(IndentSize(), ' ');
    for (int i = startLineIndex; i <= endLineIndex; ++i)
    {
        std::u32string line = *lines[i];
        if (i == endLineIndex)
        {
            line = line.substr(0, selection.end.column - 1);
        }
        if (StartsWith(line, indentString)) 
        {
            (*lines[i]).erase(0, IndentSize());
            LineEventArgs args(i, -1);
            OnLineChanged(args);
        }
    }
    InvalidateLines(selection.start.line, selection.end.line);
    SetDirty();
    SetCCDirty();
}

void TextView::ExtendSelection(const SourcePos& start, const SourcePos& end)
{
    switch (selection.fixed)
    {
        case SelectionFixed::none:
        {
            if (start < end)
            {
                selection.fixed = SelectionFixed::start;
                selection.start = start;
                selection.end = end;
            }
            else
            {
                selection.fixed = SelectionFixed::end;
                selection.end = start;
                selection.start = end;
            }
            break;
        }
        case SelectionFixed::start:
        {
            if (end == selection.start)
            {
                ResetSelection();
            }
            else if (selection.start < end)
            {
                selection.end = end;
            }
            else
            {
                selection.fixed = SelectionFixed::end;
                selection.end = start;
                selection.start = end;
            }
            break;
        }
        case SelectionFixed::end:
        {
            if (end == selection.end)
            { 
                ResetSelection();
            }
            else if (end < selection.end)
            {
                selection.start = end;
            }
            else
            {
                selection.fixed = SelectionFixed::start;
                selection.start = start;
                selection.end = end;
            }
            break;
        }
    }
    OnSelectionChanged();
}

void TextView::InvalidateLines(int startLineNumber, int endLineNumber)
{
    if (charHeight != 0 && charWidth != 0)
    {
        Point pt(0, static_cast<int>(charHeight * (startLineNumber - 1) - charHeight));
        TranslateContentLocation(pt);
        Size size = GetSize();
        Size sz(size.Width, static_cast<int>(charHeight * (endLineNumber - startLineNumber + 1) + 2 * charHeight + 1.5f));
        Rect rect(pt, sz);
        Invalidate(rect);
    }
    else
    {
        Invalidate();
    }
}

void TextView::InvalidateLineCol(int lineNumber, int columnNumber)
{
    if (charHeight != 0 && charWidth != 0)
    {
        Point pt(static_cast<int>(charWidth * (columnNumber - 1)), static_cast<int>(charHeight * (lineNumber - 1)));
        TranslateContentLocation(pt);
        Size size = GetSize();
        Size sz(size.Width - pt.X, static_cast<int>(charHeight + 1.5f));
        Rect rect(pt, sz);
        Invalidate(rect);
    }
    else
    {
        Invalidate();
    }
}

int TextView::GetLineLength(int lineNumber) const
{
    int lineLength = 0;
    if (lineNumber >= 1 && lineNumber <= lines.size())
    {
        lineLength = (*lines[lineNumber - 1]).length();
    }
    return lineLength;
}

void TextView::SetFilePath(const std::string& filePath_)
{
    filePath = filePath_;
}

void TextView::FireGotoCaretLine()
{
    OnGotoCaretLine();
}

void TextView::FireCopy()
{
    OnCopy();
}

void TextView::FireCut()
{
    OnCut();
}
void TextView::FirePaste()
{
    OnPaste();
}

void TextView::OnPaint(PaintEventArgs& args)
{
    try
    {
        PaintContent(args.graphics, args.clipRect);
        Control::OnPaint(args);
    }
    catch (const std::exception& ex)
    {
        ShowErrorMessageBox(Handle(), ex.what());
    }
}

void TextView::OnMouseDown(MouseEventArgs& args)
{
    Control::OnMouseDown(args);
    SetFocus();
    ResetSelection();
    if (args.buttons == MouseButtons::lbutton && !lines.empty())
    {
        int line = 0;
        int column = 0;
        GetLineColumn(args.location, line, column);
        if (line >= 1 && line <= lines.size())
        {
            FixColumn(column, line);
            mouseSelectionStart = SourcePos(line, column);
            SetMouseExtendSelection();
        }
    }
    if (charHeight != 0 && charWidth != 0)
    {
        if ((args.buttons == MouseButtons::lbutton || args.buttons == MouseButtons::rbutton) && args.clicks == 1)
        {
            Point loc = args.location;
            int lineNumber = static_cast<int>(loc.Y / charHeight) + 1;
            lineNumber = std::max(1, std::min(lineNumber, static_cast<int>(lines.size())));
            int columnNumber = static_cast<int>(loc.X / charWidth) + 1;
            columnNumber = std::max(columnNumber, 1 + LineNumberFieldLength());
            int lineLength = GetLineLength(lineNumber);
            columnNumber = std::min(columnNumber, lineLength + 1 + LineNumberFieldLength());
            if (update)
            {
                update = false;
                Invalidate();
            }
            SetCaretLineCol(lineNumber, columnNumber);
            if (args.buttons == MouseButtons::rbutton)
            {
                RightClickEventArgs rightClickArgs(this, loc);
                OnRightClick(rightClickArgs);
            }
        }
    }
}

void TextView::OnMouseMove(MouseEventArgs& args)
{
    Control::OnMouseMove(args);
    if (MouseExtendSelection())
    {
        int line = 0;
        int column = 0;
        GetLineColumn(args.location, line, column);
        if (line >= 1 && line <= lines.size())
        {
            FixColumn(column, line);
            mouseSelectionEnd = SourcePos(line, column);
            if (mouseSelectionStart != mouseSelectionEnd)
            {
                ExtendSelection(mouseSelectionStart, mouseSelectionEnd);
                InvalidateLines(selection.start.line, selection.end.line);
            }
        }
    }
}

void TextView::OnMouseUp(MouseEventArgs& args)
{
    Control::OnMouseUp(args);
    if (MouseExtendSelection())
    {
        ResetMouseExtendSelection();
        int line = 0;
        int column = 0;
        GetLineColumn(args.location, line, column);
        if (line >= 1 && line <= lines.size())
        {
            FixColumn(column, line);
            mouseSelectionEnd = SourcePos(line, column);
            if (mouseSelectionStart != mouseSelectionEnd)
            {
                ExtendSelection(mouseSelectionStart, mouseSelectionEnd);
                InvalidateLines(selection.start.line, selection.end.line);
            }
        }
    }
}

void TextView::OnKeyDown(KeyEventArgs& args)
{
    Control::OnKeyDown(args);
    if (!args.handled)
    {
        bool scrolled = false;
        int lineNumber = caretLine;
        int columnNumber = caretColumn;
        switch (args.key)
        {
            case Keys::escape:
            {
                OnEscape();
                args.handled = true;
                break;
            }
            case Keys::home:
            {
                ResetSelection();
                columnNumber = 1 + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::home:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(CaretLine(), 1);
                ExtendSelection(start, end);
                columnNumber = 1 + LineNumberFieldLength();
                InvalidateLineCol(CaretLine(), columnNumber);
                args.handled = true;
                break;
            }
            case Keys::end:
            {
                ResetSelection();
                columnNumber = GetLineLength(lineNumber) + 1 + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::end:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(CaretLine(), GetLineLength(lineNumber) + 1);
                ExtendSelection(start, end);
                columnNumber = GetLineLength(lineNumber) + 1 + LineNumberFieldLength();
                InvalidateLineCol(CaretLine(), 1 + LineNumberFieldLength());
                args.handled = true;
                break;
            }
            case Keys::left:
            {
                ResetSelection();
                if (columnNumber > 1 + LineNumberFieldLength())
                {
                    --columnNumber;
                }
                else if (lineNumber > 1)
                {
                    --lineNumber;
                    int lineLength = GetLineLength(lineNumber);
                    columnNumber = lineLength + 1 + LineNumberFieldLength();
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::left:
            {
                SourcePos start(CaretLine(), CaretColumn());
                if (columnNumber > 1 + LineNumberFieldLength())
                {
                    SourcePos end(start);
                    --columnNumber;
                    --end.column;
                    ExtendSelection(start, end);
                    InvalidateLines(selection.start.line, selection.end.line);
                }
                else if (lineNumber > 1)
                {
                    SourcePos end(start);
                    --lineNumber;
                    --end.line;
                    int lineLength = GetLineLength(lineNumber);
                    columnNumber = lineLength + 1 + LineNumberFieldLength();
                    end.column = lineLength + 1;
                    ExtendSelection(start, end);
                    InvalidateLines(selection.start.line, selection.end.line);
                }
                args.handled = true;
                break;
            }
            case Keys::right:
            {
                ResetSelection();
                if (lineNumber <= lines.size())
                {
                    int lineLength = GetLineLength(lineNumber);
                    if (columnNumber < lineLength + 1 + LineNumberFieldLength())
                    {
                        ++columnNumber;
                    }
                    else
                    {
                        ++lineNumber;
                        columnNumber = 1 + LineNumberFieldLength();
                    }
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::right:
            {
                SourcePos start(CaretLine(), CaretColumn());
                if (lineNumber <= lines.size())
                {
                    SourcePos end(start);
                    int lineLength = GetLineLength(lineNumber);
                    if (columnNumber < lineLength + 1 + LineNumberFieldLength())
                    {
                        ++columnNumber;
                        ++end.column;
                        ExtendSelection(start, end);
                    }
                    else
                    {
                        ++lineNumber;
                        ++end.line;
                        columnNumber = 1 + LineNumberFieldLength();
                        end.column = 1;
                        ExtendSelection(start, end);
                    }
                    InvalidateLines(selection.start.line, selection.end.line);
                }
                args.handled = true;
                break;
            }
            case Keys::down:
            {
                if (IsCCOpen())
                {
                    OnCCNext();
                }
                else
                {
                    ResetSelection();
                    if (lineNumber < lines.size())
                    {
                        ++lineNumber;
                        int lineLength = GetLineLength(lineNumber);
                        columnNumber = std::min(columnNumber, lineLength + 1 + LineNumberFieldLength());
                    }
                    else
                    {
                        lineNumber = static_cast<int>(lines.size()) + 1;
                        columnNumber = 1 + LineNumberFieldLength();
                    }
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::down:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                if (lineNumber < lines.size())
                {
                    ++lineNumber;
                    ++end.line;
                    int lineLength = GetLineLength(lineNumber);
                    columnNumber = std::min(columnNumber, lineLength + 1 + LineNumberFieldLength());
                    end.column = std::min(end.column, lineLength + 1);
                }
                else
                {
                    lineNumber = static_cast<int>(lines.size()) + 1;
                    end.line = static_cast<int>(lines.size()) + 1;
                    columnNumber = 1 + LineNumberFieldLength();
                    end.column = 1;
                }
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::up:
            {
                if (IsCCOpen())
                {
                    OnCCPrev();
                }
                else
                {
                    ResetSelection();
                    if (lineNumber > 1)
                    {
                        --lineNumber;
                        int lineLength = GetLineLength(lineNumber);
                        columnNumber = std::min(columnNumber, lineLength + 1 + LineNumberFieldLength());
                    }
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::up:
            {
                if (lineNumber > 1)
                {
                    SourcePos start(CaretLine(), CaretColumn());
                    SourcePos end(start);
                    --lineNumber;
                    --end.line;
                    int lineLength = GetLineLength(lineNumber);
                    columnNumber = std::min(columnNumber, lineLength + 1 + LineNumberFieldLength());
                    end.column = std::min(end.column, lineLength + 1);
                    ExtendSelection(start, end);
                    InvalidateLines(selection.start.line, selection.end.line);
                }
                args.handled = true;
                break;
            }
            case Keys::pageDown:
            {
                if (IsCCOpen())
                {
                    OnCCNextPage();
                }
                else
                {
                    ResetSelection();
                    Size size = GetSize();
                    int windowLines = static_cast<int>(size.Height / charHeight);
                    lineNumber = lineNumber + windowLines;
                    lineNumber = std::min(lineNumber, static_cast<int>(lines.size()) + 1);
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::pageDown:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                Size size = GetSize();
                int windowLines = static_cast<int>(size.Height / charHeight);
                lineNumber = lineNumber + windowLines;
                end.line = end.line + windowLines;
                lineNumber = std::min(lineNumber, static_cast<int>(lines.size()) + 1);
                end.line = std::min(end.line, static_cast<int>(lines.size()) + 1);
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::pageUp:
            {
                if (IsCCOpen())
                {
                    OnCCPrevPage();
                }
                else
                {
                    ResetSelection();
                    Size size = GetSize();
                    int windowLines = static_cast<int>(size.Height / charHeight);
                    lineNumber = lineNumber - windowLines;
                    lineNumber = std::max(lineNumber, 1);
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::pageUp:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                Size size = GetSize();
                int windowLines = static_cast<int>(size.Height / charHeight);
                lineNumber = lineNumber - windowLines;
                end.line = end.line - windowLines;
                lineNumber = std::max(lineNumber, 1);
                end.line = std::max(end.line, 1);
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::a:
            {
                ResetSelection();
                SourcePos start(1, 1);
                SourcePos end(static_cast<int>(lines.size() + 1), 1);
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::home:
            {
                ResetSelection();
                lineNumber = 1;
                columnNumber = 1 + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::controlModifier | Keys::home:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                lineNumber = 1;
                end.line = 1;
                columnNumber = 1 + LineNumberFieldLength();
                end.column = 1;
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::end:
            {
                ResetSelection();
                lineNumber = static_cast<int>(lines.size() + 1);
                columnNumber = 1 + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::controlModifier | Keys::end:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                lineNumber = static_cast<int>(lines.size() + 1);
                end.line = static_cast<int>(lines.size() + 1);
                columnNumber = 1 + LineNumberFieldLength();
                end.column = 1;
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::left:
            {
                ResetSelection();
                int line = lineNumber;
                int col = columnNumber - LineNumberFieldLength();
                PrevWord(line, col);
                lineNumber = line;
                columnNumber = col + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::controlModifier | Keys::left:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                int line = lineNumber;
                int col = columnNumber - LineNumberFieldLength();
                PrevWord(line, col);
                lineNumber = line;
                end.line = line;
                columnNumber = col + LineNumberFieldLength();
                end.column = col;
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::right:
            {
                ResetSelection();
                int line = lineNumber;
                int col = columnNumber - LineNumberFieldLength();
                NextWord(line, col);
                lineNumber = line;
                columnNumber = col + LineNumberFieldLength();
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::controlModifier | Keys::right:
            {
                SourcePos start(CaretLine(), CaretColumn());
                SourcePos end(start);
                int line = lineNumber;
                int col = columnNumber - LineNumberFieldLength();
                NextWord(line, col);
                lineNumber = line;
                end.line = line;
                columnNumber = col + LineNumberFieldLength();
                end.column = col;
                ExtendSelection(start, end);
                InvalidateLines(selection.start.line, selection.end.line);
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::down:
            {
                if (lines.size() < GetVisibleLineCount())
                {
                    args.handled = true;
                    return;
                }
                ResetSelection();
                ScrollLineDown();
                scrolled = true;
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::up:
            {
                if (lines.size() < GetVisibleLineCount())
                {
                    args.handled = true;
                    return;
                }
                ResetSelection();
                ScrollLineUp();
                scrolled = true;
                args.handled = true;
                break;
            }
            case Keys::controlModifier | Keys::k:
            {
                ResetSelection();
                FireGotoCaretLine();
                args.handled = true;
                return;
            }
            case Keys::delete_:
            {
                if (!IsReadOnly())
                {
                    if (selection.IsEmpty())
                    {
                        int lineIndex = caretLine - 1;
                        int columnIndex = CaretColumn() - 1;
                        AddDeleteCharCommand(lineIndex, columnIndex);
                        DeleteChar(lineIndex, columnIndex, 0, 0, false);
                    }
                    else
                    {
                        AddRemoveSelectionCommand();
                        RemoveSelection();
                    }
                    args.handled = true;
                    return;
                }
                break;
            }
            case Keys::back:
            {
                if (!IsReadOnly())
                {
                    int lineIndex = caretLine - 1;
                    int columnIndex = CaretColumn() - 1;
                    Backspace(lineIndex, columnIndex);
                    args.handled = true;
                    return;
                }
                break;
            }
            case Keys::enter:
            {
                if (IsCCOpen())
                {
                    OnCCSelect();
                    args.handled = true;
                    return;
                }
                else
                {
                    if (!IsReadOnly())
                    {
                        ResetSelection();
                        int lineIndex = caretLine - 1;
                        int columnIndex = CaretColumn() - 1;
                        AddNewLineCommand(lineIndex, columnIndex);
                        NewLine(lineIndex, columnIndex);
                        args.handled = true;
                        return;
                    }
                }
                break;
            }
            case Keys::tab:
            {
                if (!IsReadOnly())
                {
                    if (selection.IsEmpty())
                    {
                        int lineIndex = caretLine - 1;
                        int columnIndex = CaretColumn() - 1;
                        AddTabCommand(lineIndex, columnIndex);
                        Tab(lineIndex, columnIndex);
                    }
                    else
                    {
                        AddIndentSelectionCommand();
                        IndentSelection();
                    }
                    args.handled = true;
                    return;
                }
                break;
            }
            case Keys::shiftModifier | Keys::tab:
            {
                if (!IsReadOnly())
                {
                    if (selection.IsEmpty())
                    {
                        int lineIndex = caretLine - 1;
                        int columnIndex = CaretColumn() - 1;
                        AddBacktabCommand(lineIndex, columnIndex);
                        Backtab(lineIndex, columnIndex);
                    }
                    else
                    {
                        AddUnindentSelectionCommand();
                        UnindentSelection();
                    }
                    args.handled = true;
                    return;
                }
                break;
            }
            case Keys::controlModifier | Keys::insert:
            {
                if (!selection.IsEmpty())
                {
                    FireCopy();
                }
                args.handled = true;
                break;
            }
            case Keys::shiftModifier | Keys::insert:
            {
                FirePaste();
                args.handled = true;
                return;
            }
            case Keys::shiftModifier | Keys::delete_:
            {
                if (!selection.IsEmpty())
                {
                    FireCut();
                }
                args.handled = true;
                return;
            }
            case Keys::altModifier | Keys::back:
            {
                Undo();
                break;
            }
            case Keys::f4:
            {
                Redo();
                break;
            }
        }
        if (args.handled && !scrolled)
        {
            SetCaretLineCol(lineNumber, columnNumber);
            ScrollToCaret();
        }
    }
}

void TextView::OnKeyPress(KeyPressEventArgs& args)
{
    Control::OnKeyPress(args);
    if (IsReadOnly()) return;
    if (!args.handled && KeyPressed(VK_CONTROL) && args.keyChar == ' ')
    {
        OnCC();
        args.handled = true;
        return;
    }
    if (!KeyPressed(VK_MENU) && KeyPressed(VK_CONTROL))
    {
        return;
    }
    if (!args.handled)
    {
        editCommandList.BeginGroup();
        if (!selection.IsEmpty())
        {
            AddRemoveSelectionCommand();
            RemoveSelection();
        }
        char32_t c(args.keyChar);
        int lineIndex = caretLine - 1;
        int columnIndex = CaretColumn() - 1;
        AddInsertCharCommand(lineIndex, columnIndex, c);
        InsertChar(lineIndex, columnIndex, c);
        editCommandList.EndGroup();
        args.handled = true;
    }
}

void TextView::OnEscape()
{
    escape.Fire();
}

void TextView::OnCC()
{
    cc.Fire();
}

void TextView::OnCCNext()
{
    ccNext.Fire();
}

void TextView::OnCCPrev()
{
    ccPrev.Fire();
}

void TextView::OnCCNextPage()
{
    ccNextPage.Fire();
}

void TextView::OnCCPrevPage()
{
    ccPrevPage.Fire();
}

void TextView::OnCCSelect()
{
    ccSelect.Fire();
}

void TextView::OnGotoCaretLine()
{
    gotoCaretLine.Fire();
}

void TextView::OnCopy()
{
    copy.Fire();
}

void TextView::OnCut()
{
    cut.Fire();
}

void TextView::OnPaste()
{
    paste.Fire();
}

void TextView::PaintContent(Graphics& graphics, const Rect& clipRect)
{
    if (Changed())
    {
        ResetChanged();
        Measure(graphics);
        if (Focused())
        {
            SetCaretLocation();
            ShowCaret();
        }
        OnContentLocationChanged();
    }
    Gdiplus::TextRenderingHint prevRenderingHint = graphics.GetTextRenderingHint();
    CheckGraphicsStatus(graphics.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit));
    CheckGraphicsStatus(graphics.Clear(BackgroundColor()));
    int n = static_cast<int>(lines.size());
    SetLineNumberFieldLength(n);
    int lineNumberFieldLength = LineNumberFieldLength();
    PointF origin(0, 0);
    Size size = GetSize();
    for (int i = 0; i < n; ++i)
    {
        if (IsLinePartiallyVisible(i + 1))
        {
            DrawSelectionBackground(graphics, i + 1, origin, lineNumberFieldLength);
            DrawLine(graphics, i, origin);
        }
        origin.Y = origin.Y + charHeight;
    }
    CheckGraphicsStatus(graphics.SetTextRenderingHint(prevRenderingHint));
}

void TextView::Measure(Graphics& graphics)
{
    charWidth = 0;
    charHeight = 0;
    std::string ms;
    bool measure = false;
    if (IsFixed())
    {
        ms = measureString;
        maxLineLength = static_cast<int>(ToUtf32(measureString).length());
        measure = true;
    }
    else
    {
        if (!lines.empty() && maxLineLength > 0)
        {
            ms = ToUtf8(*lines[maxLineIndex]);
            measure = true;
        }
    }
    if (measure)
    {
        std::vector<const Font*> fnts;
        const Font* font = &GetFont();
        fnts.push_back(font);
        for (const std::unique_ptr<Font>& font : fonts)
        {
            fnts.push_back(font.get());
        }
        for (const Font* font : fnts)
        {
            Gdiplus::TextRenderingHint prevRenderingHint = graphics.GetTextRenderingHint();
            CheckGraphicsStatus(graphics.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit));
            RectF charRect = MeasureString(graphics, ms, *font, PointF(0, 0), stringFormat);
            CheckGraphicsStatus(graphics.SetTextRenderingHint(prevRenderingHint));
            charWidth = std::max(charWidth, charRect.Width / maxLineLength);
            charHeight = std::max(charHeight, charRect.Height);
        }
    }
    SetScrollUnits(ScrollUnits(static_cast<int>(charHeight + 0.5), static_cast<int>(2 * (charWidth + 0.5))));
    SetTextExtent();
}

void TextView::DrawSelectionBackground(Graphics& graphics, int line, const PointF& origin, int lineNumberFieldLength)
{
    if (selection.IsEmpty()) return;
    if (line < selection.start.line) return;
    if (line > selection.end.line) return;
    PointF pt(origin);
    pt.X = pt.X + charWidth * lineNumberFieldLength;
    RectF rect;
    if (line > selection.start.line && line < selection.end.line)
    {
        int lineLength = std::max(1, GetLineLength(line));
        rect = RectF(pt, SizeF(charWidth * lineLength, charHeight));
    }
    else if (selection.start.line == selection.end.line)
    {
        pt.X = pt.X + charWidth * (selection.start.column - 1);
        int selectionLength = selection.end.column - selection.start.column;
        rect = RectF(pt, SizeF(charWidth * selectionLength, charHeight));
    }
    else if (line == selection.start.line)
    {
        pt.X = pt.X + charWidth * (selection.start.column - 1);
        int lineLength = GetLineLength(line);
        int selectionLength = lineLength - selection.start.column + 1;
        rect = RectF(pt, SizeF(charWidth * selectionLength, charHeight));
    }
    else if (line == selection.end.line)
    {
        int selectionLength = selection.end.column - 1;
        rect = RectF(pt, SizeF(charWidth * selectionLength, charHeight));
    }
    Brush* brush = GetOrInsertBrush(selectionBackgroundColor);
    CheckGraphicsStatus(graphics.FillRectangle(brush, rect));
}

void TextView::DrawLine(Graphics& graphics, int lineIndex, const PointF& origin)
{
    const Font& font = GetFont();
    PointF pt(origin);
    const std::u32string& line = *lines[lineIndex];
    std::string s(ToUtf8(line));
    Brush* brush = GetOrInsertBrush(textColor);
    DrawString(graphics, s, font, pt, *brush);
}

SolidBrush* TextView::GetOrInsertBrush(const Color& color)
{
    auto it = brushMap.find(color.GetValue());
    if (it != brushMap.cend())
    {
        return it->second;
    }
    else
    {
        std::unique_ptr<SolidBrush> solidBrushPtr(new SolidBrush(color));
        SolidBrush* solidBrush = solidBrushPtr.get();
        brushMap[color.GetValue()] = solidBrush;
        brushes.push_back(std::move(solidBrushPtr));
        return solidBrush;
    }
}

void TextView::SetCursor()
{
    cmajor::wing::SetCursor(cursor);
}

void TextView::SetCaretLocation()
{
    int x = static_cast<int>(padding.left + (caretColumn - 1) * charWidth);
    int y = static_cast<int>(padding.top + (caretLine - 1) * charHeight);
    Point caretPos(x, y);
    TranslateContentLocation(caretPos);
    if (CaretCreated())
    {
        SetCaretPos(caretPos);
        OnCaretPosChanged();
    }
}

void TextView::CreateCaret()
{
    if (charHeight != 0)
    {
        ::CreateCaret(Handle(), nullptr, 1, static_cast<int>(charHeight));
    }
    else
    {
        Control::CreateCaret();
    }
    SetTimer(caretTimerId, caretTimerPeriod);
}

void TextView::SetContentLocationInternal(const Point& contentLocation)
{
    topLine = static_cast<int>(contentLocation.Y / charHeight + 1.5f);
    topLineDiff = (topLine - 1) * charHeight - contentLocation.Y;
    leftCol = static_cast<int>(contentLocation.X / charWidth + 1.5f);
    leftColDiff = (leftCol - 1) * charWidth - contentLocation.X;
    Control::SetContentLocationInternal(contentLocation);
}

void TextView::OnTimer(TimerEventArgs& args)
{
    Control::OnTimer(args);
    if (args.timerId == caretTimerId)
    {
        if (Focused())
        {
            HideCaret();
            ShowCaret();
        }
    }
}

void TextView::OnLostFocus()
{
    Control::OnLostFocus();
    KillTimer(1);
}

void TextView::OnClick()
{
    Control::OnClick();
    SetFocus();
    ResetSelection();
}

void TextView::OnMouseDoubleClick(MouseEventArgs& args)
{
    Control::OnMouseDoubleClick(args);
    ResetMouseExtendSelection();
    mouseSelectionStart = SourcePos();
    mouseSelectionEnd = SourcePos();
    SetFocus();
    ResetSelection();
    int line = 0;
    int column = 0;
    GetLineColumn(args.location, line, column);
    char32_t c = GetCharAt(line, column);
    while (!IsWhiteSpace(c) && !IsWordSeparator(c))
    {
        if (column == 1 || !Prev(line, column))
        {
            break;
        }
        c = GetCharAt(line, column);
    }
    c = GetCharAt(line, column);
    if (IsWhiteSpace(c) || IsWordSeparator(c))
    {
        Next(line, column);
    }
    SourcePos start(line, column);
    c = GetCharAt(line, column);
    while (!IsWhiteSpace(c) && !IsWordSeparator(c))
    {
        if (!Next(line, column))
        {
            break;
        }
        c = GetCharAt(line, column);
    }
    if (!IsWhiteSpace(c) && !IsWordSeparator(c))
    {
        column = column + 1;
    }
    SourcePos end(line, column);
    ExtendSelection(start, end);
    SetCaretLineCol(start.line, start.column + LineNumberFieldLength());
    ScrollToCaret();
    InvalidateLineCol(start.line, start.column);
}

int TextView::GetIndent(const std::u32string& line, int lineIndex)
{
    for (int i = 0; i < line.length(); ++i)
    {
        if (line[i] != ' ')
        {
            return i;
        }
    }
    return 0;
}

int TextView::RemoveIndent(int lineIndex) const
{
    return 0;
}

void TextView::OnSelectionChanged()
{
    selectionChanged.Fire();
}

void TextView::OnDirtyChanged()
{
    dirtyChanged.Fire();
}

void TextView::OnReadOnlyChanged()
{
    readOnlyChanged.Fire();
}

void TextView::OnCCDirtyChanged()
{
    ccDirtyChanged.Fire();
}

void TextView::OnLineInserted(LineEventArgs& args)
{
    lineInserted.Fire(args);
    int lineIndex = args.lineIndex;
    std::u32string& line = *lines[lineIndex];
    int indent = 0;
    if (args.indentLineIndex >= 0 && args.indentLineIndex < lines.size())
    {
        indent = GetIndent(*lines[args.indentLineIndex], args.indentLineIndex);
    }
    if (indent != 0)
    {
        line.insert(0, std::u32string(indent, ' '));
    }
    Invalidate();
}

void TextView::OnLineChanged(LineEventArgs& args)
{
    lineChanged.Fire(args);
    InvalidateLineCol(args.lineIndex + 1, 1 + LineNumberFieldLength());
}

void TextView::OnLineDeleted(LineEventArgs& args)
{
    lineDeleted.Fire(args);
    Invalidate();
}

void TextView::OnCaretPosChanged()
{
    caretPosChanged.Fire();
}

void TextView::OnLinesChanged()
{
    linesChanged.Fire();
}

void TextView::OnCCTextChanged()
{
    ccTextChanged.Fire();
}

void TextView::AddInsertCharCommand(int lineIndex, int columnIndex, char32_t c)
{
    bool removeIndent = false;
    while (lineIndex >= lines.size())
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
    }
    std::u32string& line = *lines[lineIndex];
    if (c == '}' && StartsWith(line, std::u32string(IndentSize(), ' ')))
    {
        removeIndent = true;
    }
    editCommandList.AddCommand(new InsertCharCommand(lineIndex, columnIndex, c, removeIndent));
}

void TextView::AddInsertLinesCommand(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToInsert)
{
    editCommandList.AddCommand(new InsertLinesCommand(lineIndex, columnIndex, linesToInsert));
}

void TextView::AddNewLineCommand(int lineIndex, int columnIndex)
{
    while (lineIndex >= lines.size())
    {
        lines.push_back(std::unique_ptr<std::u32string>(new std::u32string()));
    }
    std::u32string& line = *lines[lineIndex];
    int indent = 0;
    bool countIndent = false;
    int i = columnIndex - 1;
    while (i >= 0)
    {
        if (line[i] != ' ')
        {
            countIndent = true;
            break;
        }
        --i;
    }
    if (countIndent)
    {
        for (int i = 0; i < columnIndex - 1; ++i)
        {
            if (line[i] == ' ') ++indent; else break;
        }
    }
    int numSpaces = 0;
    i = columnIndex - 1;
    while (i >= 0 && line[i] == ' ')
    {
        ++numSpaces;
        --i;
    }
    editCommandList.AddCommand(new NewLineCommand(lineIndex, columnIndex, indent, numSpaces));
}

void TextView::AddTabCommand(int lineIndex, int columnIndex)
{
    editCommandList.AddCommand(new TabCommand(lineIndex, columnIndex));
}

void TextView::AddBacktabCommand(int lineIndex, int columnIndex)
{
    std::u32string& line = *lines[lineIndex];
    int targetCol = std::max(1, columnIndex - IndentSize() + 1);
    int numSpaces = 0;
    for (int i = columnIndex; i >= targetCol; --i)
    {
        if (line[i - 1] == ' ')
        {
            ++numSpaces;
        }
        else
        {
            break;
        }
    }
    editCommandList.AddCommand(new BacktabCommand(lineIndex, columnIndex, numSpaces));
}

void TextView::AddDeleteCharCommand(int lineIndex, int columnIndex)
{
    if (lineIndex >= lines.size()) return;
    std::u32string& line = *lines[lineIndex];
    if (columnIndex < line.length())
    {
        char32_t c = line[columnIndex];
        editCommandList.AddCommand(new DeleteCharCommand(lineIndex, columnIndex, c));
    }
    else
    {
        if (lines.size() > lineIndex + 1)
        {
            editCommandList.AddCommand(new DeleteCharCommand(lineIndex, columnIndex, char32_t()));
        }
    }
}

void TextView::AddRemoveSelectionCommand()
{
    if (selection.IsEmpty()) return;
    bool wholeLine = false;
    SelectionData selectionData = GetSelection();
    if (selection.start.line == selection.end.line)
    {
        if (selection.end.column - selection.start.column >= GetLineLength(selection.start.line))
        {
            wholeLine = true;
        }
    }
    else
    {
        int endLineIndex = selection.end.line - 1;
        if (selection.end.column > GetLineLength(endLineIndex + 1))
        {
            wholeLine = true;
        }
    }
    editCommandList.AddCommand(new RemoveSelectionCommand(selection, selectionData, wholeLine));
}

void TextView::AddIndentSelectionCommand()
{
    editCommandList.AddCommand(new IndentSelectionCommand(selection));
}

void TextView::AddUnindentSelectionCommand()
{
    editCommandList.AddCommand(new UnindentSelectionCommand(selection));
}

void TextView::FixColumn(int& column, int line)
{
    if (column < 1) column = 1;
    int n = GetLineLength(line);
    if (column > n + 1)
    {
        column = n + 1;
    }
}

std::u32string TrimEnd(const std::u32string& line)
{
    int i = line.length();
    while (i > 0 && std::isspace(line[i - 1]))
    {
        --i;
    }
    return line.substr(0, i);
}

int GetNumTrailingSpaces(const std::u32string& line)
{
    int numTrailingSpaces = 0;
    int i = line.length();
    while (i > 0 && std::isspace(line[i - 1]))
    {
        --i;
        ++numTrailingSpaces;
    }
    return numTrailingSpaces;
}

std::vector<std::u32string> SplitTextIntoLines(const std::u32string& text)
{
    std::vector<std::u32string> lines;
    std::u32string line;
    int state = 0;
    for (char32_t c : text)
    {
        switch (state)
        {
            case 0:
            {
                switch (c)
                {
                    case '\n':
                    {
                        lines.push_back(TrimEnd(line));
                        line.clear();
                        break;
                    }
                    case '\r':
                    {
                        state = 1;
                        break;
                    }
                    default:
                    {
                        line.append(1, c);
                        break;
                    }
                }
                break;
            }
            case 1:
            {
                if (c == '\n')
                {
                    lines.push_back(TrimEnd(line));
                    line.clear();
                    state = 0;
                }
                break;
            }
        }
    }
    if (!line.empty())
    {
        lines.push_back(TrimEnd(line));
    }
    return lines;
}

int MinIndent(const std::u32string& s)
{
    if (s.empty()) return 0;
    int state = 0;
    int indent = 0;
    int minIndent = std::numeric_limits<int>::max();
    for (char32_t c : s)
    {
        switch (state)
        {
            case 0:
            {
                if (c == ' ')
                {
                    ++indent;
                }
                else if (c == '\n')
                {
                    if (indent < minIndent)
                    {
                        minIndent = indent;
                    }
                    indent = 0;
                }
                else
                {
                    if (indent < minIndent)
                    {
                        minIndent = indent;
                    }
                    indent = 0;
                    state = 1;
                }
                break;
            }
            case 1:
            {
                if (c == '\n')
                {
                    state = 0;
                }
                break;
            }
        }
    }
    if (indent > 0 && indent < minIndent)
    {
        minIndent = indent;
    }
    if (minIndent == std::numeric_limits<int>::max())
    {
        minIndent = 0;
    }
    return minIndent;
}

std::u32string Unindent(const std::u32string& s, int indent)
{
    if (indent == 0) return s;
    std::u32string line;
    std::u32string result;
    int state = 0;
    for (char32_t c : s)
    {
        switch (state)
        {
            case 0:
            {
                switch (c)
                {
                    case '\n':
                    {
                        result.append(line.substr(indent)).append(1, '\n');
                        line.clear();
                        break;
                    }
                    case '\r':
                    {
                        state = 1;
                        break;
                    }
                    default:
                    {
                        line.append(1, c);
                        break;
                    }
                }
                break;
            }
            case 1:
            {
                if (c == '\n')
                {
                    result.append(line.substr(indent)).append(1, '\n');
                    line.clear();
                    state = 0;
                }
                break;
            }
        }
    }
    if (!line.empty())
    {
        result.append(line.substr(indent));
    }
    return result;
}

std::vector<int> CalculateLineStartIndeces(const std::u32string& text)
{
    std::vector<int> indeces;
    int state = 0;
    int n = text.length();
    for (int i = 0; i < n; ++i)
    {
        char32_t c = text[i];
        switch (state)
        {
            case 0:
            {
                indeces.push_back(i);
                if (c != '\n')
                {
                    state = 1;
                }
                break;
            }
            case 1:
            {
                if (c == '\n')
                {
                    state = 0;
                }
                break;
            }
        }
    }
    return indeces;
}

} } // cmajor::wing
