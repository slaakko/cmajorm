// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Console.hpp>
#include <wing/PaddedControl.hpp>

namespace cmajor { namespace wing {

ConsoleColor DefaultConsoleBackColor()
{
    return ConsoleColor::black;
}

ConsoleColor DefaultConsoleTextColor()
{
    return ConsoleColor::gray;
}

ColorCount::ColorCount() : color(ConsoleColor::black), count(0)
{
}

ColorCount::ColorCount(ConsoleColor color_, int count_) : color(color_), count(count_)
{
}
    
ConsoleCreateParams::ConsoleCreateParams() : textViewCreateParams(), padding(DefaultPadding()), defaultBackColor(DefaultConsoleBackColor()), defaultTextColor(DefaultConsoleTextColor())
{
    textViewCreateParams.WindowClassName("wing.Console");
    textViewCreateParams.BackgroundColor(GetColor(defaultBackColor));
    textViewCreateParams.FontFamilyName("Cascadia Code");
    textViewCreateParams.FontSize(12.0f);
}

ConsoleCreateParams& ConsoleCreateParams::Defaults()
{
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::WindowClassName(const std::string& windowClassName_)
{
    textViewCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    textViewCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::WindowStyle(int windowStyle_)
{
    textViewCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    textViewCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    textViewCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::Text(const std::string& text_)
{
    textViewCreateParams.Text(text_);
    return *this;
}
ConsoleCreateParams& ConsoleCreateParams::Location(const Point& location_)
{
    textViewCreateParams.Location(location_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::SetSize(const Size& size_)
{
    textViewCreateParams.SetSize(size_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::SetAnchors(Anchors anchors_)
{
    textViewCreateParams.SetAnchors(anchors_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::SetDock(Dock dock_)
{
    textViewCreateParams.SetDock(dock_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::FontFamilyName(const std::string& fontFamilyName_)
{
    textViewCreateParams.FontFamilyName(fontFamilyName_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::FontSize(float fontSize_)
{
    textViewCreateParams.FontSize(fontSize_);
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::SetPadding(const Padding& padding_)
{
    padding = padding_;
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::DefaultBackColor(ConsoleColor color)
{
    defaultBackColor = color;
    return *this;
}

ConsoleCreateParams& ConsoleCreateParams::DefaultTextColor(ConsoleColor color)
{
    defaultTextColor = color;
    return *this;
}

Console::Console(ConsoleCreateParams& createParams) : 
    TextView(createParams.textViewCreateParams), 
    outEngine(1), 
    errorEngine(2), 
    padding(createParams.padding), 
    defaultBackColor(createParams.defaultBackColor), 
    defaultTextColor(createParams.defaultTextColor),
    eof(false),
    startInputCol(1)
{
    SetReadOnly();
    SetPadding(padding);
    ColorCharOutputMethod outputMethod;
    outputMethod.Set(this, &Console::OutputChar);
    outEngine.SetColorCharOutputMethod(outputMethod);
    errorEngine.SetColorCharOutputMethod(outputMethod);
}

void Console::Write(int handle, const std::string& text)
{
    if (text.empty()) return;
    switch (handle)
    {
        case 1:
        {
            outEngine.Process(text);
            break;
        }
        case 2:
        {
            errorEngine.Process(text);
            break;
        }
        default:
        {
            throw std::runtime_error("console: invalid handle value " + std::to_string(handle) + " (must be 1 or 2)");
        }
    }
    if (!Lines().empty())
    {
        int line = Lines().size();
        int col = Lines().back()->length() + 1;
        SetCaretLineCol(line, col);
        SetTextExtent();
        ScrollToCaret();
        SetChanged();
        Invalidate();
    }
}

void Console::StartReadLine()
{
    inputLine.clear();
    eof = false;
    startInputCol = CaretColumn();
    SetFocus();
}

void Console::Clear()
{
    TextView::Clear();
    textColorLines.clear();
    backColorLines.clear();
}

void Console::PaintContent(Graphics& graphics, const Rect& clipRect)
{
    graphics.Clear(BackgroundColor());
    Gdiplus::TextRenderingHint prevTextRenderingHint = graphics.GetTextRenderingHint();
    graphics.SetTextRenderingHint(Gdiplus::TextRenderingHint::TextRenderingHintClearTypeGridFit);
    if (Changed())
    {
        ResetChanged();
        SetMaxLineLength();
        Measure(graphics);
    }
    PointF origin(padding.left, padding.top);
    int n = Lines().size();
    for (int i = 0; i < n; ++i)
    {
        if (IsLinePartiallyVisible(i + 1))
        {
            const std::vector<ColorCount>& backColorLine = backColorLines[i];
            PaintLineBackground(graphics, backColorLine, origin);
            const std::vector<ColorCount>& textColorLine = textColorLines[i];
            const std::u32string& line = *Lines()[i];
            DrawLineText(graphics, line, textColorLine, origin);
        }
        origin.Y = origin.Y + CharHeight();
    }
    graphics.SetTextRenderingHint(prevTextRenderingHint);
}

void Console::OnKeyDown(KeyEventArgs& args)
{
    int caretCol = CaretColumn();
    int caretLine = CaretLine();
    switch (args.key)
    {
        case Keys::controlModifier | Keys::z:
        {
            eof = true;
            Write(1, "^Z\n");
            OnConsoleInputReady();
            args.handled = true;
            break;
        }
        case Keys::enter:
        {
            eof = false;
            Write(1, "\n");
            OnConsoleInputReady();
            args.handled = true;
            break;
        }
        case Keys::delete_:
        {
            int index = caretCol - startInputCol;
            if (index < inputLine.length())
            {
                DeleteChar();
            }
            args.handled = true;
            break;
        }
        case Keys::back:
        {
            if (caretCol > startInputCol)
            {
                --caretCol;
                SetCaretLineCol(caretLine, caretCol);
                DeleteChar();
            }
            args.handled = true;
            break;
        }
        case Keys::left:
        {
            if (caretCol > startInputCol)
            {
                --caretCol;
                SetCaretLineCol(caretLine, caretCol);
                Invalidate();
            }
            args.handled = true;
            break;
        }
        case Keys::right:
        {
            int index = caretCol - startInputCol;
            if (index < inputLine.length())
            {
                ++caretCol;
                SetCaretLineCol(caretLine, caretCol);
                Invalidate();
            }
            args.handled = true;
            break;
        }
        case Keys::home:
        {
            caretCol = startInputCol;
            SetCaretLineCol(caretLine, caretCol);
            Invalidate();
            args.handled = true;
            break;
        }
        case Keys::end:
        {
            caretCol = startInputCol + inputLine.length();
            SetCaretLineCol(caretLine, caretCol);
            Invalidate();
            args.handled = true;
            break;
        }
    }
}

void Console::OnKeyPress(KeyPressEventArgs& args)
{
    if (eof) return;
    TextView::OnKeyPress(args);
    if (!args.handled)
    {
        char32_t ch = args.keyChar;
        InsertChar(ch);
        args.handled = true;
    }
}

void Console::OnConsoleInputReady()
{
    consoleInputReady.Fire();
}

void Console::OutputChar(ConsoleColor textColor, ConsoleColor backColor, int handle, char32_t c)
{
    if (Lines().empty() || c == '\n')
    {
        AddLine(std::u32string());
        std::vector<ColorCount> colorLine;
        textColorLines.push_back(colorLine);
        backColorLines.push_back(colorLine);
    }
    if (c != '\n')
    {
        std::vector<std::unique_ptr<std::u32string>>& lines = Lines();
        std::unique_ptr<std::u32string>& lastLine = lines.back();
        lastLine->append(1, c);
        if (lastLine->length() > MaxLineLength())
        {
            SetMaxLineLength();
        }
        AddColor(textColor, textColorLines.back());
        AddColor(backColor, backColorLines.back());
    }
}

void Console::AddColor(ConsoleColor color, std::vector<ColorCount>& colorLine)
{
    if (colorLine.empty() || color != colorLine.back().color)
    {
        ColorCount colorCount(color, 1);
        colorLine.push_back(colorCount);
    }
    else
    {
        colorLine.back().IncrementCount();
    }
}

void Console::PaintLineBackground(Graphics& graphics, const std::vector<ColorCount>& backColorLine, const PointF& origin)
{
    int n = backColorLine.size();
    if (n == 1 && backColorLine[0].color == defaultBackColor) return;
    PointF loc(origin);
    for (int i = 0; i < n; ++i)
    {
        const ColorCount& colorCount = backColorLine[i];
        SizeF size(colorCount.count * CharWidth(), CharHeight());
        if (colorCount.color != defaultBackColor)
        {
            SolidBrush* brush = GetOrInsertBrush(colorCount.color);
            RectF rect(loc, size);
            CheckGraphicsStatus(graphics.FillRectangle(brush, rect));
        }
        loc.X = loc.X + size.Width;
    }
}

void Console::DrawLineText(Graphics& graphics, const std::u32string& line, const std::vector<ColorCount>& textColorLine, const PointF& origin)
{
    PointF loc(origin);
    int start = 0;
    int n = textColorLine.size();
    for (int i = 0; i < n; ++i)
    {
        const ColorCount& colorCount = textColorLine[i];
        SolidBrush* brush = GetOrInsertBrush(colorCount.color);
        int length = colorCount.count;
        std::u32string text = line.substr(start, length);
        DrawString(graphics, ToUtf8(text), GetFont(), loc, *brush);
        loc.X = loc.X + length * CharWidth();
        start = start + length;
    }
}

void Console::InsertChar(char32_t ch)
{
    int caretCol = CaretColumn();
    int caretLine = CaretLine();
    int index = caretCol - startInputCol;
    if (index < inputLine.length())
    {
        inputLine = inputLine.substr(0, index) + std::u32string(1, ch) + inputLine.substr(index);
    }
    else
    {
        inputLine.append(1, ch);
    }
    std::vector<std::unique_ptr<std::u32string>>& lines = Lines();
    std::u32string line = *lines[caretLine - 1];
    if (caretCol < line.length())
    {
        line = line.substr(0, caretCol - 1) + std::u32string(1, ch) + line.substr(caretCol - 1);
    }
    else
    {
        line.append(1, ch);
    }
    *lines[caretLine - 1] = line;
    IncrementCaretColorCount();
    ++caretCol;
    SetCaretLineCol(caretLine, caretCol);
    SetTextExtent();
    Invalidate();
}

void Console::DeleteChar()
{
    int caretCol = CaretColumn();
    int caretLine = CaretLine();
    int index = caretCol - startInputCol;
    inputLine = inputLine.substr(0, index) + inputLine.substr(index + 1);
    std::vector<std::unique_ptr<std::u32string>>& lines = Lines();
    std::u32string line = *lines[caretLine - 1];
    line = line.substr(0, caretCol - 1) + line.substr(caretCol);
    *lines[caretLine - 1] = line;
    DecrementCaretColorCount();
    Invalidate();
}

void Console::IncrementCaretColorCount()
{
    int caretCol = CaretColumn();
    int caretLine = CaretLine();
    std::vector<ColorCount>& textColorLine = textColorLines[caretLine - 1];
    IncrementColorCount(textColorLine, caretCol);
    std::vector<ColorCount>& backColorLine = backColorLines[caretLine - 1];
    IncrementColorCount(backColorLine, caretCol);
}

void Console::IncrementColorCount(std::vector<ColorCount>& colorLine, int caretCol)
{
    int count = 0;
    bool incremented = false;
    for (ColorCount& colorCount : colorLine)
    {
        if (caretCol >= count && caretCol < count + colorCount.count)
        {
            colorCount.IncrementCount();
            incremented = true;
            break;
        }
        count = count + colorCount.count;
    }
    if (!incremented)
    {
        colorLine.back().IncrementCount();
    }
}

void Console::DecrementCaretColorCount()
{
    int caretCol = CaretColumn();
    int caretLine = CaretLine();
    std::vector<ColorCount>& textColorLine = textColorLines[caretLine - 1];
    DecrementColorCount(textColorLine, caretCol);
    std::vector<ColorCount>& backColorLine = backColorLines[caretLine - 1];
    DecrementColorCount(backColorLine, caretCol);
}

void Console::DecrementColorCount(std::vector<ColorCount>& colorLine, int caretCol)
{
    int count = 0;
    bool decremented = false;
    for (ColorCount& colorCount : colorLine)
    {
        if (caretCol >= count && caretCol < count + colorCount.count)
        {
            colorCount.DecrementCount();
            decremented = true;
            break;
        }
        count = count + colorCount.count;
    }
    if (!decremented)
    {
        colorLine.back().DecrementCount();
    }
}

SolidBrush* Console::GetOrInsertBrush(ConsoleColor color)
{
    auto it = brushMap.find(color);
    if (it != brushMap.cend())
    {
        return it->second;
    }
    else
    {
        SolidBrush* brush = new SolidBrush(GetColor(color));
        brushes.push_back(std::unique_ptr<SolidBrush>(brush));
        brushMap[color] = brush;
        return brush;
    }
}

} } // cmajor::wing
