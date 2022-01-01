// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_LOG_INCLUDED
#define CMAJOR_BUILD_LOG_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace build {

using namespace soulng::util;
using namespace soulng::unicode;

class BUILD_API Log
{
public:
    virtual ~Log();
    virtual void Write(const std::string& op, JsonObject* message) = 0;
};

BUILD_API std::unique_ptr<Log> CreateCoutLog();

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_LOG_INCLUDED
