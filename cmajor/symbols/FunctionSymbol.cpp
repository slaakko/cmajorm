// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/ModuleCache.hpp>
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
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <sstream>

namespace cmajor { namespace symbols {

AccessCheckFunction hasAccess;

void SetAccessCheckFunction(AccessCheckFunction accessCheckFunc)
{
    hasAccess = accessCheckFunc;
}

AccessCheckFunction GetAccessCheckFunction()
{
    if (!hasAccess)
    {
        throw std::runtime_error("access check functio not set");
    }
    return hasAccess;
}

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

using namespace soulng::unicode;

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

FunctionGroupSymbol::FunctionGroupSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : Symbol(SymbolType::functionGroupSymbol, span_, sourceModuleId_,name_)
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
    if (function->IsVarArg())
    {
        auto end = std::remove(varArgFunctions.begin(), varArgFunctions.end(), function);
        varArgFunctions.erase(end, varArgFunctions.end());
    }
    else
    {
        int arity = function->Arity();
        std::vector<FunctionSymbol*>& functionList = arityFunctionListMap[arity];
        auto end = std::remove(functionList.begin(), functionList.end(), function);
        functionList.erase(end, functionList.end());
    }
}

bool FunctionGroupSymbol::IsEmpty() const
{
    for (const auto& p : arityFunctionListMap)
    {
        if (!p.second.empty()) return false;
    }
    return true;
}

FunctionSymbol* FunctionGroupSymbol::GetFunction()
{
    if (arityFunctionListMap.size() == 1)
    {
        std::vector<FunctionSymbol*>& functionList = arityFunctionListMap.begin()->second;
        if (functionList.size() == 1)
        {
            return functionList[0];
        }
    }
    return nullptr;
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

void FunctionGroupSymbol::AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const
{
    for (const auto& p : arityFunctionListMap)
    {
        for (FunctionSymbol* fun : p.second)
        {
            if (fun->IsTemplateSpecialization()) continue;
            if (fun->IsProject())
            {
                std::unique_ptr<sngxml::dom::Element> functionElement = fun->ToDomElement(typeMap);
                element->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement.release()));
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
                throw SymbolCheckException("function group symbol contains empty function pointer", GetSpan(), SourceModuleId());
            }
        }
    }
}

void FunctionGroupSymbol::CheckDuplicateFunctionSymbols()
{
    for (const auto& p : arityFunctionListMap)
    {
        std::set<std::u32string> names;
        for (const auto& functionSymbol : p.second)
        {
            if (!functionSymbol->IsFunctionTemplate() && !functionSymbol->IsTemplateSpecialization())
            {
                if (names.find(functionSymbol->FullName()) != names.cend())
                {
                    throw Exception("function with identical name '" + ToUtf8(functionSymbol->FullName()) + "' already defined.", GetSpan(), SourceModuleId());
                }
                names.insert(functionSymbol->FullName());
            }
        }
    }
}

std::string FunctionGroupSymbol::GetSymbolHelp() const
{
    if (arityFunctionListMap.size() == 1)
    {
        const std::vector<FunctionSymbol*>& v = arityFunctionListMap.begin()->second;
        if (v.size() == 1)
        {
            FunctionSymbol* fun = v.front();
            return fun->GetSymbolHelp();
        }
    }
    int n = 0;
    for (const auto& p : arityFunctionListMap)
    {
        n = n + p.second.size();
    }
    std::string help = "(";
    help.append(GetSymbolCategoryDescription()).append(") ").append(ToUtf8(FullName())).append(" (").append(std::to_string(n)).append(" overloads)");
    return help;
}

bool FunctionGroupSymbol::IsValidCCFunctionGroup(FunctionSymbol* fromFunction) const
{
    AccessCheckFunction canAccess = GetAccessCheckFunction();
    if (!fromFunction) return false;
    if (StartsWith(Name(), U"@")) return false;
    if (StartsWith(Name(), U"operator")) return false;
    for (const auto& p : arityFunctionListMap)
    {
        for (auto functionSymbol : p.second)
        {
            if (canAccess(fromFunction, functionSymbol)) return true;
        }
    }
    return false;
}

std::string FunctionGroupSymbol::GetOverloadList() const
{
    sngxml::dom::Document doc;
    sngxml::dom::Element* root = new sngxml::dom::Element(U"overloadList");
    doc.AppendChild(std::unique_ptr<sngxml::dom::Node>(root));
    for (const auto& p : arityFunctionListMap)
    {
        for (auto functionSymbol : p.second)
        {
            sngxml::dom::Element* overloadElement = new sngxml::dom::Element(U"overload");
            std::u32string overloadName = functionSymbol->FullName(true);
            overloadElement->SetAttribute(U"name", overloadName);
            root->AppendChild(std::unique_ptr<sngxml::dom::Node>(overloadElement));
        }
    }
    std::stringstream s;
    CodeFormatter formatter(s);
    formatter.SetIndentSize(1);
    doc.Write(formatter);
    return s.str();
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

FunctionSymbol::FunctionSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    ContainerSymbol(SymbolType::functionSymbol, span_, sourceModuleId_, name_), functionTemplate(nullptr), master(nullptr),
    functionId(boost::uuids::nil_uuid()), groupName(), parameters(), localVariables(),
    returnType(), flags(FunctionSymbolFlags::none), index(-1), vmtIndex(-1), imtIndex(-1),
    nextTemporaryIndex(0), functionGroup(nullptr), isProgramMain(false), unwindInfoVar(nullptr),
    conversionSourceType(nullptr), conversionTargetType(nullptr)
{
}

FunctionSymbol::FunctionSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    ContainerSymbol(symbolType_, span_, sourceModuleId_, name_), functionTemplate(nullptr), master(nullptr),
    functionId(boost::uuids::nil_uuid()), groupName(), parameters(), localVariables(),
    returnType(), flags(FunctionSymbolFlags::none), index(-1), vmtIndex(-1), imtIndex(-1),
    nextTemporaryIndex(0), functionGroup(nullptr), isProgramMain(false), unwindInfoVar(nullptr),
    conversionSourceType(nullptr), conversionTargetType(nullptr)
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
    boost::uuids::uuid functionTemplateId = boost::uuids::nil_uuid();
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
    boost::uuids::uuid returnTypeId = boost::uuids::nil_uuid();
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
    if (IsConversion())
    {
        writer.GetBinaryWriter().Write(conversionSourceType->TypeId());
        writer.GetBinaryWriter().Write(conversionTargetType->TypeId());
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
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    templateArgumentTypes.resize(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        boost::uuids::uuid templateArgumentId;
        reader.GetBinaryReader().ReadUuid(templateArgumentId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, templateArgumentId, -1 - i);
    }
    if (IsConversion())
    {
        boost::uuids::uuid conversionSourceTypeId;
        reader.GetBinaryReader().ReadUuid(conversionSourceTypeId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, conversionSourceTypeId, 10000);
        boost::uuids::uuid conversionTargetTypeId;
        reader.GetBinaryReader().ReadUuid(conversionTargetTypeId);
        reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, conversionTargetTypeId, 10001);
        reader.AddConversion(this);
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
    if (index == 10000)
    {
        conversionSourceType = typeSymbol;
    }
    else if (index == 10001)
    {
        conversionTargetType = typeSymbol;
    }
    else if (index == 0)
    {
        returnType = typeSymbol;
    }
    else if (index < 0)
    {
        int templateArgumentIndex = -(index + 1);
        if (templateArgumentIndex < 0 || templateArgumentIndex > templateArgumentTypes.size())
        {
            throw Exception("invalid emplace template argument index '" + std::to_string(index) + "'", GetSpan(), SourceModuleId());
        }
        templateArgumentTypes[templateArgumentIndex] = typeSymbol;
    }
    else
    {
        throw Exception("invalid emplace type index '" + std::to_string(index) + "'", GetSpan(), SourceModuleId());
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
            name.append(parameter->GetType()->RemovePointer(GetSpan(), SourceModuleId())->FullName());
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

std::u32string FunctionSymbol::FullName(bool withParamNames) const
{
    std::u32string fullName;
    if (!Parent())
    {
        throw Exception("function symbol has no parent", GetSpan(), SourceModuleId());
    }
    std::u32string parentFullName = Parent()->FullName();
    fullName.append(parentFullName);
    if (!parentFullName.empty())
    {
        fullName.append(1, U'.');
    }
    if (groupName == U"@constructor")
    {
        fullName.append(Parent()->Name());
    }
    else if (groupName == U"@destructor")
    {
        fullName.append(U"~").append(Parent()->Name());
    }
    else
    {
        fullName.append(groupName);
    }
    fullName.append(1, U'(');
    int n = parameters.size();
    int startParamIndex = StartParamIndex();
    for (int i = startParamIndex; i < n; ++i)
    {
        if (i > startParamIndex)
        {
            fullName.append(U", ");
        }
        ParameterSymbol* parameter = parameters[i];
        fullName.append(parameter->GetType()->FullName());
        if (withParamNames)
        {
            fullName.append(U" ").append(parameter->Name());
        }
    }
    fullName.append(1, U')');
    if (IsConst())
    {
        fullName.append(U" const");
    }
    return fullName;
}

std::u32string FunctionSymbol::FullName() const
{
    return FullName(false);
}

std::u32string FunctionSymbol::FullNameWithSpecifiers() const
{
    std::u32string fullNameWithSpecifiers = ToUtf32(SymbolFlagStr(GetStableSymbolFlags()));
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
    std::string specifierStr = SymbolFlagStr(GetStableSymbolFlags());
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

void FunctionSymbol::SetConversionSourceType(TypeSymbol* conversionSourceType_)
{
    conversionSourceType = conversionSourceType_;
}

void FunctionSymbol::SetConversionTargetType(TypeSymbol* conversionTargetType_)
{
    conversionTargetType = conversionTargetType_;
}

void FunctionSymbol::GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
{
    if ((flags & OperationFlags::virtualCall) != OperationFlags::none)
    {
        GenerateVirtualCall(emitter, genObjects, flags, span, moduleId);
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
    void* callee = emitter.GetOrInsertFunction(ToUtf8(MangledName()), functionType, DontThrow());
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
        if (DontThrow() || GetSymbolType() == SymbolType::destructorSymbol || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
    else
    {
        if (DontThrow() || GetSymbolType() == SymbolType::destructorSymbol || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
}

void FunctionSymbol::GenerateVirtualCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span, const boost::uuids::uuid& moduleId)
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
                thisPtr = emitter.CreateBitCast(thisPtr, vmtPtrHolderClass->AddPointer(GetSpan(), SourceModuleId())->IrType(emitter));
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
        if (DontThrow() || GetSymbolType() == SymbolType::destructorSymbol || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
    else
    {
        if (DontThrow() || GetSymbolType() == SymbolType::destructorSymbol || (!handlerBlock && !cleanupBlock && !newCleanupNeeded))
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
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
            if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
            {
                emitter.SetCurrentBasicBlock(nextBlock);
            }
        }
    }
}

std::unique_ptr<Value> FunctionSymbol::ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span, const boost::uuids::uuid& moduleId, Value* receiver) const
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
    return parameters.size() == 1 && groupName == U"@constructor" && parameters[0]->GetType()->PointerCount() == 1 && parameters[0]->GetType()->RemovePointer(GetSpan(), SourceModuleId())->IsClassTypeSymbol();
}

bool FunctionSymbol::IsCopyConstructor() const
{
    return parameters.size() == 2 && groupName == U"@constructor" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan(), SourceModuleId())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddConst(GetSpan(), SourceModuleId())->AddLvalueReference(GetSpan(), SourceModuleId()), parameters[1]->GetType());
}

bool FunctionSymbol::IsMoveConstructor() const
{
    return parameters.size() == 2 && groupName == U"@constructor" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan(), SourceModuleId())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddRvalueReference(GetSpan(), SourceModuleId()), parameters[1]->GetType());
}

bool FunctionSymbol::IsCopyAssignment() const
{
    return parameters.size() == 2 && groupName == U"operator=" &&
        parameters[0]->GetType()->PointerCount() == 1 &&
        parameters[0]->GetType()->RemovePointer(GetSpan(), SourceModuleId())->IsClassTypeSymbol() &&
        TypesEqual(parameters[0]->GetType()->BaseType()->AddConst(GetSpan(), SourceModuleId())->AddLvalueReference(GetSpan(), SourceModuleId()), parameters[1]->GetType());
}

bool FunctionSymbol::IsMoveAssignment() const
{
    return parameters.size() == 2 && groupName == U"operator=" &&
parameters[0]->GetType()->PointerCount() == 1 &&
parameters[0]->GetType()->RemovePointer(GetSpan(), SourceModuleId())->IsClassTypeSymbol() &&
TypesEqual(parameters[0]->GetType()->BaseType()->AddRvalueReference(GetSpan(), SourceModuleId()), parameters[1]->GetType());
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
        throw Exception("only member functions can be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("only member functions can be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("only member functions can be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("only member functions can be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("only constructors can be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        SetExternal();
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("only special member functions can be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("only special member functions can be default", GetSpan(), SourceModuleId());
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
            throw Exception("function symbol cannot be throw and nothrow at the same time", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("only member functions can be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("only member functions can be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        if (!GetGlobalFlag(GlobalFlags::unitTest))
        {
            throw Exception("function symbol cannot be unit_test", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::winapi) != Specifiers::none)
    {
        SetFlag(FunctionSymbolFlags::winapi);
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

LocalVariableSymbol* FunctionSymbol::CreateTemporary(TypeSymbol* type, const Span& span, const boost::uuids::uuid& moduleId)
{
    LocalVariableSymbol* temporary = new LocalVariableSymbol(span, moduleId, U"@t" + ToUtf32(std::to_string(nextTemporaryIndex++)));
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
                    paramType = paramType->AddConst(GetSpan(), SourceModuleId())->AddLvalueReference(GetSpan(), SourceModuleId());
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

std::unique_ptr<sngxml::dom::Element> FunctionSymbol::CreateDomElement(TypeMap& typeMap)
{
    if (IsTemplateSpecialization()) return std::unique_ptr<sngxml::dom::Element>();
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(U"FunctionSymbol"));
    if (returnType)
    {
        std::unique_ptr<sngxml::dom::Element> returnTypeElement(new sngxml::dom::Element(U"returnType"));
        int typeId = typeMap.GetOrInsertType(returnType);
        returnTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(returnTypeElement.release()));
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
                        throw Exception("function symbol " + std::to_string(index) + " not found", GetSpan(), SourceModuleId());
                    }
                }
                else
                {
                    throw Exception("prototype not found", GetSpan(), SourceModuleId());
                }
            }
        }
        else if (functionTemplate)
        {
            return functionTemplate->Id();
        }
        else
        {
            throw Exception("function template expected", GetSpan(), SourceModuleId());
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
        throw SymbolCheckException("function symbol has no id", GetSpan(), SourceModuleId());
    }
    if (groupName.empty())
    {
        throw SymbolCheckException("function symbol has empty group name", GetSpan(), SourceModuleId());
    }
}

void FunctionSymbol::SetPrevUnwindInfoVar(LocalVariableSymbol* prevUnwindInfoVar_)
{
    prevUnwindInfoVar.reset(prevUnwindInfoVar_);
}

void FunctionSymbol::SetUnwindInfoVar(LocalVariableSymbol* unwindInfoVar_)
{
    unwindInfoVar.reset(unwindInfoVar_);
}

int FunctionSymbol::NextTemporaryIndex()
{
    return nextTemporaryIndex++;
}

void FunctionSymbol::CopyFrom(const Symbol* that)
{
    ContainerSymbol::CopyFrom(that);
    const FunctionSymbol* thatFunctionSymbol = static_cast<const FunctionSymbol*>(that);
    functionTemplate = thatFunctionSymbol->functionTemplate;
    master = thatFunctionSymbol->master;
    functionId = thatFunctionSymbol->functionId;
    groupName = thatFunctionSymbol->groupName;
    templateParameters = thatFunctionSymbol->templateParameters;
    templateArgumentTypes = thatFunctionSymbol->templateArgumentTypes;
    parameters = thatFunctionSymbol->parameters;
    if (thatFunctionSymbol->returnParam)
    {
        SetReturnParam(thatFunctionSymbol->returnParam->Clone());
    }
    localVariables = thatFunctionSymbol->localVariables;
    returnType = thatFunctionSymbol->returnType;
    flags = thatFunctionSymbol->flags;
    index = thatFunctionSymbol->index;
    vmtIndex = thatFunctionSymbol->vmtIndex;
    imtIndex = thatFunctionSymbol->imtIndex;
    functionGroup = thatFunctionSymbol->functionGroup;
    isProgramMain = thatFunctionSymbol->isProgramMain;
    conversionSourceType = thatFunctionSymbol->conversionSourceType;
    conversionTargetType = thatFunctionSymbol->conversionTargetType;
}

FunctionSymbol* FunctionSymbol::Copy() const
{
    FunctionSymbol* copy = new FunctionSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    return copy;
}

std::unique_ptr<Symbol> FunctionSymbol::RemoveFromParent()
{
    std::unique_ptr<Symbol> symbol = ContainerSymbol::RemoveFromParent();
    if (functionGroup)
    {
        functionGroup->RemoveFunction(this);
        if (functionGroup->IsEmpty())
        {
            std::unique_ptr<Symbol> functionGroupSymbol = functionGroup->RemoveFromParent();
        }
    }
    return symbol;
}

std::string FunctionSymbol::GetSymbolHelp() const
{
    std::string help = "(";
    help.append(GetSymbolCategoryDescription()).append(") ");
    if (returnType)
    {
        help.append(ToUtf8(returnType->FullName())).append(" ");
    }
    help.append(ToUtf8(FullName(true)));
    return help;
}

StaticConstructorSymbol::StaticConstructorSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::staticConstructorSymbol, span_, sourceModuleId_, name_)
{
    SetGroupName(U"@static_constructor");
}

FunctionSymbol* StaticConstructorSymbol::Copy() const
{
    StaticConstructorSymbol* copy = new StaticConstructorSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    return copy;
}

std::u32string StaticConstructorSymbol::FullNameWithSpecifiers() const
{
    std::u32string fullNameWithSpecifiers = ToUtf32(SymbolFlagStr(GetStableSymbolFlags(), true));
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
        throw Exception("static constructor cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be inline", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be default", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be constexpr", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception("static constructor cannot be throw and nothrow at the same time", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("static constructor cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

ConstructorSymbol::ConstructorSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::constructorSymbol, span_, sourceModuleId_, name_)
{
    SetGroupName(U"@constructor");
}

FunctionSymbol* ConstructorSymbol::Copy() const
{
    ConstructorSymbol* copy = new ConstructorSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    return copy;
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

void ConstructorSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception("ordinary constructor cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("constructor cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("constructor cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("constructor cannot be abstract", GetSpan(), SourceModuleId());
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
        throw Exception("constructor cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (IsInline())
        {
            throw Exception("suppressed member function cannot be inline", GetSpan(), SourceModuleId());
        }
        SetSuppressed();
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (IsSuppressed())
        {
            throw Exception("constructor cannot be default and suppressed at the same time", GetSpan(), SourceModuleId());
        }
        if (IsInline())
        {
            throw Exception("default member function cannot be inline", GetSpan(), SourceModuleId());
        }
        SetDefault();
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("constructor cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception("constructor cannot be throw and nothrow at the same time", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("constructor cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("constructor cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("constructor cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

DestructorSymbol::DestructorSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::destructorSymbol, span_, sourceModuleId_, name_), generated(false)
{
    SetGroupName(U"@destructor");
}

FunctionSymbol* DestructorSymbol::Copy() const
{
    DestructorSymbol* copy = new DestructorSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    copy->generated = generated;
    return copy;
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
        throw Exception("destructor must be public", GetSpan(), SourceModuleId());
    }
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception("destructor cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        SetVirtual();
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        if (IsVirtual())
        {
            throw Exception("destructor cannot be virtual and override at the same time", GetSpan(), SourceModuleId());
        }
        SetOverride();
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("destructor cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception("destructor cannot be inline", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("destructor cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("destructor cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("destructor cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        SetDefault();
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception("destructor cannot be constexpr", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("destructor cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception("destructor is implicitly nothrow", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception("destructor cannot be throw", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("destructor cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception("destructor cannot be const", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("destructor cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

std::u32string DestructorSymbol::CodeName() const
{
    return U"~" + Parent()->CodeName();
}

bool DestructorSymbol::DontThrow() const
{
    if (GetBackEnd() == BackEnd::cmcpp)
    {
        return !HasCleanup();
    }
    return true;
}

MemberFunctionSymbol::MemberFunctionSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::memberFunctionSymbol, span_, sourceModuleId_, name_)
{
}

FunctionSymbol* MemberFunctionSymbol::Copy() const
{
    MemberFunctionSymbol* copy = new MemberFunctionSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    return copy;
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
            throw Exception("member function cannot be virtual and override at the same time", GetSpan(), SourceModuleId());
        }
        SetOverride();
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        if (IsVirtual() || IsOverride())
        {
            throw Exception("member function cannot be abstract and virtual or override at the same time", GetSpan(), SourceModuleId());
        }
        SetAbstract();
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("member function cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("member function cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (IsInline())
        {
            throw Exception("suppressed member function cannot be inline", GetSpan(), SourceModuleId());
        }
        if (GroupName() == U"operator=")
        {
            SetSuppressed();
        }
        else
        {
            throw Exception("only special member functions can be suppressed", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (IsSuppressed())
        {
            throw Exception("member function cannot be default and suppressed at the same time", GetSpan(), SourceModuleId());
        }
        if (IsInline())
        {
            throw Exception("default member function cannot be inline", GetSpan(), SourceModuleId());
        }
        if (GroupName() == U"operator=")
        {
            SetDefault();
        }
        else
        {
            throw Exception("only special member functions can be default", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("member function cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception("member function cannot be throw and nothrow at the same time", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        if (IsVirtualAbstractOrOverride())
        {
            throw Exception("member function cannot be new and virtual, abstract or overridden at the same time", GetSpan(), SourceModuleId());
        }
        SetNew();
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        SetConst();
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("member function cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

int MemberFunctionSymbol::StartParamIndex() const
{
    if (IsStatic())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

ConversionFunctionSymbol::ConversionFunctionSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : 
    FunctionSymbol(SymbolType::conversionFunctionSymbol, span_, sourceModuleId_, name_)
{
    SetConversion();
}

FunctionSymbol* ConversionFunctionSymbol::Copy() const
{
    ConversionFunctionSymbol* copy = new ConversionFunctionSymbol(GetSpan(), SourceModuleId(), Name());
    copy->CopyFrom(this);
    return copy;
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

void ConversionFunctionSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be static", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be virtual", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be override", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be abstract", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        SetInline();
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be explicit", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be external", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be suppressed", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be default", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        SetConstExpr();
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be cdecl", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        SetNothrow();
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (IsNothrow())
        {
            throw Exception("conversion function cannot be throw and nothrow at the same time", GetSpan(), SourceModuleId());
        }
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be new", GetSpan(), SourceModuleId());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        SetConst();
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception("conversion function cannot be unit_test", GetSpan(), SourceModuleId());
    }
}

std::unique_ptr<sngxml::dom::Element> ConversionFunctionSymbol::CreateDomElement(TypeMap& typeMap)
{
    std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(U"ConversionFunctionSymbol"));
    if (ReturnType())
    {
        std::unique_ptr<sngxml::dom::Element> returnTypeElement(new sngxml::dom::Element(U"returnType"));
        int typeId = typeMap.GetOrInsertType(ReturnType());
        returnTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<sngxml::dom::Node>(returnTypeElement.release()));
    }
    return element;
}

FunctionGroupTypeSymbol::FunctionGroupTypeSymbol(FunctionGroupSymbol* functionGroup_, void* boundFunctionGroup_) : 
    TypeSymbol(SymbolType::functionGroupTypeSymbol, functionGroup_->GetSpan(), functionGroup_->SourceModuleId(), functionGroup_->Name()), functionGroup(functionGroup_), boundFunctionGroup(boundFunctionGroup_)
{
    SetModule(functionGroup->GetModule());
}

MemberExpressionTypeSymbol::MemberExpressionTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_, void* boundMemberExpression_) :
    TypeSymbol(SymbolType::memberExpressionTypeSymbol, span_, sourceModuleId_, name_), boundMemberExpression(boundMemberExpression_)
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
