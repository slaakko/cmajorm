// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;

class BoundCompileUnit;

struct FunctionTemplateKey
{
    FunctionTemplateKey(FunctionSymbol* functionTemplate_, const std::vector<TypeSymbol*>& templateArgumentTypes_) : functionTemplate(functionTemplate_), templateArgumentTypes(templateArgumentTypes_)
    {
    }
    FunctionSymbol* functionTemplate;
    std::vector<TypeSymbol*> templateArgumentTypes;
};

bool operator==(const FunctionTemplateKey& left, const FunctionTemplateKey& right);
bool operator!=(const FunctionTemplateKey& left, const FunctionTemplateKey& right);

struct FunctionTemplateKeyHash
{
    size_t operator()(const FunctionTemplateKey& key) const
    {
        size_t x = std::hash<FunctionSymbol*>()(key.functionTemplate);
        int n = key.templateArgumentTypes.size();
        for (int i = 0; i < n; ++i)
        {
            x = x ^ std::hash<TypeSymbol*>()(key.templateArgumentTypes[i]);
        }
        return x;
    }
};

class FunctionTemplateRepository
{
public:
    FunctionTemplateRepository(BoundCompileUnit& boundCompileUnit_);
    FunctionSymbol* Instantiate(FunctionSymbol* functionTemplate, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMapping, const Span& span, const boost::uuids::uuid& moduleId);
private:
    BoundCompileUnit& boundCompileUnit;
    std::unordered_map<FunctionTemplateKey, FunctionSymbol*, FunctionTemplateKeyHash> functionTemplateMap;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_FUNCTION_TEMPLATE_REPOSITORY_INCLUDED
