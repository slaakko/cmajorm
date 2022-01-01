// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/ProgressBar.hpp>

namespace cmajor { namespace wing {

Color DefaultProgressBarBackgroundColor()
{
    return Color::White;
}

Color DefaultProgressBarProgressColor()
{
    return Color(0, 122, 204);
}

Color DefaultProgressBarSunkenBorderOuterTopLeftColor()
{
    return Color(160, 160, 160);
}

Color DefaultProgressBarSunkenBorderInnerTopLeftColor()
{
    return Color(105, 105, 105);
}

Color DefaultProgressBarSunkenBorderOuterRightBottomColor()
{
    return Color::White;
}

Color DefaultProgressBarSunkenBorderInnerRightBottomColor()
{
    return Color(227, 227, 227);
}

ProgressBarBrushesAndPens::ProgressBarBrushesAndPens(ProgressBar* progressBar_) : 
    progressBar(progressBar_),
    progressBrush(nullptr),
    backgroundBrush(nullptr),
    sunkenBorderOuterTopLeftPen(nullptr),
    sunkenBorderInnerTopLeftPen(nullptr),
    sunkenBorderOuterRightBottomPen(nullptr),
    sunkenBorderInnerRightBottomPen(nullptr)
{
}

Brush* ProgressBarBrushesAndPens::ProgressBrush()
{
    if (progressBrush) return progressBrush;
    progressBrush = new SolidBrush(progressBar->ProgressColor());
    brushes.push_back(std::unique_ptr<Brush>(progressBrush));
    return progressBrush;
}

Brush* ProgressBarBrushesAndPens::BackgroundBrush()
{
    if (backgroundBrush) return backgroundBrush;
    backgroundBrush = new SolidBrush(progressBar->BackgroundColor());
    brushes.push_back(std::unique_ptr<Brush>(backgroundBrush));
    return backgroundBrush;
}

Pen* ProgressBarBrushesAndPens::SunkenBorderOuterTopLeftPen()
{
    if (sunkenBorderOuterTopLeftPen) return sunkenBorderOuterTopLeftPen;
    sunkenBorderOuterTopLeftPen = new Pen(progressBar->SunkenBorderOuterTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderOuterTopLeftPen));
    return sunkenBorderOuterTopLeftPen;
}

Pen* ProgressBarBrushesAndPens::SunkenBorderInnerTopLeftPen()
{
    if (sunkenBorderInnerTopLeftPen) return sunkenBorderInnerTopLeftPen;
    sunkenBorderInnerTopLeftPen = new Pen(progressBar->SunkenBorderInnerTopLeftColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderInnerTopLeftPen));
    return sunkenBorderInnerTopLeftPen;
}

Pen* ProgressBarBrushesAndPens::SunkenBorderOuterRightBottomPen()
{
    if (sunkenBorderOuterRightBottomPen) return sunkenBorderOuterRightBottomPen;
    sunkenBorderOuterRightBottomPen = new Pen(progressBar->SunkenBorderOuterRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderOuterRightBottomPen));
    return sunkenBorderOuterRightBottomPen;
}

Pen* ProgressBarBrushesAndPens::SunkenBorderInnerRightBottomPen()
{
    if (sunkenBorderInnerRightBottomPen) return sunkenBorderInnerRightBottomPen;
    sunkenBorderInnerRightBottomPen = new Pen(progressBar->SunkenBorderInnerRightBottomColor());
    pens.push_back(std::unique_ptr<Pen>(sunkenBorderInnerRightBottomPen));
    return sunkenBorderInnerRightBottomPen;
}

ProgressBarCreateParams::ProgressBarCreateParams() : controlCreateParams()
{
    controlCreateParams.WindowClassBackgroundColor(COLOR_WINDOW);
    controlCreateParams.WindowClassName("wing.ProgressBar");
    controlCreateParams.BackgroundColor(DefaultProgressBarBackgroundColor());
    progressColor = DefaultProgressBarProgressColor();
    sunkenBorderOuterTopLeftColor = DefaultProgressBarSunkenBorderOuterTopLeftColor();
    sunkenBorderInnerTopLeftColor = DefaultProgressBarSunkenBorderInnerTopLeftColor();
    sunkenBorderOuterRightBottomColor = DefaultProgressBarSunkenBorderOuterRightBottomColor();
    sunkenBorderInnerRightBottomColor = DefaultProgressBarSunkenBorderInnerRightBottomColor();
}

ProgressBarCreateParams& ProgressBarCreateParams::Defaults()
{
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::WindowClassName(const std::string& windowClassName_)
{
    controlCreateParams.WindowClassName(windowClassName_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::WindowClassStyle(uint32_t windowClassStyle_)
{
    controlCreateParams.WindowClassStyle(windowClassStyle_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::WindowStyle(int windowStyle_)
{
    controlCreateParams.WindowStyle(windowStyle_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::WindowClassBackgroundColor(int windowClassBackgroundColor_)
{
    controlCreateParams.WindowClassBackgroundColor(windowClassBackgroundColor_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::BackgroundColor(const Color& backgroundColor_)
{
    controlCreateParams.BackgroundColor(backgroundColor_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::Text(const std::string& text_)
{
    controlCreateParams.Text(text_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::Location(Point location_)
{
    controlCreateParams.Location(location_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SetSize(Size size_)
{
    controlCreateParams.SetSize(size_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SetAnchors(Anchors anchors_)
{
    controlCreateParams.SetAnchors(anchors_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SetDock(Dock dock_)
{
    controlCreateParams.SetDock(dock_);
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::ProgressColor(const Color& color)
{
    progressColor = color;
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SunkenBorderOuterTopLeftColor(const Color& color)
{
    sunkenBorderOuterTopLeftColor = color;
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SunkenBorderInnerTopLeftColor(const Color& color)
{
    sunkenBorderInnerTopLeftColor = color;
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SunkenBorderOuterRightBottomColor(const Color& color)
{
    sunkenBorderOuterRightBottomColor = color;
    return *this;
}

ProgressBarCreateParams& ProgressBarCreateParams::SunkenBorderInnerRightBottomColor(const Color& color)
{
    sunkenBorderInnerRightBottomColor = color;
    return *this;
}

ProgressBar::ProgressBar(ProgressBarCreateParams& createParams) : 
    Control(createParams.controlCreateParams),
    progressPercent(0.0f),
    padding(2, 2, 2, 2),
    progressColor(createParams.progressColor),
    sunkenBorderOuterTopLeftColor(createParams.sunkenBorderOuterTopLeftColor),
    sunkenBorderInnerTopLeftColor(createParams.sunkenBorderInnerTopLeftColor),
    sunkenBorderOuterRightBottomColor(createParams.sunkenBorderOuterRightBottomColor),
    sunkenBorderInnerRightBottomColor(createParams.sunkenBorderInnerRightBottomColor),
    brushesAndPens(this)
{
}

void ProgressBar::SetProgressPercent(float percent)
{
    progressPercent = percent;
    Invalidate();
}

void ProgressBar::OnPaint(PaintEventArgs& args)
{
    DrawSunkenBorder(args.graphics);
    Size size = GetSize();
    int height = size.Height;
    int width = size.Width;
    Brush* progressBrush = brushesAndPens.ProgressBrush();
    int progressWidth = (progressPercent / 100.0f) * (width - padding.Horizontal());
    Rect progressRect(padding.left, padding.top, progressWidth, height - padding.Vertical());
    CheckGraphicsStatus(args.graphics.FillRectangle(progressBrush, progressRect));
    Brush* backgroundBrush = brushesAndPens.BackgroundBrush();
    int backgroundWidth = (width - padding.Horizontal()) - progressWidth;
    Rect backgroundRect(padding.left + progressWidth, padding.top, backgroundWidth, height - padding.Vertical());
    CheckGraphicsStatus(args.graphics.FillRectangle(backgroundBrush, backgroundRect));
    Control::OnPaint(args);
}

void ProgressBar::DrawSunkenBorder(Graphics& graphics)
{
    Point location(0, 0);
    Size size = GetSize();
    Pen* outerTopLeftPen = brushesAndPens.SunkenBorderOuterTopLeftPen(); 
    Pen* innerTopLeftPen = brushesAndPens.SunkenBorderInnerTopLeftPen();
    Pen* outerRightBottomPen = brushesAndPens.SunkenBorderOuterRightBottomPen();
    Pen* innerRightBottomPen = brushesAndPens.SunkenBorderInnerRightBottomPen();
    CheckGraphicsStatus(graphics.DrawLine(outerTopLeftPen, location, Point(location.X + size.Width - 2, location.Y)));
    CheckGraphicsStatus(graphics.DrawLine(outerTopLeftPen, location, Point(location.X, location.Y + size.Height - 2)));
    CheckGraphicsStatus(graphics.DrawLine(outerRightBottomPen, Point(location.X + size.Width - 1, location.Y), Point(location.X + size.Width - 1, location.Y + size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(outerRightBottomPen, Point(location.X, location.Y + size.Height - 1), Point(location.X + size.Width - 1, location.Y + size.Height - 1)));
    CheckGraphicsStatus(graphics.DrawLine(innerTopLeftPen, Point(location.X + 1, location.Y + 1), Point(location.X + size.Width - 3, location.Y + 1)));
    CheckGraphicsStatus(graphics.DrawLine(innerTopLeftPen, Point(location.X + 1, location.Y + 1), Point(location.X + 1, location.Y + size.Height - 3)));
    CheckGraphicsStatus(graphics.DrawLine(innerRightBottomPen, Point(location.X + size.Width - 2, location.Y + 1), Point(location.X + size.Width - 2, location.Y + size.Height - 2)));
    CheckGraphicsStatus(graphics.DrawLine(innerRightBottomPen, Point(location.X + 1, location.Y + size.Height - 2), Point(location.X + size.Width - 2, location.Y + size.Height - 2)));
}

} } // cmajor::wing
