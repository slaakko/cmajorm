// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_DB_DATA_VIEW_INCLUDED
#define CMSX_DB_DATA_VIEW_INCLUDED
#include <system-x/db/DebugView.hpp>
#include <wing/Control.hpp>

namespace cmsx::db {

using namespace cmajor::wing;

class DataRange
{
public:
    DataRange();
    virtual ~DataRange();
    cmsx::machine::Machine* GetMachine() const { return machine; }
    void SetMachine(cmsx::machine::Machine* machine_);
    cmsx::kernel::Process* GetProcess() const { return process; }
    void SetProcess(cmsx::kernel::Process* process_);
    virtual std::string Name() const = 0;
    virtual int64_t Start() const = 0;
    virtual int64_t Length() const = 0;
private:
    cmsx::machine::Machine* machine;
    cmsx::kernel::Process* process;
};

class DataSegmentDataRange : public DataRange
{
public:
    std::string Name() const override { return "Data"; }
    int64_t Start() const override;
    int64_t Length() const override;
};

class ArgumentsDataRange : public DataRange
{
public:
    std::string Name() const override { return "Arguments"; }
    int64_t Start() const override;
    int64_t Length() const override;
};

class EnvironmentDataRange : public DataRange
{
public:
    std::string Name() const override { return "Environment"; }
    int64_t Start() const override;
    int64_t Length() const override;
};

class HeapDataRange : public DataRange
{
public:
    std::string Name() const override { return "Heap"; }
    int64_t Start() const override;
    int64_t Length() const override;
};

class StackDataRange : public DataRange
{
public:
    std::string Name() const override { return "Stack"; }
    int64_t Start() const override;
    int64_t Length() const override;
};

class DataRanges
{
public:
    DataRanges();
    const std::vector<std::unique_ptr<DataRange>>& Ranges() const { return ranges; }
    DataRange* GetDataRange(const std::string& rangeName) const;
    void SetMachine(cmsx::machine::Machine* machine);
    void SetProcess(cmsx::kernel::Process* process);
private:
    std::vector<std::unique_ptr<DataRange>> ranges;
    std::map<std::string, DataRange*> rangeMap;
};

std::string DefaultDataViewFontFamilyName();
float DefaultDataViewFontSize();
Color DefaultDataViewAddressColor();
Color DefaultDataViewOctaColor();
Color DefaultDataViewByteColor();
Color DefaultDataViewCharColor();

struct DataViewCreateParams
{
    DataViewCreateParams();
    DataViewCreateParams& Defaults();
    ControlCreateParams controlCreateParams;
    std::string fontFamilyName;
    float fontSize;
    Color addressColor; 
    Color octaColor;
    Color byteColor;
    Color charColor;
};

class DataView : public Control, public DebugView
{
public:
    DataView(DataViewCreateParams& createParams, DataRange* dataRange_);
    void SetMachine(cmsx::machine::Machine* machine_) override;
    void SetProcess(cmsx::kernel::Process* process_) override;
    void UpdateView(bool updateCurrentAddress) override;
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
    void Paint(Graphics& graphics);
    void PrintValue(const PointF& origin, Graphics& graphics, uint64_t address, uint64_t value, 
        const SolidBrush& addressBrush, const SolidBrush& octaBrush, const SolidBrush& byteBrush, const SolidBrush& charBrush);
    DataRange* dataRange;
    cmajor::wing::StringFormat stringFormat;
    SolidBrush addressBrush;
    SolidBrush octaBrush;
    SolidBrush byteBrush;
    SolidBrush charBrush;
    float lineHeight;
    float charWidth;
    int numLines;
    int64_t currentAddress;
};

} // namespace cmsx::db

#endif // CMSX_DB_DATA_VIEW_INCLUDED
