// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_WING_MESSAGE_BOX_INCLUDED
#define CMAJOR_WING_MESSAGE_BOX_INCLUDED
#include <cmajor/wing/WingApi.hpp>
#include <string>

#undef MessageBox

namespace cmajor { namespace wing {

class Control;

enum class MessageBoxResult : int
{
    ok = 1,
    cancel = 2,
    abort = 3,
    retry = 4,
    ignore = 5,
    yes = 6,
    no = 7,
    tryAgain = 10,
    continue_ = 11
};

class WING_API MessageBox
{
public:
    static MessageBoxResult Show(const std::string& message, const std::string& caption, Control* owner, int messageBoxType);
};

} } // cmajor::wing

#endif // CMAJOR_WING_MESSAGE_BOX_INCLUDED
