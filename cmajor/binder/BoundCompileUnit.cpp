// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>
#include <boost/filesystem.hpp>

namespace cmajor { namespace binder {

using namespace soulng::util;
using namespace soulng::unicode;

class ClassTypeConversion : public FunctionSymbol
{
public:
    ClassTypeConversion(const std::u32string& name_, ConversionType conversionType_, uint8_t conversionDistance_, TypeSymbol* sourceType_, TypeSymbol* targetType_);
    ConversionType GetConversionType() const override { return conversionType; }
    uint8_t ConversionDistance() const override { return conversionDistance; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "ClassTypeConversion"; }
private:
    ConversionType conversionType;
    uint8_t conversionDistance;
    TypeSymbol* sourceType;
    TypeSymbol* targetType;
};

ClassTypeConversion::ClassTypeConversion(const std::u32string& name_, ConversionType conversionType_, uint8_t conversionDistance_, TypeSymbol* sourceType_, TypeSymbol* targetType_) : 
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), name_), conversionType(conversionType_), conversionDistance(conversionDistance_), sourceType(sourceType_), targetType(targetType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(sourceType);
    SetConversionTargetType(targetType);
}

void ClassTypeConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateBitCast(value, targetType->IrType(emitter)));
}

class NullPtrToPtrConversion : public FunctionSymbol
{
public:
    NullPtrToPtrConversion(TypeSymbol* nullPtrType_, TypeSymbol* targetPointerType_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    std::unique_ptr<Value> ConvertValue(const std::unique_ptr<Value>& value) const override;
    const char* ClassName() const override { return "NullPtrToPtrConversion"; }
private:
    TypeSymbol* nullPtrType;
    TypeSymbol* targetPointerType;
};

NullPtrToPtrConversion::NullPtrToPtrConversion(TypeSymbol* nullPtrType_, TypeSymbol* targetPointerType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"nullptr2ptr"), nullPtrType(nullPtrType_), targetPointerType(targetPointerType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(nullPtrType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(targetPointerType->PlainType(GetSpan(), SourceModuleId()));

}

void NullPtrToPtrConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateBitCast(value, targetPointerType->IrType(emitter)));
}

std::unique_ptr<Value> NullPtrToPtrConversion::ConvertValue(const std::unique_ptr<Value>& value) const
{
    TypeSymbol* type = value->GetType(&nullPtrType->GetModule()->GetSymbolTable());
    if (type->IsPointerType())
    {
        return std::unique_ptr<Value>(new PointerValue(value->GetSpan(), value->ModuleId(), type, nullptr));
    }
    else
    {
        return std::unique_ptr<Value>();
    }
}

class VoidPtrToPtrConversion : public FunctionSymbol
{
public:
    VoidPtrToPtrConversion(TypeSymbol* voidPtrType_, TypeSymbol* targetPointerType_);
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "VoidPtrToPtrConversion"; }
private:
    TypeSymbol* voidPtrType;
    TypeSymbol* targetPointerType;
};

VoidPtrToPtrConversion::VoidPtrToPtrConversion(TypeSymbol* voidPtrType_, TypeSymbol* targetPointerType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"voidPtr2ptr"), voidPtrType(voidPtrType_), targetPointerType(targetPointerType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(voidPtrType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(targetPointerType->PlainType(GetSpan(), SourceModuleId()));
}

void VoidPtrToPtrConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateBitCast(value, targetPointerType->IrType(emitter)));
}

class PtrToVoidPtrConversion : public FunctionSymbol
{
public:
    PtrToVoidPtrConversion(TypeSymbol* sourcePtrType_, TypeSymbol* voidPtrType_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 10; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "PtrToVoidPtrConversion"; }
private:
    TypeSymbol* sourcePtrType;
    TypeSymbol* voidPtrType;
};

PtrToVoidPtrConversion::PtrToVoidPtrConversion(TypeSymbol* sourcePtrType_, TypeSymbol* voidPtrType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"ptr2voidPtr"), sourcePtrType(sourcePtrType_), voidPtrType(voidPtrType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(sourcePtrType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(voidPtrType->PlainType(GetSpan(), SourceModuleId()));
}

void PtrToVoidPtrConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateBitCast(value, voidPtrType->IrType(emitter)));
}

class PtrToULongConversion : public FunctionSymbol
{
public:
    PtrToULongConversion(TypeSymbol* ptrType_, TypeSymbol* ulongType_);
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "PtrToULongConversion"; }
private:
    TypeSymbol* ptrType;
    TypeSymbol* ulongType;
};

PtrToULongConversion::PtrToULongConversion(TypeSymbol* ptrType_, TypeSymbol* ulongType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"ptr2ulong"), ptrType(ptrType_), ulongType(ulongType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(ptrType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(ulongType->PlainType(GetSpan(), SourceModuleId()));
}

void PtrToULongConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreatePtrToInt(value, ulongType->IrType(emitter)));
}

class ULongToVoidPtrConversion : public FunctionSymbol
{
public:
    ULongToVoidPtrConversion(TypeSymbol* ulongType_, TypeSymbol* voidPtrType_);
    ConversionType GetConversionType() const override { return ConversionType::explicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    const char* ClassName() const override { return "ULongToVoidPtrConversion"; }
private:
    TypeSymbol* ulongType;
    TypeSymbol* voidPtrType;
};

ULongToVoidPtrConversion::ULongToVoidPtrConversion(TypeSymbol* ulongType_, TypeSymbol* voidPtrType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"ulong2voidPtr"), ulongType(ulongType_), voidPtrType(voidPtrType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(ulongType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(voidPtrType->PlainType(GetSpan(), SourceModuleId()));
}

void ULongToVoidPtrConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    emitter.SetCurrentDebugLocation(span);
    void* value = emitter.Stack().Pop();
    emitter.Stack().Push(emitter.CreateIntToPtr(value, voidPtrType->IrType(emitter)));
}

class CharacterPointerLiteralToStringFunctionContainerConversion : public FunctionSymbol
{
public:
    CharacterPointerLiteralToStringFunctionContainerConversion(TypeSymbol* characterPtrType_, TypeSymbol* stringFunctionContainerType_);
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 1; }
    bool IsBasicTypeOperation() const override { return true; }
    void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId) override;
    std::unique_ptr<Value> ConvertValue(const std::unique_ptr<Value>& value) const override;
    const char* ClassName() const override { return "CharacterPointerLiteralToStringFunctionContainerConversion"; }
private:
    TypeSymbol* characterPtrType;
    TypeSymbol* stringFunctionContainerType;
};

CharacterPointerLiteralToStringFunctionContainerConversion::CharacterPointerLiteralToStringFunctionContainerConversion(TypeSymbol* characterPtrType_, TypeSymbol* stringFunctionContainerType_) :
    FunctionSymbol(Span(), boost::uuids::nil_uuid(), U"charlit2stringFun"), characterPtrType(characterPtrType_), stringFunctionContainerType(stringFunctionContainerType_)
{
    SetConversion();
    SetGroupName(U"@conversion");
    SetAccess(SymbolAccess::public_);
    SetConversionSourceType(characterPtrType->PlainType(GetSpan(), SourceModuleId()));
    SetConversionTargetType(stringFunctionContainerType->PlainType(GetSpan(), SourceModuleId()));
}

void CharacterPointerLiteralToStringFunctionContainerConversion::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    throw std::runtime_error(ToUtf8(Name()) + " function provides compile time calls only");
}

std::unique_ptr<Value> CharacterPointerLiteralToStringFunctionContainerConversion::ConvertValue(const std::unique_ptr<Value>& value) const
{
    return std::unique_ptr<Value>();
}

BoundCompileUnit::BoundCompileUnit(Module& module_, CompileUnitNode* compileUnitNode_, AttributeBinder* attributeBinder_) :
    BoundNode(Span(), boost::uuids::nil_uuid(), BoundNodeType::boundCompileUnit), module(module_), symbolTable(module.GetSymbolTable()), 
    compileUnitNode(compileUnitNode_), attributeBinder(attributeBinder_), currentNamespace(nullptr),
    hasGotos(false), operationRepository(*this), functionTemplateRepository(*this), classTemplateRepository(*this), inlineFunctionRepository(*this), 
    constExprFunctionRepository(*this), conversionTable(ConversionTable::Owner::compileUnit, nullptr), bindingTypes(false), compileUnitIndex(-2), immutable(false), nextExitEntryIndex(0),
    systemRuntimeUnwindInfoSymbol(nullptr), systemRuntimeAddCompileUnitFunctionSymbol(nullptr), pushCompileUnitUnwindInfoInitFunctionSymbol(nullptr),
    initUnwindInfoDelegateType(nullptr), globalInitFunctionSymbol(nullptr), latestIdentifierNode(nullptr)
{
    if (compileUnitNode)
    {
        boost::filesystem::path fileName = boost::filesystem::path(compileUnitNode->FilePath()).filename();
        boost::filesystem::path directory = module.DirectoryPath();
        boost::filesystem::path objectFileDirectory = module.ObjectFileDirectoryPath();
        boost::filesystem::path llfp = (directory / fileName).replace_extension(".ll");
        boost::filesystem::path cppfp = (directory / fileName).replace_extension(".cpp");
        boost::filesystem::path bcfp = (directory / fileName).replace_extension(".bc");
        boost::filesystem::path optbcfp = (directory / fileName).replace_extension(".opt.bc");
        boost::filesystem::path optllfp = (directory / fileName).replace_extension(".opt.ll");
        boost::filesystem::path objfp;
#ifdef _WIN32
        if (GetBackEnd() == BackEnd::llvm)
        {
            objfp = (objectFileDirectory / fileName).replace_extension(".obj");
        }
        else if (GetBackEnd() == BackEnd::cmsx)
        {
            objfp = (objectFileDirectory / fileName).replace_extension(".o");
        }
        else if (GetBackEnd() == BackEnd::cmcpp)
        {
            std::string platform = GetPlatform();
            std::string toolChain = GetToolChain();
            const Tool& compilerTool = GetCompilerTool(platform, toolChain);
            objfp = (objectFileDirectory / fileName).replace_extension(compilerTool.outputFileExtension);
        }
#else
        objfp = (objectFileDirectory / fileName).replace_extension(".o");
#endif
        llFilePath = GetFullPath(llfp.generic_string());
        cppFilePath = GetFullPath(cppfp.generic_string());
        bcFilePath = GetFullPath(bcfp.generic_string());
        optBCFilePath = GetFullPath(optbcfp.generic_string());
        optLLFilePath = GetFullPath(optllfp.generic_string());
        objectFilePath = GetFullPath(objfp.generic_string());
    }
}

void BoundCompileUnit::Load(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot load from compile unit", GetSpan(), module.Id());
}

void BoundCompileUnit::Store(Emitter& emitter, OperationFlags flags)
{
    throw Exception("cannot store to compile unit", GetSpan(), module.Id());
}

void BoundCompileUnit::Accept(BoundNodeVisitor& visitor)
{
    visitor.Visit(*this);
}

void BoundCompileUnit::AddFileScope(FileScope* fileScope)
{
    fileScopes.push_back(std::unique_ptr<FileScope>(fileScope));
}

void BoundCompileUnit::RemoveLastFileScope()
{
    if (fileScopes.empty())
    {
        throw Exception("file scopes of bound compile unit is empty", GetSpan(), module.Id());
    }
    fileScopes.erase(fileScopes.end() - 1);
}

FileScope* BoundCompileUnit::ReleaseLastFileScope()
{
    if (fileScopes.empty())
    {
        throw Exception("file scopes of bound compile unit is empty", GetSpan(), module.Id());
    }
    FileScope* fileScope = fileScopes.back().release();
    RemoveLastFileScope();
    return fileScope;
}

void BoundCompileUnit::AddBoundNode(std::unique_ptr<BoundNode>&& boundNode)
{
    if (immutable)
    {
        throw std::runtime_error("internal error: " + compileUnitNode->FilePath() + " is immutable");
    }
    if (currentNamespace)
    {
        currentNamespace->AddMember(std::move(boundNode));
    }
    else
    {
        boundNodes.push_back(std::move(boundNode));
    }
}

FunctionSymbol* BoundCompileUnit::GetConversion(TypeSymbol* sourceType, TypeSymbol* targetType, ContainerScope* containerScope, BoundFunction* currentFunction, 
    const Span& span, const boost::uuids::uuid& moduleId, ArgumentMatch& argumentMatch)
{
    FunctionSymbol* conversion = symbolTable.GetConversion(sourceType, targetType, span, moduleId);
    if (conversion && conversion->GetSymbolType() == SymbolType::conversionFunctionSymbol)
    {
        argumentMatch.preReferenceConversionFlags = OperationFlags::addr;
    }
    if (!conversion)
    {
        conversion = conversionTable.GetConversion(sourceType, targetType, span, moduleId);
        if (!conversion)
        {
            if (sourceType->IsNullPtrType() && targetType->IsPointerType() && !targetType->IsReferenceType())
            {
                std::unique_ptr<FunctionSymbol> nullPtrToPtrConversion(new NullPtrToPtrConversion(symbolTable.GetTypeByName(U"@nullptr_type"), targetType));
                nullPtrToPtrConversion->SetParent(&symbolTable.GlobalNs());
                conversion = nullPtrToPtrConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(nullPtrToPtrConversion));
                return conversion;
            }
            else if (sourceType->IsVoidPtrType() && targetType->IsPointerType() && !targetType->IsReferenceType())
            {
                std::unique_ptr<FunctionSymbol> voidPtrToPtrConversion(new VoidPtrToPtrConversion(symbolTable.GetTypeByName(U"void")->AddPointer(span, moduleId), targetType));
                voidPtrToPtrConversion->SetParent(&symbolTable.GlobalNs());
                conversion = voidPtrToPtrConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(voidPtrToPtrConversion));
                return conversion;
            }
            else if (sourceType->PlainType(span, moduleId)->IsPointerType() && targetType == symbolTable.GetTypeByName(U"ulong"))
            {
                std::unique_ptr<FunctionSymbol> ptrToULongConversion(new PtrToULongConversion(sourceType->PlainType(span, moduleId), symbolTable.GetTypeByName(U"ulong")));
                ptrToULongConversion->SetParent(&symbolTable.GlobalNs());
                conversion = ptrToULongConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(ptrToULongConversion));
                return conversion;
            }
            else if (sourceType == symbolTable.GetTypeByName(U"ulong") && targetType->IsVoidPtrType())
            {
                std::unique_ptr<FunctionSymbol> ulongToVoidPtrConversion(new ULongToVoidPtrConversion(sourceType, targetType));
                ulongToVoidPtrConversion->SetParent(&symbolTable.GlobalNs());
                conversion = ulongToVoidPtrConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(ulongToVoidPtrConversion));
                return conversion;
            }
            else if (sourceType->IsDelegateType() && targetType->IsVoidPtrType())
            {
                std::unique_ptr<FunctionSymbol> dlgToVoidPtrConversion(new DelegateToVoidPtrConversion(sourceType, targetType));
                dlgToVoidPtrConversion->SetParent(&symbolTable.GlobalNs());
                conversion = dlgToVoidPtrConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(dlgToVoidPtrConversion));
                return conversion;
            }
            else if (sourceType->IsVoidPtrType() && targetType->IsDelegateType())
            {
                std::unique_ptr<FunctionSymbol> voidPtrToDlgConversion(new VoidPtrToDelegateConversion(sourceType, targetType, symbolTable.GetTypeByName(U"ulong")));
                voidPtrToDlgConversion->SetParent(&symbolTable.GlobalNs());
                conversion = voidPtrToDlgConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(voidPtrToDlgConversion));
                return conversion;
            }
            else if (sourceType->PlainType(span, moduleId)->IsPointerType() && targetType->RemoveConst(span, moduleId)->IsVoidPtrType())
            {
                std::unique_ptr<FunctionSymbol> ptrToVoidPtrConversion(new PtrToVoidPtrConversion(sourceType->PlainType(span, moduleId), symbolTable.GetTypeByName(U"void")->AddPointer(span, moduleId)));
                ptrToVoidPtrConversion->SetParent(&symbolTable.GlobalNs());
                conversion = ptrToVoidPtrConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(ptrToVoidPtrConversion));
                return conversion;
            }
            else if (sourceType->PlainType(span, moduleId)->IsCharacterPointerType() && targetType->IsStringFunctionContainer())
            {
                std::unique_ptr<FunctionSymbol> charPtr2StringFunctionsConversion(
                    new CharacterPointerLiteralToStringFunctionContainerConversion(sourceType->PlainType(span, moduleId), symbolTable.GetTypeByName(U"@string_functions")));
                charPtr2StringFunctionsConversion->SetParent(&symbolTable.GlobalNs());
                conversion = charPtr2StringFunctionsConversion.get();
                conversionTable.AddConversion(conversion);
                conversionTable.AddGeneratedConversion(std::move(charPtr2StringFunctionsConversion));
                return conversion;
            }
            else if (sourceType->BaseType()->IsClassTypeSymbol() && targetType->BaseType()->IsClassTypeSymbol())
            {
                if (sourceType->PointerCount() == targetType->PointerCount() &&
                    (!sourceType->IsArrayType() && !targetType->IsArrayType()))
                {
                    ClassTypeSymbol* sourceClassType = static_cast<ClassTypeSymbol*>(sourceType->BaseType());
                    ClassTypeSymbol* targetClassType = static_cast<ClassTypeSymbol*>(targetType->BaseType());
                    uint8_t conversionDistance = 0;
                    if (sourceClassType->HasBaseClass(targetClassType, conversionDistance))
                    {
                        //if (targetType->IsLvalueReferenceType() && !sourceType->IsReferenceType()) // bug???
                        if (targetType->IsReferenceType() && !sourceType->IsReferenceType())
                        {
                            argumentMatch.preReferenceConversionFlags = OperationFlags::addr;
                            sourceType = sourceType->AddLvalueReference(span, moduleId);
                            if (targetType->IsConstType())
                            {
                                sourceType = sourceType->AddConst(span, moduleId);
                            }
                        }
                        std::u32string conversionName = sourceType->FullName() + U"2" + targetType->FullName();
                        std::unique_ptr<FunctionSymbol> implicitClassTypeConversion(new ClassTypeConversion(conversionName, ConversionType::implicit_, conversionDistance, sourceType, targetType));
                        conversion = implicitClassTypeConversion.get();
                        // do not add entry to the conversion table
                        conversionTable.AddGeneratedConversion(std::move(implicitClassTypeConversion));
                        return conversion;
                    }
                    else
                    {
                        uint8_t conversionDistance = 0;
                        if (targetClassType->HasBaseClass(sourceClassType, conversionDistance))
                        {
                            //if (targetType->IsLvalueReferenceType() && !sourceType->IsReferenceType()) // bug???
                            if (targetType->IsReferenceType() && !sourceType->IsReferenceType())
                            {
                                argumentMatch.preReferenceConversionFlags = OperationFlags::addr;
                                sourceType = sourceType->AddLvalueReference(span, moduleId);
                                if (targetType->IsConstType())
                                {
                                    sourceType = sourceType->AddConst(span, moduleId);
                                }
                            }
                            std::u32string conversionName = sourceType->FullName() + U"2" + targetType->FullName();
                            std::unique_ptr<FunctionSymbol> explicitClassTypeConversion(new ClassTypeConversion(conversionName, ConversionType::explicit_, conversionDistance, sourceType, targetType));
                            conversion = explicitClassTypeConversion.get();
                            // do not add entry to the conversion table
                            conversionTable.AddGeneratedConversion(std::move(explicitClassTypeConversion));
                            return conversion;
                        }
                    }
                }
            }
            else if ((sourceType->GetSymbolType() == SymbolType::functionGroupTypeSymbol || sourceType->GetSymbolType() == SymbolType::memberExpressionTypeSymbol) && 
                targetType->GetSymbolType() == SymbolType::delegateTypeSymbol)
            {
                FunctionGroupSymbol* functionGroupSymbol = nullptr;
                BoundMemberExpression* boundMemberExpression = nullptr;
                if (sourceType->GetSymbolType() == SymbolType::functionGroupTypeSymbol)
                {
                    FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<FunctionGroupTypeSymbol*>(sourceType);
                    functionGroupSymbol = functionGroupTypeSymbol->FunctionGroup();
                }
                else if (sourceType->GetSymbolType() == SymbolType::memberExpressionTypeSymbol)
                {
                    MemberExpressionTypeSymbol* memberExpressionTypeSymbol = static_cast<MemberExpressionTypeSymbol*>(sourceType);
                    boundMemberExpression = static_cast<BoundMemberExpression*>(memberExpressionTypeSymbol->BoundMemberExpression());
                    if (boundMemberExpression->Member()->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                    {
                        BoundFunctionGroupExpression* boundFunctionGroupExpression = static_cast<BoundFunctionGroupExpression*>(boundMemberExpression->Member());
                        functionGroupSymbol = boundFunctionGroupExpression->FunctionGroup();
                    }
                }
                if (functionGroupSymbol)
                {
                    DelegateTypeSymbol* delegateTypeSymbol = static_cast<DelegateTypeSymbol*>(targetType);
                    int arity = delegateTypeSymbol->Arity();
                    ViableFunctionSet viableFunctions;
                    functionGroupSymbol->CollectViableFunctions(arity, viableFunctions, &GetModule());
                    for (FunctionSymbol* viableFunction : viableFunctions.Get())
                    {
                        if (viableFunction->GetSymbolType() == SymbolType::memberFunctionSymbol && !viableFunction->IsStatic()) continue;
                        bool found = true;
                        for (int i = 0; i < arity; ++i)
                        {
                            ParameterSymbol* sourceParam = viableFunction->Parameters()[i];
                            ParameterSymbol* targetParam = delegateTypeSymbol->Parameters()[i];
                            if (!TypesEqual(sourceParam->GetType(), targetParam->GetType()))
                            {
                                found = false;
                                break;
                            }
                        }
                        if (found)
                        {
                            found = TypesEqual(viableFunction->ReturnType(), delegateTypeSymbol->ReturnType());
                        }
                        if (found)
                        {
                            if (boundMemberExpression)
                            {
                                boundMemberExpression->ResetClassPtr();
                            }
                            if (viableFunction->IsFunctionTemplate())
                            {
                                if (sourceType->GetSymbolType() == SymbolType::functionGroupTypeSymbol)
                                {
                                    FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<FunctionGroupTypeSymbol*>(sourceType);
                                    BoundFunctionGroupExpression* boundFunctionGroupExpression = static_cast<BoundFunctionGroupExpression*>(functionGroupTypeSymbol->BoundFunctionGroup());
                                    std::unordered_map<TemplateParameterSymbol*, TypeSymbol*> templateParameterMap;
                                    int n = viableFunction->TemplateParameters().size();
                                    if (boundFunctionGroupExpression->TemplateArgumentTypes().size() == n)
                                    {
                                        for (int i = 0; i < n; ++i)
                                        {
                                            TemplateParameterSymbol* templateParameterSymbol = viableFunction->TemplateParameters()[i];
                                            TypeSymbol* templateArgumentType = boundFunctionGroupExpression->TemplateArgumentTypes()[i];
                                            templateParameterMap[templateParameterSymbol] = templateArgumentType;
                                        }
                                        viableFunction = InstantiateFunctionTemplate(viableFunction, templateParameterMap, span, moduleId);
                                    }
                                    else
                                    {
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    return nullptr;
                                }
                            }
                            std::unique_ptr<FunctionSymbol> functionToDelegateConversion(new FunctionToDelegateConversion(sourceType, delegateTypeSymbol, viableFunction));
                            functionToDelegateConversion->SetParent(&symbolTable.GlobalNs());
                            conversion = functionToDelegateConversion.get();
                            conversionTable.AddConversion(conversion);
                            conversionTable.AddGeneratedConversion(std::move(functionToDelegateConversion));
                            return conversion;
                        }
                    }
                }
            }
            else if ((sourceType->GetSymbolType() == SymbolType::functionGroupTypeSymbol || sourceType->GetSymbolType() == SymbolType::memberExpressionTypeSymbol) && 
                targetType->PlainType(span, moduleId)->GetSymbolType() == SymbolType::classDelegateTypeSymbol && currentFunction)
            {
                ClassDelegateTypeSymbol* classDelegateType = static_cast<ClassDelegateTypeSymbol*>(targetType->PlainType(span, moduleId));
                FunctionGroupSymbol* functionGroup = nullptr;
                if (sourceType->GetSymbolType() == SymbolType::functionGroupTypeSymbol)
                {
                    FunctionGroupTypeSymbol* functionGroupTypeSymbol = static_cast<FunctionGroupTypeSymbol*>(sourceType);
                    BoundFunctionGroupExpression* boundFunctionGroup = static_cast<BoundFunctionGroupExpression*>(functionGroupTypeSymbol->BoundFunctionGroup());
                    functionGroup = functionGroupTypeSymbol->FunctionGroup();
                }
                else if (sourceType->GetSymbolType() == SymbolType::memberExpressionTypeSymbol)
                {
                    MemberExpressionTypeSymbol* memberExpressionType = static_cast<MemberExpressionTypeSymbol*>(sourceType);
                    BoundMemberExpression* boundMemberExpr = static_cast<BoundMemberExpression*>(memberExpressionType->BoundMemberExpression());
                    if (boundMemberExpr->Member()->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                    {
                        BoundFunctionGroupExpression* boundFunctionGroup = static_cast<BoundFunctionGroupExpression*>(boundMemberExpr->Member());
                        functionGroup = boundFunctionGroup->FunctionGroup();
                    }
                }
                if (functionGroup)
                {
                    int arity = classDelegateType->Arity();
                    ViableFunctionSet viableFunctions;
                    functionGroup->CollectViableFunctions(arity + 1, viableFunctions, &GetModule());
                    for (FunctionSymbol* viableFunction : viableFunctions.Get())
                    {
                        bool found = true;
                        for (int i = 1; i < arity + 1; ++i)
                        {
                            ParameterSymbol* sourceParam = viableFunction->Parameters()[i];
                            ParameterSymbol* targetParam = classDelegateType->Parameters()[i - 1];
                            if (!TypesEqual(sourceParam->GetType(), targetParam->GetType()))
                            {
                                found = false;
                                break;
                            }
                        }
                        if (found)
                        {
                            found = TypesEqual(viableFunction->ReturnType(), classDelegateType->ReturnType());
                        }
                        if (found)
                        {
                            //LocalVariableSymbol* objectDelegatePairVariable = currentFunction->GetFunctionSymbol()->CreateTemporary(classDelegateType->ObjectDelegatePairType(), span);
                            std::unique_ptr<FunctionSymbol> memberFunctionToClassDelegateConversion(new MemberFunctionToClassDelegateConversion(span, moduleId, sourceType, classDelegateType, viableFunction));
                            memberFunctionToClassDelegateConversion->SetParent(&symbolTable.GlobalNs());
                            conversion = memberFunctionToClassDelegateConversion.get();
                            conversionTable.AddConversion(conversion);
                            conversionTable.AddGeneratedConversion(std::move(memberFunctionToClassDelegateConversion));
                            return conversion;
                        }
                    }
                }
            }
            else if (targetType->PlainType(span, moduleId)->GetSymbolType() == SymbolType::interfaceTypeSymbol && currentFunction)
            {
                InterfaceTypeSymbol* targetInterfaceType = static_cast<InterfaceTypeSymbol*>(targetType->PlainType(span, moduleId));
                if (sourceType->IsClassTypeSymbol()) 
                {
                    ClassTypeSymbol* sourceClassType = static_cast<ClassTypeSymbol*>(sourceType);
                    int32_t n = sourceClassType->ImplementedInterfaces().size();
                    for (int32_t i = 0; i < n; ++i)
                    {
                        InterfaceTypeSymbol* sourceInterfaceType = sourceClassType->ImplementedInterfaces()[i];
                        if (TypesEqual(targetInterfaceType, sourceInterfaceType))
                        {
                            //LocalVariableSymbol* temporaryInterfaceObjectVar = currentFunction->GetFunctionSymbol()->CreateTemporary(targetInterfaceType, span);
                            std::unique_ptr<FunctionSymbol> classToInterfaceConversion(new ClassToInterfaceConversion(sourceClassType, targetInterfaceType, i, span, moduleId));
                            classToInterfaceConversion->SetParent(&symbolTable.GlobalNs());
                            classToInterfaceConversion->SetModule(&GetModule());
                            conversion = classToInterfaceConversion.get();
                            conversionTable.AddConversion(conversion);
                            conversionTable.AddGeneratedConversion(std::move(classToInterfaceConversion));
                            return conversion;
                        }
                    }
                }
            }
        }
    }
    if (conversion)
    {
        if (conversion->Parent() && !conversion->IsGeneratedFunction() && conversion->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(conversion->Parent());
            if (specialization->GetModule() != &GetModule())
            {
                specialization = symbolTable.GetCurrentClassTemplateSpecialization(specialization);
                int index = conversion->GetIndex();
                conversion = specialization->GetFunctionByIndex(index);
            }
            bool firstTry = InstantiateClassTemplateMemberFunction(conversion, containerScope, currentFunction, span, moduleId);
            if (!firstTry)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(conversion->Parent());
                std::lock_guard<std::recursive_mutex> lock(symbolTable.GetModule()->GetLock());
                ClassTemplateSpecializationSymbol* copy = symbolTable.CopyClassTemplateSpecialization(specialization);
                classTemplateRepository.BindClassTemplateSpecialization(copy, symbolTable.GlobalNs().GetContainerScope(), span, moduleId);
                int index = conversion->GetIndex();
                conversion = copy->GetFunctionByIndex(index);
                bool secondTry = InstantiateClassTemplateMemberFunction(conversion, containerScope, currentFunction, span, moduleId);
                if (!secondTry)
                {
                    throw Exception("internal error: could not instantiate member function of a class template specialization '" + ToUtf8(specialization->FullName()) + "'",
                        specialization->GetSpan(), specialization->SourceModuleId());
                }
            }
        }
        else if (GetGlobalFlag(GlobalFlags::release) && conversion->IsInline())
        {
            conversion = InstantiateInlineFunction(conversion, containerScope, span, moduleId);
        }
    }
    return conversion;
}

void BoundCompileUnit::CollectViableFunctions(const std::u32string& groupName, ContainerScope* containerScope, std::vector<std::unique_ptr<BoundExpression>>& arguments, 
    BoundFunction* currentFunction, ViableFunctionSet& viableFunctions, std::unique_ptr<Exception>& exception, const Span& span, const boost::uuids::uuid& moduleId, CollectFlags flags)
{
    operationRepository.CollectViableFunctions(groupName, containerScope, arguments, currentFunction, viableFunctions, exception, span, moduleId, flags);
}

FunctionSymbol* BoundCompileUnit::InstantiateFunctionTemplate(FunctionSymbol* functionTemplate, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMapping, 
    const Span& span, const boost::uuids::uuid& moduleId)
{
    return functionTemplateRepository.Instantiate(functionTemplate, templateParameterMapping, span, moduleId);
}

bool BoundCompileUnit::InstantiateClassTemplateMemberFunction(FunctionSymbol* memberFunction, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId)
{
    return classTemplateRepository.Instantiate(memberFunction, containerScope, currentFunction, span, moduleId);
}

FunctionSymbol* BoundCompileUnit::InstantiateInlineFunction(FunctionSymbol* inlineFunction, ContainerScope* containerScope, const Span& span, const boost::uuids::uuid& moduleId)
{
    return inlineFunctionRepository.Instantiate(inlineFunction, containerScope, span, moduleId);
}

FunctionNode* BoundCompileUnit::GetFunctionNodeFor(FunctionSymbol* constExprFunctionSymbol)
{
    return constExprFunctionRepository.GetFunctionNodeFor(constExprFunctionSymbol);
}

void BoundCompileUnit::GenerateCopyConstructorFor(ClassTypeSymbol* classTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId)
{
    operationRepository.GenerateCopyConstructorFor(classTypeSymbol, containerScope, currentFunction, span, moduleId);
}

void BoundCompileUnit::GenerateCopyConstructorFor(InterfaceTypeSymbol* interfaceTypeSymbol, ContainerScope* containerScope, BoundFunction* currentFunction, const Span& span, const boost::uuids::uuid& moduleId)
{
    operationRepository.GenerateCopyConstructorFor(interfaceTypeSymbol, containerScope, currentFunction, span, moduleId);
}

int BoundCompileUnit::Install(const std::string& str)
{
    return utf8StringRepository.Install(str);
}

int BoundCompileUnit::Install(const std::u16string& str)
{
    return utf16StringRepository.Install(str);
}

int BoundCompileUnit::Install(const std::u32string& str)
{
    return utf32StringRepository.Install(str);
}

int BoundCompileUnit::Install(const boost::uuids::uuid& uuid)
{
    return uuidRepository.Install(uuid);
}

const std::string& BoundCompileUnit::GetUtf8String(int stringId) const
{
    return utf8StringRepository.GetString(stringId);
}

const std::u16string& BoundCompileUnit::GetUtf16String(int stringId) const
{
    return utf16StringRepository.GetString(stringId);
}

const std::u32string& BoundCompileUnit::GetUtf32String(int stringId) const
{
    return utf32StringRepository.GetString(stringId);
}

const unsigned char* BoundCompileUnit::GetUtf8CharPtr(int stringId) const
{
    return utf8StringRepository.CharPtr(stringId);
}

const char16_t* BoundCompileUnit::GetUtf16CharPtr(int stringId) const
{
    return utf16StringRepository.CharPtr(stringId);
}

const char32_t* BoundCompileUnit::GetUtf32CharPtr(int stringId) const
{
    return utf32StringRepository.CharPtr(stringId);
}

const boost::uuids::uuid& BoundCompileUnit::GetUuid(int uuidId) const
{
    return uuidRepository.GetUuid(uuidId);
}

void BoundCompileUnit::AddConstantArray(ConstantSymbol* constantArraySymbol)
{
    constantArrayRepository.AddConstantArray(constantArraySymbol);
}

void BoundCompileUnit::AddConstantStructure(ConstantSymbol* constantStructureSymbol)
{
    constantStructureRepository.AddConstantStructure(constantStructureSymbol);
}

void BoundCompileUnit::PushBindingTypes()
{
    bindingTypesStack.push(bindingTypes);
    bindingTypes = true;
}

void BoundCompileUnit::PopBindingTypes()
{
    bindingTypes = bindingTypesStack.top();
    bindingTypesStack.pop();
}

void BoundCompileUnit::FinalizeBinding(ClassTemplateSpecializationSymbol* classTemplateSpecialization)
{
    if (classTemplateSpecialization->GetModule() == &module && classTemplateSpecialization->StatementsNotBound())
    {
        classTemplateSpecialization->ResetStatementsNotBound(); 
        FileScope* fileScope = classTemplateSpecialization->ReleaseFileScope();
        bool fileScopeAdded = false;
        if (fileScope)
        {
            AddFileScope(fileScope);
            fileScopeAdded = true;
        }
        StatementBinder statementBinder(*this);
        classTemplateSpecialization->GlobalNs()->Accept(statementBinder); 
        if (fileScopeAdded)
        {
            RemoveLastFileScope();
        }
    }
}

void BoundCompileUnit::PushNamespace(BoundNamespace* ns)
{
    namespaceStack.push(currentNamespace);
    currentNamespace = ns;
}

void BoundCompileUnit::PopNamespace()
{
    currentNamespace = namespaceStack.top();
    namespaceStack.pop();
}

bool BoundCompileUnit::HasCopyConstructorFor(const boost::uuids::uuid& typeId) const
{
    return copyConstructorMap.find(typeId) != copyConstructorMap.cend();
}

FunctionSymbol* BoundCompileUnit::GetCopyConstructorFor(const boost::uuids::uuid& typeId) const
{
    auto it = copyConstructorMap.find(typeId);
    if (it != copyConstructorMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("internal error: copy constructor for type not found from compile unit");
    }
}
 
void BoundCompileUnit::AddCopyConstructorFor(const boost::uuids::uuid& typeId, std::unique_ptr<FunctionSymbol>&& copyConstructor)
{
    copyConstructorMap[typeId] = copyConstructor.get();
    copyConstructors.push_back(std::move(copyConstructor));
}

void BoundCompileUnit::AddCopyConstructorToMap(const boost::uuids::uuid& typeId, FunctionSymbol* copyConstructor)
{
    copyConstructorMap[typeId] = copyConstructor;
}

void BoundCompileUnit::AddGlobalNs(std::unique_ptr<NamespaceNode>&& globalNs)
{
    globalNamespaceNodes.push_back(std::move(globalNs));
}

bool BoundCompileUnit::IsGeneratedDestructorInstantiated(DestructorSymbol* generatedDestructorSymbol) const
{
    return instantiatedGeneratedDestructors.find(generatedDestructorSymbol) != instantiatedGeneratedDestructors.cend();
}

void BoundCompileUnit::SetGeneratedDestructorInstantiated(DestructorSymbol* generatedDestructorSymbol)
{
    instantiatedGeneratedDestructors.insert(generatedDestructorSymbol);
}

void BoundCompileUnit::SetSystemRuntimeUnwindInfoSymbol(TypeSymbol* systemRuntimeUnwindInfoSymbol_)
{
    systemRuntimeUnwindInfoSymbol = systemRuntimeUnwindInfoSymbol_;
}

void BoundCompileUnit::GenerateInitUnwindInfoFunctionSymbol()
{
    if (cmajor::symbols::GetBackEnd() == cmajor::symbols::BackEnd::cmsx) return;
    std::string compileUnitId = compileUnitNode->Id();
    std::u32string groupName = U"InitUnwindInfo_" + ToUtf32(compileUnitId);
    FunctionSymbol* functionSymbol = new FunctionSymbol(Span(), boost::uuids::nil_uuid(), groupName);
    functionSymbol->SetParent(&symbolTable.GlobalNs());
    functionSymbol->SetGroupName(groupName);
    functionSymbol->SetAccess(SymbolAccess::public_);
    functionSymbol->SetCDecl();
    functionSymbol->SetNothrow();
    functionSymbol->ComputeMangledName();
    functionSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(functionSymbol);
    initUnwindInfoFunctionSymbol.reset(functionSymbol);
}

void BoundCompileUnit::GenerateCompileUnitInitialization()
{
    if (module.IsCore()) return;
    if (cmajor::symbols::GetBackEnd() == cmajor::symbols::BackEnd::cmsx) return;
    std::string compileUnitId = compileUnitNode->Id();
    std::u32string groupName = U"InitCompileUnit_" + ToUtf32(compileUnitId);
    FunctionSymbol* functionSymbol = new FunctionSymbol(Span(), boost::uuids::nil_uuid(), groupName);
    functionSymbol->SetParent(&symbolTable.GlobalNs());
    functionSymbol->SetGroupName(groupName);
    functionSymbol->SetAccess(SymbolAccess::public_);
    functionSymbol->SetCDecl();
    functionSymbol->SetNothrow();
    functionSymbol->ComputeMangledName();
    functionSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(functionSymbol);
    initCompileUnitFunctionSymbol.reset(functionSymbol);
    Symbol* symbol = symbolTable.GlobalNs().GetContainerScope()->Lookup(U"System.Runtime.PushCompileUnitUnwindInfoInit");
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
        {
            FunctionGroupSymbol* functionGroup = static_cast<FunctionGroupSymbol*>(symbol);
            pushCompileUnitUnwindInfoInitFunctionSymbol = functionGroup->GetFunction();
        }
    }
    if (!pushCompileUnitUnwindInfoInitFunctionSymbol)
    {
        throw std::runtime_error("internal error: 'System.Runtime.PushCompileUnitUnwindInfoInit' symbol not found");
    }
    Span span = initCompileUnitFunctionSymbol->GetSpan();
    boost::uuids::uuid moduleId = initCompileUnitFunctionSymbol->SourceModuleId();
    compileUnitUnwindInfoVarSymbol.reset(new GlobalVariableSymbol(span, moduleId, U"unwindInfoInit_" + ToUtf32(compileUnitId)));
    compileUnitUnwindInfoVarSymbol->SetAccess(SymbolAccess::public_);
    compileUnitUnwindInfoVarSymbol->ComputeMangledName();
    Symbol* cuUnwindInfoTypeSymbol = symbolTable.GlobalNs().GetContainerScope()->Lookup(U"System.Runtime.CompileUnitUnwindInfo");
    if (cuUnwindInfoTypeSymbol && cuUnwindInfoTypeSymbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(cuUnwindInfoTypeSymbol);
        ClassTypeSymbol* classTypeSymbol = classGroup->GetClass(0);
        if (!classTypeSymbol)
        {
            throw std::runtime_error("internal error: 'System.Runtime.CompileUnitUnwindInfo' class not found");
        }
        compileUnitUnwindInfoVarSymbol->SetType(classTypeSymbol);
        BoundGlobalVariable* compileUnitUnwindInfoVar = new BoundGlobalVariable(span, moduleId, compileUnitUnwindInfoVarSymbol.get());
        AddBoundNode(std::unique_ptr<BoundNode>(compileUnitUnwindInfoVar));
    }
    else
    {
        throw std::runtime_error("internal error: 'System.Runtime.CompileUnitUnwindInfo' symbol not found");
    }
    Symbol* initUnwindInfoDelegateSymbol = symbolTable.GlobalNs().GetContainerScope()->Lookup(U"System.Runtime.InitCompileUnitUnwindInfoFunction");
    if (initUnwindInfoDelegateSymbol && initUnwindInfoDelegateSymbol->IsTypeSymbol())
    {
        initUnwindInfoDelegateType = static_cast<TypeSymbol*>(initUnwindInfoDelegateSymbol);
    }
    else
    {
        throw std::runtime_error("internal error: 'System.Runtime.InitCompileUnitUnwindInfoFunction' symbol not found");
    }
}

void BoundCompileUnit::GenerateGlobalInitializationFunction()
{
    if (cmajor::symbols::GetBackEnd() == cmajor::symbols::BackEnd::cmsx) return;
    std::u32string groupName = U"GlobalInitCompileUnits";
    globalInitFunctionSymbol = new FunctionSymbol(Span(), boost::uuids::nil_uuid(), groupName);
    globalInitFunctionSymbol->SetParent(&symbolTable.GlobalNs());
    globalInitFunctionSymbol->SetGroupName(groupName);
    globalInitFunctionSymbol->SetAccess(SymbolAccess::public_);
    globalInitFunctionSymbol->SetCDecl();
    globalInitFunctionSymbol->SetNothrow();
    globalInitFunctionSymbol->ComputeMangledName();
    globalInitFunctionSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));
    symbolTable.SetFunctionIdFor(globalInitFunctionSymbol);
    symbolTable.GlobalNs().AddMember(globalInitFunctionSymbol);
    const std::set<std::string>& compileUnitIds = symbolTable.GetModule()->AllCompileUnitIds();
    for (const std::string& compileUnitId : compileUnitIds)
    {
        Assert(!compileUnitId.empty(), "compiler unit ID is empty");
        std::u32string groupName = U"InitCompileUnit_" + ToUtf32(compileUnitId);
        FunctionSymbol* initFunctionSymbol = new FunctionSymbol(Span(), boost::uuids::nil_uuid(), groupName);
        initFunctionSymbol->SetParent(&symbolTable.GlobalNs());
        initFunctionSymbol->SetGroupName(groupName);
        initFunctionSymbol->SetAccess(SymbolAccess::public_);
        initFunctionSymbol->SetCDecl();
        initFunctionSymbol->SetNothrow();
        initFunctionSymbol->ComputeMangledName();
        initFunctionSymbol->SetReturnType(symbolTable.GetTypeByName(U"void"));
        symbolTable.SetFunctionIdFor(initFunctionSymbol);
        allCompileUnitInitFunctionSymbols.push_back(std::unique_ptr<FunctionSymbol>(initFunctionSymbol));
    }
}

bool BoundCompileUnit::CodeGenerated(FunctionSymbol* functionSymbol) const
{
    return codeGenerated.find(functionSymbol) != codeGenerated.cend();
}

void BoundCompileUnit::SetCodeGenerated(FunctionSymbol* functionSymbol)
{
    codeGenerated.insert(functionSymbol);
}

bool BoundCompileUnit::CanReuse(FunctionSymbol* functionSymbol) const
{
    return canReuse.find(functionSymbol) != canReuse.cend();
}

void BoundCompileUnit::SetCanReuse(FunctionSymbol* functionSymbol)
{
    canReuse.insert(functionSymbol);
}

} } // namespace cmajor::binder
