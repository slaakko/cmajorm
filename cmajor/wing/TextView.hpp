// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_TEXT_VIEW_INCLUDED
#define CMAJOR_WING_TEXT_VIEW_INCLUDED
#include <cmajor/wing/Control.hpp>
#include <cmajor/wing/EditCommandList.hpp>

namespace cmajor { namespace wing {

using SelectionChangedEvent = Event;
using DirtyChangedEvent = Event;
using ReadOnlyChangedEvent = Event;
using CCDirtyChangedEvent = Event;
using CaretPosChangedEvent = Event;
using LinesChangedEvent = Event;
using CCTextChangedEvent = Event;
using EscapeEvent = Event;
using CCEvent = Event;
using CCNextEvent = Event;
using CCPrevEvent = Event;
using CCNextPageEvent = Event;
using CCPrevPageEvent = Event;
using CCSelectEvent = Event;
using GotoCaretLineEvent = Event;
using CopyEvent = Event;
using CutEvent = Event;
using PasteEvent = Event;

const int caretTimerId = 1;
const int defaultCaretTimerPeriod = 5000; // 5 seconds

WING_API Color DefaultSelectionBackgroundColor();

int ReadCaretTimeoutFromRegistry();

enum class TextViewFlags : int
{
    none = 0, changed = 1 << 0, painting = 1 << 1, readOnly = 1 << 2, fixed = 1 << 3, dirty = 1 << 4, ccdirty = 1 << 5, mouseExtendSelection = 1 << 6, ccOpen = 1 << 7
};

WING_API inline TextViewFlags operator&(TextViewFlags left, TextViewFlags right)
{
    return TextViewFlags(int(left) & int(right));
}

WING_API inline TextViewFlags operator|(TextViewFlags left, TextViewFlags right)
{
    return TextViewFlags(int(left) | int(right));
}

WING_API inline TextViewFlags operator~(TextViewFlags flags)
{
    return TextViewFlags(~int(flags));
}

struct WING_API SourcePos
{
    SourcePos();
    SourcePos(int line_, int column_);
    bool IsValid() const { return line != 0 && column != 0; }
    int line;
    int column;
};

WING_API inline bool operator==(const SourcePos& left, const SourcePos& right)
{
    return left.line == right.line && left.column == right.column;
}

WING_API inline bool operator!=(const SourcePos& left, const SourcePos& right)
{
    return !(left == right);
}

WING_API inline bool operator<(const SourcePos& left, const SourcePos& right)
{
    if (left.line < right.line) return true;
    if (left.line > right.line) return false;
    return left.column < right.column;
}

enum class SelectionFixed : int
{
    none, start, end
};

struct WING_API Selection
{
    Selection();
    bool IsEmpty() const { return !start.IsValid() && !end.IsValid(); }
    SelectionFixed fixed;
    SourcePos start;
    SourcePos end;
};

struct WING_API SelectionData
{
    SelectionData();
    SelectionData(const std::u32string& selectedText_, int indent_, int numTrailingSpaces_);
    std::u32string selectedText;
    int indent;
    int numTrailingSpaces;
};

struct WING_API LineEventArgs
{
    LineEventArgs(int lineIndex_, int indentLineIndex_);
    int lineIndex;
    int indentLineIndex;
};

WING_API bool IsWordSeparator(char32_t c);

using LineInsertedEvent = EventWithArgs<LineEventArgs>;
using LineChangedEvent = EventWithArgs<LineEventArgs>;
using LineDeletedEvent = EventWithArgs<LineEventArgs>;

struct WING_API TextViewCreateParams
{
    TextViewCreateParams();
    TextViewCreateParams& Defaults();
    TextViewCreateParams& WindowClassName(const std::string& windowClassName_);
    TextViewCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    TextViewCreateParams& WindowStyle(int windowStyle_);
    TextViewCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    TextViewCreateParams& BackgroundColor(const Color& backgroundColor_);
    TextViewCreateParams& Text(const std::string& text_);
    TextViewCreateParams& Location(const Point& location_);
    TextViewCreateParams& SetSize(const Size& size_);
    TextViewCreateParams& SetAnchors(Anchors anchors_);
    TextViewCreateParams& SetDock(Dock dock_);
    TextViewCreateParams& TextColor(const Color& textColor_);
    TextViewCreateParams& SelectionBackgroundColor(const Color& selectionBackgroundColor_);
    TextViewCreateParams& FontFamilyName(const std::string& fontFamilyName_);
    TextViewCreateParams& FontSize(float fontSize_);
    ControlCreateParams controlCreateParams;
    Color textColor;
    Color selectionBackgroundColor;
    std::string fontFamilyName;
    float fontSize;
};

class WING_API TextView : public Control
{
public:
    TextView(TextViewCreateParams& createParams);
    bool IsTextView() const override { return true; }
    bool Changed() const { return (flags & TextViewFlags::changed) != TextViewFlags::none; }
    void SetChanged() { flags = flags | TextViewFlags::changed; }
    void ResetChanged() { flags = flags & ~TextViewFlags::changed; }
    bool IsReadOnly() const { return (flags & TextViewFlags::readOnly) != TextViewFlags::none; }
    void SetReadOnly();
    void SetReadWrite();
    bool IsFixed() const { return (flags & TextViewFlags::fixed) != TextViewFlags::none; }
    void SetFixed() { flags = flags | TextViewFlags::fixed; }
    void ResetFixed() { flags = flags & ~TextViewFlags::fixed; }
    bool IsDirty() const { return (flags & TextViewFlags::dirty) != TextViewFlags::none; }
    void SetDirty();
    void ResetDirty();
    bool IsCCDirty() const { return (flags & TextViewFlags::ccdirty) != TextViewFlags::none; }
    void SetCCDirty();
    void ResetCCDirty();
    bool MouseExtendSelection() const { return (flags & TextViewFlags::mouseExtendSelection) != TextViewFlags::none; }
    void SetMouseExtendSelection() { flags = flags | TextViewFlags::mouseExtendSelection; }
    void ResetMouseExtendSelection() { flags = flags & ~TextViewFlags::mouseExtendSelection; }
    bool IsCCOpen() const { return (flags & TextViewFlags::ccOpen) != TextViewFlags::none; }
    void SetCCOpen() { flags = flags | TextViewFlags::ccOpen; }
    void ResetCCOpen() { flags = flags & ~TextViewFlags::ccOpen; }
    const std::vector<std::unique_ptr<std::u32string>>& Lines() const { return lines; }
    std::vector<std::unique_ptr<std::u32string>>& Lines() { return lines; }
    void AddLine(const std::u32string& line);
    int CaretLine() const { return caretLine; }
    int CaretColumn() const { return caretColumn - LineNumberFieldLength(); }
    void SetUndoRedoMenuItems(MenuItem* undoMenuItem, MenuItem* redoMenuItem);
    void Select();
    void Undo();
    void Redo();
    float CharWidth() const { return charWidth; }
    float CharHeight() const { return charHeight; }
    float TextWidth() const { return textWidth; }
    float TextHeight() const { return textHeight; }
    int TopLine() const { return topLine; }
    float TopLineDiff() const { return topLineDiff; }
    int LeftColumn() const { return leftCol; }
    float LeftColumnDiff() const { return leftColDiff; }
    int MaxLineLength() const { return maxLineLength; }
    int MaxLineIndex() const { return maxLineIndex; }
    void SetMaxLineLength();
    int IndentSize() const { return indentSize; }
    void SetIndentSize(int indentSize_) { indentSize = indentSize_; }
    const Padding& GetPadding() const { return padding; }
    void SetPadding(const Padding& padding_);
    void SetCaretLineCol(int line, int column);
    void SetTopLineCol(int line, int column);
    int GetVisibleLineCount() const;
    int GetVisibleColumnCount() const;
    bool IsLinePartiallyVisible(int line) const;
    bool IsLineVisible(int line) const;
    void GetLineColumn(const Point& contentLocation, int& line, int& column) const;
    void EnsureLineVisible(int line);
    virtual void Clear();
    void SetTextContent(const std::u32string& textContent);
    char32_t GetCharAt(int line, int col) const;
    bool Prev(int& line, int& col);
    bool Next(int& line, int& col);
    void PrevWord(int& line, int& col);
    void NextWord(int& line, int& col);
    void SetCaretPosByCharIndex(int charIndex);
    void ScrollToCaret();
    void SaveText();
    void SetTextExtent();
    Point CaretPos();
    Point CCPos();
    const std::u32string& GetCCTect() const { return cctext; }
    void SetCCText(const std::u32string& line, int columnIndex);
    void ResetCCText();
    void ReplaceCCText(const std::u32string& replacement);
    std::u32string GetCursorText() const;
    void InsertChar(int lineIndex, int columnIndex, char32_t c);
    void InsertText(int lineIndex, int columnIndex, const std::u32string& text);
    void InsertLines(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToInsert);
    void InsertIntoLine(int lineIndex, int columnIndex, const std::u32string& text);
    void RemoveFromLine(int lineIndex, int columnIndex, int count);
    void NewLine(int lineIndex, int columnIndex);
    void Tab(int lineIndex, int columnIndex);
    void Backtab(int lineIndex, int columnIndex);
    void AddSpaces(int lineIndex, int columnIndex, int numSpaces);
    void RemoveSpaces(int lineIndex, int columnIndex, int numSpaces);
    void DeleteChar(int lineIndex, int columnIndex, int indent, int numSpaces, bool removeIndent);
    void Backspace(int lineIndex, int columnIndex);
    void DeleteLines(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToDelete);
    void SetSelection(const Selection& selection_);
    void ResetSelection();
    bool IsSelectionEmpty() const;
    SelectionData GetSelection() const;
    void InsertSelection(const Selection& selectionToInsert, const SelectionData& selectionData, bool wholeLine);
    void RemoveSelection();
    void IndentSelection();
    void UnindentSelection();
    void ExtendSelection(const SourcePos& start, const SourcePos& end);
    void InvalidateLines(int startLineNumber, int endLineNumber);
    void InvalidateLineCol(int lineNumber, int columnNumber);
    int GetLineLength(int lineNumber) const;
    virtual int LineNumberFieldLength() const { return 0; }
    const std::string& FilePath() const { return filePath; }
    void SetFilePath(const std::string& filePath_);
    void FireGotoCaretLine();
    void FireCopy();
    void FireCut();
    void FirePaste();
    void AddRemoveSelectionCommand();
    SelectionChangedEvent& SelectionChanged() { return selectionChanged; }
    DirtyChangedEvent& DirtyChanged() { return dirtyChanged; }
    ReadOnlyChangedEvent& ReadOnlyChanged() { return readOnlyChanged; }
    CCDirtyChangedEvent& CCDirtyChanged() { return ccDirtyChanged; }
    CaretPosChangedEvent& CaretPosChanged() { return caretPosChanged; }
    LineInsertedEvent& LineInserted() { return lineInserted; }
    LineChangedEvent& LineChanged() { return lineChanged; }
    LineDeletedEvent& LineDeleted() { return lineDeleted; }
    CCEvent& CC() { return cc; }
    CCNextEvent& CCNext() { return ccNext; }
    CCPrevEvent& CCPrev() { return ccPrev; }
    CCNextPageEvent& CCNextPage() { return ccNextPage; }
    CCPrevPageEvent& CCPrevPage() { return ccPrevPage; }
    CCSelectEvent& CCSelect() { return ccSelect; }
    GotoCaretLineEvent& GotoCaretLine() { return gotoCaretLine; }
    CopyEvent& Copy() { return copy; }
    CutEvent& Cut() { return cut; }
    PasteEvent& Paste() { return paste; }
    const std::string& FontFamilyName() const { return fontFamilyName; }
    float FontSize() const { return fontSize; }
protected:
    void OnPaint(PaintEventArgs& args) override;
    void OnMouseDown(MouseEventArgs& args) override;
    void OnMouseMove(MouseEventArgs& args) override;
    void OnMouseUp(MouseEventArgs& args) override;
    void OnKeyDown(KeyEventArgs& args) override;
    void OnKeyPress(KeyPressEventArgs& args) override;
    void OnContentLocationChanged() override;
    virtual void OnEscape();
    virtual void OnCC();
    virtual void OnCCNext();
    virtual void OnCCPrev();
    virtual void OnCCNextPage();
    virtual void OnCCPrevPage();
    virtual void OnCCSelect();
    virtual void OnGotoCaretLine();
    virtual void OnCopy();
    virtual void OnCut();
    virtual void OnPaste();
    virtual void PaintContent(Graphics& graphics, const Rect& clipRect);
    virtual void DrawLine(Graphics& graphics, int lineIndex, const PointF& origin);
    virtual void Measure(Graphics& graphics);
    virtual void DrawSelectionBackground(Graphics& graphics, int line, const PointF& origin, int lineNumberFieldLength);
    SolidBrush* GetOrInsertBrush(const Color& color);
    void SetCursor() override;
    void SetCaretLocation() override;
    void CreateCaret() override;
    void SetContentLocationInternal(const Point& contentLocation) override;
    void OnTimer(TimerEventArgs& args) override;
    void OnLostFocus() override;
    void OnClick() override;
    void OnMouseDoubleClick(MouseEventArgs& args) override;
    virtual int GetIndent(const std::u32string& line, int lineIndex);
    virtual int RemoveIndent(int lineIndex) const;
    virtual void SetLineNumberFieldLength(int lineCount) {}
    virtual void OnSelectionChanged();
    virtual void OnDirtyChanged();
    virtual void OnReadOnlyChanged();
    virtual void OnCCDirtyChanged();
    virtual void OnLineInserted(LineEventArgs& args);
    virtual void OnLineChanged(LineEventArgs& args);
    virtual void OnLineDeleted(LineEventArgs& args);
    virtual void OnCaretPosChanged();
    virtual void OnLinesChanged();
    virtual void OnCCTextChanged();
private:
    void AddInsertCharCommand(int lineIndex, int columnIndex, char32_t c);
    void AddInsertLinesCommand(int lineIndex, int columnIndex, const std::vector<std::u32string>& linesToInsert);
    void AddInsertIntoLineCommand(int lineIndex, int columnIndex, const std::u32string& text);
    void AddNewLineCommand(int lineIndex, int columnIndex);
    void AddBacktabCommand(int lineIndex, int columnIndex);
    void AddTabCommand(int lineIndex, int columnIndex);
    void AddDeleteCharCommand(int lineIndex, int columnIndex);
    void AddIndentSelectionCommand();
    void AddUnindentSelectionCommand();
    void FixColumn(int& column, int line);
    TextViewFlags flags;
    Color textColor;
    Color selectionBackgroundColor;
    std::string fontFamilyName;
    float fontSize;
    Cursor cursor;
    int caretTimerPeriod;
    int caretColumn;
    int caretLine;
    float charWidth;
    float charHeight;
    float textWidth;
    float textHeight;
    int topLine;
    float topLineDiff;
    int leftCol;
    float leftColDiff;
    int maxLineLength;
    int maxLineIndex;
    int indentSize;
    Padding padding;
    Selection selection;
    std::vector<std::unique_ptr<std::u32string>> lines;
    std::string filePath;
    std::u32string cctext;
    EditCommandList editCommandList;
    std::vector<int> lineStartIndeces;
    std::vector<std::unique_ptr<SolidBrush>> brushes;
    std::vector<std::unique_ptr<Font>> fonts;
    StringFormat stringFormat;
    bool update;
    std::string measureString;
    SourcePos mouseSelectionStart;
    SourcePos mouseSelectionEnd;
    std::map<DWORD, SolidBrush*> brushMap;
    SelectionChangedEvent selectionChanged;
    DirtyChangedEvent dirtyChanged;
    ReadOnlyChangedEvent readOnlyChanged;
    CCDirtyChangedEvent ccDirtyChanged;
    CaretPosChangedEvent caretPosChanged;
    CCTextChangedEvent ccTextChanged;
    LinesChangedEvent linesChanged;
    LineInsertedEvent lineInserted;
    LineChangedEvent lineChanged;
    LineDeletedEvent lineDeleted;
    EscapeEvent escape;
    CCEvent cc;
    CCNextEvent ccNext;
    CCPrevEvent ccPrev;
    CCNextPageEvent ccNextPage;
    CCPrevPageEvent ccPrevPage;
    CCSelectEvent ccSelect;
    GotoCaretLineEvent gotoCaretLine;
    CopyEvent copy;
    CutEvent cut;
    PasteEvent paste;
};

WING_API std::u32string TrimEnd(const std::u32string& line);
WING_API int GetNumTrailingSpaces(const std::u32string& line);
WING_API std::vector<std::u32string> SplitTextIntoLines(const std::u32string& text);
WING_API int MinIndent(const std::u32string& s);
WING_API std::u32string Unindent(const std::u32string& s, int indent);
WING_API std::vector<int> CalculateLineStartIndeces(const std::u32string& text);

} } // cmajor::wing

#endif // CMAJOR_WING_TEXT_VIEW_INCLUDED
