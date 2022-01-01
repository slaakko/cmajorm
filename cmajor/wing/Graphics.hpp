// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_GRAPHICS_INCLUDED
#define CMAJOR_WING_GRAPHICS_INCLUDED
#include <wing/Wing.hpp>
#include <memory>

namespace cmajor { namespace wing {

using Color = Gdiplus::Color;
using Point = Gdiplus::Point;
using PointF = Gdiplus::PointF;
using Size = Gdiplus::Size;
using SizeF = Gdiplus::SizeF;
using Rect = Gdiplus::Rect;
using RectF = Gdiplus::RectF;
using FontFamily = Gdiplus::FontFamily;
using FontStyle = Gdiplus::FontStyle;
using Unit = Gdiplus::Unit;
using Graphics = Gdiplus::Graphics;
using GraphicsState = Gdiplus::GraphicsState;
using StringFormat = Gdiplus::StringFormat;
using StringAlignment = Gdiplus::StringAlignment;
using HotKeyPrefix = Gdiplus::HotkeyPrefix;
using Bitmap = Gdiplus::Bitmap;
using Matrix = Gdiplus::Matrix;
using Region = Gdiplus::Region;
using Brush = Gdiplus::Brush;
using SolidBrush = Gdiplus::SolidBrush;
using Pen = Gdiplus::Pen;

class WING_API GdiPlusException : public std::runtime_error
{
public:
    GdiPlusException(Gdiplus::Status status_);
    Gdiplus::Status Status() const { return status; }
private:
    Gdiplus::Status status;
};

void WING_API CheckGraphicsStatus(Gdiplus::Status status);

class WING_API Font
{
public:
    Font() : nativeFont(nullptr) {}
    ~Font();
    Font(const FontFamily& family, float size, FontStyle style, Unit unit);
    Font(const Font& that);
    Font(Font&& that) noexcept;
    Font& operator=(const Font& that);
    Font& operator=(Font&& that) noexcept;
    bool IsNull() const { return nativeFont == nullptr; }
    Gdiplus::Font* NativeFont() const { return nativeFont; }
private:
    Gdiplus::Font* nativeFont;
};

class WING_API FontHandle
{
public:
    FontHandle(HFONT handle_);
    ~FontHandle();
    FontHandle(const FontHandle&) = delete;
    FontHandle(FontHandle&& that) noexcept;
    FontHandle& operator=(const FontHandle&) = delete;
    FontHandle& operator=(FontHandle&& that) noexcept;
    bool IsNull() const { return handle == nullptr; }
    HFONT Handle() const { return handle; }
private:
    HFONT handle;
};

WING_API FontHandle ToFontHandle(Graphics& graphics, const Font& font);

WING_API inline bool operator==(const Point& p, const Point& q)
{
    return p.X == q.X && p.Y == q.Y;
}

WING_API inline bool operator!=(const Point& p, const Point& q)
{
    return !(p == q);
}

WING_API inline bool operator==(const Size& s, const Size& t)
{
    return s.Width == t.Width && s.Height == t.Height;
}

WING_API inline bool operator!=(const Size& s, const Size& t)
{
    return !(s == t);
}

WING_API inline bool operator==(const Color& left, const Color& right)
{
    return left.GetValue() == right.GetValue();
}

WING_API inline bool operator!=(const Color& left, const Color& right)
{
    return !(left == right);
}

WING_API Point GetMessagePos();
WING_API Rect ToRect(const RECT& winRect);
WING_API RECT ToWinRect(const Rect& rect);
WING_API Size LParamSize(Message& msg);
WING_API Point LParamLocation(Message& msg);
WING_API HWND LParamHandle(Message& msg);

WING_API void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const Brush& brush);
WING_API void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat, const Brush& brush);
WING_API void DrawString(Graphics& graphics, const std::string& text, const Font& font, const RectF& rect, const StringFormat& stringFormat, const Brush& brush);
WING_API RectF MeasureString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat);

WING_API Color ToGray(const Color& color, const Color& transparentColor);
WING_API std::unique_ptr<Bitmap> ToGrayBitmap(Bitmap* bm, const Color& transparentColor);

} } // cmajor::wing

#endif // CMAJOR_WING_GRAPHICS_INCLUDED
