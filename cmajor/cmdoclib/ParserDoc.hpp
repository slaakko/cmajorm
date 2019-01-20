// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
#define CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/ast/Project.hpp>
#include <string>
#include <unordered_map>

namespace cmajor { namespace cmdoclib {

struct GrammarInfo
{
    GrammarInfo(const std::u32string& grammarName_, const std::u32string& grammarId_, const std::string& grammarFileName_) :
        grammarName(grammarName_), grammarId(grammarId_), grammarFileName(grammarFileName_)
    {
    }
    std::u32string grammarName;
    std::u32string grammarId;
    std::string grammarFileName;
};

struct ByGrammarName
{
    bool operator()(const GrammarInfo& left, const GrammarInfo& right) const
    {
        return left.grammarName < right.grammarName;
    }
};

void GeneratePPXml(cmajor::ast::Project* project);
void BuildParserDocs(Input* input, const std::string& ppXmlFilePath, const std::string& grammarXmlFilePath, const std::string& relativeModuleDir, 
    std::vector<std::string>& grammarFilePaths, const std::u32string& moduleName, std::vector<GrammarInfo>& grammars);

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_PARSER_DOC_INCLUDED
