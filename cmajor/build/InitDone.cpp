// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/InitDone.hpp>
#include <cmajor/build/Message.hpp>
#include <cmajor/build/BuildServerMessage.hpp>
#include <cmajor/build/ServerConfig.hpp>

namespace cmajor { namespace build {

void Init()
{
    MessageInit();
    BuildServerMessageInit();
    ServerInit();
}

void Done()
{
    ServerDone();
    BuildServerMessageDone();
    MessageDone();
}

} } // namespace cmajor::build
