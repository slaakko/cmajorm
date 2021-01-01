// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmbs/Error.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmbs {

using namespace soulng::unicode;

CompileError ParsingExceptionToError(const soulng::lexer::ParsingException& ex)
{
    CompileError error;
    error.message = ex.Message();
    error.project = ex.Project();
    error.file = ex.FileName();
    soulng::lexer::Span span = ex.GetSpan();
    error.line = std::to_string(span.line);
    cmajor::symbols::Module* mod = static_cast<cmajor::symbols::Module*>(ex.Module());
    if (mod)
    {
        int startCol = 0;
        int endCol = 0;
        mod->GetColumns(span, startCol, endCol);
        error.scol = std::to_string(startCol);
        error.ecol = std::to_string(endCol);
    }
    return error;
}

std::vector<CompileError> SymbolsExceptionToErrors(const cmajor::symbols::Exception& ex)
{
    std::vector<CompileError> errors;
    CompileError mainError;
    mainError.message = ex.Message();
    cmajor::symbols::Module* mod = cmajor::symbols::GetModuleById(ex.DefinedModuleId());
    Span span = ex.Defined();
    std::u32string code = mod->GetErrorLines(span);
    mainError.message.append("\n").append(ToUtf8(code));
    mainError.project = ToUtf8(mod->Name());
    mainError.file = mod->GetFilePath(span.fileIndex);
    mainError.line = std::to_string(span.line);
    int startCol = 0;
    int endCol = 0;
    mod->GetColumns(span, startCol, endCol);
    mainError.scol = std::to_string(startCol);
    mainError.ecol = std::to_string(endCol);
    errors.push_back(mainError);
    for (const std::pair<Span, boost::uuids::uuid>& spanModuleId : ex.References())
    {
        CompileError referenceError;
        referenceError.message = "See:";
        cmajor::symbols::Module* mod = cmajor::symbols::GetModuleById(spanModuleId.second);
        std::u32string code = mod->GetErrorLines(spanModuleId.first);
        referenceError.message.append("\n").append(ToUtf8(code));
        referenceError.file = mod->GetFilePath(spanModuleId.first.fileIndex);
        referenceError.line = std::to_string(spanModuleId.first.line);
        int startCol = 0;
        int endCol = 0;
        mod->GetColumns(spanModuleId.first, startCol, endCol);
        referenceError.scol = std::to_string(startCol);
        referenceError.ecol = std::to_string(endCol);
        errors.push_back(referenceError);
    }
    return errors;
}

} // namespace cmbs
