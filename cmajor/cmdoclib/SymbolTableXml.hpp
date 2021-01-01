// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_SYMBOL_TABLE_XML_INCLUDED
#define CMAJOR_CMDOCLIB_SYMBOL_TABLE_XML_INCLUDED
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/cmdoclib/File.hpp>
#include <cmajor/symbols/Module.hpp>
#include <sngcm/ast/Project.hpp>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;
using namespace sngcm::ast;

CMDOCLIB_API bool SymbolTableXmlFilesUpToDate(Project* project);
CMDOCLIB_API void GenerateSymbolTableXml(Module* rootModule, std::unordered_map<int, File>& fileMap);
CMDOCLIB_API void ReadExternalModuleDocuments(Input* input);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_SYMBOL_TABLE_XML_INCLUDED
