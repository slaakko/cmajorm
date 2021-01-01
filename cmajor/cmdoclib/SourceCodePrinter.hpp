// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED
#define CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED
#include <cmajor/cmdoclib/CmDocLibApi.hpp>
#include <sngcm/ast/Project.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/cmdoclib/File.hpp>

namespace cmajor { namespace cmdoclib {

CMDOCLIB_API bool HtmlSourceFilePathsUpToDate(sngcm::ast::Project* project);
CMDOCLIB_API void GenerateSourceCode(sngcm::ast::Project* project, cmajor::binder::BoundCompileUnit* boundCompileUnit, std::unordered_map<int, File>& fileMap);
CMDOCLIB_API void GenerateLexerAndParserHtmlSources(sngcm::ast::Project* project, int maxFileIndex, std::unordered_map<int, File>& fileMap);
CMDOCLIB_API void SourceCodePrinterInit();
CMDOCLIB_API void SourceCodePrinterDone();

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_SOURCE_CODE_PRINTER_INCLUDED
