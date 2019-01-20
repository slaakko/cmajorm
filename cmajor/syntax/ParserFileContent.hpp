// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYNTAX_PARSERFILECONTENT_INCLUDED
#define CMAJOR_SYNTAX_PARSERFILECONTENT_INCLUDED

#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Namespace.hpp>

namespace cmajor { namespace syntax {

class ParserFileContent
{
public:
    ParserFileContent(int id_, cmajor::parsing::ParsingDomain* parsingDomain_);
    void SetFilePath(const std::string& filePath_) { filePath = filePath_; }
    void AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_);
    void AddUsingObject(cmajor::codedom::UsingObject* usingObjectSubject);
    void AddGrammar(cmajor::parsing::Grammar* grammar);
    void BeginNamespace(const std::u32string& ns);
    void EndNamespace();
    int Id() const { return id; }
    const std::string& FilePath() const { return filePath; }
    cmajor::parsing::ParsingDomain* ParsingDomain() const { return parsingDomain; }
    const std::vector<std::pair<std::string, std::string>>& IncludeDirectives() const { return includeDirectives; }
    const std::vector<std::unique_ptr<cmajor::parsing::UsingObject>>& UsingObjects() const { return usingObjects; }
    cmajor::parsing::Scope* CurrentScope() const { return parsingDomain->CurrentScope(); }
private:
    int id;
    std::string filePath;
    cmajor::parsing::ParsingDomain* parsingDomain;
    std::vector<std::pair<std::string, std::string>> includeDirectives;
    std::vector<std::unique_ptr<cmajor::parsing::UsingObject>> usingObjects;
};

} } // namespace cmajor::syntax

#endif // CMAJOR_SYNTAX_PARSERFILECONTENT_INCLUDED
