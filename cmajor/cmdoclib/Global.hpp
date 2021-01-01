// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
#define CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
#include <cmajor/cmdoclib/Input.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;

CMDOCLIB_API void ReadGlobals(const std::vector<std::u32string>& moduleNames);
CMDOCLIB_API void WriteGlobals(const std::vector<std::u32string>& moduleNames);
CMDOCLIB_API void AddDerivedClass(ClassTypeSymbol* baseClass, ClassTypeSymbol* derivedClass);
CMDOCLIB_API std::vector<std::u32string> GetDerivedClassIds(const std::u32string& moduleName, const std::u32string& baseClassId);
CMDOCLIB_API void AddConceptRefinement(ConceptSymbol* refinedConcept, ConceptSymbol* conceptSymbol);
CMDOCLIB_API std::vector<std::u32string> GetDerivedConceptIds(const std::u32string& moduleName, const std::u32string& refinedConceptId);
CMDOCLIB_API void WriteModuleXml(const std::string& targetDir, const std::vector<std::u32string>& moduleNames);
CMDOCLIB_API void GlobalInit();
CMDOCLIB_API void GlobalDone();

} } // namespace cmajor::cmdoclib

#endif // CMAJOR_CMDOCLIB_GLOBAL_INCLUDED
