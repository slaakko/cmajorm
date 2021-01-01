// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_SOURCE_FILE_MODULE_MAP_INCLUDED
#define CMAJOR_SYMBOLS_SOURCE_FILE_MODULE_MAP_INCLUDED
#include <sngcm/ast/Project.hpp>
#include <cmajor/symbols/SymbolsApi.hpp>
#include <boost/uuid/uuid.hpp>

namespace cmajor { namespace symbols {

class Module;

SYMBOLS_API void MapSourceFileToModuleId(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath, const boost::uuids::uuid& moduleId);
SYMBOLS_API Module* GetModuleBySourceFile(sngcm::ast::BackEnd backend, sngcm::ast::Config config, const std::string& sourceFilePath);
SYMBOLS_API void InitSourceFileModuleMap();
SYMBOLS_API void DoneSourceFileModuleMap();

} } // namespace cmajor::symbols;

#endif // CMAJOR_SYMBOLS_SOURCE_FILE_MODULE_MAP_INCLUDED
