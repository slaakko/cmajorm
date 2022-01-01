// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_CURSOR_INCLUDED
#define CMAJOR_WING_CURSOR_INCLUDED
#include <wing/ResourceManager.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

enum class StandardCursorId : int
{
    appStarting = 32650,
    arrow = 32512,
    cross = 32515,
    hand = 32649,
    help = 32651,
    ibeam = 32513,
    no = 32648,
    sizeAll = 32646,
    sizeNewSW = 32643,
    sizeNS = 32645,
    sizeNWSE = 32642,
    sizeWE = 32644,
    upArrow = 32516,
    wait = 32514
};

class WING_API Cursor : public Resource
{
public:
    Cursor();
    Cursor(HCURSOR handle_);
    Cursor(HCURSOR handle_, bool standard_);
    Cursor(const Cursor&) = delete;
    Cursor(Cursor&& that) noexcept;
    Cursor& operator=(const Cursor&) = delete;
    Cursor& operator=(Cursor&& that) noexcept;
    ~Cursor();
    bool IsCursor() const override { return true; }
    HCURSOR Handle() const { return handle; }
    bool Standard() const { return standard; }
private:
    HCURSOR handle;
    bool standard;
};

WING_API Cursor LoadStandardCursor(StandardCursorId standardCursorId);
WING_API Cursor LoadCursorByName(const std::string& cursorName);
WING_API void SetCursor(const Cursor& cursor);

} } // cmajor::wing

#endif // CMAJOR_WING_CURSOR_INCLUDED
