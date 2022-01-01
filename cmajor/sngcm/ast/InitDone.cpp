// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/ast/InitDone.hpp>
#include <sngcm/ast/Node.hpp>
#include <sngcm/ast/SystemFileIndex.hpp>

namespace sngcm { namespace ast {

void Init()
{
    NodeInit();
    SystemFileIndexInit();
}

void Done()
{
    NodeDone();
}

} } // namespace sngcm::ast
