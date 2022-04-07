// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/guiterm/TerminalControl.hpp>
#include <system-x/kernel/OsApi.hpp>
#include <wing/Container.hpp>
#include <wing/TabControl.hpp>
#include <wing/Application.hpp>
#include <soulng/util/Unicode.hpp>

#undef min
#undef max

namespace cmsx::guiterm {

using namespace soulng::unicode;

const int caretTimerId = 1;
const int caretTimerPeriod = 5000; // 5 seconds

bool IsPrintChar(char32_t ch)
{
    return ch >= 32 && ch < static_cast<char32_t>(cmsx::kernel::specialKeyStart);
}

TerminalControlCreateParams::TerminalControlCreateParams() :
    controlCreateParams()
{
    controlCreateParams.WindowClassName("guiterm.terminal");
    controlCreateParams.BackgroundColor(Color(12, 12, 12));
    controlCreateParams.WindowClassBackgroundColor(COLOR_BACKGROUND);
    textColor = Color(204, 204, 204);
    fontFamilyName = "Cascadia Mono";
    fontSize = 10.0f;
}

TerminalControlCreateParams& TerminalControlCreateParams::Defaults()
{
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::Location(const Point& location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::SetSize(const Size& size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::TextColor(const Color& textColor_)
{
    textColor = textColor_;
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    fontFamilyName = fontFamilyName_;
    return *this;
}

TerminalControlCreateParams& TerminalControlCreateParams::FontSize(float fontSize_)
{
    fontSize = fontSize_;
    return *this;
}

TerminalControl::TerminalControl(TerminalControlCreateParams& createParams) :
    cmajor::wing::Control(createParams.controlCreateParams), foreBrush(nullptr), backBrush(nullptr), changed(false), active(false), stringFormat(), charWidth(0), charHeight(0),
    rows(0), cols(0), cursorX(0), cursorY(0), outputEndCursorPosX(0), outputEndCursorPosY(0), inputMode(InputMode::cooked), echo(true), eof(false), pos(0), lineIndex(0)
{
    std::u16string fontFamilyName = ToUtf16(createParams.fontFamilyName);
    SetFont(
        cmajor::wing::Font(cmajor::wing::FontFamily((WCHAR*)fontFamilyName.c_str()),
        createParams.fontSize,
        cmajor::wing::FontStyle::FontStyleRegular,
        cmajor::wing::Unit::UnitPoint));
    defaultForeColor = ConsoleColor::gray;
    defaultBackColor = ConsoleColor::black;
    SetColors(defaultForeColor, defaultBackColor);
    SetSize(40, 120);
    changed = true;
    stringFormat.SetLineAlignment(StringAlignment::StringAlignmentNear);
    stringFormat.SetAlignment(StringAlignment::StringAlignmentNear);
    Invalidate();
}

void TerminalControl::SetColors(ConsoleColor foreColor_, ConsoleColor backColor_)
{
    foreColor = foreColor_;
    backColor = backColor_;
    foreBrush = GetOrInsertSolidBrush(foreColor);
    backBrush = GetOrInsertSolidBrush(backColor);
}

void TerminalControl::SetSize(int32_t rows_, int32_t cols_)
{
    rows = rows_;
    cols = cols_;
    screen.resize(rows);
    for (int i = 0; i < rows; ++i)
    {
        screen[i].resize(cols);
    }
    ClearScreen();
}

void TerminalControl::SetCursorPos(int32_t cursorX_, int32_t cursorY_)
{
    cursorX = std::min(cols - 1, cursorX_);
    cursorY = std::min(rows - 1, cursorY_);
    if (charWidth > 0 && charHeight > 0)
    {
        Point caretPos(charWidth * cursorX, charHeight * cursorY);
        SetCaretPos(caretPos);
    }
}

void TerminalControl::SetOutputEndCursorPos(int32_t x, int32_t y)
{
    outputEndCursorPosX = x;
    outputEndCursorPosY = y;
}

SolidBrush* TerminalControl::GetOrInsertSolidBrush(ConsoleColor color)
{
    auto it = brushMap.find(color);
    if (it != brushMap.cend())
    {
        return it->second;
    }
    SolidBrush* solidBrush = new SolidBrush(GetColor(color));
    brushes.push_back(std::unique_ptr<SolidBrush>(solidBrush));
    brushMap[color] = solidBrush;
    return solidBrush;
}

void TerminalControl::Write(const std::u32string& text)
{
    for (char32_t c : text)
    {
        if (c == '\n')
        {
            ++cursorY;
            cursorX = 0;
            while (cursorY >= rows)
            {
                Scroll();
                --cursorY;
            }
        }
        else
        {
            screen[cursorY][cursorX] = ColorChar(c, foreColor, backColor);
            ++cursorX;
        }
    }
    Invalidate();
}

void TerminalControl::Activate()
{
    active = true;
    Component* component = GetContainer()->Parent();
    while (!component->IsTabPage())
    {
        component = component->GetContainer()->Parent();
    }
    TabPage* tabPage = static_cast<TabPage*>(component);
    tabPage->Select();
    SetFocus();
}

void TerminalControl::HandleKeyDown(KeyEventArgs& args)
{
    OnKeyDown(args);
}

void TerminalControl::HandleKeyPress(KeyPressEventArgs& args)
{
    OnKeyPress(args);
}

void TerminalControl::SetCooked()
{
    inputMode = InputMode::cooked;
}

void TerminalControl::SetRaw()
{
    inputMode = InputMode::raw;
}

void TerminalControl::SetEcho(bool echo_)
{
    echo = echo_;
}

std::vector<uint8_t> TerminalControl::ReadInput(int64_t count)
{
    Activate();
    while (!eof && terminalInputBuffer.empty())
    {
        Application::ProcessMessages();
    }
    std::vector<uint8_t> bytes;
    if (terminalInputBuffer.empty())
    {
        if (eof)
        {
            eof = false;
            return std::vector<uint8_t>();
        }
    }
    else
    {
        int64_t n = std::min(int64_t(terminalInputBuffer.size()), count);
        for (int64_t i = 0; i < n; ++i)
        {
            uint8_t x = terminalInputBuffer.front();
            terminalInputBuffer.pop_front();
            bytes.push_back(x);
        }
        return bytes;
    }
    return std::vector<uint8_t>();
}

void TerminalControl::ScrollToCursor()
{
    Size sz = GetSize();
    int windowLines = static_cast<int>(sz.Height / charHeight);
    Point cursorLocation(0, std::max(static_cast<int>(0), static_cast<int>((cursorY - windowLines) * charHeight)));
    SetContentLocation(cursorLocation);
}

void TerminalControl::ScrollToLine(int lineIndex)
{
    Point loc(0, lineIndex * charHeight);
    SetContentLocation(loc);
}

void TerminalControl::PushLines()
{
    linesStack.push(std::move(lines));
}

void TerminalControl::PopLines()
{
    if (!linesStack.empty())
    {
        lines = std::move(linesStack.top());
        linesStack.pop();
    }
}

void TerminalControl::ClearScreen()
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            screen[i][j] = ColorChar(' ', foreColor, backColor);
        }
    }
}

void TerminalControl::Scroll()
{
    for (int i = 0; i < rows - 1; ++i)
    {
        screen[i] = screen[i + 1];
    }
    screen[rows - 1].resize(cols);
    std::vector<ColorChar>& bottomLine = screen[rows - 1];
    for (int i = 0; i < cols; ++i)
    {
        bottomLine[i] = ColorChar(' ', foreColor, backColor);
    }
    Invalidate();
}

void TerminalControl::Measure(cmajor::wing::Graphics& graphics)
{
    PointF origin;
    std::string measureString = "This is a XXX test string";
    RectF rect = MeasureString(graphics, measureString, GetFont(), origin, stringFormat);
    charWidth = rect.Width / measureString.length();
    charHeight = rect.Height;
    Size contentSize(Size(static_cast<int>((rect.Width / measureString.length()) * cols + 0.5), static_cast<int>(rect.Height * rows + 0.5)));
    SetContentSize(contentSize);
}

void TerminalControl::OutputLine(Graphics& graphics, PointF& origin, std::u32string& line)
{
    RectF rect(origin, SizeF(line.length(), charHeight));
    graphics.FillRectangle(backBrush, rect);
    std::string ln = ToUtf8(line);
    DrawString(graphics, ln, GetFont(), origin, *foreBrush);
    origin.X += charWidth * line.length();
    line.clear();
}

void TerminalControl::HandleInputChar(char32_t ch)
{
    if (inputMode == InputMode::raw)
    {
        if (echo)
        {
            if (IsPrintChar(ch))
            {
                PrintChar(ch);
            }
            else if (ch == '\n')
            {
                PrintNewLine();
            }
        }
        if (ch == cmsx::kernel::keyControlD)
        {
            HandleEof();
        }
        else
        {
            std::string chars = ToUtf8(std::u32string(1, ch));
            for (char c : chars)
            {
                terminalInputBuffer.push_back(static_cast<uint8_t>(c));
            }
        }
    }
    else if (inputMode == InputMode::cooked)
    {
        if (ch < 32)
        {
            switch (ch)
            {
                case '\n':
                {
                    HandleNewLine();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyBackspace):
                {
                    HandleBackspace();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyTab):
                {
                    HandleTab();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyControlD):
                {
                    HandleEof();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyEscape):
                {
                    HandleEscape();
                    break;
                }
            }
        }
        else if (ch >= cmsx::kernel::specialKeyStart && ch <= cmsx::kernel::specialKeyEnd)
        {
            switch (ch)
            {
                case static_cast<char32_t>(cmsx::kernel::keyHome):
                {
                    HandleHome();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyEnd):
                {
                    HandleEnd();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyLeft):
                {
                    HandleLeft();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyRight):
                {
                    HandleRight();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyUp):
                {
                    HandleUp();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyDown):
                {
                    HandleDown();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyControlLeft):
                {
                    HandleControlLeft();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyControlRight):
                {
                    HandleControlRight();
                    break;
                }
                case static_cast<char32_t>(cmsx::kernel::keyDel):
                {
                    HandleDel();
                    break;
                }
            }
        }
        else
        {
            HandleChar(ch);
        }
    }
}

void TerminalControl::HandleEof()
{
    eof = true;
}

void TerminalControl::PrintChar(char32_t ch)
{
    std::u32string chars(1, ch);
    Write(chars);
}

void TerminalControl::PrintNewLine()
{
    std::u32string chars(1, '\n');
    Write(chars);
}

void TerminalControl::HandleChar(char32_t ch)
{
    if (pos < line.length())
    {
        line = line.substr(0, pos) + std::u32string(1, ch) + line.substr(pos);
    }
    else
    {
        line.append(1, ch);
    }
    ++pos;
    UpdateLine();
    UpdateCursorPos();
}

void TerminalControl::HandleNewLine()
{
    if (!line.empty())
    {
        bool found = false;
        for (const std::u32string& prevLine : lines)
        {
            if (line == prevLine)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            lines.push_back(line);
            lineIndex = lines.size();
        }
    }
    line.append(1, '\n');
    std::string inputLine = ToUtf8(line);
    line.clear();
    pos = 0;
    if (cursorY >= rows - 1)
    {
        Write(U"\n");
    }
    else
    {
        ++cursorY;
    }
    SetCursorPos(0, cursorY);
    outputEndCursorPosX = 0;
    outputEndCursorPosY = cursorY;
    for (char c : inputLine)
    {
        terminalInputBuffer.push_back(static_cast<uint8_t>(c));
    }
}

void TerminalControl::HandleBackspace()
{
    if (pos > 0)
    {
        line = line.substr(0, pos - 1) + line.substr(pos);
        --pos;
        UpdateLine();
        UpdateCursorPos();
    }
}

void TerminalControl::HandleTab()
{
    // todo
}

void TerminalControl::HandleEscape()
{
    ClearUpdateLine();
    line.clear();
    pos = 0;
    UpdateLine();
    UpdateCursorPos();
}

void TerminalControl::HandleHome()
{
    pos = 0;
    UpdateCursorPos();
}

void TerminalControl::HandleEnd()
{
    pos = line.length();
    UpdateCursorPos();
}

void TerminalControl::HandleLeft()
{
    if (pos > 0)
    {
        --pos;
        UpdateCursorPos();
    }
}

void TerminalControl::HandleRight()
{
    if (pos < line.length())
    {
        ++pos;
        UpdateCursorPos();
    }
}

void TerminalControl::HandleUp()
{
    if (lineIndex > 0)
    {
        ClearUpdateLine();
        --lineIndex;
        if (lineIndex < lines.size())
        {
            line = lines[lineIndex];
        }
        UpdateLine();
        HandleEnd();
    }
}

void TerminalControl::HandleDown()
{
    if (lineIndex < int32_t(lines.size()) - 1)
    {
        ClearUpdateLine();
        ++lineIndex;
        line = lines[lineIndex];
        UpdateLine();
        HandleEnd();
    }
}

void TerminalControl::HandleControlLeft()
{
    if (pos > 0)
    {
        --pos;
        if (pos > 0)
        {
            char c = line[pos];
            while (c == ' ')
            {
                if (pos > 0)
                {
                    --pos;
                }
                if (pos > 0)
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            while (c != ' ')
            {
                if (pos > 0)
                {
                    --pos;
                }
                if (pos > 0)
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            if (c == ' ')
            {
                ++pos;
            }
        }
    }
    UpdateCursorPos();
}

void TerminalControl::HandleControlRight()
{
    if (pos < line.length())
    {
        ++pos;
        if (pos < line.length())
        {
            char c = line[pos];
            while (c != ' ')
            {
                if (pos < line.length())
                {
                    ++pos;
                }
                if (pos < line.length())
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
            while (c == ' ')
            {
                if (pos < line.length())
                {
                    ++pos;
                }
                if (pos < line.length())
                {
                    c = line[pos];
                }
                else
                {
                    UpdateCursorPos();
                    return;
                }
            }
        }
    }
    UpdateCursorPos();
}

void TerminalControl::HandleDel()
{
    if (pos < line.length())
    {
        ++pos;
        HandleBackspace();
    }
}

void TerminalControl::ClearUpdateLine()
{
    if (!echo) return;
    int32_t prevCursorPosX = cursorX;
    int32_t prevCursorPosY = cursorY;
    SetCursorPos(outputEndCursorPosX, outputEndCursorPosY);
    std::u32string updateLine;
    int32_t x = outputEndCursorPosX;
    int32_t y = outputEndCursorPosY;
    for (int i = 0; i < line.length(); ++i)
    {
        if (x + i < cols)
        {
            updateLine.append(1, ' ');
        }
        else
        {
            Write(updateLine);
            updateLine.clear();
            x = 0;
            ++y;
            updateLine.append(1, ' ');
        }
    }
    if (!updateLine.empty())
    {
        Write(updateLine);
        x += updateLine.length();
        updateLine.clear();
    }
    for (int i = x; i < cols; ++i)
    {
        updateLine.append(1, ' ');
    }
    if (!updateLine.empty())
    {
        Write(updateLine);
    }
    SetCursorPos(prevCursorPosX, prevCursorPosY);
}

void TerminalControl::UpdateLine()
{
    if (!echo) return;
    int32_t prevCursorPosX = cursorX;
    int32_t prevCursorPosY = cursorY;
    SetCursorPos(outputEndCursorPosX, outputEndCursorPosY);
    int32_t x = outputEndCursorPosX;
    int32_t y = outputEndCursorPosY;
    std::u32string updateLine;
    for (int i = 0; i < line.length(); ++i)
    {
        if (x < cols)
        {
            updateLine.append(1, line[i]);
            ++x;
        }
        else
        {
            Write(updateLine);
            updateLine.clear();
            x = 0;
            ++y;
            updateLine.append(1, line[i]);
        }
    }
    if (!updateLine.empty())
    {
        Write(updateLine);
        updateLine.clear();
    }
    for (int i = x; i < cols; ++i)
    {
        updateLine.append(1, ' ');
    }
    if (!updateLine.empty())
    {
        Write(updateLine);
    }
    SetCursorPos(prevCursorPosX, prevCursorPosY);
}

void TerminalControl::UpdateCursorPos()
{
    if (!echo) return;
    int32_t x = (outputEndCursorPosX + pos) % cols;
    int32_t y = outputEndCursorPosY + (outputEndCursorPosX + pos) / cols;
    if (y >= rows)
    {
        y = rows - 1;
    }
    SetCursorPos(x, y);
}

void TerminalControl::OnPaint(PaintEventArgs& args)
{
    if (changed)
    {
        changed = false;
        Measure(args.graphics);
    }
    foreBrush = GetOrInsertSolidBrush(foreColor);
    backBrush = GetOrInsertSolidBrush(backColor);
    for (int i = 0; i < rows; ++i)
    {
        PointF origin(0, i * charHeight);
        std::u32string line;
        for (int j = 0; j < cols; ++j)
        {
            const ColorChar& colorChar = screen[i][j];
            if (colorChar.foreColor != foreColor || colorChar.backColor != backColor)
            {
                if (!line.empty())
                {
                    OutputLine(args.graphics, origin, line);
                }
                if (colorChar.foreColor != foreColor)
                {
                    foreColor = colorChar.foreColor;
                    foreBrush = GetOrInsertSolidBrush(foreColor);
                }
                if (colorChar.backColor != backColor)
                {
                    backColor = colorChar.backColor;
                    backBrush = GetOrInsertSolidBrush(backColor);
                }
            }
            line.append(1, colorChar.ch);
        }
        if (!line.empty())
        {
            OutputLine(args.graphics, origin, line);
        }
    }
}

void TerminalControl::OnKeyDown(KeyEventArgs& args)
{
    switch (args.key)
    {
        case Keys::back:
        {
            HandleInputChar(cmsx::kernel::keyBackspace);
            args.handled = true;
            break;
        }
        case Keys::tab:
        {
            HandleInputChar(cmsx::kernel::keyTab);
            args.handled = true;
            break;
        }
        case Keys::return_:
        {
            HandleInputChar(cmsx::kernel::keyNewLine);
            args.handled = true;
            break;
        }
        case Keys::space:
        {
            HandleInputChar(' ');
            args.handled = true;
            break;
        }
        case Keys::a:
        {
            HandleInputChar('a');
            args.handled = true;
            break;
        }
        case Keys::b:
        {
            HandleInputChar('b');
            args.handled = true;
            break;
        }
        case Keys::c:
        {
            HandleInputChar('c');
            args.handled = true;
            break;
        }
        case Keys::d:
        {
            HandleInputChar('d');
            args.handled = true;
            break;
        }
        case Keys::e:
        {
            HandleInputChar('e');
            args.handled = true;
            break;
        }
        case Keys::f:
        {
            HandleInputChar('f');
            args.handled = true;
            break;
        }
        case Keys::g:
        {
            HandleInputChar('g');
            args.handled = true;
            break;
        }
        case Keys::h:
        {
            HandleInputChar('h');
            args.handled = true;
            break;
        }
        case Keys::i:
        {
            HandleInputChar('i');
            args.handled = true;
            break;
        }
        case Keys::j:
        {
            HandleInputChar('j');
            args.handled = true;
            break;
        }
        case Keys::k:
        {
            HandleInputChar('k');
            args.handled = true;
            break;
        }
        case Keys::l:
        {
            HandleInputChar('l');
            args.handled = true;
            break;
        }
        case Keys::m:
        {
            HandleInputChar('m');
            args.handled = true;
            break;
        }
        case Keys::n:
        {
            HandleInputChar('n');
            args.handled = true;
            break;
        }
        case Keys::o:
        {
            HandleInputChar('o');
            args.handled = true;
            break;
        }
        case Keys::p:
        {
            HandleInputChar('p');
            args.handled = true;
            break;
        }
        case Keys::q:
        {
            HandleInputChar('q');
            args.handled = true;
            break;
        }
        case Keys::r:
        {
            HandleInputChar('r');
            args.handled = true;
            break;
        }
        case Keys::s:
        {
            HandleInputChar('s');
            args.handled = true;
            break;
        }
        case Keys::t:
        {
            HandleInputChar('t');
            args.handled = true;
            break;
        }
        case Keys::u:
        {
            HandleInputChar('u');
            args.handled = true;
            break;
        }
        case Keys::v:
        {
            HandleInputChar('v');
            args.handled = true;
            break;
        }
        case Keys::w:
        {
            HandleInputChar('w');
            args.handled = true;
            break;
        }
        case Keys::x:
        {
            HandleInputChar('x');
            args.handled = true;
            break;
        }
        case Keys::y:
        {
            HandleInputChar('y');
            args.handled = true;
            break;
        }
        case Keys::z:
        {
            HandleInputChar('z');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::a:
        {
            HandleInputChar('A');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::b:
        {
            HandleInputChar('B');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::c:
        {
            HandleInputChar('C');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::d:
        {
            HandleInputChar('D');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::e:
        {
            HandleInputChar('E');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f:
        {
            HandleInputChar('F');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::g:
        {
            HandleInputChar('G');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::h:
        {
            HandleInputChar('H');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::i:
        {
            HandleInputChar('I');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::j:
        {
            HandleInputChar('J');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::k:
        {
            HandleInputChar('K');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::l:
        {
            HandleInputChar('L');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::m:
        {
            HandleInputChar('M');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::n:
        {
            HandleInputChar('N');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::o:
        {
            HandleInputChar('O');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::p:
        {
            HandleInputChar('P');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::q:
        {
            HandleInputChar('Q');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::r:
        {
            HandleInputChar('R');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::s:
        {
            HandleInputChar('S');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::t:
        {
            HandleInputChar('T');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::u:
        {
            HandleInputChar('U');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::v:
        {
            HandleInputChar('V');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::w:
        {
            HandleInputChar('W');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::x:
        {
            HandleInputChar('X');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::y:
        {
            HandleInputChar('Y');
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::z:
        {
            HandleInputChar('Z');
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::a:
        {
            HandleInputChar(cmsx::kernel::keyControlA);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::b:
        {
            HandleInputChar(cmsx::kernel::keyControlB);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::c:
        {
            HandleInputChar(cmsx::kernel::keyControlC);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::d:
        {
            HandleInputChar(cmsx::kernel::keyControlD);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::e:
        {
            HandleInputChar(cmsx::kernel::keyControlE);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f:
        {
            HandleInputChar(cmsx::kernel::keyControlF);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::g:
        {
            HandleInputChar(cmsx::kernel::keyControlG);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::h:
        {
            HandleInputChar(cmsx::kernel::keyControlH);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::i:
        {
            HandleInputChar(cmsx::kernel::keyControlI);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::j:
        {
            HandleInputChar(cmsx::kernel::keyControlJ);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::k:
        {
            HandleInputChar(cmsx::kernel::keyControlK);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::m:
        {
            HandleInputChar(cmsx::kernel::keyControlM);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::n:
        {
            HandleInputChar(cmsx::kernel::keyControlN);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::o:
        {
            HandleInputChar(cmsx::kernel::keyControlO);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::p:
        {
            HandleInputChar(cmsx::kernel::keyControlP);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::q:
        {
            HandleInputChar(cmsx::kernel::keyControlQ);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::r:
        {
            HandleInputChar(cmsx::kernel::keyControlR);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::s:
        {
            HandleInputChar(cmsx::kernel::keyControlS);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::t:
        {
            HandleInputChar(cmsx::kernel::keyControlT);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::u:
        {
            HandleInputChar(cmsx::kernel::keyControlU);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::v:
        {
            HandleInputChar(cmsx::kernel::keyControlV);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::w:
        {
            HandleInputChar(cmsx::kernel::keyControlW);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::x:
        {
            HandleInputChar(cmsx::kernel::keyControlX);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::y:
        {
            HandleInputChar(cmsx::kernel::keyControlY);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::z:
        {
            HandleInputChar(cmsx::kernel::keyControlZ);
            args.handled = true;
            break;
        }
        case Keys::escape:
        {
            HandleInputChar(cmsx::kernel::keyEscape);
            args.handled = true;
            break;
        }
        case Keys::down:
        {
            HandleInputChar(cmsx::kernel::keyDown);
            args.handled = true;
            break;
        }
        case Keys::up:
        {
            HandleInputChar(cmsx::kernel::keyUp);
            args.handled = true;
            break;
        }
        case Keys::left:
        {
            HandleInputChar(cmsx::kernel::keyLeft);
            args.handled = true;
            break;
        }
        case Keys::right:
        {
            HandleInputChar(cmsx::kernel::keyRight);
            args.handled = true;
            break;
        }
        case Keys::home:
        {
            HandleInputChar(cmsx::kernel::keyHome);
            args.handled = true;
            break;
        }
        case Keys::f1:
        {
            HandleInputChar(cmsx::kernel::keyF1);
            args.handled = true;
            break;
        }
        case Keys::f2:
        {
            HandleInputChar(cmsx::kernel::keyF2);
            args.handled = true;
            break;
        }
        case Keys::f3:
        {
            HandleInputChar(cmsx::kernel::keyF3);
            args.handled = true;
            break;
        }
        case Keys::f4:
        {
            HandleInputChar(cmsx::kernel::keyF4);
            args.handled = true;
            break;
        }
        case Keys::f5:
        {
            HandleInputChar(cmsx::kernel::keyF5);
            args.handled = true;
            break;
        }
        case Keys::f6:
        {
            HandleInputChar(cmsx::kernel::keyF6);
            args.handled = true;
            break;
        }
        case Keys::f7:
        {
            HandleInputChar(cmsx::kernel::keyF7);
            args.handled = true;
            break;
        }
        case Keys::f8:
        {
            HandleInputChar(cmsx::kernel::keyF8);
            args.handled = true;
            break;
        }
        case Keys::f9:
        {
            HandleInputChar(cmsx::kernel::keyF9);
            args.handled = true;
            break;
        }
        case Keys::f10:
        {
            HandleInputChar(cmsx::kernel::keyF10);
            args.handled = true;
            break;
        }
        case Keys::f11:
        {
            HandleInputChar(cmsx::kernel::keyF11);
            args.handled = true;
            break;
        }
        case Keys::f12:
        {
            HandleInputChar(cmsx::kernel::keyF12);
            args.handled = true;
            break;
        }
        case Keys::delete_:
        {
            HandleInputChar(cmsx::kernel::keyDel);
            args.handled = true;
            break;
        }
        case Keys::insert:
        {
            HandleInputChar(cmsx::kernel::keyIns);
            args.handled = true;
            break;
        }
        case Keys::pageDown:
        {
            HandleInputChar(cmsx::kernel::keyPgDown);
            args.handled = true;
            break;
        }
        case Keys::pageUp:
        {
            HandleInputChar(cmsx::kernel::keyPgUp);
            args.handled = true;
            break;
        }
        case Keys::print:
        {
            HandleInputChar(cmsx::kernel::keyPrint);
            args.handled = true;
            break;
        }
        case Keys::end:
        {
            HandleInputChar(cmsx::kernel::keyEnd);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::delete_:
        {
            HandleInputChar(cmsx::kernel::keyShiftDel);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::end:
        {
            HandleInputChar(cmsx::kernel::keyShiftEnd);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::home:
        {
            HandleInputChar(cmsx::kernel::keyShiftHome);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::left:
        {
            HandleInputChar(cmsx::kernel::keyShiftLeft);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::right:
        {
            HandleInputChar(cmsx::kernel::keyShiftRight);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::up:
        {
            HandleInputChar(cmsx::kernel::keyShiftUp);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::down:
        {
            HandleInputChar(cmsx::kernel::keyShiftDown);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::up:
        {
            HandleInputChar(cmsx::kernel::keyControlUp);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::down:
        {
            HandleInputChar(cmsx::kernel::keyControlDown);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::left:
        {
            HandleInputChar(cmsx::kernel::keyControlLeft);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::right:
        {
            HandleInputChar(cmsx::kernel::keyControlRight);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::pageUp:
        {
            HandleInputChar(cmsx::kernel::keyControlPgUp);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::pageDown:
        {
            HandleInputChar(cmsx::kernel::keyControlPgDown);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::home:
        {
            HandleInputChar(cmsx::kernel::keyControlHome);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::end:
        {
            HandleInputChar(cmsx::kernel::keyControlEnd);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::delete_:
        {
            HandleInputChar(cmsx::kernel::keyControlDel);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f1:
        {
            HandleInputChar(cmsx::kernel::keyControlF1);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f2:
        {
            HandleInputChar(cmsx::kernel::keyControlF2);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f3:
        {
            HandleInputChar(cmsx::kernel::keyControlF3);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f4:
        {
            HandleInputChar(cmsx::kernel::keyControlF4);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f5:
        {
            HandleInputChar(cmsx::kernel::keyControlF5);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f6:
        {
            HandleInputChar(cmsx::kernel::keyControlF6);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f7:
        {
            HandleInputChar(cmsx::kernel::keyControlF7);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f8:
        {
            HandleInputChar(cmsx::kernel::keyControlF8);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f9:
        {
            HandleInputChar(cmsx::kernel::keyControlF9);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f10:
        {
            HandleInputChar(cmsx::kernel::keyControlF10);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f11:
        {
            HandleInputChar(cmsx::kernel::keyControlF11);
            args.handled = true;
            break;
        }
        case Keys::controlModifier | Keys::f12:
        {
            HandleInputChar(cmsx::kernel::keyControlF12);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::pageUp:
        {
            HandleInputChar(cmsx::kernel::keyShiftPgUp);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::pageDown:
        {
            HandleInputChar(cmsx::kernel::keyShiftPgDown);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f1:
        {
            HandleInputChar(cmsx::kernel::keyShiftF1);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f2:
        {
            HandleInputChar(cmsx::kernel::keyShiftF2);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f3:
        {
            HandleInputChar(cmsx::kernel::keyShiftF3);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f4:
        {
            HandleInputChar(cmsx::kernel::keyShiftF4);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f5:
        {
            HandleInputChar(cmsx::kernel::keyShiftF5);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f6:
        {
            HandleInputChar(cmsx::kernel::keyShiftF6);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f7:
        {
            HandleInputChar(cmsx::kernel::keyShiftF7);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f8:
        {
            HandleInputChar(cmsx::kernel::keyShiftF8);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f9:
        {
            HandleInputChar(cmsx::kernel::keyShiftF9);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f10:
        {
            HandleInputChar(cmsx::kernel::keyShiftF10);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f11:
        {
            HandleInputChar(cmsx::kernel::keyShiftF11);
            args.handled = true;
            break;
        }
        case Keys::shiftModifier | Keys::f12:
        {
            HandleInputChar(cmsx::kernel::keyShiftF12);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::a:
        {
            HandleInputChar(cmsx::kernel::keyAltA);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::b:
        {
            HandleInputChar(cmsx::kernel::keyAltB);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::c:
        {
            HandleInputChar(cmsx::kernel::keyAltC);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::d:
        {
            HandleInputChar(cmsx::kernel::keyAltD);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::e:
        {
            HandleInputChar(cmsx::kernel::keyAltE);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f:
        {
            HandleInputChar(cmsx::kernel::keyAltF);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::g:
        {
            HandleInputChar(cmsx::kernel::keyAltG);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::h:
        {
            HandleInputChar(cmsx::kernel::keyAltH);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::i:
        {
            HandleInputChar(cmsx::kernel::keyAltI);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::j:
        {
            HandleInputChar(cmsx::kernel::keyAltJ);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::k:
        {
            HandleInputChar(cmsx::kernel::keyAltK);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::l:
        {
            HandleInputChar(cmsx::kernel::keyAltL);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::m:
        {
            HandleInputChar(cmsx::kernel::keyAltM);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::n:
        {
            HandleInputChar(cmsx::kernel::keyAltN);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::o:
        {
            HandleInputChar(cmsx::kernel::keyAltO);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::p:
        {
            HandleInputChar(cmsx::kernel::keyAltP);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::q:
        {
            HandleInputChar(cmsx::kernel::keyAltQ);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::r:
        {
            HandleInputChar(cmsx::kernel::keyAltR);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::s:
        {
            HandleInputChar(cmsx::kernel::keyAltS);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::t:
        {
            HandleInputChar(cmsx::kernel::keyAltT);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::u:
        {
            HandleInputChar(cmsx::kernel::keyAltU);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::v:
        {
            HandleInputChar(cmsx::kernel::keyAltV);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::w:
        {
            HandleInputChar(cmsx::kernel::keyAltW);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::x:
        {
            HandleInputChar(cmsx::kernel::keyAltX);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::y:
        {
            HandleInputChar(cmsx::kernel::keyAltY);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::z:
        {
            HandleInputChar(cmsx::kernel::keyAltZ);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f1:
        {
            HandleInputChar(cmsx::kernel::keyAltF1);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f2:
        {
            HandleInputChar(cmsx::kernel::keyAltF2);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f3:
        {
            HandleInputChar(cmsx::kernel::keyAltF3);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f4:
        {
            HandleInputChar(cmsx::kernel::keyAltF4);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f5:
        {
            HandleInputChar(cmsx::kernel::keyAltF5);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f6:
        {
            HandleInputChar(cmsx::kernel::keyAltF6);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f7:
        {
            HandleInputChar(cmsx::kernel::keyAltF7);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f8:
        {
            HandleInputChar(cmsx::kernel::keyAltF8);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f9:
        {
            HandleInputChar(cmsx::kernel::keyAltF9);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f10:
        {
            HandleInputChar(cmsx::kernel::keyAltF10);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f11:
        {
            HandleInputChar(cmsx::kernel::keyAltF11);
            args.handled = true;
            break;
        }
        case Keys::altModifier | Keys::f12:
        {
            HandleInputChar(cmsx::kernel::keyAltF12);
            args.handled = true;
            break;
        }
    }
}

void TerminalControl::OnKeyPress(KeyPressEventArgs& args)
{
    HandleInputChar(args.keyChar);
    args.handled = true;
}

} // cmsx::guiterm
