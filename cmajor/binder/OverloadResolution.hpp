// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_OVERLOAD_RESOLUTION_INCLUDED
#define CMAJOR_BINDER_OVERLOAD_RESOLUTION_INCLUDED
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/symbols/Scope.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>

namespace cmajor { namespace binder {

class BoundExpression;
class BoundFunctionCall;
class BoundCompileUnit;
class BoundFunction;
class BoundConstraint;

using namespace cmajor::symbols;

enum class OverloadResolutionFlags : uint8_t
{
    none = 0,
    dontThrow = 1 << 0,
    dontInstantiate = 1 << 1,
    noTemplates = 1 << 2,
    noRvalueRef = 1 << 3,
    includeSystemDefaultFunctions = 1 << 4
};

inline OverloadResolutionFlags operator&(OverloadResolutionFlags left, OverloadResolutionFlags right)
{
    return OverloadResolutionFlags(uint8_t(left) & uint8_t(right));
}

inline OverloadResolutionFlags operator|(OverloadResolutionFlags left, OverloadResolutionFlags right)
{
    return OverloadResolutionFlags(uint8_t(left) | uint8_t(right));
}

struct FunctionScopeLookup
{
    FunctionScopeLookup(ScopeLookup scopeLookup_) : scopeLookup(scopeLookup_), scope(nullptr) {}
    FunctionScopeLookup(ScopeLookup scopeLookup_, ContainerScope* scope_) : scopeLookup(scopeLookup_), scope(scope_) {}
    ScopeLookup scopeLookup;
    ContainerScope* scope;
};

struct ArgumentMatch
{
    ArgumentMatch() : preReferenceConversionFlags(OperationFlags::none), conversionFun(nullptr), postReferenceConversionFlags(OperationFlags::none), conversionDistance(0) {}
    ArgumentMatch(OperationFlags preReferenceConversionFlags_, FunctionSymbol* conversionFun_, OperationFlags postReferenceConversionFlags_, int conversionDistance_) :
        preReferenceConversionFlags(preReferenceConversionFlags_), conversionFun(conversionFun_), postReferenceConversionFlags(postReferenceConversionFlags_), conversionDistance(conversionDistance_) {}
    OperationFlags preReferenceConversionFlags;
    FunctionSymbol* conversionFun;
    OperationFlags postReferenceConversionFlags;
    int conversionDistance;
};

inline bool BetterArgumentMatch(const ArgumentMatch& left, const ArgumentMatch& right)
{
    if (left.preReferenceConversionFlags == OperationFlags::none && right.preReferenceConversionFlags != OperationFlags::none) return true;
    if (left.preReferenceConversionFlags != OperationFlags::none && right.preReferenceConversionFlags == OperationFlags::none) return false;
    if (left.conversionFun == nullptr && right.conversionFun != nullptr) return true;
    if (right.conversionFun == nullptr && left.conversionFun != nullptr) return false;
    if (left.postReferenceConversionFlags == OperationFlags::none && right.postReferenceConversionFlags != OperationFlags::none) return true;
    if (left.postReferenceConversionFlags != OperationFlags::none && right.postReferenceConversionFlags == OperationFlags::none) return false;
    if (left.conversionDistance < right.conversionDistance) return true;
    if (left.conversionDistance > right.conversionDistance) return false;
    return false;
}

struct FunctionMatch
{
    FunctionMatch(FunctionSymbol* fun_) : 
        fun(fun_), numConversions(0), numQualifyingConversions(0), referenceMustBeInitialized(false), castRequired(false), cannotBindConstArgToNonConstParam(false), cannotAssignToConstObject(false),
        sourceType(nullptr), targetType(nullptr), conceptCheckException(nullptr), boundConstraint(nullptr) {}
    FunctionSymbol* fun;
    std::vector<ArgumentMatch> argumentMatches;
    int numConversions;
    int numQualifyingConversions;
    bool referenceMustBeInitialized;
    bool castRequired;
    bool cannotBindConstArgToNonConstParam;
    bool cannotAssignToConstObject;
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
    std::unordered_map<TemplateParameterSymbol*, TypeSymbol*> templateParameterMap;
    Exception* conceptCheckException;
    BoundConstraint* boundConstraint;
};

struct BetterFunctionMatch
{
    bool operator()(const FunctionMatch& left, const FunctionMatch& right) const;
};

bool FindConversions(BoundCompileUnit& boundCompileUnit, FunctionSymbol* function, std::vector<std::unique_ptr<BoundExpression>>& arguments, FunctionMatch& functionMatch, 
    ConversionType conversionType, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId);

std::unique_ptr<BoundFunctionCall> ResolveOverload(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<FunctionScopeLookup>& functionScopeLookups,
    std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundCompileUnit& boundCompileUnit, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId);

std::unique_ptr<BoundFunctionCall> ResolveOverload(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<FunctionScopeLookup>& functionScopeLookups,
    std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundCompileUnit& boundCompileUnit, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId,
    OverloadResolutionFlags flags, std::vector<TypeSymbol*>& templateArgumentTypes, std::unique_ptr<Exception>& exception);

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_OVERLOAD_RESOLUTION_INCLUDED
