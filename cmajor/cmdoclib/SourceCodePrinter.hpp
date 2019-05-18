// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED
#define CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED
#include <cmajor/cmdoclib/CmDocLibApi.hpp>
#include <cmajor/ast/Project.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/cmdoclib/File.hpp>

namespace cmajor { namespace cmdoclib {

CMDOCLIB_API bool HtmlSourceFilePathsUpToDate(cmajor::ast::Project* project);
CMDOCLIB_API void GenerateSourceCode(cmajor::ast::Project* project, cmajor::binder::BoundCompileUnit* boundCompileUnit, std::unordered_map<int, File>& fileMap);
CMDOCLIB_API void SourceCodePrinterInit();
CMDOCLIB_API void SourceCodePrinterDone();

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED