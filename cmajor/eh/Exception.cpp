// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/eh/Exception.hpp>
#include <typeinfo>

extern "C" void* EhGetExceptionTypeId()
{
    const std::type_info& exceptionTypeId = typeid(cmajor::eh::Exception);
    const void* ti = &exceptionTypeId;
    return const_cast<void*>(ti);
}

