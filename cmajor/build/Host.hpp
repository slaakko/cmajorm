// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BUILD_HOST_INCLUDED
#define CMAJOR_BUILD_HOST_INCLUDED
#include <cmajor/build/BuildApi.hpp>
#include <string>

namespace cmajor { namespace build {

class BUILD_API Host
{
public:
    virtual ~Host();
    virtual const std::string& Name() const = 0;
    virtual void Exit() = 0;
};

} } // namespace cmajor::build

#endif // CMAJOR_BUILD_HOST_INCLUDED
