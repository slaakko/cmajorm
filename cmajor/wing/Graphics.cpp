// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/wing/Graphics.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

std::string GdiplusStatusStr(Gdiplus::Status status)
{
    switch (status)
    {
        case Gdiplus::Status::Ok: return "ok";
        case Gdiplus::Status::GenericError: return "generic error";
        case Gdiplus::Status::InvalidParameter: return "invalid parameter";
        case Gdiplus::Status::OutOfMemory: return "out of memory";
        case Gdiplus::Status::ObjectBusy: return "object busy";
        case Gdiplus::Status::InsufficientBuffer: return "insufficient buffer";
        case Gdiplus::Status::NotImplemented: return "not implemented";
        case Gdiplus::Status::Win32Error: return "WIN32 error";
        case Gdiplus::Status::WrongState: return "wrong state";
        case Gdiplus::Status::Aborted: return "aborted";
        case Gdiplus::Status::FileNotFound: return "file not found";
        case Gdiplus::Status::ValueOverflow: return "value overflow";
        case Gdiplus::Status::AccessDenied: return "access denied";
        case Gdiplus::Status::UnknownImageFormat: return "unknown image format";
        case Gdiplus::Status::FontFamilyNotFound: return "font family not found";
        case Gdiplus::Status::NotTrueTypeFont: return "not TrueType font";
        case Gdiplus::Status::UnsupportedGdiplusVersion: return "unsupported GDI+ version";
        case Gdiplus::Status::GdiplusNotInitialized: return "GDI+ not initialized";
        case Gdiplus::Status::PropertyNotFound: return "property not found";
        case Gdiplus::Status::PropertyNotSupported: return "property not supported";
        default: return "unknown GDI+ error";
    }
}

GdiPlusException::GdiPlusException(Gdiplus::Status status_) : std::runtime_error("GDI+ error: " + GdiplusStatusStr(status_)), status(status_)
{
}

void CheckGraphicsStatus(Gdiplus::Status status)
{
    if (status != Gdiplus::Status::Ok)
    {
        throw GdiPlusException(status);
    }
}

Font::Font(const FontFamily& family, float size, FontStyle style, Unit unit) : nativeFont(new Gdiplus::Font(&family, size, style, unit))
{
}

Font::~Font()
{
    if (nativeFont)
    {
        delete nativeFont;
    }
}

Gdiplus::Font* CloneFont(Gdiplus::Font* nativeFont)
{
    if (nativeFont)
    {
        return nativeFont->Clone();
    }
    return nullptr;
}

Font::Font(const Font& that) : nativeFont(CloneFont(that.nativeFont))
{
}

Font::Font(Font&& that) noexcept : nativeFont(that.nativeFont)
{
    that.nativeFont = nullptr;
}

Font& Font::operator=(const Font& that)
{
    if (nativeFont != that.nativeFont)
    {
        if (nativeFont != nullptr)
        {
            delete nativeFont;
        }
        nativeFont = CloneFont(that.nativeFont);
    }
    return *this;
}

Font& Font::operator=(Font&& that) noexcept
{
    std::swap(nativeFont, that.nativeFont);
    return *this;
}

FontHandle::FontHandle(HFONT handle_) : handle(handle_)
{
}

FontHandle::~FontHandle()
{
    if (handle)
    {
        DeleteObject(handle);
    }
}

FontHandle::FontHandle(FontHandle&& that) noexcept : handle(that.handle)
{
    that.handle = nullptr;
}

FontHandle& FontHandle::operator=(FontHandle&& that) noexcept
{
    std::swap(handle, that.handle);
    return *this;
}

FontHandle ToFontHandle(Graphics& graphics, const Font& font)
{
    LOGFONTW logFont;
    CheckGraphicsStatus(font.NativeFont()->GetLogFontW(&graphics, &logFont));
    return FontHandle(CreateFontIndirectW(&logFont));
}

Point GetMessagePos()
{
    DWORD pos = ::GetMessagePos();
    int x = pos & 0xFFFF;
    int y = (pos >> 16) & 0xFFFF;
    return Point(x, y);
}

Rect ToRect(const RECT& winRect)
{
    return Rect(winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top);
}

RECT ToWinRect(const Rect& rect)
{
    RECT r;
    r.left = rect.X;
    r.right = rect.X + rect.Width;
    r.top = rect.Y;
    r.bottom = rect.Y + rect.Height;
    return r;
}

Size LParamSize(Message& msg)
{
    uint32_t s = msg.LParamLoDWord();
    uint16_t sw = static_cast<uint16_t>(s);
    uint16_t sh = static_cast<uint16_t>(s >> 16);
    return Size(sw, sh);
}

Point LParamLocation(Message& msg)
{
    uint32_t s = msg.LParamLoDWord();
    uint16_t sx = static_cast<uint16_t>(s);
    uint16_t sy = static_cast<uint16_t>(s >> 16);
    return Point(sx, sy);
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const Brush& brush)
{
    std::u16string txt = ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &brush));
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat, const Brush& brush)
{
    std::u16string txt = ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &stringFormat, &brush));
}

void DrawString(Graphics& graphics, const std::string& text, const Font& font, const RectF& rect, const StringFormat& stringFormat, const Brush& brush)
{
    std::u16string txt = ToUtf16(text);
    CheckGraphicsStatus(graphics.DrawString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), rect, &stringFormat, &brush));
}

RectF MeasureString(Graphics& graphics, const std::string& text, const Font& font, const PointF& origin, const StringFormat& stringFormat)
{
    std::u16string txt = ToUtf16(text);
    RectF boundingBox;
    CheckGraphicsStatus(graphics.MeasureString((const WCHAR*)txt.c_str(), txt.length(), font.NativeFont(), origin, &stringFormat, &boundingBox));
    return boundingBox;
}

} } // cmajor::wing
