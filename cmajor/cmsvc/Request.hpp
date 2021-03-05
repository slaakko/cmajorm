// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_REQUEST_INCLUDED
#define CMAJOR_SERVICE_REQUEST_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <memory>

namespace cmajor { namespace service {

enum class RequestKind : int
{
    startBuildRequest = 0, stopBuildRequest = 1
};

class CMSVC_API Request
{
public:
    Request(RequestKind kind_);
    virtual ~Request();
    RequestKind Kind() const { return kind; }
private:
    RequestKind kind;
};

class CMSVC_API StartBuildRequest : public Request
{
public:
    StartBuildRequest();
};

class CMSVC_API StopBuildRequest : public Request
{
public:
    StopBuildRequest();
};

CMSVC_API void PutRequest(Request* request);
CMSVC_API std::unique_ptr<Request> GetRequest();
CMSVC_API void Exit();
CMSVC_API void InitRequest();
CMSVC_API void DoneRequest();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_REQUEST_INCLUDED
