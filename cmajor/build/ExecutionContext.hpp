// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_EXECUTION_CONTEXT_INCLUDED
#define CMAJOR_BUILD_EXECUTION_CONTEXT_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <memory>
#include <string>

namespace cmajor { namespace build {

class Log;
class BuildClient;
class Connection;

class BUILD_API ExecutionContext
{
public:
    ExecutionContext();
    virtual ~ExecutionContext();
    Log* GetLog() const { return log.get(); }
    virtual BuildClient* GetClient() = 0;
    virtual Connection* GetConnection() = 0;
private:
    std::unique_ptr<Log> log;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_EXECUTION_CONTEXT_INCLUDED
