// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_PROGRESS_BAR_INCLUDED
#define CMAJOR_WING_PROGRESS_BAR_INCLUDED
#include <wing/Control.hpp>
#include <wing/Container.hpp>

namespace cmajor { namespace wing {

WING_API Color DefaultProgressBarBackgroundColor();
WING_API Color DefaultProgressBarProgressColor();
WING_API Color DefaultProgressBarSunkenBorderOuterTopLeftColor();
WING_API Color DefaultProgressBarSunkenBorderInnerTopLeftColor();
WING_API Color DefaultProgressBarSunkenBorderOuterRightBottomColor();
WING_API Color DefaultProgressBarSunkenBorderInnerRightBottomColor();

class ProgressBar;

class WING_API ProgressBarBrushesAndPens
{
public:
    ProgressBarBrushesAndPens(ProgressBar* progressBar_);
    ProgressBarBrushesAndPens(const ProgressBarBrushesAndPens&) = delete;
    ProgressBarBrushesAndPens(ProgressBarBrushesAndPens&&) = delete;
    ProgressBarBrushesAndPens& operator=(const ProgressBarBrushesAndPens&) = delete;
    ProgressBarBrushesAndPens& operator=(ProgressBarBrushesAndPens&&) = delete;
    Brush* ProgressBrush();
    Brush* BackgroundBrush();
    Pen* SunkenBorderOuterTopLeftPen();
    Pen* SunkenBorderInnerTopLeftPen();
    Pen* SunkenBorderOuterRightBottomPen();
    Pen* SunkenBorderInnerRightBottomPen();
private:
    ProgressBar* progressBar;
    std::vector<std::unique_ptr<Brush>> brushes;
    Brush* progressBrush;
    Brush* backgroundBrush;
    std::vector<std::unique_ptr<Pen>> pens;
    Pen* sunkenBorderOuterTopLeftPen;
    Pen* sunkenBorderInnerTopLeftPen;
    Pen* sunkenBorderOuterRightBottomPen;
    Pen* sunkenBorderInnerRightBottomPen;
};

struct WING_API ProgressBarCreateParams
{
    ProgressBarCreateParams();
    ProgressBarCreateParams& Defaults();
    ProgressBarCreateParams& WindowClassName(const std::string& windowClassName_);
    ProgressBarCreateParams& WindowClassStyle(uint32_t windowClassStyle_);
    ProgressBarCreateParams& WindowStyle(int windowStyle_);
    ProgressBarCreateParams& WindowClassBackgroundColor(int windowClassBackgroundColor_);
    ProgressBarCreateParams& BackgroundColor(const Color& backgroundColor_);
    ProgressBarCreateParams& Text(const std::string& text_);
    ProgressBarCreateParams& Location(Point location_);
    ProgressBarCreateParams& SetSize(Size size_);
    ProgressBarCreateParams& SetAnchors(Anchors anchors_);
    ProgressBarCreateParams& SetDock(Dock dock_);
    ProgressBarCreateParams& ProgressColor(const Color& color);
    ProgressBarCreateParams& SunkenBorderOuterTopLeftColor(const Color& color);
    ProgressBarCreateParams& SunkenBorderInnerTopLeftColor(const Color& color);
    ProgressBarCreateParams& SunkenBorderOuterRightBottomColor(const Color& color);
    ProgressBarCreateParams& SunkenBorderInnerRightBottomColor(const Color& color);
    ControlCreateParams controlCreateParams;
    Color progressColor;
    Color sunkenBorderOuterTopLeftColor;
    Color sunkenBorderInnerTopLeftColor;
    Color sunkenBorderOuterRightBottomColor;
    Color sunkenBorderInnerRightBottomColor;
};

class WING_API ProgressBar : public Control
{
public:
    ProgressBar(ProgressBarCreateParams& createParams);
    void SetProgressPercent(float percent);
    const Color& ProgressColor() const { return progressColor; }
    const Color& SunkenBorderOuterTopLeftColor() const { return sunkenBorderOuterTopLeftColor; }
    const Color& SunkenBorderInnerTopLeftColor() const { return sunkenBorderInnerTopLeftColor; }
    const Color& SunkenBorderOuterRightBottomColor() const { return sunkenBorderOuterRightBottomColor; }
    const Color& SunkenBorderInnerRightBottomColor() const { return sunkenBorderInnerRightBottomColor; }
protected:
    void OnPaint(PaintEventArgs& args) override;
private:
    void DrawSunkenBorder(Graphics& graphics);
    float progressPercent;
    Padding padding;
    Color progressColor;
    Color sunkenBorderOuterTopLeftColor;
    Color sunkenBorderInnerTopLeftColor;
    Color sunkenBorderOuterRightBottomColor;
    Color sunkenBorderInnerRightBottomColor;
    ProgressBarBrushesAndPens brushesAndPens;
};

} } // cmajor::wing

#endif // CMAJOR_WING_PROGRESS_BAR_INCLUDED
