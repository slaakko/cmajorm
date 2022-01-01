// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMRES_RESOURCE_PROCESSOR_INCLUDED
#define CMAJOR_CMRES_RESOURCE_PROCESSOR_INCLUDED
#include <cmajor/cmres/CmResApi.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/symbols/Module.hpp>

namespace cmajor { namespace resources {

CMRES_API void ProcessResourcesInProject(sngcm::ast::Project& project, cmajor::symbols::Module& currentModule, sngcm::ast::BackEnd backend);
CMRES_API void InitResources();
CMRES_API void DoneResources();

} } // namespace cmajor::resources

#endif // CMAJOR_CMRES_RESOURCE_PROCESSOR_INCLUDED
