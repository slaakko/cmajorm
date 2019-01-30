// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_META_INCLUDED
#define CMAJOR_SYMBOLS_META_INCLUDED
#include <cmajor/symbols/Value.hpp>

namespace cmajor { namespace symbols {

class Module;

class SYMBOLS_API IntrinsicFunction
{
public:
    IntrinsicFunction(Module* module_);
    virtual ~IntrinsicFunction();
    virtual int Arity() const = 0;
    virtual const char* GroupName() const = 0;
    virtual int NumberOfTypeParameters() const = 0;
    virtual TypeSymbol* ReturnType(SymbolTable& symbolTable) const = 0;
    virtual std::unique_ptr<Value> DoEvaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span) = 0;
    std::unique_ptr<Value> Evaluate(const std::vector<std::unique_ptr<Value>>& arguments, const std::vector<TypeSymbol*>& templateArguments, const Span& span);
private:
    Module* module;
};

SYMBOLS_API void MetaInit(SymbolTable& symbolTable);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_META_INCLUDED
