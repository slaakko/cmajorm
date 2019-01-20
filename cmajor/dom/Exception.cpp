// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/dom/Exception.hpp>

namespace cmajor { namespace dom {

DomException::DomException(const std::string& message_) : std::runtime_error(message_)
{
}

} } // namespace cmajor::dom
