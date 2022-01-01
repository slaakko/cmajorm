// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/Icon.hpp>
#include <wing/Wing.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace wing {

using namespace soulng::unicode;

Icon::Icon() : handle(nullptr), standard(false)
{
}

Icon::Icon(HICON handle_) : handle(handle_), standard(false)
{
}

Icon::Icon(HICON handle_, bool standard_) : handle(handle_), standard(standard_)
{
}

Icon::Icon(Icon&& that) noexcept : handle(that.handle), standard(that.standard)
{
    that.handle = nullptr;
    that.standard = false;
}

Icon& Icon::operator=(Icon&& that) noexcept
{
    std::swap(handle, that.handle);
    std::swap(standard, that.standard);
    return *this;
}

Icon::~Icon()
{
    if (handle && !standard)
    {
        DestroyIcon(handle);
    }
}

Icon LoadStandardIcon(int standardIconId)
{
    HICON iconHandle = LoadIcon(nullptr, MAKEINTRESOURCE(standardIconId));
    return Icon(iconHandle, true);
}

Icon LoadIconByName(const std::string& iconName)
{
    std::u16string name = ToUtf16(iconName);
    HICON iconHandle = LoadIcon(Instance(), LPCWSTR(name.c_str()));
    return Icon(iconHandle, false);
}

} } // cmajor::wing
