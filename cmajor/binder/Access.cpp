// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/Access.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

void CheckAccess(FunctionSymbol* fromFunction, Symbol* toSymbol) 
{
    FunctionSymbol* toContainingFunction = toSymbol->ContainingFunctionNoThrow();
    if (toContainingFunction)
    {
        if (fromFunction == toContainingFunction || FunctionSymbolsEqual()(fromFunction, toContainingFunction))
        {
            return;
        }
    }
    ClassTypeSymbol* toContainingClass = toSymbol->ContainingClassNoThrow();
    if (toContainingClass)
    {
        CheckAccess(fromFunction, toContainingClass);
    }
    switch (toSymbol->DeclaredAccess())
    {
        case SymbolAccess::public_: return;
        case SymbolAccess::protected_:
        {
            ClassTypeSymbol* fromContainingClass = fromFunction->ContainingClassNoThrow();
            if (fromContainingClass)
            {
                if (toContainingClass->IsSameParentOrAncestorOf(fromContainingClass))
                {
                    return;
                }
                if (fromContainingClass->HasBaseClass(toContainingClass))
                {
                    return;
                }
            }
            break;
        }
        case SymbolAccess::internal_:
        {
            if (fromFunction->GetModule() == toSymbol->GetModule())
            {
                return;
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
                        return;
                    }
                }
            }
            break;
        }
    }
    throw Exception(toSymbol->GetModule(), toSymbol->TypeString() + " '" + ToUtf8(toSymbol->FullName()) + "' is inaccessible due to its protection level", fromFunction->GetSpan(), toSymbol->GetSpan());
}

} } // namespace cmajor::binder
