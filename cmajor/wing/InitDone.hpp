// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_INIT_DONE_INCLUDED
#define CMAJOR_INIT_DONE_INCLUDED
#include <cmajor/wing/WingApi.hpp>
#include <Windows.h>

namespace cmajor { namespace wing {

WING_API void Init(HINSTANCE instance);
WING_API void Done();

} } // cmajor::wing

#endif // CMAJOR_INIT_DONE_INCLUDED
