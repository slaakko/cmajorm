// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ast/InitDone.hpp>
#include <cmajor/ast/Node.hpp>
#include <cmajor/ast/SystemFileIndex.hpp>

namespace cmajor { namespace ast {

void Init()
{
    NodeInit();
    SystemFileIndexInit();
}

void Done()
{
    NodeDone();
}

} } // namespace cmajor::ast
