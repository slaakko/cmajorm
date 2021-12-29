// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_ERROR_INCLUDED
#define CMAJOR_SYMBOLS_ERROR_INCLUDED
#include <cmajor/symbols/SymbolsApi.hpp>
#include <stdexcept>

namespace cmajor { namespace symbols {

class SYMBOLS_API Error : public std::runtime_error
{
public:
    Error(const std::string& message_);
    const std::string& Message() const { return message; }
private:
    std::string message;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_ERROR_INCLUDED
