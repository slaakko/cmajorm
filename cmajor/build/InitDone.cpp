// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/build/InitDone.hpp>
#include <cmajor/build/Message.hpp>
#include <cmajor/build/BuildServerMessage.hpp>

namespace cmajor { namespace build {

void Init()
{
    MessageInit();
    BuildServerMessageInit();
}

void Done()
{
    BuildServerMessageDone();
    MessageDone();
}

} } // namespace cmajor::build
