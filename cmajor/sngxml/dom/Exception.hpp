// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/dom/DomApi.hpp>
#include <stdexcept>

namespace sngxml { namespace dom {

class SNGXML_DOM_API DomException : std::runtime_error
{
public:
    DomException(const std::string& message_);
};

} } // namespace sngxml::dom
