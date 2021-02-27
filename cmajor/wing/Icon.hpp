// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_ICON_INCLUDED
#define CMAJOR_WING_ICON_INCLUDED
#include <cmajor/wing/ResourceManager.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

class WING_API Icon : public Resource
{
public:
    Icon();
    Icon(HICON handle_);
    Icon(HICON handle_, bool standard_);
    Icon(const Icon&) = delete;
    Icon(Icon&& that) noexcept;
    Icon& operator=(const Icon&) = delete;
    Icon& operator=(Icon&& that) noexcept;
    ~Icon();
    HICON Handle() const { return handle; }
    bool Standard() const { return standard; }
private:
    HICON handle;
    bool standard;
};

WING_API Icon LoadStandardIcon(int standardIconId);
WING_API Icon LoadIconByName(const std::string& iconName);

} } // cmajor::wing

#endif // CMAJOR_WING_ICON_INCLUDED
