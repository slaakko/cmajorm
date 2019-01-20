// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
#define CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;

void ReadGlobals(const std::vector<std::u32string>& moduleNames);
void WriteGlobals(const std::vector<std::u32string>& moduleNames);
void AddDerivedClass(ClassTypeSymbol* baseClass, ClassTypeSymbol* derivedClass);
std::vector<std::u32string> GetDerivedClassIds(const std::u32string& moduleName, const std::u32string& baseClassId);
void AddConceptRefinement(ConceptSymbol* refinedConcept, ConceptSymbol* concept);
std::vector<std::u32string> GetDerivedConceptIds(const std::u32string& moduleName, const std::u32string& refinedConceptId);
void WriteModuleXml(const std::string& targetDir, const std::vector<std::u32string>& moduleNames);
void ReadGrammars(Input* input);
void WriteGrammars(Input* input, const std::vector<std::string>& grammarFilePaths);
void GlobalInit();
void GlobalDone();

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
