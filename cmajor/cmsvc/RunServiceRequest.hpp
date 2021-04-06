// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsvc/Request.hpp>

namespace cmajor { namespace service {

class CMSVC_API StartProgramRequest : public Request
{
public:
    StartProgramRequest(const std::string& executableName_, const std::string& programArguments_, const std::string& processName_);
    void Execute() override;
    std::string Name() const override;
private:
    std::string executableName;
    std::string programArguments;
    std::string processName;
};

class CMSVC_API PutProgramInputLineRequest : public Request
{
public:
    PutProgramInputLineRequest(const std::string& inputLine_);
    void Execute() override;
    std::string Name() const override;
private:
    std::string inputLine;
};

class CMSVC_API SetProgramEofRequest : public Request
{
public:
    SetProgramEofRequest();
    void Execute() override;
    std::string Name() const override;
};

class CMSVC_API TerminateProcessRequest : public Request
{
public:
    TerminateProcessRequest();
    void Execute() override;
    std::string Name() const override;
};

class CMSVC_API StopRunServiceRequest : public Request
{
public:
    StopRunServiceRequest();
    void Execute() override;
    std::string Name() const override;
};

} } // namespace cmajor::service
