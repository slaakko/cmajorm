// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/Log.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <iostream>

namespace cmajor { namespace build {

Log::~Log()
{
}

class CoutLog : public Log
{
public:
    CoutLog();
    void Write(const std::string& op, JsonObject* message);
private:
    CodeFormatter formatter;
};

CoutLog::CoutLog() : formatter(std::cout)
{
    formatter.SetLogging();
}

void CoutLog::Write(const std::string& op, JsonObject* message)
{
    std::cout << op;
    if (message)
    {
        message->Write(formatter);
    }
}

std::unique_ptr<Log> CreateCoutLog()
{
    return std::unique_ptr<Log>(new CoutLog());
}

} } // namespace cmajor::build
