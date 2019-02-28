// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_CONSTRAINT_INCLUDED
#define CMAJOR_CMDOCLIB_CONSTRAINT_INCLUDED
#include <cmajor/binder/BoundCompileUnit.hpp>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::ast;
using namespace cmajor::symbols;
using namespace cmajor::binder;

void BindConstraintSymbols(Node* node, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_CONSTRAINT_INCLUDED
