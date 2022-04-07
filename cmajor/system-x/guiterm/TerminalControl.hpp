// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_GUITERM_TERMINAL_CONTROL_INCLUDED
#define CMSX_GUITERM_TERMINAL_CONTROL_INCLUDED
#include <system-x/guiterm/Api.hpp>
#include <wing/Control.hpp>
#include <wing/Color.hpp>
#include <stack>

namespace cmsx::guiterm {

using namespace cmajor::wing;

struct CMSX_GUITERM_API TerminalControlCreateParams
{
    TerminalControlCreateParams();
    TerminalControlCreateParams& Defaults();
    TerminalControlCreateParams& WindowClassName(const std::string& windowClassName_);
    TerminalControlCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    TerminalControlCreateParams& WindowStyle(int windowStyle_);
    TerminalControlCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    TerminalControlCreateParams& BackgroundColor(const Color& backgroundColor_);
    TerminalControlCreateParams& Text(const std::string& text_);
    TerminalControlCreateParams& Location(const Point& location_);
    TerminalControlCreateParams& SetSize(const Size& size_);
    TerminalControlCreateParams& SetAnchors(Anchors anchors_);
    TerminalControlCreateParams& SetDock(Dock dock_);
    TerminalControlCreateParams& TextColor(const Color& textColor_);
    TerminalControlCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    TerminalControlCreateParams& FontSize(float fontSize_);
    ControlCreateParams controlCreateParams;
    Color textColor;
    std::string fontFamilyName;
    float fontSize;
};

struct ColorChar
{
    ColorChar() : ch(' '), foreColor(), backColor() {}
    ColorChar(char32_t ch_, ConsoleColor foreColor_, ConsoleColor backColor_) : ch(ch_), foreColor(foreColor_), backColor(backColor_) {}
    char32_t ch;
    ConsoleColor foreColor;
    ConsoleColor backColor;
};

enum class InputMode
{
    cooked = 0, raw = 1
};

class CMSX_GUITERM_API TerminalControl : public cmajor::wing::Control
{
public:
    TerminalControl(TerminalControlCreateParams& createParams);
    void SetColors(ConsoleColor foreColor_, ConsoleColor backColor_);
    void SetSize(int32_t rows_, int32_t cols_);
    int Rows() const { return rows; }
    int Cols() const { return cols; }
    void SetCursorPos(int32_t cursorX_, int32_t cursorY_);
    void SetOutputEndCursorPos(int32_t x, int32_t y);
    int32_t CursorX() const { return cursorX; }
    int32_t CursorY() const { return cursorY; }
    ConsoleColor DefaultForeColor() const { return defaultForeColor; }
    ConsoleColor DefaultBackColor() const { return defaultBackColor; }
    void Write(const std::u32string& text);
    void ClearScreen();
    void Activate();
    bool IsActive() const { return active; }
    void HandleKeyDown(KeyEventArgs& args);
    void HandleKeyPress(KeyPressEventArgs& args);
    void SetCooked();
    void SetRaw();
    void SetEcho(bool echo_);
    std::vector<uint8_t> ReadInput(int64_t count);
    void ScrollToCursor();
    void ScrollToLine(int lineIndex);
    void PushLines();
    void PopLines();
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnKeyDown(KeyEventArgs& args) override;
    void OnKeyPress(KeyPressEventArgs& args) override;
private:
    void Measure(cmajor::wing::Graphics& graphics);
    void Scroll();
    SolidBrush* GetOrInsertSolidBrush(ConsoleColor color);
    void OutputLine(Graphics& graphics, PointF& origin, std::u32string& line);
    void HandleInputChar(char32_t ch);
    void HandleEof();
    void PrintChar(char32_t ch);
    void PrintNewLine();
    void HandleChar(char32_t ch);
    void HandleNewLine();
    void HandleBackspace();
    void HandleTab();
    void HandleEscape();
    void HandleHome();
    void HandleEnd();
    void HandleLeft();
    void HandleRight();
    void HandleUp();
    void HandleDown();
    void HandleControlLeft();
    void HandleControlRight();
    void HandleDel();
    void ClearUpdateLine();
    void UpdateLine();
    void UpdateCursorPos();
    StringFormat stringFormat;
    std::map<ConsoleColor, SolidBrush*> brushMap;
    std::vector<std::unique_ptr<SolidBrush>> brushes;
    ConsoleColor foreColor;
    ConsoleColor backColor;
    ConsoleColor defaultForeColor;
    ConsoleColor defaultBackColor;
    SolidBrush* foreBrush;
    SolidBrush* backBrush;
    int32_t rows;
    int32_t cols;
    int32_t cursorX;
    int32_t cursorY;
    int32_t outputEndCursorPosX;
    int32_t outputEndCursorPosY;
    bool changed;
    bool active;
    std::vector<std::vector<ColorChar>> screen;
    float charWidth;
    float charHeight;
    InputMode inputMode;
    bool echo;
    bool eof;
    std::list<uint8_t> terminalInputBuffer;
    std::vector<std::u32string> lines;
    std::stack<std::vector<std::u32string>> linesStack;
    std::u32string line;
    int32_t lineIndex;
    int pos;
};

} // cmsx::guiterm

#endif // CMSX_GUITERM_TERMINAL_CONTROL_INCLUDED
