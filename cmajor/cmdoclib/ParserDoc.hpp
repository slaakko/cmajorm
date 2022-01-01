// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
#define CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
#include <cmajor/cmdoclib/Input.hpp>
#include <sngcm/ast/Project.hpp>
#include <string>
#include <unordered_map>

namespace cmajor { namespace cmdoclib {

struct CMDOCLIB_API GrammarInfo
{
    GrammarInfo(const std::u32string& grammarName_, const std::u32string& grammarId_, const std::string& grammarFileName_) :
        grammarName(grammarName_), grammarId(grammarId_), grammarFileName(grammarFileName_)
    {
    }
    std::u32string grammarName;
    std::u32string grammarId;
    std::string grammarFileName;
};

struct CMDOCLIB_API ByGrammarName
{
    bool operator()(const GrammarInfo& left, const GrammarInfo& right) const
    {
        return left.grammarName < right.grammarName;
    }
};

CMDOCLIB_API void GeneratePPXml(sngcm::ast::Project* project);
CMDOCLIB_API void BuildParserDocs(Input* input, const std::string& ppXmlFilePath, const std::string& grammarXmlFilePath, const std::string& relativeModuleDir,
    std::vector<std::string>& grammarFilePaths, const std::u32string& moduleName, std::vector<GrammarInfo>& grammars);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
