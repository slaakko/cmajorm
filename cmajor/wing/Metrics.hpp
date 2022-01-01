// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_METRICS_INCLUDED
#define CMAJOR_WING_METRICS_INCLUDED
#include <wing/Graphics.hpp>
#include <sngxml/dom/Element.hpp>

namespace cmajor { namespace wing {

const float inchMM = 25.4f;
const float pointMM = 0.351450f;

WING_API inline float InchToMM(float inches)
{
    return inches * inchMM;
}

WING_API inline float MMToInch(float mm)
{
    return mm / inchMM;
}

WING_API inline float PointToMM(float points)
{
    return points * pointMM;
}

WING_API inline float MMToPoint(float mm)
{
    return mm / pointMM;
}

WING_API inline int MMToPixels(float mm, float dpi)
{
    return static_cast<int>(mm * dpi / inchMM);
}

WING_API inline float PixelsToMM(int pixels, float dpi)
{
    return pixels * inchMM / dpi;
}

WING_API std::string GetDefaultMetricsFilePath();

class WING_API SizeElement
{
public:
    SizeElement(const std::string& name_);
    const std::string& Name() const { return name; }
    const SizeF& Get() const { return size; }
    void Set(const SizeF& size_) { size = size_; }
    void Read(sngxml::dom::Element* parentElement);
    void Write(sngxml::dom::Element* parentElement);
private:
    std::string name;
    SizeF size;
};

class WING_API Metrics
{
public:
    Metrics();
    void SetElementValues();
    void LoadFromFile(const std::string& fileName);
    void SaveToFile(const std::string& fileName, bool setElementValues);
    void Calculate(Graphics& graphics);
    float DPIX() const { return dpiX; }
    float DPIY() const { return dpiY; }
    int MMToHorizontalPixels(float mm) const { return MMToPixels(mm, dpiX); }
    int HorizontalPixelsToMM(int pixels) const { return PixelsToMM(pixels, dpiX); }
    int MMToVerticalPixels(float mm) const { return MMToPixels(mm, dpiY); }
    int VerticalPixelsToMM(int pixels) const { return PixelsToMM(pixels, dpiY); }
    const Size& DefaultButtonSize() const { return defaultButtonSize; }
    void SetDefaultButtonSize(const Size& defaultButtonSize_) { defaultButtonSize = defaultButtonSize_; }
    const Size& DefaultLabelSize() const { return defaultLabelSize; }
    void SetDefaultLabelSize(const Size& defaultLabelSize_) { defaultLabelSize = defaultLabelSize_; }
    const Size& DefaultTextBoxSize() const { return defaultTextBoxSize; }
    void SetDefaultTextBoxSize(const Size& defaultTextBoxSize_) { defaultTextBoxSize = defaultTextBoxSize_; }
    const Size& DefaultListBoxSize() const { return defaultListBoxSize; }
    void SetDefaultListBoxSize(const Size& defaultListBoxSize_) { defaultListBoxSize = defaultListBoxSize_; }
    const Size& DefaultComboBoxSize() const { return defaultComboBoxSize; }
    void SetDefaultComboBoxSize(const Size& defaultComboBoxSize_) { defaultComboBoxSize = defaultComboBoxSize_; }
    const Size& DefaultCheckBoxSize() const { return defaultCheckBoxSize; }
    void SetDefaultCheckBoxSize(const Size& defaultCheckBoxSize_) { defaultCheckBoxSize = defaultCheckBoxSize_; }
    const Size& DefaultControlSpacing() const { return defaultControlSpacing; }
    void SetDefaultControlSpacing(const Size& defaultControlSpacing_) { defaultControlSpacing = defaultControlSpacing_; }
private:
    void Read(sngxml::dom::Element* parentElement);
    void Write(sngxml::dom::Element* parentElement);
    float dpiX;
    float dpiY;
    Size defaultButtonSize;
    SizeElement defaultButtonSizeElement;
    Size defaultLabelSize;
    SizeElement defaultLabelSizeElement;
    Size defaultTextBoxSize;
    SizeElement defaultTextBoxSizeElement;
    Size defaultListBoxSize;
    SizeElement defaultListBoxSizeElement;
    Size defaultComboBoxSize;
    SizeElement defaultComboBoxSizeElement;
    Size defaultCheckBoxSize;
    SizeElement defaultCheckBoxSizeElement;
    Size defaultControlSpacing;
    SizeElement defaultControlSpacingElement;
};

class WING_API ScreenMetrics
{
public:
    static void Load();
    static void Save(bool setElementValues);
    static void Calculate(Graphics& graphics);
    static Metrics& Get() { return metrics; }
    static void Set(Metrics& metrics_);
    static bool Calculated() { return calculated; }
    static void SetCalculated() { calculated = true; }
private:
    static Metrics metrics;
    static bool calculated;
};

WING_API void LoadMetrics();

} } // cmajor::wing

#endif // CMAJOR_WING_METRICS_INCLUDED
