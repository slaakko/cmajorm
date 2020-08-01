// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundConstraint.hpp>
#include <cmajor/binder/Concept.hpp>
#include <cmajor/binder/TypeBinder.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/Warning.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>
#include <algorithm>

namespace cmajor { namespace binder {

using namespace soulng::unicode;

bool BetterFunctionMatch::operator()(const FunctionMatch& left, const FunctionMatch& right) const
{
    int leftBetterArgumentMatches = 0;
    int rightBetterArgumentMatches = 0;
    int n = std::max(int(left.argumentMatches.size()), int(right.argumentMatches.size()));
    for (int i = 0; i < n; ++i)
    {
        ArgumentMatch leftMatch;
        if (i < int(left.argumentMatches.size()))
        {
            leftMatch = left.argumentMatches[i];
        }
        ArgumentMatch rightMatch;
        if (i < int(right.argumentMatches.size()))
        {
            rightMatch = right.argumentMatches[i];
        }
        if (BetterArgumentMatch(leftMatch, rightMatch))
        {
            ++leftBetterArgumentMatches;
        }
        else if (BetterArgumentMatch(rightMatch, leftMatch))
        {
            ++rightBetterArgumentMatches;
        }
    }
    if (leftBetterArgumentMatches > rightBetterArgumentMatches)
    {
        return true;
    }
    if (rightBetterArgumentMatches > leftBetterArgumentMatches)
    {
        return false;
    }
    if (left.numConversions < right.numConversions)
    {
        return true;
    }
    if (left.numConversions > right.numConversions)
    {
        return false;
    }
    if (left.numQualifyingConversions < right.numQualifyingConversions)
    {
        return true;
    }
    if (left.numQualifyingConversions > right.numQualifyingConversions)
    {
        return false;
    }
    if (!left.fun->IsFunctionTemplate() && right.fun->IsFunctionTemplate())
    {
        return true;
    }
    if (left.fun->IsFunctionTemplate() && !right.fun->IsFunctionTemplate())
    {
        return false;
    }
    if (!left.fun->IsTemplateSpecialization() && right.fun->IsTemplateSpecialization())
    {
        return true;
    }
    if (left.fun->IsTemplateSpecialization() && !right.fun->IsTemplateSpecialization())
    {
        return false;
    }
    if (left.boundConstraint && !right.boundConstraint)
    {
        return true;
    }
    if (!left.boundConstraint && right.boundConstraint)
    {
        return false;
    }
    if (left.boundConstraint && right.boundConstraint)
    {
        bool leftSubsumeRight = left.boundConstraint->Subsume(right.boundConstraint);
        bool rightSubsumeLeft = right.boundConstraint->Subsume(left.boundConstraint);
        if (leftSubsumeRight && !rightSubsumeLeft)
        {
            return true;
        }
        if (rightSubsumeLeft && !leftSubsumeRight)
        {
            return false;
        }
    }
    return false;
}

bool FindQualificationConversion(TypeSymbol* sourceType, TypeSymbol* targetType, BoundExpression* argument, ConversionType conversionType, const Span& span, FunctionMatch& functionMatch, ArgumentMatch& argumentMatch)
{
    int distance = 0;
    if (argumentMatch.conversionFun)
    {
        distance = argumentMatch.conversionFun->ConversionDistance();
    }
    if (targetType->IsRvalueReferenceType() && !sourceType->IsRvalueReferenceType())
    {
        ++functionMatch.numQualifyingConversions;
    }
    if (sourceType->IsConstType())
    {
        if (targetType->IsConstType() || !targetType->IsReferenceType())
        {
            ++distance;
        }
        else if (conversionType == ConversionType::implicit_)
        {
            if (sourceType->PointerCount() < 1)
            {
                functionMatch.cannotBindConstArgToNonConstParam = true;
                functionMatch.sourceType = sourceType;
                functionMatch.targetType = targetType;
                return false;
            }
            else
            {
                ++distance;
            }
        }
        else
        {
            distance = 255;
        }
    }
    else
    {
        if (targetType->IsConstType())
        {
            distance += 2;
        }
        else
        {
            distance += 3;
        }
    }
    if (sourceType->IsReferenceType() && !targetType->IsReferenceType())
    {
        argumentMatch.postReferenceConversionFlags = OperationFlags::deref;
        argumentMatch.conversionDistance = distance;
        ++functionMatch.numQualifyingConversions;
        return true;
    }
    else if (!sourceType->IsReferenceType() && (targetType->IsReferenceType() || targetType->IsClassTypeSymbol() || targetType->GetSymbolType() == SymbolType::classDelegateTypeSymbol)) 
    {
        if (targetType->IsConstType() || targetType->IsClassTypeSymbol() || targetType->GetSymbolType() == SymbolType::classDelegateTypeSymbol)
        {
            argumentMatch.postReferenceConversionFlags = OperationFlags::addr;
            argumentMatch.conversionDistance = distance;
            ++functionMatch.numQualifyingConversions;
            return true;
        }
        else if ((!sourceType->IsConstType() || sourceType->PointerCount() >= 1) && argument->IsLvalueExpression())
        {
            if (targetType->IsRvalueReferenceType() && !sourceType->IsRvalueReferenceType())
            {
                if (argument->GetFlag(BoundExpressionFlags::bindToRvalueReference))
                {
                    distance = 0;
                }
                else
                {
                    distance += 10;
                }
            }
            argumentMatch.postReferenceConversionFlags = OperationFlags::addr;
            argumentMatch.conversionDistance = distance;
            ++functionMatch.numQualifyingConversions;
            return true;
        }
        else 
        {
            functionMatch.cannotBindConstArgToNonConstParam = true;
            functionMatch.sourceType = sourceType;
            functionMatch.targetType = targetType;
        }
    }
    else if (sourceType->IsConstType() && !targetType->IsConstType())
    {
        ++functionMatch.numQualifyingConversions;
        ++distance;
        if (sourceType->IsLvalueReferenceType() && targetType->IsRvalueReferenceType())
        {
            ++distance;
            ++functionMatch.numQualifyingConversions;
        }
        argumentMatch.conversionDistance = distance;
        return true;
    }
    else if (!sourceType->IsConstType() && targetType->IsConstType())
    {
        ++functionMatch.numQualifyingConversions;
        ++distance;
        if (sourceType->IsLvalueReferenceType() && targetType->IsRvalueReferenceType())
        {
            ++distance;
            ++functionMatch.numQualifyingConversions;
        }
        argumentMatch.conversionDistance = distance;
        return true;
    }
    else if (sourceType->IsLvalueReferenceType() && targetType->IsRvalueReferenceType())
    {
        ++distance;
        ++functionMatch.numQualifyingConversions;
        argumentMatch.conversionDistance = distance;
        return true;
    }
    else if (sourceType->IsRvalueReferenceType() && targetType->IsLvalueReferenceType())
    {
        ++distance;
        ++functionMatch.numQualifyingConversions;
        argumentMatch.conversionDistance = distance;
        return true;
    }
    else if (argumentMatch.conversionFun)
    {
        argumentMatch.conversionDistance = distance;
        return true;
    }
    return false;
}

bool FindTemplateParameterMatch(TypeSymbol* sourceType, TypeSymbol* targetType, ConversionType conversionType, BoundExpression* argument,
    BoundCompileUnit& boundCompileUnit, FunctionMatch& functionMatch, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    if (targetType->BaseType()->GetSymbolType() != SymbolType::templateParameterSymbol) return false;
    TemplateParameterSymbol* templateParameter = static_cast<TemplateParameterSymbol*>(targetType->BaseType());
    TypeSymbol* templateArgumentType = nullptr;
    auto it = functionMatch.templateParameterMap.find(templateParameter);
    if (it == functionMatch.templateParameterMap.cend())
    {
        templateArgumentType = sourceType->RemoveDerivations(targetType->DerivationRec(), span);
        if (templateArgumentType)
        {
            functionMatch.templateParameterMap[templateParameter] = templateArgumentType;
        }
        else
        {
            return false;
        }
    }
    else
    {
        templateArgumentType = it->second;
    }
    targetType = targetType->Unify(templateArgumentType, span);
    if (!targetType)
    {
        return false;
    }
    if (TypesEqual(sourceType, targetType))
    {
        functionMatch.argumentMatches.push_back(ArgumentMatch());
        return true;
    }
    else
    {
        bool qualificationConversionMatch = false;
        ArgumentMatch argumentMatch;
        if (TypesEqual(sourceType->PlainType(span), targetType->PlainType(span)))
        {
            qualificationConversionMatch = FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch);
        }
        if (qualificationConversionMatch)
        {
            functionMatch.argumentMatches.push_back(argumentMatch);
            return true;
        }
        else
        {
            FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(sourceType, targetType, containerScope, currentFunction, span, argumentMatch);
            if (conversionFun)
            {
                if (conversionFun->GetConversionType() == conversionType || conversionFun->GetConversionType() == ConversionType::implicit_)
                {
                    ++functionMatch.numConversions;
                    argumentMatch.conversionFun = conversionFun;
                    //if (argumentMatch.postReferenceConversionFlags == OperationFlags::none) bug???
                    if (argumentMatch.preReferenceConversionFlags == OperationFlags::none)
                    {
                        if (FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch))
                        {
                            functionMatch.argumentMatches.push_back(argumentMatch);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (FindQualificationConversion(conversionFun->ConversionSourceType(), targetType, argument, conversionType, span, functionMatch, argumentMatch))
                        {
                            functionMatch.argumentMatches.push_back(argumentMatch);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

bool FindClassTemplateSpecializationMatch(TypeSymbol* sourceType, TypeSymbol* targetType, ConversionType conversionType, BoundExpression* argument,
    BoundCompileUnit& boundCompileUnit, FunctionMatch& functionMatch, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    if (targetType->BaseType()->GetSymbolType() != SymbolType::classTemplateSpecializationSymbol)
    {
        return false;
    }
    ClassTemplateSpecializationSymbol* targetClassTemplateSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(targetType->BaseType());
    int n = targetClassTemplateSpecialization->TemplateArgumentTypes().size();
    int numArgumentMatches = functionMatch.argumentMatches.size();
    if (sourceType->BaseType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* sourceClassTemplateSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(sourceType->BaseType());
        int m = sourceClassTemplateSpecialization->TemplateArgumentTypes().size();
        if (n != m) return false;
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* sourceArgumentType = sourceClassTemplateSpecialization->TemplateArgumentTypes()[i];
            TypeSymbol* targetArgumentType = targetClassTemplateSpecialization->TemplateArgumentTypes()[i];
            if (FindTemplateParameterMatch(sourceArgumentType, targetArgumentType, conversionType, argument, boundCompileUnit, functionMatch, containerScope, currentFunction, span))
            {
                continue;
            }
            else if (FindClassTemplateSpecializationMatch(sourceArgumentType, targetArgumentType, conversionType, argument, boundCompileUnit, functionMatch, containerScope, currentFunction, span))
            {
                continue;
            }
            else
            {
                return false;
            }
        }
    }
    functionMatch.argumentMatches.resize(numArgumentMatches);
    std::vector<TypeSymbol*> targetTemplateArguments;
    for (int i = 0; i < n; ++i)
    {
        TypeSymbol* templateArgumentType = targetClassTemplateSpecialization->TemplateArgumentTypes()[i]->BaseType()->UnifyTemplateArgumentType(boundCompileUnit.GetSymbolTable(), functionMatch.templateParameterMap, span);
        if (templateArgumentType)
        {
            targetTemplateArguments.push_back(templateArgumentType);
        }
        else
        {
            return false;
        }
    }
    TypeSymbol* plainTargetType = boundCompileUnit.GetSymbolTable().MakeClassTemplateSpecialization(targetClassTemplateSpecialization->GetClassTemplate(), targetTemplateArguments, span);
    targetType = boundCompileUnit.GetSymbolTable().MakeDerivedType(plainTargetType, targetType->DerivationRec(), span);
    if (TypesEqual(sourceType, targetType))
    {
        functionMatch.argumentMatches.push_back(ArgumentMatch());
        return true;
    }
    else
    {
        bool qualificationConversionMatch = false;
        ArgumentMatch argumentMatch;
        if (TypesEqual(sourceType->PlainType(span), targetType->PlainType(span)))
        {
            qualificationConversionMatch = FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch);
        }
        if (qualificationConversionMatch)
        {
            functionMatch.argumentMatches.push_back(argumentMatch);
            return true;
        }
        else
        {
            FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(sourceType, targetType, containerScope, currentFunction, span, argumentMatch);
            if (conversionFun)
            {
                if (conversionFun->GetConversionType() == conversionType || conversionFun->GetConversionType() == ConversionType::implicit_)
                {
                    argumentMatch.conversionFun = conversionFun;
                    ++functionMatch.numConversions;
                    if (argumentMatch.preReferenceConversionFlags == OperationFlags::none)
                    {
                        if (FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch))
                        {
                            functionMatch.argumentMatches.push_back(argumentMatch);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (FindQualificationConversion(conversionFun->ConversionSourceType(), targetType, argument, conversionType, span, functionMatch, argumentMatch))
                        {
                            functionMatch.argumentMatches.push_back(argumentMatch);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

bool FindConversions(BoundCompileUnit& boundCompileUnit, FunctionSymbol* function, std::vector<std::unique_ptr<BoundExpression>>& arguments, FunctionMatch& functionMatch, 
    ConversionType conversionType, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span)
{
    if (!currentFunction)
    {
        if (function->IsProject() && !function->IsBound())
        {
            Node* node = boundCompileUnit.GetSymbolTable().GetNodeNoThrow(function);
            if (node)
            {
                TypeBinder typeBinder(boundCompileUnit);
                typeBinder.SetContainerScope(containerScope);
                typeBinder.SetCurrentFunctionSymbol(function);
                node->Accept(typeBinder);
            }
        }
    }
    int arity = arguments.size();
    if (arity == 1 && function->GroupName() == U"@constructor" && arguments[0]->GetType()->IsReferenceType())
    {
        functionMatch.referenceMustBeInitialized = true;
        return false;
    }
    int n = std::min(arity, function->Arity());
    if (!function->IsVarArg())
    {
        Assert(arity == function->Arity(), "wrong arity");
    }
    for (int i = 0; i < n; ++i)
    {
        BoundExpression* argument = arguments[i].get();
        TypeSymbol* sourceType = argument->GetType();
        if (sourceType->RemoveConst(span)->IsBasicTypeSymbol())
        {
            sourceType = sourceType->RemoveConst(span);
        }
        ParameterSymbol* parameter = function->Parameters()[i];
        TypeSymbol* targetType = parameter->GetType();
        if (arity == 2 && function->GroupName() == U"operator=")
        {
            if (targetType->IsRvalueReferenceType() && !sourceType->IsRvalueReferenceType() && !argument->GetFlag(BoundExpressionFlags::bindToRvalueReference))
            {
                ++functionMatch.numQualifyingConversions;
            }
            if (i == 0)
            {
                if (targetType->IsConstType() && targetType->PointerCount() <= 1)
                {
                    functionMatch.cannotAssignToConstObject = true;
                    return false;
                }
            }
            if (i == 0 && TypesEqual(sourceType, targetType))    // exact match
            {
                if (sourceType->IsReferenceType() && !function->IsConstructorDestructorOrNonstaticMemberFunction())
                {
                    functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, nullptr, OperationFlags::deref, 1));
                    ++functionMatch.numConversions;
                    continue;
                }
            }
            else if (i == 1 && sourceType->IsReferenceType() && targetType->IsLvalueReferenceType() && TypesEqual(sourceType->RemoveReference(span)->RemoveConst(span), targetType->RemoveReference(span)))
            {
                if (!function->IsConstructorDestructorOrNonstaticMemberFunction())
                {
                    functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, nullptr, OperationFlags::deref, 1));
                    ++functionMatch.numConversions;
                    continue;
                }
            }
            else if (i == 1 && sourceType->IsReferenceType() && targetType->IsRvalueReferenceType() && TypesEqual(sourceType->RemoveReference(span), targetType->RemoveReference(span)))
            {
                if (!function->IsConstructorDestructorOrNonstaticMemberFunction())
                {
                    functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, nullptr, OperationFlags::none, 0));
                    continue;
                }
            }
            else if (i == 1 && !sourceType->IsReferenceType() && argument->IsLvalueExpression() && targetType->IsReferenceType() && TypesEqual(sourceType, targetType->RemoveReference(span)))
            {
                if (!function->IsConstructorDestructorOrNonstaticMemberFunction())
                {
                    functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, nullptr, OperationFlags::none , 0));
                    continue;
                }
            }
            else if (i == 1 && function->IsLvalueReferenceCopyAssignment() && TypesEqual(sourceType, targetType->RemoveReference(span)))
            {
                functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, nullptr, OperationFlags::none, 0));
                continue;
            }
            else if (i == 1 && function->IsLvalueReferenceCopyAssignment())
            {
                ArgumentMatch argumentMatch;
                FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(sourceType, targetType->RemoveReference(span), containerScope, currentFunction, span, argumentMatch);
                if (conversionFun && (conversionFun->GetConversionType() == conversionType || conversionFun->GetConversionType() == ConversionType::implicit_))
                {
                    ++functionMatch.numConversions;
                    functionMatch.argumentMatches.push_back(ArgumentMatch(OperationFlags::none, conversionFun, OperationFlags::none, 1));
                    continue;
                }
            }
        }
        if (TypesEqual(sourceType, targetType))    // exact match
        {
            functionMatch.argumentMatches.push_back(ArgumentMatch());
        }
        else
        {
            if (arity == 2 && (function->GroupName() == U"@constructor" || function->GroupName() == U"operator="))
            {
                if (i == 0)
                {
                    return false;
                }
            }
            bool qualificationConversionMatch = false;
            ArgumentMatch argumentMatch;
            if (TypesEqual(sourceType->PlainType(span), targetType->PlainType(span)))
            {
                qualificationConversionMatch = FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch);
                functionMatch.argumentMatches.push_back(argumentMatch);
            }
            if (!qualificationConversionMatch)
            {
                FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(sourceType, targetType, containerScope, currentFunction, span, argumentMatch);
                if (conversionFun)
                {
                    if (conversionFun->GetConversionType() == conversionType || conversionFun->GetConversionType() == ConversionType::implicit_)
                    {
                        argumentMatch.conversionFun = conversionFun;
                        ++functionMatch.numConversions;
                        if (argumentMatch.preReferenceConversionFlags == OperationFlags::none)
                        {
                            if (FindQualificationConversion(sourceType, targetType, argument, conversionType, span, functionMatch, argumentMatch))
                            {
                                functionMatch.argumentMatches.push_back(argumentMatch);
                                continue;
                            }
                            else
                            {
                                return false;
                            }
                        }
                        else
                        {
                            if (FindQualificationConversion(conversionFun->ConversionSourceType(), targetType, argument, conversionType, span, functionMatch, argumentMatch))
                            {
                                functionMatch.argumentMatches.push_back(argumentMatch);
                                continue;
                            }
                            else
                            {
                                return false;
                            }
                        }
                    }
                    else
                    {
                        if (arity == 2 && i == 1 && conversionType == ConversionType::implicit_ && conversionFun->GetConversionType() == ConversionType::explicit_)
                        {
                            functionMatch.castRequired = true;
                            functionMatch.sourceType = sourceType;
                            functionMatch.targetType = targetType;
                        }
                        return false;
                    }
                }
                else
                {
                    if (function->IsFunctionTemplate())
                    {
                        if (FindTemplateParameterMatch(sourceType, targetType, conversionType, argument, boundCompileUnit, functionMatch, containerScope, currentFunction, span))
                        {
                            continue;
                        }
                        if (FindClassTemplateSpecializationMatch(sourceType, targetType, conversionType, argument, boundCompileUnit, functionMatch, containerScope, currentFunction, span))
                        {
                            continue;
                        }
                    }
                    return false;
                }
            }
        }
    }
    return true;
}

std::string MakeOverloadName(const std::u32string& groupName, const std::vector<std::unique_ptr<BoundExpression>>& arguments, const Span& span)
{
    std::string overloadName = ToUtf8(groupName);
    overloadName.append(1, '(');
    bool first = true;
    for (const std::unique_ptr<BoundExpression>& argument : arguments)
    {
        bool wasFirst = first;
        if (first)
        {
            first = false;
        }
        else
        {
            overloadName.append(", ");
        }
        if (wasFirst && (groupName == U"@constructor" || groupName == U"operator="))
        {
            overloadName.append(ToUtf8(argument->GetType()->RemovePointer(span)->FullName()));
        }
        else
        {
            overloadName.append(ToUtf8(argument->GetType()->FullName()));
        }
    }
    overloadName.append(1, ')');
    return overloadName;
}

std::unique_ptr<BoundFunctionCall> FailWithNoViableFunction(Module* module, const std::u32string& groupName, const std::vector<std::unique_ptr<BoundExpression>>& arguments, 
    const Span& span, OverloadResolutionFlags flags, std::unique_ptr<Exception>& exception)
{
    std::string overloadName = MakeOverloadName(groupName, arguments, span);
    int arity = arguments.size();
    if (groupName == U"@constructor" && arity == 1 && arguments[0]->GetType()->IsReferenceType())
    {
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new NoViableFunctionException(module, "overload resolution failed: '" + overloadName + "' not found. Note: reference must be initialized.", span, arguments[0]->GetSpan()));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw NoViableFunctionException(module, "overload resolution failed: '" + overloadName + "' not found. Note: reference must be initialized.", span, arguments[0]->GetSpan());
        }
    }
    else
    {
        std::string note;
        if (exception)
        {
            note.append(": Note: ").append(exception->Message());
        }
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new NoViableFunctionException(module, "overload resolution failed: '" + overloadName + "' not found. " +
                "No viable functions taking " + std::to_string(arity) + " arguments found in function group '" + ToUtf8(groupName) + "'" + note, span));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw NoViableFunctionException(module, "overload resolution failed: '" + overloadName + "' not found. " +
                "No viable functions taking " + std::to_string(arity) + " arguments found in function group '" + ToUtf8(groupName) + "'" + note, span);
        }
    }
}

std::unique_ptr<BoundFunctionCall> FailWithOverloadNotFound(Module* module, const ViableFunctionSet& viableFunctions, const std::u32string& groupName, 
    const std::vector<std::unique_ptr<BoundExpression>>& arguments, const std::vector<FunctionMatch>& failedFunctionMatches, const Span& span, 
    OverloadResolutionFlags flags, std::unique_ptr<Exception>& exception)
{
    std::string overloadName = MakeOverloadName(groupName, arguments, span);
    bool referenceMustBeInitialized = false;
    bool castRequired = false;
    bool cannotBindConstArgToNonConstParam = false;
    bool cannotAssignToConstObject = false;
    TypeSymbol* sourceType = nullptr;
    TypeSymbol* targetType = nullptr;
    std::vector<Span> references;
    std::string note;
    if (exception)
    {
        note.append(" Note: ").append(exception->What());
    }
    if (!failedFunctionMatches.empty())
    {
        int n = failedFunctionMatches.size();
        for (int i = 0; i < n; ++i)
        {
            if (failedFunctionMatches[i].referenceMustBeInitialized)
            {
                referenceMustBeInitialized = true;
                break;
            }
        }
        if (!referenceMustBeInitialized)
        {
            for (int i = 0; i < n; ++i)
            {
                if (failedFunctionMatches[i].castRequired)
                {
                    castRequired = true;
                    sourceType = failedFunctionMatches[i].sourceType;
                    targetType = failedFunctionMatches[i].targetType;
                    references.push_back(failedFunctionMatches[i].fun->GetSpan());
                    break;
                }
            }
        }
        if (!referenceMustBeInitialized && !castRequired)
        {
            for (int i = 0; i < n; ++i)
            {
                if (failedFunctionMatches[i].cannotBindConstArgToNonConstParam)
                {
                    cannotBindConstArgToNonConstParam = true;
                    sourceType = failedFunctionMatches[i].sourceType;
                    targetType = failedFunctionMatches[i].targetType;
                    references.push_back(failedFunctionMatches[i].fun->GetSpan());
                    break;
                }
            }
        }
        if (!referenceMustBeInitialized && !castRequired && !cannotBindConstArgToNonConstParam)
        {
            for (int i = 0; i < n; ++i)
            {
                if (failedFunctionMatches[i].cannotAssignToConstObject)
                {
                    cannotAssignToConstObject = true;
                    references.push_back(failedFunctionMatches[i].fun->GetSpan());
                    break;
                }
            }
        }
        if (!referenceMustBeInitialized && !castRequired && !cannotBindConstArgToNonConstParam && !cannotAssignToConstObject)
        {
            for (int i = 0; i < n; ++i)
            {
                if (failedFunctionMatches[i].conceptCheckException)
                {
                    if (!note.empty())
                    {
                        note.append(".");
                    }
                    note.append(" Note: concept check failed: " + failedFunctionMatches[i].conceptCheckException->Message());
                    references.insert(references.end(), failedFunctionMatches[i].conceptCheckException->References().begin(), failedFunctionMatches[i].conceptCheckException->References().end());
                    break;
                }
            }
        }
    }
    if (referenceMustBeInitialized || groupName == U"@constructor" && arguments.size() == 1 && arguments[0]->GetType()->IsReferenceType())
    {
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            references.push_back(arguments[0]->GetSpan());
            exception.reset(new Exception(module, "overload resolution failed: '" + overloadName + "' not found. Note: reference must be initialized.", span, references));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            references.push_back(arguments[0]->GetSpan());
            throw Exception(module, "overload resolution failed: '" + overloadName + "' not found. Note: reference must be initialized.", span, references);
        }
    }
    else if (castRequired)
    {
        Assert(sourceType, "source type not set");
        Assert(targetType, "target type not set");
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new CastOverloadException(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot convert implicitly from '" +
                ToUtf8(sourceType->FullName()) + "' to '" + ToUtf8(targetType->FullName()) + "' but explicit conversion (cast) exists.", span, references));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw CastOverloadException(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot convert implicitly from '" +
                ToUtf8(sourceType->FullName()) + "' to '" + ToUtf8(targetType->FullName()) + "' but explicit conversion (cast) exists.", span, references);
        }
    }
    else if (cannotBindConstArgToNonConstParam)
    {
        Assert(sourceType, "source type not set");
        Assert(targetType, "target type not set");
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new CannotBindConstToNonconstOverloadException(module, "overload resolution failed: '" + overloadName +
                "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot bind constant '" + ToUtf8(sourceType->FullName()) + "' argument " +
                " to nonconstant '" + ToUtf8(targetType->FullName()) + "' parameter", span, references));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw CannotBindConstToNonconstOverloadException(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot bind constant '" + ToUtf8(sourceType->FullName()) + "' argument " +
                " to nonconstant '" + ToUtf8(targetType->FullName()) + "' parameter", span, references);
        }
    }
    else if (cannotAssignToConstObject)
    {
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new CannotAssignToConstOverloadException(module, "overload resolution failed: '" + overloadName +
                "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot assign to const object.", span, references));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw CannotAssignToConstOverloadException(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined. Note: cannot assign to const object. ", span, references);
        }
    }
    else
    {
        if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
        {
            exception.reset(new Exception(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined." + note, span, references));
            return std::unique_ptr<BoundFunctionCall>();
        }
        else
        {
            throw Exception(module, "overload resolution failed: '" + overloadName + "' not found, or there are no acceptable conversions for all argument types. " +
                std::to_string(viableFunctions.Get().size()) + " viable functions examined." + note, span, references);
        }
    }
}

std::unique_ptr<BoundFunctionCall> FailWithAmbiguousOverload(Module* module, const std::u32string& groupName, std::vector<std::unique_ptr<BoundExpression>>& arguments, 
    const std::vector<FunctionMatch>& functionMatches, const Span& span, OverloadResolutionFlags flags, std::unique_ptr<Exception>& exception)
{
    std::string overloadName = MakeOverloadName(groupName, arguments, span);
    std::string matchedFunctionNames;
    bool first = true;
    FunctionMatch equalMatch = std::move(functionMatches[0]);
    std::vector<FunctionMatch> equalMatches;
    equalMatches.push_back(std::move(equalMatch));
    int n = int(functionMatches.size());
    for (int i = 1; i < n; ++i)
    {
        FunctionMatch match = std::move(functionMatches[i]);
        if (!BetterFunctionMatch()(equalMatches[0], match))
        {
            equalMatches.push_back(std::move(match));
        }
    }
    std::vector<Span> references;
    for (const FunctionMatch& match : equalMatches)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            matchedFunctionNames.append(", or ");
        }
        matchedFunctionNames.append(ToUtf8(match.fun->FullName()));
        references.push_back(match.fun->GetSpan());
    }
    if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
    {
        exception.reset(new Exception(module, "overload resolution for overload name '" + overloadName + "' failed: call is ambiguous: \n" + matchedFunctionNames, span, references));
        return std::unique_ptr<BoundFunctionCall>();
    }
    else
    {
        throw Exception(module, "overload resolution for overload name '" + overloadName + "' failed: call is ambiguous: \n" + matchedFunctionNames, span, references);
    }
}

std::unique_ptr<BoundFunctionCall> CreateBoundFunctionCall(FunctionSymbol* bestFun, std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundCompileUnit& boundCompileUnit, 
    BoundFunction* boundFunction, const FunctionMatch& bestMatch, ContainerScope* containerScope, const Span& span)
{
    Module* module = &boundCompileUnit.GetModule();
    std::unique_ptr<BoundFunctionCall> boundFunctionCall(new BoundFunctionCall(module, span, bestFun));
    int arity = arguments.size();
    int n = std::min(arity, bestFun->Arity());
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<BoundExpression>& argument = arguments[i];
        if (i == 0 && !bestFun->IsConstructorDestructorOrNonstaticMemberFunction() && 
            (bestFun->GroupName() == U"@constructor" || bestFun->GroupName() == U"operator=" || bestFun->GroupName() == U"operator->"))
        {
            if (argument->GetBoundNodeType() == BoundNodeType::boundAddressOfExpression)
            {
                BoundAddressOfExpression* addrOf = static_cast<BoundAddressOfExpression*>(argument.get());
                std::unique_ptr<BoundExpression> subject(std::move(addrOf->Subject()));
                addrOf->MoveTemporaryDestructorCallsTo(*subject);
                argument.reset(subject.release());
            }
        }
        const ArgumentMatch& argumentMatch = bestMatch.argumentMatches[i];
        if (argumentMatch.preReferenceConversionFlags != OperationFlags::none)
        {
            if (argumentMatch.preReferenceConversionFlags == OperationFlags::addr)
            {
                if (!argument->IsLvalueExpression())
                {
                    BoundLocalVariable* backingStore = nullptr;
                    if (boundFunction)
                    {
                        backingStore = new BoundLocalVariable(module, span, boundFunction->GetFunctionSymbol()->CreateTemporary(argument->GetType(), span));
                    }
                    argument.reset(new BoundTemporary(module, std::move(argument), std::unique_ptr<BoundLocalVariable>(backingStore)));
                }
                TypeSymbol* type = nullptr;
                if (argument->GetType()->IsClassTypeSymbol() && argument->GetFlag(BoundExpressionFlags::bindToRvalueReference))
                {
                    type = argument->GetType()->AddRvalueReference(span);
                }
                else
                {
                    type = argument->GetType()->AddLvalueReference(span);
                }
                BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(module, std::move(argument), type);
                argument.reset(addressOfExpression);
            }
            else if (argumentMatch.preReferenceConversionFlags == OperationFlags::deref)
            {
                TypeSymbol* type = argument->GetType()->RemoveReference(span);
                BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(module, std::move(argument), type);
                argument.reset(dereferenceExpression);
            }
        }
        if (argumentMatch.conversionFun)
        {
            FunctionSymbol* conversionFun = argumentMatch.conversionFun;
            if (conversionFun->GetSymbolType() == SymbolType::constructorSymbol)
            {
                if (!boundFunction)
                {
                    return std::unique_ptr<BoundFunctionCall>();
                }
                BoundFunctionCall* constructorCall = new BoundFunctionCall(module, span, conversionFun);
                TypeSymbol* conversionTargetType = conversionFun->ConversionTargetType();
                LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(conversionTargetType, span);
                constructorCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(new BoundLocalVariable(module, span, temporary)),
                    conversionTargetType->AddPointer(span))));
                if (conversionTargetType->IsClassTypeSymbol())
                {
                    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(conversionTargetType);
                    if (classType->Destructor())
                    {
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(module, span, classType->Destructor()));
                        destructorCall->AddArgument(std::unique_ptr<BoundExpression>(constructorCall->Arguments()[0]->Clone()));
                        boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span);
                    }
                }
                constructorCall->AddArgument(std::move(argument));
                std::vector<LocalVariableSymbol*> temporaryLocalVariables = conversionFun->CreateTemporariesTo(boundFunction->GetFunctionSymbol());
                for (LocalVariableSymbol* temporaryLocalVariable : temporaryLocalVariables)
                {
                    constructorCall->AddTemporary(std::unique_ptr<BoundLocalVariable>(new BoundLocalVariable(module, span, temporaryLocalVariable)));
                }
                BoundConstructAndReturnTemporaryExpression* conversion = new BoundConstructAndReturnTemporaryExpression(module, std::unique_ptr<BoundExpression>(constructorCall),
                    std::unique_ptr<BoundExpression>(new BoundLocalVariable(module, span, temporary)));
                argument.reset(conversion);
            }
            else if (conversionFun->GetSymbolType() == SymbolType::conversionFunctionSymbol && conversionFun->ReturnsClassInterfaceOrClassDelegateByValue())
            {
                if (!boundFunction)
                {
                    return std::unique_ptr<BoundFunctionCall>();
                }
                BoundFunctionCall* conversionFunctionCall = new BoundFunctionCall(module, span, conversionFun);
                conversionFunctionCall->AddArgument(std::move(argument));
                TypeSymbol* conversionTargetType = conversionFun->ConversionTargetType();
                LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(conversionTargetType, span);
                conversionFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(new BoundLocalVariable(module, span, temporary)),
                    conversionTargetType->AddPointer(span))));
                std::vector<LocalVariableSymbol*> temporaryLocalVariables = conversionFun->CreateTemporariesTo(boundFunction->GetFunctionSymbol());
                for (LocalVariableSymbol* temporaryLocalVariable : temporaryLocalVariables)
                {
                    conversionFunctionCall->AddTemporary(std::unique_ptr<BoundLocalVariable>(new BoundLocalVariable(module, span, temporaryLocalVariable)));
                }
                BoundLocalVariable* conversionResult = new BoundLocalVariable(module, span, temporary);
                if (conversionTargetType->IsClassTypeSymbol())
                {
                    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(conversionTargetType);
                    if (classType->Destructor())
                    {
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(module, span, classType->Destructor()));
                        TypeSymbol* type = conversionResult->GetType()->AddPointer(span);
                        destructorCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(module, std::unique_ptr<BoundExpression>(conversionResult->Clone()), type)));
                        boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span);
                    }
                }
                BoundClassOrClassDelegateConversionResult* conversion = new BoundClassOrClassDelegateConversionResult(module, std::unique_ptr<BoundExpression>(conversionResult),
                    std::unique_ptr<BoundFunctionCall>(conversionFunctionCall));
                argument.reset(conversion);
            }
            else
            {
                BoundConversion* conversion = new BoundConversion(module, std::move(argument), conversionFun);
                std::vector<LocalVariableSymbol*> temporaryLocalVariables = conversionFun->CreateTemporariesTo(boundFunction->GetFunctionSymbol());
                for (LocalVariableSymbol* temporaryLocalVariable : temporaryLocalVariables)
                {
                    conversion->AddTemporary(std::unique_ptr<BoundLocalVariable>(new BoundLocalVariable(module, span, temporaryLocalVariable)));
                }
                argument.reset(conversion);
            }
        }
        if (argumentMatch.postReferenceConversionFlags != OperationFlags::none)
        {
            if (argumentMatch.postReferenceConversionFlags == OperationFlags::addr)
            {
                if (!argument->IsLvalueExpression())
                {
                    BoundLocalVariable* backingStore = nullptr;
                    if (boundFunction)
                    {
                        backingStore = new BoundLocalVariable(module, span, boundFunction->GetFunctionSymbol()->CreateTemporary(argument->GetType(), span));
                    }
                    argument.reset(new BoundTemporary(module, std::move(argument), std::unique_ptr<BoundLocalVariable>(backingStore)));
                }
                TypeSymbol* type = nullptr;
                if (argument->GetType()->IsClassTypeSymbol() && argument->GetFlag(BoundExpressionFlags::bindToRvalueReference))
                {
                    type = argument->GetType()->AddRvalueReference(span);
                }
                else
                {
                    type = argument->GetType()->AddLvalueReference(span);
                }
                BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(module, std::move(argument), type);
                argument.reset(addressOfExpression);
            }
            else if (argumentMatch.postReferenceConversionFlags == OperationFlags::deref)
            {
                TypeSymbol* type = argument->GetType()->RemoveReference(span);
                BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(module, std::move(argument), type);
                argument.reset(dereferenceExpression);
            }
        }
        if (argument->GetType()->IsClassTypeSymbol() || argument->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol || argument->GetType()->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            if (argument->GetType()->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(argument->GetType());
                if (!classType->CopyConstructor())
                {
                    try
                    {
                        boundCompileUnit.GenerateCopyConstructorFor(classType, containerScope, boundFunction, span);
                    }
                    catch (const Exception& ex)
                    {
                        std::vector<Span> references;
                        references.push_back(ex.Defined());
                        references.insert(references.end(), ex.References().begin(), ex.References().end());
                        throw Exception(module, "cannot pass class '" + ToUtf8(classType->FullName()) + "' by value because: " + ex.Message(), argument->GetSpan(), references);
                    }
                }
                TypeSymbol* type = classType->AddConst(span)->AddLvalueReference(span);
                argument.reset(new BoundAddressOfExpression(module, std::move(argument), type));
            }
            else if (argument->GetType()->GetSymbolType() == SymbolType::classDelegateTypeSymbol)
            {
                TypeSymbol* type = argument->GetType()->AddConst(span)->AddLvalueReference(span);
                argument.reset(new BoundAddressOfExpression(module, std::move(argument), type));
            }
            else if (argument->GetType()->GetSymbolType() == SymbolType::interfaceTypeSymbol)
            {
                InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(argument->GetType());
                if (!interfaceTypeSymbol->CopyConstructor())
                { 
                    boundCompileUnit.GenerateCopyConstructorFor(interfaceTypeSymbol, containerScope, boundFunction, span);
                }
                TypeSymbol* type = argument->GetType()->AddConst(span)->AddLvalueReference(span);
                argument.reset(new BoundAddressOfExpression(module, std::move(argument), type));
            }
        }
        boundFunctionCall->AddArgument(std::move(argument));
    }
    for (int i = n; i < arity; ++i)
    {
        std::unique_ptr<BoundExpression>& argument = arguments[i];
        boundFunctionCall->AddArgument(std::move(argument));
    }
    if (boundFunction)
    {
        std::vector<LocalVariableSymbol*> temporaryLocalVariables = bestFun->CreateTemporariesTo(boundFunction->GetFunctionSymbol());
        for (LocalVariableSymbol* temporaryLocalVariable : temporaryLocalVariables)
        {
            boundFunctionCall->AddTemporary(std::unique_ptr<BoundLocalVariable>(new BoundLocalVariable(module, span, temporaryLocalVariable)));
        }
    }
    FunctionSymbol* functionSymbol = boundFunctionCall->GetFunctionSymbol();
    if (functionSymbol->GetSymbolType() == SymbolType::destructorSymbol)
    {
        DestructorSymbol* destructorSymbol = static_cast<DestructorSymbol*>(functionSymbol);
        if (destructorSymbol->IsGeneratedFunction() && !GetGlobalFlag(GlobalFlags::info))
        {
            if (destructorSymbol->Parent()->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(destructorSymbol->Parent());
                if (!boundCompileUnit.IsGeneratedDestructorInstantiated(destructorSymbol))
                {
                    boundCompileUnit.SetGeneratedDestructorInstantiated(destructorSymbol);
                    std::unique_ptr<BoundClass> boundClass(new BoundClass(module, classType));
                    GenerateDestructorImplementation(boundClass.get(), destructorSymbol, boundCompileUnit, containerScope, boundFunction, span);
                    boundCompileUnit.AddBoundNode(std::move(boundClass));
                }
            }
        }
    }
    return boundFunctionCall;
}

std::unique_ptr<BoundFunctionCall> SelectViableFunction(const ViableFunctionSet& viableFunctions, const std::u32string& groupName, 
    std::vector<std::unique_ptr<BoundExpression>>& arguments, ContainerScope* containerScope, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, const Span& span,
    OverloadResolutionFlags flags, std::vector<TypeSymbol*>& templateArgumentTypes, std::unique_ptr<Exception>& exception)
{
    Module* module = &boundCompileUnit.GetModule();
    std::vector<FunctionMatch> functionMatches;
    std::vector<FunctionMatch> failedFunctionMatches;
    std::vector<std::unique_ptr<Exception>> conceptCheckExceptions;
    std::vector<std::unique_ptr<BoundConstraint>> boundConstraints;
    for (FunctionSymbol* viableFunction : viableFunctions.Get())
    {
        if (viableFunction->GetFlag(FunctionSymbolFlags::dontReuse))
        {
            if (!boundCompileUnit.CanReuse(viableFunction))
            {
                continue;
            }
        }
        if (viableFunction->IsSystemDefault())
        {
            if ((flags & OverloadResolutionFlags::includeSystemDefaultFunctions) == OverloadResolutionFlags::none)
            {
                continue;
            }
        }
        FunctionMatch functionMatch(viableFunction);
        if (viableFunction->IsFunctionTemplate())
        {
            if ((flags & OverloadResolutionFlags::noTemplates) != OverloadResolutionFlags::none)
            {
                continue;
            }
            int n = templateArgumentTypes.size();
            if (n > viableFunction->TemplateParameters().size())
            {
                continue;
            }
            else
            {
                for (int i = 0; i < n; ++i)
                {
                    TemplateParameterSymbol* templateParameter = viableFunction->TemplateParameters()[i];
                    functionMatch.templateParameterMap[templateParameter] = templateArgumentTypes[i];
                }
            }
        }
        else
        {
            if (!templateArgumentTypes.empty())
            {
                continue;
            }
        }
        if (FindConversions(boundCompileUnit, viableFunction, arguments, functionMatch, ConversionType::implicit_, containerScope, boundFunction, span))
        {
            if (viableFunction->IsFunctionTemplate())
            {
                bool allTemplateParametersFound = true;
                int n = viableFunction->TemplateParameters().size();
                for (int i = 0; i < n; ++i)
                {
                    TemplateParameterSymbol* templateParameterSymbol = viableFunction->TemplateParameters()[i];
                    auto it = functionMatch.templateParameterMap.find(templateParameterSymbol);
                    if (it == functionMatch.templateParameterMap.cend())
                    {
                        allTemplateParametersFound = false;
                        break;
                    }
                }
                if (allTemplateParametersFound)
                {
                    if (!viableFunction->Constraint())
                    {
                        Node* node = boundCompileUnit.GetSymbolTable().GetNodeNoThrow(viableFunction);
                        if (node)
                        {
                            Assert(node->GetNodeType() == NodeType::functionNode, "function node expected");
                            FunctionNode* functionNode = static_cast<FunctionNode*>(node);
                            ConstraintNode* constraint = functionNode->WhereConstraint();
                            if (constraint)
                            {
                                CloneContext cloneContext;
                                SpanMapper spanMapper;
                                cloneContext.SetSpanMapper(&spanMapper);
                                viableFunction->SetConstraint(static_cast<ConstraintNode*>(constraint->Clone(cloneContext)));
                            }
                        }
                    }
                    if (viableFunction->Constraint())
                    {
                        std::unique_ptr<Exception> conceptCheckException;
                        std::unique_ptr<BoundConstraint> boundConstraint;
                        bool candidateFound = CheckConstraint(viableFunction->Constraint(), viableFunction->UsingNodes(), boundCompileUnit, containerScope, boundFunction,
                            viableFunction->TemplateParameters(), functionMatch.templateParameterMap, boundConstraint, span, viableFunction, conceptCheckException);
                        if (candidateFound)
                        {
                            functionMatch.boundConstraint = boundConstraint.get();
                            functionMatches.push_back(functionMatch);
                            boundConstraints.push_back(std::move(boundConstraint));
                        }
                        else
                        {
                            functionMatch.conceptCheckException = conceptCheckException.get();
                            failedFunctionMatches.push_back(functionMatch);
                            conceptCheckExceptions.push_back(std::move(conceptCheckException));
                        }
                    }
                    else
                    {
                        functionMatches.push_back(functionMatch);
                    }
                }
                else
                {
                    failedFunctionMatches.push_back(functionMatch);
                }
            }
            else
            {
                functionMatches.push_back(functionMatch);
            }
        }
        else
        {
            failedFunctionMatches.push_back(functionMatch);
        }
    }
    if (functionMatches.empty())
    {
        if ((flags & OverloadResolutionFlags::includeSystemDefaultFunctions) == OverloadResolutionFlags::none)
        {
            return SelectViableFunction(viableFunctions, groupName, arguments, containerScope, boundCompileUnit, boundFunction, span,
                (flags | OverloadResolutionFlags::includeSystemDefaultFunctions), templateArgumentTypes, exception);
        }
        return FailWithOverloadNotFound(module, viableFunctions, groupName, arguments, failedFunctionMatches, span, flags, exception);
    }
    else if (functionMatches.size() > 1)
    {
        std::sort(functionMatches.begin(), functionMatches.end(), BetterFunctionMatch());
        if (BetterFunctionMatch()(functionMatches[0], functionMatches[1]))
        {
            const FunctionMatch& bestMatch = functionMatches[0];
            FunctionSymbol* bestFun = bestMatch.fun;
            if (bestFun->IsSuppressed())
            {
                if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
                {
                    exception.reset(new Exception(module, "cannot call a suppressed member function '" + ToUtf8(bestFun->FullName()) + "'", span, bestFun->GetSpan()));
                    return std::unique_ptr<BoundFunctionCall>();
                }
                else
                {
                    throw Exception(module, "cannot call a suppressed member function '" + ToUtf8(bestFun->FullName()) + "'", span, bestFun->GetSpan());
                }
            }
            bool instantiate = (flags & OverloadResolutionFlags::dontInstantiate) == OverloadResolutionFlags::none;
            if (bestFun->IsFunctionTemplate())
            {
                if (instantiate)
                {
                    bestFun = boundCompileUnit.InstantiateFunctionTemplate(bestFun, bestMatch.templateParameterMap, span);
                }
                else
                {
                    templateArgumentTypes.clear();
                    for (TemplateParameterSymbol* templateParameter : bestFun->TemplateParameters())
                    {
                        auto it = bestMatch.templateParameterMap.find(templateParameter);
                        if (it != bestMatch.templateParameterMap.cend())
                        {
                            TypeSymbol* templateArgumentType = it->second;
                            templateArgumentTypes.push_back(templateArgumentType);
                        }
                    }
                }
            }
            else if (!bestFun->IsGeneratedFunction() && bestFun->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                if (instantiate)
                {
                    ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(bestFun->Parent());
                    if (specialization->GetModule() != module)
                    {
                        specialization = boundCompileUnit.GetSymbolTable().GetCurrentClassTemplateSpecialization(specialization);
                        int index = bestFun->GetIndex();
                        bestFun = specialization->GetFunctionByIndex(index);
                    }
                    bool firstTry = boundCompileUnit.InstantiateClassTemplateMemberFunction(bestFun, containerScope, boundFunction, span);
                    if (!firstTry)
                    {
                        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(bestFun->Parent());
                        ClassTemplateSpecializationSymbol* copy = boundCompileUnit.GetSymbolTable().CopyClassTemplateSpecialization(specialization);
                        boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(
                            copy, boundCompileUnit.GetSymbolTable().GlobalNs().GetContainerScope(), span);
                        int index = bestFun->GetIndex();
                        bestFun = copy->GetFunctionByIndex(index);
                        bool secondTry = boundCompileUnit.InstantiateClassTemplateMemberFunction(bestFun, containerScope, boundFunction, span);
                        if (!secondTry)
                        {
                            throw Exception(GetRootModuleForCurrentThread(),
                                "internal error: could not instantiate member function of a class template specialization '" + ToUtf8(specialization->FullName()) + "'",
                                specialization->GetSpan());
                        }
                    }
                }
            }
            else if (!bestFun->IsGeneratedFunction() && GetGlobalFlag(GlobalFlags::release) && bestFun->IsInline())
            {
                if (instantiate)
                {
                    if (bestFun->IsTemplateSpecialization())
                    {
                        FunctionSymbol* functionTemplate = bestFun->FunctionTemplate();
                        std::unordered_map<TemplateParameterSymbol*, TypeSymbol*> templateParameterMap;
                        int n = functionTemplate->TemplateParameters().size();
                        for (int i = 0; i < n; ++i)
                        {
                            TemplateParameterSymbol* templateParameter = functionTemplate->TemplateParameters()[i];
                            TypeSymbol* templateArgumentType = bestFun->TemplateArgumentTypes()[i];
                            templateParameterMap[templateParameter] = templateArgumentType;
                        }
                        bestFun = boundCompileUnit.InstantiateFunctionTemplate(functionTemplate, templateParameterMap, span);
                    }
                    else
                    {
                        bestFun = boundCompileUnit.InstantiateInlineFunction(bestFun, containerScope, span);
                    }
                }
            }
            if (boundFunction && boundFunction->GetFunctionSymbol()->DontThrow() && !boundFunction->GetFunctionSymbol()->HasTry() && !bestFun->DontThrow())
            {
                std::vector<Span> references;
                references.push_back(boundFunction->GetFunctionSymbol()->GetSpan());
                references.push_back(bestFun->GetSpan());
                if (GetGlobalFlag(GlobalFlags::strictNothrow))
                {
                    if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
                    {
                        exception.reset(new Exception(module, "a nothrow function cannot call a function that can throw unless it handles exceptions (compiled with --strict-nothrow)", span, references));
                        return std::unique_ptr<BoundFunctionCall>();
                    }
                    else
                    {
                        throw Exception(module, "a nothrow function cannot call a function that can throw unless it handles exceptions (compiled with --strict-nothrow)", span, references);
                    }
                }
                else
                {
                    Warning warning(module->GetCurrentProjectName(), "a nothrow function calls a function that can throw and does not handle exceptions");
                    warning.SetDefined(span);
                    warning.SetReferences(references);
                    warning.SetModule(module);
                    module->WarningCollection().AddWarning(warning);
                }
            }
            return CreateBoundFunctionCall(bestFun, arguments, boundCompileUnit, boundFunction, bestMatch, containerScope, span);
        }
        else
        {
            return FailWithAmbiguousOverload(module, groupName, arguments, functionMatches, span, flags, exception);
        }
    }
    else
    {
        const FunctionMatch& bestMatch = functionMatches[0];
        FunctionSymbol* singleBest = bestMatch.fun;
        if (singleBest->IsSuppressed())
        {
            if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
            {
                exception.reset(new Exception(module, "cannot call a suppressed member function '" + ToUtf8(singleBest->FullName()) + "'", span, singleBest->GetSpan()));
                return std::unique_ptr<BoundFunctionCall>();
            }
            else
            {
                throw Exception(module, "cannot call a suppressed member function '" + ToUtf8(singleBest->FullName()) + "'", span, singleBest->GetSpan());
            }
        }
        bool instantiate = (flags & OverloadResolutionFlags::dontInstantiate) == OverloadResolutionFlags::none;
        if (singleBest->IsFunctionTemplate())
        {
            if (instantiate)
            {
                singleBest = boundCompileUnit.InstantiateFunctionTemplate(singleBest, bestMatch.templateParameterMap, span);
            }
            else
            {
                templateArgumentTypes.clear();
                for (TemplateParameterSymbol* templateParameter : singleBest->TemplateParameters())
                {
                    auto it = bestMatch.templateParameterMap.find(templateParameter);
                    if (it != bestMatch.templateParameterMap.cend())
                    {
                        TypeSymbol* templateArgumentType = it->second;
                        templateArgumentTypes.push_back(templateArgumentType);
                    }
                }
            }
        }
        else if (!singleBest->IsGeneratedFunction() && singleBest->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            if (instantiate)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(singleBest->Parent());
                if (specialization->GetModule() != module)
                {
                    specialization = boundCompileUnit.GetSymbolTable().GetCurrentClassTemplateSpecialization(specialization);
                    int index = singleBest->GetIndex();
                    singleBest = specialization->GetFunctionByIndex(index);
                }
                bool firstTry = boundCompileUnit.InstantiateClassTemplateMemberFunction(singleBest, containerScope, boundFunction, span);
                if (!firstTry)
                {
                    ClassTemplateSpecializationSymbol* copy = boundCompileUnit.GetSymbolTable().CopyClassTemplateSpecialization(specialization);
                    boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(copy, boundCompileUnit.GetSymbolTable().GlobalNs().GetContainerScope(), span);
                    int index = singleBest->GetIndex();
                    singleBest = copy->GetFunctionByIndex(index);
                    bool secondTry = boundCompileUnit.InstantiateClassTemplateMemberFunction(singleBest, containerScope, boundFunction, span);
                    if (!secondTry)
                    {
                        throw Exception(GetRootModuleForCurrentThread(),
                            "internal error: could not instantiate member function of a class template specialization '" + ToUtf8(specialization->FullName()) + "'",
                            specialization->GetSpan());
                    }
                }
            }
        }
        else if (!singleBest->IsGeneratedFunction() && GetGlobalFlag(GlobalFlags::release) && singleBest->IsInline())
        {
            if (instantiate)
            {
                if (singleBest->IsTemplateSpecialization())
                {
                    FunctionSymbol* functionTemplate = singleBest->FunctionTemplate();
                    std::unordered_map<TemplateParameterSymbol*, TypeSymbol*> templateParameterMap;
                    int n = functionTemplate->TemplateParameters().size();
                    for (int i = 0; i < n; ++i)
                    {
                        TemplateParameterSymbol* templateParameter = functionTemplate->TemplateParameters()[i];
                        TypeSymbol* templateArgumentType = singleBest->TemplateArgumentTypes()[i];
                        templateParameterMap[templateParameter] = templateArgumentType;
                    }
                    singleBest = boundCompileUnit.InstantiateFunctionTemplate(functionTemplate, templateParameterMap, span);
                }
                else
                {
                    singleBest = boundCompileUnit.InstantiateInlineFunction(singleBest, containerScope, span);
                }
            }
        }
        if (boundFunction && boundFunction->GetFunctionSymbol()->DontThrow() && !boundFunction->GetFunctionSymbol()->HasTry() && !singleBest->DontThrow())
        {
            std::vector<Span> references;
            references.push_back(boundFunction->GetFunctionSymbol()->GetSpan());
            references.push_back(singleBest->GetSpan());
            if (GetGlobalFlag(GlobalFlags::strictNothrow))
            {
                if ((flags & OverloadResolutionFlags::dontThrow) != OverloadResolutionFlags::none)
                {
                    exception.reset(new Exception(module, "a nothrow function cannot call a function that can throw unless it handles exceptions (compiled with --strict-nothrow)", span, references));
                    return std::unique_ptr<BoundFunctionCall>();
                }
                else
                {
                    throw Exception(module, "a nothrow function cannot call a function that can throw unless it handles exceptions (compiled with --strict-nothrow)", span, references);
                }
            }
            else
            {
                Warning warning(module->GetCurrentProjectName(), "a nothrow function calls a function that can throw and does not handle exceptions");
                warning.SetDefined(span);
                warning.SetReferences(references);
                module->WarningCollection().AddWarning(warning);
            }
        }
        return CreateBoundFunctionCall(singleBest, arguments, boundCompileUnit, boundFunction, bestMatch, containerScope, span);
    }
}

void CollectViableFunctionsFromSymbolTable(int arity, const std::u32string& groupName, const std::vector<FunctionScopeLookup>& functionScopeLookups, BoundCompileUnit& boundCompileUnit,
    ViableFunctionSet& viableFunctions)
{
    std::unordered_set<ContainerScope*> scopesLookedUp;
    bool fileScopesLookedUp = false;
    for (const FunctionScopeLookup& functionScopeLookup : functionScopeLookups)
    {
        if (functionScopeLookup.scopeLookup == ScopeLookup::fileScopes && !fileScopesLookedUp)
        {
            fileScopesLookedUp = true;
            for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
            {
                fileScope->CollectViableFunctions(arity, groupName, scopesLookedUp, viableFunctions, &boundCompileUnit.GetModule());
            }
        }
        else
        {
            ContainerScope* scope = functionScopeLookup.scope;
            scope->CollectViableFunctions(arity, groupName, scopesLookedUp, functionScopeLookup.scopeLookup, viableFunctions, &boundCompileUnit.GetModule());
        }
    }
}

std::unique_ptr<BoundFunctionCall> ResolveOverload(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<FunctionScopeLookup>& functionScopeLookups,
    std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundCompileUnit& boundCompileUnit, BoundFunction* currentFunction, const Span& span)
{
    std::unique_ptr<Exception> exception;
    std::vector<TypeSymbol*> templateArgumentTypes;
    return ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, currentFunction, span, OverloadResolutionFlags::none, templateArgumentTypes, exception);
}

std::unique_ptr<BoundFunctionCall> ResolveOverload(const std::u32string& groupName, ContainerScope* containerScope, const std::vector<FunctionScopeLookup>& functionScopeLookups, 
    std::vector<std::unique_ptr<BoundExpression>>& arguments, BoundCompileUnit& boundCompileUnit, BoundFunction* currentFunction, const Span& span, 
    OverloadResolutionFlags flags, std::vector<TypeSymbol*>& templateArgumentTypes, std::unique_ptr<Exception>& exception)
{
    Module* module = &boundCompileUnit.GetModule();
    int arity = arguments.size();
    ViableFunctionSet viableFunctions;
    if (currentFunction)
    {
        CollectFlags collectFlags = CollectFlags::none;
        if ((flags & OverloadResolutionFlags::noRvalueRef) != OverloadResolutionFlags::none)
        {
            collectFlags = collectFlags | CollectFlags::noRvalueRef;
        }
        boundCompileUnit.CollectViableFunctions(groupName, containerScope, arguments, currentFunction, viableFunctions, exception, span, collectFlags);
    }
    if (viableFunctions.Get().empty())
    {
        if ((flags & OverloadResolutionFlags::dontThrow) == OverloadResolutionFlags::none && exception)
        {
            throw *exception;
        }
        CollectViableFunctionsFromSymbolTable(arity, groupName, functionScopeLookups, boundCompileUnit, viableFunctions);
    }
    if (viableFunctions.Get().empty())
    {
        return FailWithNoViableFunction(module, groupName, arguments, span, flags, exception);
    }
    else
    {
        return SelectViableFunction(viableFunctions, groupName, arguments, containerScope, boundCompileUnit, currentFunction, span, flags, templateArgumentTypes, exception);
    }
}

} } // namespace cmajor::binder
