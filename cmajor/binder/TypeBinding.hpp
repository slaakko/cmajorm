// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_TYPE_BINDING_INCLUDED
#define CMAJOR_BINDER_TYPE_BINDING_INCLUDED
#include <cmajor/binder/BinderApi.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/CompileUnit.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;
using namespace sngcm::ast;

BINDER_API std::vector<std::string> BindTypes(Module* module, CompileUnitNode* compileUnit);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_TYPE_BINDING_INCLUDED
