// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/ClassTemplateSpecializationSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/util/Unicode.hpp>
#include <cmajor/util/Sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace cmajor { namespace symbols {

bool FunctionSymbolsEqual::operator()(FunctionSymbol* left, FunctionSymbol* right) const
{
    if (left->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol && right->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* leftSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(left->Parent());
        ClassTemplateSpecializationSymbol* rightSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(right->Parent());
        if (TypesEqual(leftSpecialization, rightSpecialization))
        {
            int leftIndex = left->GetIndex();
            int rightIndex = right->GetIndex();
            if (leftIndex == -1 || rightIndex == -1)
            {
                return left == right;
            }
            else
            {
                return leftIndex == rightIndex;
            }
        }
    }
    else if (left->IsTemplateSpecialization() && right->IsTemplateSpecialization())
    {
        if (left->FunctionTemplate() == right->FunctionTemplate())
        {
            int n = left->TemplateArgumentTypes().size();
            if (right->TemplateArgumentTypes().size() == n)
            {
                bool equal = true;
                for (int i = 0; i < n; ++i)
                {
                    if (!TypesEqual(left->TemplateArgumentTypes()[i], right->TemplateArgumentTypes()[i]))
                    {
                        equal = false;
                        break;
                    }
                }
                return equal;
            }
        }
    }
    else if (GetGlobalFlag(GlobalFlags::release) && !left->IsGeneratedFunction() && left->IsInline() && !right->IsGeneratedFunction() && right->IsInline())
    {
        return left->FunctionId() == right->FunctionId();
    }
    return left == right;
}

size_t FunctionSymbolHash::operator()(FunctionSymbol* fun) const
{
    if (fun->Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(fun->Parent());
        return boost::hash<boost::uuids::uuid>()(specialization->TypeId()) ^ fun->GetIndex();
    }
    else if (fun->IsTemplateSpecialization())
    {
        size_t hashValue = boost::hash<boost::uuids::uuid>()(fun->FunctionTemplate()->FunctionId());
        int n = fun->TemplateArgumentTypes().size();
        for (int i = 0; i < n; ++i)
        {
            hashValue ^= boost::hash<boost::uuids::uuid>()(fun->TemplateArgumentTypes()[i]->TypeId());
        }
        return hashValue;
    }
    else if (GetGlobalFlag(GlobalFlags::release) && !fun->IsGeneratedFunction() && fun->IsInline())
    {
        return boost::hash<boost::uuids::uuid>()(fun->FunctionId());
    }
    return std::hash<FunctionSymbol*>()(fun);
}

void ViableFunctionSet::Insert(FunctionSymbol* fun)
{
    auto it = set.find(fun);
    if (it != set.cend())
    {
        set.erase(it);
    }
    set.insert(fun);
}

using namespace cmajor::unicode;

class OperatorMangleMap
{
public:
    static OperatorMangleMap& Instance() { Assert(instance, "operator mangle map not initialized");  return *instance; }
    static void Init();
    static void Done();
    std::u32string Mangle(const std::u32string& groupName);
private:
    static std::unique_ptr<OperatorMangleMap> instance;
    std::unordered_map<std::u32string, std::u32string> mangleMap;
    OperatorMangleMap();
};

std::unique_ptr<OperatorMangleMap> OperatorMangleMap::instance;

void OperatorMangleMap::Init()
{
    instance.reset(new OperatorMangleMap());
}

void OperatorMangleMap::Done()
{
    instance.reset();
}

OperatorMangleMap::OperatorMangleMap()
{
    mangleMap[U"operator<<"] = U"op_shl";
    mangleMap[U"operator>>"] = U"op_shr";
    mangleMap[U"operator=="] = U"op_eq";
    mangleMap[U"operator="] = U"op_assign";
    mangleMap[U"operator<"] = U"op_less";
    mangleMap[U"operator->"] = U"op_arrow";
    mangleMap[U"operator++"] = U"op_plusplus";
    mangleMap[U"operator--"] = U"op_minusminus";
    mangleMap[U"operator+"] = U"op_plus";
    mangleMap[U"operator-"] = U"op_minus";
    mangleMap[U"operator*"] = U"op_star";
    mangleMap[U"operator/"] = U"op_div";
    mangleMap[U"operator%"] = U"op_rem";
    mangleMap[U"operator&"] = U"op_and";
    mangleMap[U"operator|"] = U"op_or";
    mangleMap[U"operator^"] = U"op_xor";
    mangleMap[U"operator!"] = U"op_not";
    mangleMap[U"operator~"] = U"op_cpl";
    mangleMap[U"operator[]"] = U"op_index";
    mangleMap[U"operator()"] = U"op_apply";
}

std::u32string OperatorMangleMap::Mangle(const std::u32string& groupName)
{
    auto it = mangleMap.find(groupName);
    if (it != mangleMap.cend())
    {
        return it->second;
    }
    else
    {
        return U"operator";
    }
}

FunctionGroupSymbol::FunctionGroupSymbol(const Span& span_, const std::u32string& name_) : Symbol(SymbolType::functionGroupSymbol, span_, name_)
{
}

void FunctionGroupSymbol::ComputeMangledName()
{
    std::u32string mangledName = ToUtf32(TypeString());
    if (Name().find(U"operator") != std::u32string::npos)
    {
        mangledName.append(1, U'_').append(OperatorMangleMap::Instance().Mangle(Name()));
    }
    else if (Name().find(U'@') == std::u32string::npos)
    {
        mangledName.append(1, U'_').append(Name());
    }
    mangledName.append(1, U'_').append(ToUtf32(GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()))));
    SetMangledName(mangledName);
}

void FunctionGroupSymbol::AddFunction(FunctionSymbol* function)
{
    if (function->IsProgramMain()) return;
    Assert(function->GroupName() == Name(), "wrong function group");
    if (function->IsVarArg())
    {
        varArgFunctions.push_back(function);
    }
    else
    {
        int arity = function->Arity();
        std::vector<FunctionSymbol*>& functionList = arityFunctionListMap[arity];
        functionList.push_back(function);
        function->SetFunctionGroup(this);
    }
}

void FunctionGroupSymbol::RemoveFunction(FunctionSymbol* function)
{
    int arity = function->Arity();
    std::vector<FunctionSymbol*>& functionList = arityFunctionListMap[arity];
    auto end = std::remove(functionList.begin(), functionList.end(), function);
    functionList.erase(end, functionList.end());
}

void FunctionGroupSymbol::CollectViableFunctions(int arity, ViableFunctionSet& viableFunctions, Module* module)
{
    auto it = arityFunctionListMap.find(arity);
    if (it != arityFunctionListMap.cend())
    {
        std::vector<FunctionSymbol*>& functionList = it->second;
        for (FunctionSymbol* function : functionList)
        {
            viableFunctions.Insert(function);
        }
    }
    for (FunctionSymbol* varArgFunction : varArgFunctions)
    {
        if (arity >= varArgFunction->Arity())
        {
            viableFunctions.Insert(varArgFunction);
        }
    }
}

bool FunctionGroupSymbol::HasProjectMembers() const
{
    for (const auto& p : arityFunctionListMap)
    {
        for (FunctionSymbol* fun : p.second)
        {
            if (fun->IsTemplateSpecialization()) continue;
            if (fun->IsProject())
            {
                return true;
            }
        }
    }
    return false;
}

void FunctionGroupSymbol::AppendChildElements(dom::Element* element, TypeMap& typeMap) const
{
    for (const auto& p : arityFunctionListMap)
    {
        for (FunctionSymbol* fun : p.second)
        {
            if (fun->IsTemplateSpecialization()) continue;
            if (fun->IsProject())
            {
                std::unique_ptr<dom::Element> functionElement = fun->ToDomElement(typeMap);
                element->AppendChild(std::unique_ptr<dom::Node>(functionElement.release()));
            }
        }
    }
}

void FunctionGroupSymbol::Check()
{
    Symbol::Check();
    for (const auto& p : arityFunctionListMap)
    {
        for (const auto& q : p.second)
        {
            if (!q)
            {
                throw SymbolCheckException(GetRootModuleForCurrentThread(), "function group symbol contains empty function pointer", GetSpan());
            }
        }
    }
}

std::string FunctionSymbolFlagStr(FunctionSymbolFlags flags)
{
    std::string s;
    if ((flags & FunctionSymbolFlags::inline_) != FunctionSymbolFlags::none)
    {
        s.append("inline");
    }
    if ((flags & FunctionSymbolFlags::constExpr) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("constexpr");
    }
    if ((flags & FunctionSymbolFlags::cdecl_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("cdecl");
    }
    if ((flags & FunctionSymbolFlags::suppress) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("suppress");
    }
    if ((flags & FunctionSymbolFlags::default_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("default");
    }
    if ((flags & FunctionSymbolFlags::explicit_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("explicit");
    }
    if ((flags & FunctionSymbolFlags::virtual_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("virtual");
    }
    if ((flags & FunctionSymbolFlags::override_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("override");
    }
    if ((flags & FunctionSymbolFlags::abstract_) != FunctionSymbolFlags::none)
    {
        if (!s.empty())
        {
            s.append(1, ' ');
        }
        s.append("abstract");
    }
    return s;
}

FunctionSymbol::FunctionSymbol(const Span& span_, const std::u32string& name_) : 
    ContainerSymbol(SymbolType::functionSymbol, span_, name_), functionTemplate(nullptr), master(nullptr),
    functionId(boost::uuids::nil_generator()()), groupName(), parameters(), localVariables(), 
    returnType(), flags(FunctionSymbolFlags::none), index(-1), vmtIndex(-1), imtIndex(-1),
    nextTemporaryIndex(0), functionGroup(nullptr), isProgramMain(false)
{
}

FunctionSymbol::FunctionSymbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_) : 
    ContainerSymbol(symbolType_, span_, name_), functionTemplate(nullptr), master(nullptr),
    functionId(boost::uuids::nil_generator()()), groupName(), parameters(), localVariables(), 
    returnType(), flags(FunctionSymbolFlags::none), index(-1), vmtIndex(-1), imtIndex(-1),
    nextTemporaryIndex(0), functionGroup(nullptr), isProgramMain(false)
{
}

void FunctionSymbol::Write(SymbolWriter& writer)
{
    ContainerSymbol::Write(writer);
    Assert(!functionId.is_nil(), "function id not initialized");
    writer.GetBinaryWriter().Write(functionId);
    writer.GetBinaryWriter().Write(index);
    writer.GetBinaryWriter().Write(groupName);
    writer.GetBinaryWriter().Write(static_cast<uint32_t>(flags));
    boost::uuids::uuid functionTemplateId = boost::uuids::nil_generator()();
    if (functionTemplate)
    {
        functionTemplateId = functionTemplate->FunctionId();
    }
    writer.GetBinaryWriter().Write(functionTemplateId);
    if (IsFunctionTemplate() || (GetGlobalFlag(GlobalFlags::release) && IsInline()) || IsConstExpr())
    {
        usingNodes.Write(writer.GetAstWriter());
        Node* node = GetRootModuleForCurrentThread()->GetSymbolTable().GetNode(this);
        writer.GetAstWriter().Write(node);
    }
    boost::uuids::uuid returnTypeId = boost::uuids::nil_generator()();
    if (returnType)
    {
        returnTypeId = returnType->TypeId();
    }
    writer.GetBinaryWriter().Write(returnTypeId);
    writer.GetBinaryWriter().Write(vmtIndex);
    writer.GetBinaryWriter().Write(imtIndex);
    bool hasReturnParam = returnParam != nullptr;
    writer.GetBinaryWriter().Write(hasReturnParam);
    if (hasReturnParam)
    {
        writer.Write(returnParam.get());
    }
    if (constraint)
    {
        writer.GetBinaryWriter().Write(true);
        writer.GetAstWriter().Write(constraint.get());
    }
    else
    {
        writer.GetBinaryWriter().Write(false);
    }
    uint32_t n = templateArgumentTypes.size();
    writer.GetBinaryWriter().WriteULEB128UInt(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        TypeSymbol* templateArgumentType = templateArgumentTypes[i];
        writer.GetBinaryWriter().Write(templateArgumentType->TypeId());
    }
}

void FunctionSymbol::Read(SymbolReader& reader)
{
    ContainerSymbol::Read(reader);
    reader.GetBinaryReader().ReadUuid(functionId);
    index = reader.GetBinaryReader().ReadInt();
    reader.GetSymbolTable()->AddFunctionSymbolToFunctionIdMap(this);
    groupName = reader.GetBinaryReader().ReadUtf32String();
    flags = static_cast<FunctionSymbolFlags>(reader.GetBinaryReader().ReadUInt());
    boost::uuids::uuid functionTemplateId;
    reader.GetBinaryReader().ReadUuid(functionTemplateId);
    if (!functionTemplateId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, functionTemplateId, 0);
    }
    if (IsFunctionTemplate() || (GetGlobalFlag(GlobalFlags::release) && IsInline()) || IsConstExpr())
    {
        usingNodes.Read(reader.GetAstReader());
        Node* node = reader.GetAstReader().ReadNode();
        FunctionNode* funNode = static_cast<FunctionNode*>(node);
        functionNode.reset(funNode);
    }
    boost::uuids::uuid returnTypeId;
    reader.GetBinaryReader().ReadUuid(returnTypeId);
    if (!returnTypeId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, returnTypeId, 0);
    }
    vmtIndex = reader.GetBinaryReader().ReadInt();
    imtIndex = reader.GetBinaryReader().ReadInt();
    bool hasReturnParam = reader.GetBinaryReader().ReadBool();
    if (hasReturnParam)
    {
        returnParam.reset(reader.ReadParameterSymbol(this));
    }
    bool hasConstraint = reader.GetBinaryReader().ReadBool();
    if (hasConstraint)
    {
        constraint.reset(reader.GetAstReader().ReadConstraintNode());
    }
    if (IsConversion())
    {
        reader.AddConversion(this);
    }
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    templateArgumentTypes.resize(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid templateArgumentId;
        reader.GetBinaryReader().ReadUuid(templateArgumentId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, templateArgumentId, -1 - i);
    }
}

void FunctionSymbol::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    if (index == 0)
    {
        functionTemplate = functionSymbol;
    }
    else
    {
        Assert(false, "invalid emplace function index");
    }
}

void FunctionSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_ && !IsGeneratedFunction())
    {
        collector->AddFunction(this);
    }
}

void FunctionSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    if (index == 0)
    {
        returnType = typeSymbol;
    }
    else if (index < 0)
    {
        int templateArgumentIndex = -(index + 1);
        if (templateArgumentIndex < 0 || templateArgumentIndex > templateArgumentTypes.size())
        {
            throw Exception(GetRootModuleForCurrentThread(), "invalid emplace template argument index '" + std::to_string(index) + "'", GetSpan());
        }
        templateArgumentTypes[templateArgumentIndex] = typeSymbol;
    }
    else
    {
        throw Exception(GetRootModuleForCurrentThread(), "invalid emplace type index '" + std::to_string(index) + "'", GetSpan());
    }
}

void FunctionSymbol::AddMember(Symbol* member)
{
    ContainerSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::templateParameterSymbol)
    {
        templateParameters.push_back(static_cast<TemplateParameterSymbol*>(member));
    }
    else if (member->GetSymbolType() == SymbolType::parameterSymbol)
    {
        parameters.push_back(static_cast<ParameterSymbol*>(member));
    }
}

bool FunctionSymbol::IsExportSymbol() const
{
    
    if (IsTemplateSpecialization() && Parent()->GetSymbolType() != SymbolType::classTemplateSpecializationSymbol) return false;
    if (IsGeneratedFunction()) return false;
    if (intrinsic) return false;
    if (IsCopy()) return false;
    return ContainerSymbol::IsExportSymbol();
}

void FunctionSymbol::ComputeName()
{
    std::u32string name;
    name.append(groupName);
    name.append(1, U'(');
    int n = parameters.size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            name.append(U", ");
        }
        ParameterSymbol* parameter = parameters[i];
        if (i == 0 && (groupName == U"@constructor" || groupName == U"operator=" || IsConstructorDestructorOrNonstaticMemberFunction()))
        {
            name.append(parameter->GetType()->RemovePointer(GetSpan())->FullName());
        }
        else
        {
            name.append(parameter->GetType()->FullName());
        }
        name.append(1, U' ');
        name.append(std::u32string(parameter->Name()));
    }
    name.append(1, U')');
    SetName(name);
    if (!IsBasicTypeOperation())
    {
        ComputeMangledName();
    }
}

void FunctionSymbol::ComputeMangledName()
{
    if (IsCDecl())
    {
        SetMangledName(GroupName());
        return;
    }
    std::u32string mangledName = ToUtf32(TypeString());
    if (groupName.find(U"operator") != std::u32string::npos)
    {
        mangledName.append(1, U'_').append(OperatorMangleMap::Instance().Mangle(GroupName()));
    }
    else if (groupName.find(U'@') == std::u32string::npos)
    {
        mangledName.append(1, U'_').append(GroupName());
    }
    SymbolType symbolType = GetSymbolType();
    switch (symbolType)
    {
        case SymbolType::staticConstructorSymbol: case SymbolType::constructorSymbol: case SymbolType::destructorSymbol: case SymbolType::memberFunctionSymbol:
        {
            Symbol* parentClass = Parent();
            mangledName.append(1, U'_').append(parentClass->SimpleName());
        }
    }
    std::string templateArgumentString;
    for (TypeSymbol* templateArgumentType : templateArgumentTypes)
    {
        templateArgumentString.append("_").append(ToUtf8(templateArgumentType->FullName()));
    }
    std::string constraintString;
    if (Constraint())
    {
        constraintString = " " + Constraint()->ToString();
    }
    mangledName.append(1, U'_').append(ToUtf32(GetSha1MessageDigest(ToUtf8(FullNameWithSpecifiers()) + templateArgumentString + constraintString)));
    SetMangledName(mangledName);
}

std::u32string FunctionSymbol::FullName() const
{
    std::u32string fullName;
    std::u32string parentFullName = Parent()->FullName();
    fullName.append(parentFullName);
    if (!parentFullName.empty())
    {
        fullName.append(1, U'.');
    }
    fullName.append(groupName);
    fullName.append(1, U'(');
    int n = parameters.size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            fullName.append(U", ");
        }
        ParameterSymbol* parameter = parameters[i];
        if (i == 0 && (groupName == U"@constructor" || groupName == U"operator=" || IsConstructorDestructorOrNonstaticMemberFunction()))
        {
            fullName.append(parameter->GetType()->RemovePointer(GetSpan())->FullName());
        }
        else
        {
            fullName.append(parameter->GetType()->FullName());
        }
    }
    fullName.append(1, U')');
    return fullName;
}

std::u32string FunctionSymbol::FullNameWithSpecifiers() const
{
    std::u32string fullNameWithSpecifiers = ToUtf32(SymbolFlagStr(GetSymbolFlags()));
    std::u32string f = ToUtf32(FunctionSymbolFlagStr(flags));
    if (!f.empty())
    {
        if (!fullNameWithSpecifiers.empty())
        {
            fullNameWithSpecifiers.append(1, U' ');
        }
        fullNameWithSpecifiers.append(f);
    }
    if (!fullNameWithSpecifiers.empty())
    {
        fullNameWithSpecifiers.append(1, U' ');
    }
    fullNameWithSpecifiers.append(FullName());
    return fullNameWithSpecifiers;
}

std::u32string FunctionSymbol::DocName() const
{
    std::u32string docName = groupName;
    if (!templateParameters.empty())
    {
        docName.append(1, '<');
        bool first = true;
        for (TemplateParameterSymbol* templateParameter : templateParameters)
        {
            if (first)
            {
                first = false;
            }
            else
            {
                docName.append(U", ");
            }
            docName.append(templateParameter->DocName());
        }
        docName.append(1, '>');
    }
    docName.append(1, '(');
    int n = parameters.size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            docName.append(U", ");
        }
        ParameterSymbol* parameter = parameters[i];
        if (parameter->GetType()->Ns() == Ns())
        {
            docName.append(parameter->GetType()->Name());
        }
        else
        {
            docName.append(parameter->GetType()->FullName());
        }
        docName.append(1, ' ').append(parameter->Name());
    }
    docName.append(1, ')');
    return docName;
}

std::string FunctionSymbol::GetSpecifierStr() const
{
    std::string specifierStr = SymbolFlagStr(GetSymbolFlags());
    std::string f = FunctionSymbolFlagStr(flags);
    if (!f.empty())
    {
        if (!specifierStr.empty())
        {
            specifierStr.append(1, ' ');
        }
        specifierStr.append(f);
    }
    return specifierStr;
}

std::string FunctionSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    if (ReturnType())
    {
        syntax.append(ToUtf8(ReturnType()->DocName()));
        syntax.append(1, ' ');
    }
    syntax.append(ToUtf8(DocName()));
    syntax.append(1, ';');
    return syntax;
}

void FunctionSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    if ((flags & OperationFlags::virtualCall) != OperationFlags::none)
    {
        GenerateVirtualCall(emitter, genObjects, flags, span);
        return;
    }
    int na = genObjects.size();
    for (int i = 0; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, flags & OperationFlags::functionCallFlags);
    }
    emitter.SetCurrentDebugLocation(span);
    void* functionType = IrType(emitter);
    void* callee = emitter.GetOrInsertFunction(ToUtf8(MangledName()), functionType);
    std::vector<void*> args;
    int n = parameters.size();
    if (ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ++n;
    }
    args.resize(n);
    for (int i = 0; i < n; ++i)
    {
        void* arg = emitter.Stack().Pop();
        args[n - i - 1] = arg;
    }
    void* handlerBlock = emitter.HandlerBlock();
    void* cleanupBlock = emitter.CleanupBlock();
    bool newCleanupNeeded = emitter.NewCleanupNeeded();
    Pad* currentPad = emitter.CurrentPad();
    std::vector<void*> bundles;
    if (currentPad != nullptr)
    {
        bundles.push_back(currentPad->value);
    }
    if (ReturnType() && ReturnType()->GetSymbolType() != SymbolType::voidTypeSymbol && !ReturnsClassInterfaceOrClassDelegateByValue())
    {
        if (DontThrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateCall(callee, args));
            }
            else
            {
                emitter.Stack().Push(emitter.CreateCallInst(callee, args, bundles, span));
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm)
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateInvoke(callee, nextBlock, unwindBlock, args));
            }
            else
            {
                emitter.Stack().Push(emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span));
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
    else
    {
        if (DontThrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.CreateCall(callee, args);
            }
            else
            {
                emitter.CreateCallInst(callee, args, bundles, span);
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm)
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.CreateInvoke(callee, nextBlock, unwindBlock, args);
            }
            else
            {
                emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span);
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
}

void FunctionSymbol::GenerateVirtualCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span)
{
    int na = genObjects.size();
    Assert(na > 0, "nonempty argument list expected");
    Assert(VmtIndex() != -1, "member function has invalid vmt index");
    GenObject* classPtrArg = genObjects[0];
    TypeSymbol* type = static_cast<TypeSymbol*>(classPtrArg->GetType());
    Assert(type, "type expected");
    Assert(type->BaseType()->IsClassTypeSymbol(), "class type pointer expected");
    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
    ClassTypeSymbol* vmtPtrHolderClass = classType->VmtPtrHolderClass();
    void* callee = nullptr;
    for (int i = 0; i < na; ++i)
    {
        GenObject* genObject = genObjects[i];
        genObject->Load(emitter, OperationFlags::none);
        if (i == 0)
        {
            emitter.Stack().Dup();
            void* thisPtr = emitter.Stack().Pop();
            if (classType != vmtPtrHolderClass)
            {
                thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan())->IrType(emitter));
            }
            void* vmtPtr = emitter.GetVmtPtr(thisPtr, vmtPtrHolderClass->VmtPtrIndex(), classType->VmtPtrType(emitter));
            void* methodPtr = emitter.GetMethodPtr(vmtPtr, VmtIndex() + GetFunctionVmtIndexOffset());
            callee = emitter.CreateBitCast(methodPtr, emitter.GetIrTypeForPtrType(IrType(emitter)));
        }
    }
    std::vector<void*> args;
    int n = Parameters().size();
    if (ReturnsClassInterfaceOrClassDelegateByValue())
    {
        ++n;
    }
    args.resize(n);
    for (int i = 0; i < n; ++i)
    {
        void* arg = emitter.Stack().Pop();
        args[n - i - 1] = arg;
    }
    emitter.SetCurrentDebugLocation(span);
    void* handlerBlock = emitter.HandlerBlock();
    void* cleanupBlock = emitter.CleanupBlock();
    bool newCleanupNeeded = emitter.NewCleanupNeeded();
    std::vector<void*> bundles;
    Pad* currentPad = emitter.CurrentPad();
    if (currentPad != nullptr)
    {
        bundles.push_back(currentPad->value);
    }
    if (ReturnType() && !ReturnType()->IsVoidType() && !ReturnsClassInterfaceOrClassDelegateByValue())
    {
        if (DontThrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateCall(callee, args));
            }
            else
            {
                emitter.Stack().Push(emitter.CreateCallInst(callee, args, bundles, span));
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm)
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.Stack().Push(emitter.CreateInvoke(callee, nextBlock, unwindBlock, args));
            }
            else
            {
                emitter.Stack().Push(emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span));
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
    else
    {
        if (DontThrow() || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
        {
            if (currentPad == nullptr)
            {
                emitter.CreateCall(callee, args);
            }
            else
            {
                emitter.CreateCallInst(callee, args, bundles, span);
            }
        }
        else
        {
            void* nextBlock = nullptr;
            if (GetBackEnd() == BackEnd::llvm)
            {
                nextBlock = emitter.CreateBasicBlock("next");
            }
            if (newCleanupNeeded)
            {
                emitter.CreateCleanup();
                cleanupBlock = emitter.CleanupBlock();
            }
            void* unwindBlock = cleanupBlock;
            if (unwindBlock == nullptr)
            {
                unwindBlock = handlerBlock;
                Assert(unwindBlock, "no unwind block");
            }
            if (currentPad == nullptr)
            {
                emitter.CreateInvoke(callee, nextBlock, unwindBlock, args);
            }
            else
            {
                emitter.CreateInvokeInst(callee, nextBlock, unwindBlock, args, bundles, span);
            }
            if (GetBackEnd() == BackEnd::llvm)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
}

std::unique_ptr<Value> FunctionSymbol::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span) const
{
    return std::unique_ptr<Value>();
}

std::unique_ptr<Value> FunctionSymbol::ConvertValue(const std::unique_ptr<Value>& value) const
{
    return std::unique_ptr<Value>();
}

void FunctionSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("group name: " + ToUtf8(groupName));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    if (!MangledName().empty())
    {
        formatter.WriteLine("mangled name: " + ToUtf8(MangledName()));
    }
    if (constraint)
    {
        formatter.WriteLine("constraint: " + constraint->ToString());
    }
    if (IsConversion())
    {
        formatter.WriteLine("conversion:");
        formatter.IncIndent();
        std::string conversionTypeStr; 
        if (GetConversionType() == ConversionType::implicit_)
        {
            conversionTypeStr = "implicit";
        }
        else if (GetConversionType() == ConversionType::explicit_)
        {
            conversionTypeStr = "explicit";
        }
        formatter.WriteLine("conversion type: " + conversionTypeStr);
        formatter.WriteLine("source type: " + ToUtf8(ConversionSourceType()->FullName()));
        formatter.WriteLine("target type: " + ToUtf8(ConversionTargetType()->FullName()));
        formatter.WriteLine("distance: " + std::to_string(ConversionDistance()));
        formatter.DecIndent();
    }
}

bool FunctionSymbol::IsDefaultConstructor() const
{
    return parameters.size() == 1 && groupName == U"@constructor" && parameters[0]->GetType()->PointerCount() == 1 && parameters[0]->GetType()->RemovePointer(GetSpan())->IsClassTypeSymbol();
}

bool FunctionSymbol::IsCopyConstructor() const
{
    return parameters.size() == 2 && groupName == U"@constructor" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddConst(GetSpan())->AddLvalueReference(GetSpan()), parameters[1]->GetType());
}

bool FunctionSymbol::IsMoveConstructor() const
{
    return parameters.size() == 2 && groupName == U"@constructor" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddRvalueReference(GetSpan()), parameters[1]->GetType());
}

bool FunctionSymbol::IsCopyAssignment() const
{
    return parameters.size() == 2 && groupName == U"operator=" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddConst(GetSpan())->AddLvalueReference(GetSpan()), parameters[1]->GetType());
}

bool FunctionSymbol::IsMoveAssignment() const
{
    return parameters.size() == 2 && groupName == U"operator=" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddRvalueReference(GetSpan()), parameters[1]->GetType());
}

void FunctionSymbol::AddLocalVariable(LocalVariableSymbol* localVariable)
{
    localVariables.push_back(localVariable);
}

bool FunctionSymbol::ReturnsClassInterfaceOrClassDelegateByValue() const
{
    return returnType && (returnType->IsClassTypeSymbol() || returnType->GetSymbolType() == SymbolType::classDelegateTypeSymbol || returnType->GetSymbolType() == SymbolType::interfaceTypeSymbol);
}

void FunctionSymbol::SetReturnParam(ParameterSymbol* returnParam_)
{
    returnParam.reset(returnParam_);
}

void FunctionSymbol::SetGroupName(const std::u32string& groupName_)
{
    groupName = groupName_;
}

void FunctionSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only constructors can be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        SetExternal();
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only special member functions can be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only special member functions can be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        SetCDecl();
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "function symbol cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "only member functions can be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        if (!GetGlobalFlag(GlobalFlags::unitTest))
        {
            throw Exception(GetRootModuleForCurrentThread(), "function symbol cannot be unit_test", GetSpan());
        }
    }
}

void FunctionSymbol::CloneUsingNodes(const std::vector<Node*>& usingNodes_)
{
    CloneContext cloneContext;
    for (Node* usingNode : usingNodes_)
    {
        usingNodes.Add(usingNode->Clone(cloneContext));
    }
}

LocalVariableSymbol* FunctionSymbol::CreateTemporary(TypeSymbol* type, const Span& span)
{
    LocalVariableSymbol* temporary = new LocalVariableSymbol(span, U"@t" + ToUtf32(std::to_string(nextTemporaryIndex++)));
    temporary->SetType(type);
    AddMember(temporary);
    AddLocalVariable(temporary);
    return temporary;
}

std::vector<LocalVariableSymbol*> FunctionSymbol::CreateTemporariesTo(FunctionSymbol* currentFunction)
{
    return std::vector<LocalVariableSymbol*>();
}

void* FunctionSymbol::IrType(Emitter& emitter)
{
    void* localIrType = emitter.GetFunctionIrType(this);
    if (!localIrType)
    {
        void* retType = emitter.GetIrTypeForVoid();
        if (returnType && returnType->GetSymbolType() != SymbolType::voidTypeSymbol && !ReturnsClassInterfaceOrClassDelegateByValue())
        {
            retType = returnType->IrType(emitter);
        }
        bool interfaceMemFun = Parent()->GetSymbolType() == SymbolType::interfaceTypeSymbol;
        std::vector<void*> paramTypes;
        int np = parameters.size();
        for (int i = 0; i < np; ++i)
        {
            if (i == 0 && interfaceMemFun)
            {
                paramTypes.push_back(emitter.GetIrTypeForVoidPtrType());
            }
            else
            {
                ParameterSymbol* parameter = parameters[i];
                TypeSymbol* paramType = parameter->GetType();
                if (paramType->IsClassTypeSymbol() || paramType->GetSymbolType() == SymbolType::classDelegateTypeSymbol || paramType->GetSymbolType() == SymbolType::interfaceTypeSymbol)
                {
                    paramType = paramType->AddConst(GetSpan())->AddLvalueReference(GetSpan());
                }
                paramTypes.push_back(paramType->IrType(emitter));
            }
        }
        if (returnParam)
        {
            paramTypes.push_back(returnParam->GetType()->IrType(emitter));
        }
        localIrType = emitter.GetIrTypeForFunction(retType, paramTypes);
        emitter.SetFunctionIrType(this, localIrType);
    }
    return localIrType;
}

std::unique_ptr<dom::Element> FunctionSymbol::CreateDomElement(TypeMap& typeMap)
{
    if (IsTemplateSpecialization()) return std::unique_ptr<dom::Element>();
    std::unique_ptr<dom::Element> element(new dom::Element(U"FunctionSymbol"));
    if (returnType)
    {
        std::unique_ptr<dom::Element> returnTypeElement(new dom::Element(U"returnType"));
        int typeId = typeMap.GetOrInsertType(returnType);
        returnTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<dom::Node>(returnTypeElement.release()));
    }
    return element;
}

std::u32string FunctionSymbol::Id() const
{
    if (IsFunctionTemplate())
    {
        return ContainerSymbol::Id();
    }
    else if (IsTemplateSpecialization())
    {
        if (Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            Symbol* parent = const_cast<Symbol*>(Parent());
            ClassTemplateSpecializationSymbol* parentSpecialization = static_cast<ClassTemplateSpecializationSymbol*>(parent);
            if (parentSpecialization->IsPrototype())
            {
                return ContainerSymbol::Id();
            }
            else
            {
                ClassTypeSymbol* classTemplate = parentSpecialization->GetClassTemplate();
                ClassTemplateSpecializationSymbol* prototype = classTemplate->Prototype();
                if (prototype)
                {
                    FunctionSymbol* functionSymbol = prototype->GetFunctionByIndex(index);
                    if (functionSymbol)
                    {
                        return functionSymbol->Id();
                    }
                    else
                    {
                        throw Exception(GetRootModuleForCurrentThread(), "function symbol " + std::to_string(index) + " not found", GetSpan());
                    }
                }
                else
                {
                    throw Exception(GetRootModuleForCurrentThread(), "prototype not found", GetSpan());
                }
            }
        }
        else if (functionTemplate)
        {
            return functionTemplate->Id();
        }
        else
        {
            throw Exception(GetRootModuleForCurrentThread(), "function template expected", GetSpan());
        }
    }
    else
    {
        return ContainerSymbol::Id();
    }
}

void FunctionSymbol::SetTemplateArgumentTypes(const std::vector<TypeSymbol*>& templateArgumentTypes_)
{
    templateArgumentTypes = templateArgumentTypes_;
}

void FunctionSymbol::Check()
{
    ContainerSymbol::Check();
    if (functionId.is_nil())
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "function symbol has no id", GetSpan());
    }
    if (groupName.empty())
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "function symbol has empty group name", GetSpan());
    }
}

StaticConstructorSymbol::StaticConstructorSymbol(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::staticConstructorSymbol, span_, name_)
{
    SetGroupName(U"@static_constructor");
}

std::u32string StaticConstructorSymbol::FullNameWithSpecifiers() const
{
    std::u32string fullNameWithSpecifiers = ToUtf32(SymbolFlagStr(GetSymbolFlags(), true));
    std::u32string f = ToUtf32(FunctionSymbolFlagStr(GetFunctionSymbolFlags()));
    if (!f.empty())
    {
        if (!fullNameWithSpecifiers.empty())
        {
            fullNameWithSpecifiers.append(1, U' ');
        }
        fullNameWithSpecifiers.append(f);
    }
    if (!fullNameWithSpecifiers.empty())
    {
        fullNameWithSpecifiers.append(1, U' ');
    }
    fullNameWithSpecifiers.append(FullName());
    return fullNameWithSpecifiers;
}

std::u32string StaticConstructorSymbol::CodeName() const
{
    return Parent()->CodeName();
}

void StaticConstructorSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    SetStatic();
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "static constructor cannot be unit_test", GetSpan());
    }
}

ConstructorSymbol::ConstructorSymbol(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::constructorSymbol, span_, name_)
{
    SetGroupName(U"@constructor");
}

std::string ConstructorSymbol::TypeString() const 
{
    if (IsDefaultConstructor())
    {
        return "default_constructor";
    }
    else if (IsCopyConstructor())
    {
        return "copy_constructor";
    }
    else if (IsMoveConstructor())
    {
        return "move_constructor";
    }
    else
    {
        return "constructor";
    }
}

std::u32string ConstructorSymbol::DocName() const
{
    std::u32string docName;
    docName.append(Parent()->DocName());
    docName.append(1, '(');
    int n = Parameters().size();
    for (int i = 1; i < n; ++i)
    {
        if (i > 1)
        {
            docName.append(U", ");
        }
        ParameterSymbol* parameter = Parameters()[i];
        if (parameter->GetType()->Ns() == Ns())
        {
            docName.append(parameter->GetType()->Name());
        }
        else
        {
            docName.append(parameter->GetType()->FullName());
        }
        docName.append(1, ' ').append(parameter->Name());
    }
    docName.append(1, ')');
    return docName;
}

std::u32string ConstructorSymbol::CodeName() const
{
    return Parent()->CodeName();
}

uint8_t ConstructorSymbol::ConversionDistance() const
{
    return 5;
}

TypeSymbol* ConstructorSymbol::ConversionSourceType() const
{
    return Parameters()[1]->GetType();
}

TypeSymbol* ConstructorSymbol::ConversionTargetType() const
{
    return Parameters()[0]->GetType()->RemovePointer(GetSpan());
}

void ConstructorSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "ordinary constructor cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        SetExplicit();
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (IsInline())
        {
            throw Exception(GetRootModuleForCurrentThread(), "suppressed member function cannot be inline", GetSpan());
        }
        SetSuppressed();
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (IsSuppressed())
        {
            throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be default and suppressed at the same time", GetSpan());
        }
        if (IsInline())
        {
            throw Exception(GetRootModuleForCurrentThread(), "default member function cannot be inline", GetSpan());
        }
        SetDefault();
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constructor cannot be unit_test", GetSpan());
    }
}

DestructorSymbol::DestructorSymbol(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::destructorSymbol, span_, name_), generated(false)
{
    SetGroupName(U"@destructor");
}

bool DestructorSymbol::IsExportSymbol() const
{
    if (Parent()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        const ClassTemplateSpecializationSymbol* classTemplateSpecialization = static_cast<const ClassTemplateSpecializationSymbol*>(Parent());
        if (classTemplateSpecialization->IsPrototype())
        {
            return false;
        }
    }
    return true;
}

void DestructorSymbol::Write(SymbolWriter& writer)
{
    FunctionSymbol::Write(writer);
    writer.GetBinaryWriter().Write(generated);
}

void DestructorSymbol::Read(SymbolReader& reader)
{
    FunctionSymbol::Read(reader);
    generated = reader.GetBinaryReader().ReadBool();
}

void DestructorSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    if (accessSpecifiers != Specifiers::public_)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor must be public", GetSpan());
    }
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        SetVirtual();
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        if (IsVirtual())
        {
            throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be virtual and override at the same time", GetSpan());
        }
        SetOverride();
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        SetDefault();
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor is implicitly nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "destructor cannot be unit_test", GetSpan());
    }
}

std::u32string DestructorSymbol::CodeName() const
{
    return U"~" + Parent()->CodeName();
}

MemberFunctionSymbol::MemberFunctionSymbol(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::memberFunctionSymbol, span_, name_)
{
}

std::string MemberFunctionSymbol::TypeString() const 
{ 
    if (IsCopyAssignment())
    {
        return "copy_assignment";
    }
    else if (IsMoveAssignment())
    {
        return "move_assignment";
    }
    else
    {
        return "member_function";
    }
}

std::u32string MemberFunctionSymbol::DocName() const
{
    std::u32string docName;
    docName.append(GroupName());
    docName.append(1, '(');
    int n = Parameters().size();
    int start = 1;
    if (IsStatic())
    {
        start = 0;
    }
    for (int i = start; i < n; ++i)
    {
        if (i > start)
        {
            docName.append(U", ");
        }
        ParameterSymbol* parameter = Parameters()[i];
        if (parameter->GetType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* classTemplateSpecializationSymbol = static_cast<ClassTemplateSpecializationSymbol*>(parameter->GetType());
            if (classTemplateSpecializationSymbol->GetClassTemplate()->Ns() == Ns())
            {
                docName.append(classTemplateSpecializationSymbol->Name());
            }
            else
            {
                docName.append(classTemplateSpecializationSymbol->FullName());
            }
        }
        else
        {
            if (parameter->GetType()->Ns() == Ns())
            {
                docName.append(parameter->GetType()->Name());
            }
            else
            {
                docName.append(parameter->GetType()->FullName());
            }
        }
        docName.append(1, ' ').append(parameter->Name());
    }
    docName.append(1, ')');
    if (IsConst())
    {
        docName.append(U" const");
    }
    return docName;
}

void MemberFunctionSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        SetStatic();
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        SetVirtual();
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        if (IsVirtual())
        {
            throw Exception(GetRootModuleForCurrentThread(), "member function cannot be virtual and override at the same time", GetSpan());
        }
        SetOverride();
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        if (IsVirtual() || IsOverride())
        {
            throw Exception(GetRootModuleForCurrentThread(), "member function cannot be abstract and virtual or override at the same time", GetSpan());
        }
        SetAbstract();
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member function cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member function cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (IsInline())
        {
            throw Exception(GetRootModuleForCurrentThread(), "suppressed member function cannot be inline", GetSpan());
        }
        if (GroupName() == U"operator=")
        {
            SetSuppressed();
        }
        else
        {
            throw Exception(GetRootModuleForCurrentThread(), "only special member functions can be suppressed", GetSpan());
        }
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (IsSuppressed())
        {
            throw Exception(GetRootModuleForCurrentThread(), "member function cannot be default and suppressed at the same time", GetSpan());
        }
        if (IsInline())
        {
            throw Exception(GetRootModuleForCurrentThread(), "default member function cannot be inline", GetSpan());
        }
        if (GroupName() == U"operator=")
        {
            SetDefault();
        }
        else
        {
            throw Exception(GetRootModuleForCurrentThread(), "only special member functions can be default", GetSpan());
        }
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member function cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "member function cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        if (IsVirtualAbstractOrOverride())
        {
            throw Exception(GetRootModuleForCurrentThread(), "member function cannot be new and virtual, abstract or overridden at the same time", GetSpan());
        }
        SetNew();
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        SetConst();
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member function cannot be unit_test", GetSpan());
    }
}

ConversionFunctionSymbol::ConversionFunctionSymbol(const Span& span_, const std::u32string& name_) : FunctionSymbol(SymbolType::conversionFunctionSymbol, span_, name_)
{
    SetConversion();
}

std::u32string ConversionFunctionSymbol::DocName() const
{
    std::u32string docName;
    docName.append(U"operator ");
    TypeSymbol* type = ReturnType();
    if (type->Ns() == Ns())
    {
        docName.append(type->Name());
    }
    else
    {
        docName.append(type->FullName());
    }
    docName.append(U"()");
    if (IsConst())
    {
        docName.append(U" const");
    }
    return docName;
}

TypeSymbol* ConversionFunctionSymbol::ConversionSourceType() const
{
    Assert(Parent()->IsTypeSymbol(), "type symbol expected");
    return const_cast<TypeSymbol*>(static_cast<const TypeSymbol*>(Parent()));
}

TypeSymbol* ConversionFunctionSymbol::ConversionTargetType() const
{
    return ReturnType();
}

void ConversionFunctionSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be throw and nothrow at the same time", GetSpan());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        SetConst();
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "conversion function cannot be unit_test", GetSpan());
    }
}

std::unique_ptr<dom::Element> ConversionFunctionSymbol::CreateDomElement(TypeMap& typeMap)
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"ConversionFunctionSymbol"));
    if (ReturnType())
    {
        std::unique_ptr<dom::Element> returnTypeElement(new dom::Element(U"returnType"));
        int typeId = typeMap.GetOrInsertType(ReturnType());
        returnTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<dom::Node>(returnTypeElement.release()));
    }
    return element;
}

FunctionGroupTypeSymbol::FunctionGroupTypeSymbol(FunctionGroupSymbol* functionGroup_, void* boundFunctionGroup_) : 
    TypeSymbol(SymbolType::functionGroupTypeSymbol, Span(), functionGroup_->Name()), functionGroup(functionGroup_), boundFunctionGroup(boundFunctionGroup_)
{
    SetModule(functionGroup->GetModule());
}

MemberExpressionTypeSymbol::MemberExpressionTypeSymbol(const Span& span_, const std::u32string& name_, void* boundMemberExpression_) :
    TypeSymbol(SymbolType::memberExpressionTypeSymbol, span_, name_), boundMemberExpression(boundMemberExpression_)
{
}

void InitFunctionSymbol()
{
    OperatorMangleMap::Init();
}

void DoneFunctionSymbol()
{
    OperatorMangleMap::Done();
}

} } // namespace cmajor::symbols
