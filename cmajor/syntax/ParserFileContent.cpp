// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/syntax/ParserFileContent.hpp>
#include <cmajor/parsing/Grammar.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;

ParserFileContent::ParserFileContent(int id_, cmajor::parsing::ParsingDomain* parsingDomain_): id(id_), parsingDomain(parsingDomain_)
{
}

void ParserFileContent::AddIncludeDirective(const std::string& includeDirective_, const std::string& fileAttribute_)
{
    includeDirectives.push_back(std::make_pair(includeDirective_, fileAttribute_));
}

void ParserFileContent::AddUsingObject(cmajor::codedom::UsingObject* usingObjectSubject)
{
    cmajor::parsing::Scope* enclosingScope = parsingDomain->CurrentScope();
    UsingObject* usingObject(new UsingObject(usingObjectSubject, enclosingScope));
    usingObject->SetOwner(id);
    usingObjects.push_back(std::unique_ptr<UsingObject>(usingObject));
}

void ParserFileContent::AddGrammar(cmajor::parsing::Grammar* grammar)
{
    grammar->SetOwner(id);
    parsingDomain->AddGrammar(grammar);
}

void ParserFileContent::BeginNamespace(const std::u32string& ns)
{
    parsingDomain->BeginNamespace(ns);
}

void ParserFileContent::EndNamespace()
{
    parsingDomain->EndNamespace();
}

} } // namespace cmajor::syntax
