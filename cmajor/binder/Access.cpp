// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/Access.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

bool HasAccess(FunctionSymbol* fromFunction, Symbol* toSymbol)
{
    if (!fromFunction) return false;
    FunctionSymbol* toContainingFunction = toSymbol->ContainingFunctionNoThrow();
    if (toContainingFunction)
    {
        if (fromFunction == toContainingFunction || FunctionSymbolsEqual()(fromFunction, toContainingFunction))
        {
            return true;
        }
    }
    ClassTypeSymbol* toContainingClass = toSymbol->ContainingClassNoThrow();
    if (toContainingClass)
    {
        if (!HasAccess(fromFunction, toContainingClass)) return false;
    }
    switch (toSymbol->DeclaredAccess())
    {
        case SymbolAccess::public_: return true;
        case SymbolAccess::protected_:
        {
            ClassTypeSymbol* fromContainingClass = fromFunction->ContainingClassNoThrow();
            if (fromContainingClass)
            {
                if (toContainingClass && toContainingClass->IsSameParentOrAncestorOf(fromContainingClass))
                {
                    return true;
                }
                if (fromContainingClass->HasBaseClass(toContainingClass))
                {
                    return true;
                }
            }
            break;
        }
        case SymbolAccess::internal_:
        {
            if (fromFunction->GetModule() == toSymbol->GetModule())
            {
                return true;;
            }
            break;
        }
        case SymbolAccess::private_:
        {
            if (toContainingClass)
            {
                ClassTypeSymbol* fromContainingClass = fromFunction->ContainingClassNoThrow();
                if (fromContainingClass)
                {
                    if (toContainingClass->IsSameParentOrAncestorOf(fromContainingClass))
                    {
                        return true;
                    }
                }
            }
            break;
        }
    }
    return false;
}

void CheckAccess(FunctionSymbol* fromFunction, Symbol* toSymbol) 
{
    if (!HasAccess(fromFunction, toSymbol))
    {
        throw Exception(toSymbol->TypeString() + " '" + ToUtf8(toSymbol->FullName()) + "' is inaccessible due to its protection level",
            fromFunction->GetSpan(), fromFunction->SourceModuleId(), toSymbol->GetSpan(), toSymbol->SourceModuleId());
    }
}

} } // namespace cmajor::binder
