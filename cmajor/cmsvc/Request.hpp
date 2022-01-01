// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SERVICE_REQUEST_INCLUDED
#define CMAJOR_SERVICE_REQUEST_INCLUDED
#include <cmajor/cmsvc/ServiceApi.hpp>
#include <string>
#include <memory>

namespace cmajor { namespace service {

class CMSVC_API Request
{
public:
    virtual ~Request();
    virtual void Execute() = 0;
    virtual std::string Name() const = 0;
};

CMSVC_API void PutRequest(Request* request);
CMSVC_API std::unique_ptr<Request> GetRequest();
CMSVC_API void Exit();
CMSVC_API bool Exiting();
CMSVC_API void InitRequest();
CMSVC_API void DoneRequest();

} } // namespace cmajor::service

#endif // CMAJOR_SERVICE_REQUEST_INCLUDED
