// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <stdexcept>

namespace cmajor { namespace dom {

class DomException : std::runtime_error
{
public:        
    DomException(const std::string& message_);
};

} } // namespace cmajor::dom
