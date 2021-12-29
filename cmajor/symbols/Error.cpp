// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/Error.hpp>

namespace cmajor { namespace symbols {

Error::Error(const std::string& message_) : std::runtime_error(message_), message(message_)
{
}

} } // namespace cmajor::symbols
