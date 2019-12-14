// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/Exception.hpp>

namespace sngxml { namespace dom {

DomException::DomException(const std::string& message_) : std::runtime_error(message_)
{
}

} } // namespace sngxml::dom
