// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <wing/MessageBox.hpp>
#include <wing/Control.hpp>
#include <soulng/util/Unicode.hpp>

#undef MessageBox

namespace cmajor { namespace wing {

using namespace soulng::unicode;

MessageBoxResult MessageBox::Show(const std::string& message, const std::string& caption, Control* owner, int messageBoxType)
{
    std::u16string msg = ToUtf16(message);
    std::u16string cap = ToUtf16(caption);
    return static_cast<MessageBoxResult>(MessageBoxW(owner->Handle(), reinterpret_cast<LPCWSTR>(msg.c_str()), reinterpret_cast<LPCWSTR>(cap.c_str()), messageBoxType));
}

} } // cmajor::wing
