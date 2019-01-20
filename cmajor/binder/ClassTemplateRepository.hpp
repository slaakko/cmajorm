// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_CLASS_TEMPLATE_REPOSITORY_INCLUDED
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BoundCompileUnit;
class BoundFunction;

struct ClassIdMemberFunctionIndexHash
{
    size_t operator()(const std::pair<boost::uuids::uuid, int>& p) const;
};

class ClassTemplateRepository
{
public:
    ClassTemplateRepository(BoundCompileUnit& boundCompileUnit_);
    void ResolveDefaultTemplateArguments(std::vector<TypeSymbol*>& templateArgumentTypes, ClassTypeSymbol* classTemplate, ContainerScope* containerScope, const Span& span);
    void BindClassTemplateSpecialization(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, const Span& span);
    bool Instantiate(FunctionSymbol* memberFunction, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
private:
    BoundCompileUnit& boundCompileUnit;
    std::unordered_set<FunctionSymbol*> instantiatedMemberFunctions;
    std::unordered_set<std::pair<boost::uuids::uuid, int>, ClassIdMemberFunctionIndexHash> classIdMemberFunctionIndexSet;
    bool InstantiateDestructorAndVirtualFunctions(ClassTemplateSpecializationSymbol* classTemplateSpecialization, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span);
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_CLASS_TEMPLATE_REPOSITORY_INCLUDED
