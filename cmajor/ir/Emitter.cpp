// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/ir/Emitter.hpp>
#include <cmajor/util/Path.hpp>

namespace cmajor { namespace ir {

using namespace cmajor::util;

Emitter::Emitter(llvm::LLVMContext& context_) : 
    context(context_), builder(context), module(nullptr), dataLayout(nullptr), diCompileUnit(nullptr), diFile(nullptr), diBuilder(nullptr), currentCompileUnitNode(nullptr), compileUnitIndex(-1),
    stack(), objectPointer(nullptr), function(nullptr), currentBasicBlock(nullptr), inPrologue(false)
{
}

Emitter::~Emitter()
{
}

void Emitter::SaveObjectPointer(llvm::Value* objectPointer_)
{
    if (objectPointer == nullptr)
    {
        objectPointer = objectPointer_;
    }
}

void Emitter::PushScope(llvm::DIScope* scope)
{
    scopes.push_back(scope);
}

void Emitter::PopScope()
{
    scopes.pop_back();
}

llvm::DIScope* Emitter::CurrentScope()
{
    llvm::DIScope* currentScope = diCompileUnit;
    if (!scopes.empty())
    {
        currentScope = scopes.back();
    }
    return currentScope;
}

int Emitter::GetColumn(const Span& span) const
{
    int column = 1;
    if (currentCompileUnitNode)
    {
        column = currentCompileUnitNode->GetColumn(span);
    }
    return column;
}

llvm::DebugLoc Emitter::GetDebugLocation(const Span& span) 
{
    if (!diCompileUnit || !span.Valid() || !diBuilder) return llvm::DebugLoc();
    int column = GetColumn(span);
    return llvm::DebugLoc::get(span.LineNumber(), column, CurrentScope());
}

void Emitter::ResetCurrentDebugLocation()
{
    currentDebugLocation = llvm::DebugLoc();
    builder.SetCurrentDebugLocation(currentDebugLocation);
}

void Emitter::SetCurrentDebugLocation(const Span& span)
{
    if (!diCompileUnit || !diBuilder) return;
    if (inPrologue || !span.Valid())
    {
        currentDebugLocation = llvm::DebugLoc();
        builder.SetCurrentDebugLocation(currentDebugLocation);
    }
    else
    {
        currentDebugLocation = GetDebugLocation(span);
        builder.SetCurrentDebugLocation(currentDebugLocation);
    }
}

void Emitter::SetDIFile(llvm::DIFile* diFile_)
{
    diFile = diFile_;
}

llvm::DIFile* Emitter::GetFile(int32_t fileIndex)
{
    if (fileIndex == -1)
    {
        return diFile;
    }
    auto it = fileMap.find(fileIndex);
    if (it != fileMap.cend())
    {
        return it->second;
    }
    std::string sourceFilePath = GetSourceFilePath(fileIndex);
    if (sourceFilePath.empty())
    {
        return diFile;
    }
    llvm::DIFile* file = diBuilder->createFile(Path::GetFileName(sourceFilePath), Path::GetDirectoryName(sourceFilePath));
    fileMap[fileIndex] = file;
    return file;
}

llvm::DIType* Emitter::GetDITypeByTypeId(const boost::uuids::uuid& typeId) const
{
    auto it = diTypeTypeIdMap.find(typeId);
    if (it != diTypeTypeIdMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Emitter::SetDITypeByTypeId(const boost::uuids::uuid& typeId, llvm::DIType* diType)
{
    diTypeTypeIdMap[typeId] = diType;
}

llvm::DIDerivedType* Emitter::GetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId) const
{
    auto it = diMemberTypeMap.find(memberVariableId);
    if (it != diMemberTypeMap.cend())
    {
        return it->second;
    }
    return nullptr;
}

void Emitter::SetDIMemberType(const std::pair<boost::uuids::uuid, int32_t>& memberVariableId, llvm::DIDerivedType* diType)
{
    diMemberTypeMap[memberVariableId] = diType;
}

void Emitter::MapFwdDeclaration(llvm::DIType* fwdDeclaration, const boost::uuids::uuid& typeId)
{
    fwdDeclarationMap[fwdDeclaration] = typeId;
}

void Emitter::MapClassPtr(const boost::uuids::uuid& typeId, void* classPtr)
{
    if (classPtrMap.find(typeId) == classPtrMap.cend())
    {
        classPtrMap[typeId] = classPtr;
    }
}

void Emitter::ReplaceForwardDeclarations()
{
    std::unordered_map<llvm::DIType*, boost::uuids::uuid> currentFwdDeclarationMap;
    std::swap(currentFwdDeclarationMap, fwdDeclarationMap);
    while (!currentFwdDeclarationMap.empty())
    {
        for (const auto& p : currentFwdDeclarationMap)
        {
            llvm::DIType* fwdDeclaration = p.first;
            const boost::uuids::uuid& typeId = p.second;
            llvm::DIType* diType = GetDITypeByTypeId(typeId);
            if (!diType)
            {
                auto it = classPtrMap.find(typeId);
                if (it != classPtrMap.cend())
                {
                    void* classPtr = it->second;
                    diType = CreateClassDIType(classPtr);
                }
                else
                {
                    throw std::runtime_error("Emitter::ReplaceForwardDeclarations(): class ptr not mapped");
                }
            }
            fwdDeclaration->replaceAllUsesWith(diType);
        }
        currentFwdDeclarationMap.clear();
        std::swap(currentFwdDeclarationMap, fwdDeclarationMap);
    }
}

llvm::Value* Emitter::GetIrObject(void* symbol) const
{
    auto it = irObjectMap.find(symbol);
    if (it != irObjectMap.cend())
    {
        return it->second;
    }
    else
    {
        throw std::runtime_error("emitter: IR object not found");
    }
}

void Emitter::SetIrObject(void* symbol, llvm::Value* irObject)
{
    irObjectMap[symbol] = irObject;
}

llvm::Type* Emitter::GetIrTypeByTypeId(const boost::uuids::uuid& typeId)
{
    auto it = irTypeTypeIdMap.find(typeId);
    if (it != irTypeTypeIdMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetIrTypeByTypeId(const boost::uuids::uuid& typeId, llvm::Type* irType)
{
    irTypeTypeIdMap[typeId] = irType;
}

llvm::FunctionType* Emitter::GetFunctionIrType(void* symbol) const
{
    auto it = functionIrTypeMap.find(symbol);
    if (it != functionIrTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetFunctionIrType(void* symbol, llvm::FunctionType* irType)
{
    functionIrTypeMap[symbol] = irType;
}

bool Emitter::IsVmtObjectCreated(void* symbol) const
{
    return vmtObjectCreatedSet.find(symbol) != vmtObjectCreatedSet.cend();
}

void Emitter::SetVmtObjectCreated(void* symbol)
{
    vmtObjectCreatedSet.insert(symbol);
}

bool Emitter::IsStaticObjectCreated(void* symbol) const
{
    return staticObjectCreatedSet.find(symbol) != staticObjectCreatedSet.cend();
}

void Emitter::SetStaticObjectCreated(void* symbol)
{
    staticObjectCreatedSet.insert(symbol);
}

llvm::StructType* Emitter::GetStaticObjectType(void* symbol) const
{
    auto it = staticTypeMap.find(symbol);
    if (it != staticTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetStaticObjectType(void* symbol, llvm::StructType* type)
{
    staticTypeMap[symbol] = type;
}

llvm::ArrayType* Emitter::GetVmtObjectType(void* symbol) const
{
    auto it = vmtObjectTypeMap.find(symbol);
    if (it != vmtObjectTypeMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void Emitter::SetVmtObjectType(void* symbol, llvm::ArrayType* vmtObjectType)
{
    vmtObjectTypeMap[symbol] = vmtObjectType;
}

std::string Emitter::GetStaticObjectName(void* symbol) const
{
    auto it = staticObjectNameMap.find(symbol);
    if (it != staticObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetStaticObjectName(void* symbol, const std::string& staticObjectName)
{
    staticObjectNameMap[symbol] = staticObjectName;
}

std::string Emitter::GetVmtObjectName(void* symbol) const
{
    auto it = vmtObjectNameMap.find(symbol);
    if (it != vmtObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetVmtObjectName(void* symbol, const std::string& vmtObjectName)
{
    vmtObjectNameMap[symbol] = vmtObjectName;
}

std::string Emitter::GetImtArrayObjectName(void* symbol) const
{
    auto it = imtArrayObjectNameMap.find(symbol);
    if (it != imtArrayObjectNameMap.cend())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}

void Emitter::SetImtArrayObjectName(void* symbol, const std::string& imtArrayObjectName)
{
    imtArrayObjectNameMap[symbol] = imtArrayObjectName;
}

} } // namespace cmajor::ir
