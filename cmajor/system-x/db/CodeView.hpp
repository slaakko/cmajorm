// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_CODE_VIEW_INCLUDED
#define CMSX_DB_CODE_VIEW_INCLUDED
#include <system-x/db/DebugView.hpp>
#include <wing/Control.hpp>
#include <stack>

namespace cmsx::db {

using namespace cmajor::wing;

enum class CodeViewItemKind
{
    selection_background, pc_background, space, currentAddress, assembledByte, label, opCode, number, reg, punctuation, undefined, address
};

struct CodeViewItem
{
    CodeViewItem(CodeViewItemKind kind_, const std::u32string& text_);
    CodeViewItemKind kind;
    std::u32string text;
};

class CodeViewLine
{
public:
    CodeViewLine();
    void AddItem(CodeViewItem&& item);
    const std::vector<CodeViewItem>& Items() const { return items; }
private:
    std::vector<CodeViewItem> items;
};

std::string DefaultCodeViewFontFamilyName();
float DefaultCodeViewFontSize();
Color DefaultSelectLineBackgroundColor();
Color DefaultCurrentAddressColor();
Color DefaultPCColor();
Color DefaultAssembledByteColor();
Color DefaultLabelColor();
Color DefaultOpCodeColor();
Color DefaultNumberColor();
Color DefaultRegisterColor();
Color DefaultPunctuationColor();
Color DefaultUndefinedColor();
Color DefaultAddressColor();

struct CodeViewCreateParams
{
    CodeViewCreateParams();
    CodeViewCreateParams& Defaults();
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color selectLineBackgroundColor;
};

class CodeView : public Control, public DebugView
{
public:
    CodeView(CodeViewCreateParams& createParams);
    void Reset();
    void SetMachine(cmsx::machine::Machine* machine_) override;
    void SetProcess(cmsx::os::Process* process_) override;
    void UpdateView() override;
    void NextLine() override;
    void PrevLine() override;
    void NextQuarter() override;
    void PrevQuarter() override;
    void NextPage() override;
    void PrevPage() override;
    void ToStart() override;
    void ToEnd() override;
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void Measure(Graphics& graphics);
    void FetchAddressRange(int64_t startAddress, int64_t byteCount);
    void PaintLines(Graphics& graphics, int64_t pc);
    void PaintLine(Graphics& graphics, const PointF& origin, const CodeViewLine& line, int64_t address, int64_t curAddr, int64_t pc);
    Brush& GetItemBrush(CodeViewItemKind itemKind);
    int64_t currentAddress;
    std::stack<int64_t> currentAddressStack;
    cmajor::wing::StringFormat stringFormat;
    float lineHeight;
    float charWidth;
    float viewHeight;
    int numLines;
    std::map<int64_t, CodeViewLine> lineMap;
    std::map <int64_t, int64_t> setAddressMap;
    cmsx::machine::Machine* machine;
    cmsx::os::Process* process;
    std::map<CodeViewItemKind, SolidBrush*> brushMap;
    std::map<CodeViewItemKind, Color> colorMap;
    std::vector<std::unique_ptr<SolidBrush>> brushes;
};

} // namespace cmsx::db

#endif // CMSX_DB_CODE_VIEW_INCLUDED