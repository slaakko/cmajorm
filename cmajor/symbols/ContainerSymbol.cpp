// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ContainerSymbol.hpp>
#include <cmajor/symbols/FunctionSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/ConceptSymbol.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <soulng/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

ContainerSymbol::ContainerSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : Symbol(symbolType_, span_, sourceModuleId_, name_)
{
    containerScope.SetContainer(this);
}

void ContainerSymbol::Write(SymbolWriter& writer)
{
    Symbol::Write(writer);
    std::vector<Symbol*> exportSymbols;
    for (const std::unique_ptr<Symbol>& member : members)
    {
        if (member->IsExportSymbol())
        {
            exportSymbols.push_back(member.get());
        }
    }
    uint32_t n = uint32_t(exportSymbols.size());
    writer.GetBinaryWriter().WriteULEB128UInt(n);
    for (uint32_t i = 0; i < n; ++i)
    {
        writer.Write(exportSymbols[i]);
    }
}

void ContainerSymbol::Read(SymbolReader& reader)
{
    Symbol::Read(reader);
    uint32_t n = reader.GetBinaryReader().ReadULEB128UInt();
    for (uint32_t i = 0; i < n; ++i)
    {
        Symbol* symbol = reader.ReadSymbol(this);
        AddMember(symbol);
    }
}

void ContainerSymbol::AddMember(Symbol* member)
{
#ifdef IMMUTABLE_MODULE_CHECK
    if (GetModule() && GetModule()->IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), GetModule(), GetSpan(), member->GetSpan());
    }
#endif
    if (IsImmutable())
    {
        throw ModuleImmutableException(GetRootModuleForCurrentThread(), GetModule(), GetSpan(), member->GetSpan());
    }
    if (GetModule())
    {
        member->SetModule(GetModule());
    }
    member->SetParent(this);
    member->SetSymbolIndex(members.size());
    members.push_back(std::unique_ptr<Symbol>(member));
    if (member->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(member);
        FunctionGroupSymbol* functionGroupSymbol = MakeFunctionGroupSymbol(functionSymbol->GroupName(), functionSymbol->GetSpan(), functionSymbol->SourceModuleId());
        functionGroupSymbol->AddFunction(functionSymbol);
        functionIndexMap[functionSymbol->GetIndex()] = functionSymbol;
    }
    else if (member->GetSymbolType() == SymbolType::conceptSymbol)
    {
        ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(member);
        ConceptGroupSymbol* conceptGroupSymbol = MakeConceptGroupSymbol(conceptSymbol->GroupName(), conceptSymbol->GetSpan(), conceptSymbol->SourceModuleId());
        conceptGroupSymbol->AddConcept(conceptSymbol);
    }
    else if (member->GetSymbolType() == SymbolType::classTypeSymbol || member->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(member);
        ClassGroupTypeSymbol* classGroupTypeSymbol = MakeClassGroupTypeSymbol(classTypeSymbol->GroupName(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
        classGroupTypeSymbol->AddClass(classTypeSymbol);
    }
    else if (member->GetSymbolType() == SymbolType::globalVariableSymbol)
    {
        GlobalVariableSymbol* globalVariableSymbol = static_cast<GlobalVariableSymbol*>(member);
        GlobalVariableGroupSymbol* globalVariableGroupSymbol = MakeGlobalVariableGroupSymbol(globalVariableSymbol->GroupName(), globalVariableSymbol->GetSpan(), globalVariableSymbol->SourceModuleId());
        globalVariableGroupSymbol->AddGlobalVariable(globalVariableSymbol);
    }
    else
    {
        containerScope.Install(member);
    }
}

void ContainerSymbol::AddOwnedMember(Symbol* ownedMember)
{
    if (ownedMember->IsFunctionSymbol())
    {
        FunctionSymbol* functionSymbol = static_cast<FunctionSymbol*>(ownedMember);
        FunctionGroupSymbol* functionGroupSymbol = MakeFunctionGroupSymbol(functionSymbol->GroupName(), functionSymbol->GetSpan(), functionSymbol->SourceModuleId());
        functionGroupSymbol->AddFunction(functionSymbol);
        functionIndexMap[functionSymbol->GetIndex()] = functionSymbol;
    }
    else if (ownedMember->GetSymbolType() == SymbolType::conceptSymbol)
    {
        ConceptSymbol* conceptSymbol = static_cast<ConceptSymbol*>(ownedMember);
        ConceptGroupSymbol* conceptGroupSymbol = MakeConceptGroupSymbol(conceptSymbol->GroupName(), conceptSymbol->GetSpan(), conceptSymbol->SourceModuleId());
        conceptGroupSymbol->AddConcept(conceptSymbol);
    }
    else if (ownedMember->GetSymbolType() == SymbolType::classTypeSymbol || ownedMember->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(ownedMember);
        ClassGroupTypeSymbol* classGroupTypeSymbol = MakeClassGroupTypeSymbol(classTypeSymbol->GroupName(), classTypeSymbol->GetSpan(), classTypeSymbol->SourceModuleId());
        classGroupTypeSymbol->AddClass(classTypeSymbol);
    }
    else if (ownedMember->GetSymbolType() == SymbolType::globalVariableSymbol)
    {
        GlobalVariableSymbol* globalVariableSymbol = static_cast<GlobalVariableSymbol*>(ownedMember);
        GlobalVariableGroupSymbol* globalVariableGroupSymbol = MakeGlobalVariableGroupSymbol(globalVariableSymbol->GroupName(), globalVariableSymbol->GetSpan(), globalVariableSymbol->SourceModuleId());
        globalVariableGroupSymbol->AddGlobalVariable(globalVariableSymbol);
    }
    else
    {
        containerScope.Install(ownedMember);
    }
}

std::unique_ptr<Symbol> ContainerSymbol::RemoveMember(int symbolIndex)
{
    if (symbolIndex == -1)
    {
        throw std::runtime_error("internal error: ContainerSymboil::RemoveMember(): symbol index is -1");
    }
    std::unique_ptr<Symbol> symbol = std::move(members[symbolIndex]);
    members.erase(members.begin() + symbolIndex);
    for (int i = symbolIndex; i < members.size(); ++i)
    {
        members[i]->SetSymbolIndex(i);
    }
    if (symbol->IsInstalled())
    {
        containerScope.Uninstall(symbol.get());
    }
    return symbol;
}

void ContainerSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject())
    {
        for (std::unique_ptr<Symbol>& member : members)
        {
            member->Accept(collector);
        }
    }
}

void ContainerSymbol::Clear()
{
    containerScope.Clear();
    members.clear();
}

FunctionGroupSymbol* ContainerSymbol::MakeFunctionGroupSymbol(const std::u32string& groupName, const Span& span, const boost::uuids::uuid& sourceModuleId)
{
    Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        FunctionGroupSymbol* functionGroupSymbol = new FunctionGroupSymbol(span, sourceModuleId, groupName);
        AddMember(functionGroupSymbol);
        return functionGroupSymbol;
    }
    if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol)
    {
        return static_cast<FunctionGroupSymbol*>(symbol);
    }
    else
    {
        throw Exception("name of symbol '" + ToUtf8(symbol->FullName()) + "' conflicts with a function group '" + ToUtf8(groupName) + "'", symbol->GetSpan(), symbol->SourceModuleId(), span, sourceModuleId);
    }
}

ConceptGroupSymbol* ContainerSymbol::MakeConceptGroupSymbol(const std::u32string& groupName, const Span& span, const boost::uuids::uuid& sourceModuleId)
{
    Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        ConceptGroupSymbol* conceptGroupSymbol = new ConceptGroupSymbol(span, sourceModuleId, groupName);
        AddMember(conceptGroupSymbol);
        return conceptGroupSymbol;
    }
    if (symbol->GetSymbolType() == SymbolType::conceptGroupSymbol)
    {
        return static_cast<ConceptGroupSymbol*>(symbol);
    }
    else
    {
        throw Exception("name of symbol '" + ToUtf8(symbol->FullName()) + "' conflicts with a concept group '" + ToUtf8(groupName) + "'", symbol->GetSpan(), symbol->SourceModuleId(), span, sourceModuleId);
    }
}

ClassGroupTypeSymbol* ContainerSymbol::MakeClassGroupTypeSymbol(const std::u32string& groupName, const Span& span, const boost::uuids::uuid& sourceModuleId)
{
    Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        ClassGroupTypeSymbol* classGroupTypeSymbol = new ClassGroupTypeSymbol(span, sourceModuleId, groupName);
        GetRootModuleForCurrentThread()->GetSymbolTable().SetTypeIdFor(classGroupTypeSymbol);
        AddMember(classGroupTypeSymbol);
        return classGroupTypeSymbol;
    }
    if (symbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        return static_cast<ClassGroupTypeSymbol*>(symbol);
    }
    else
    {
        throw Exception("name of symbol '" + ToUtf8(symbol->FullName()) + "' conflicts with a class group '" + ToUtf8(groupName) + "'", symbol->GetSpan(), symbol->SourceModuleId(), span, sourceModuleId);
    }
}

GlobalVariableGroupSymbol* ContainerSymbol::MakeGlobalVariableGroupSymbol(const std::u32string& groupName, const Span& span, const boost::uuids::uuid& sourceModuleId)
{
    Symbol* symbol = containerScope.Lookup(groupName);
    if (!symbol)
    {
        GlobalVariableGroupSymbol* globalVariableGroupSymbol = new GlobalVariableGroupSymbol(span, sourceModuleId, groupName);
        AddMember(globalVariableGroupSymbol);
        return globalVariableGroupSymbol;
    }
    if (symbol->GetSymbolType() == SymbolType::globalVariableGroupSymbol)
    {
        return static_cast<GlobalVariableGroupSymbol*>(symbol);
    }
    else
    {
        throw Exception("name of symbol '" + ToUtf8(symbol->FullName()) + "' conflicts with a global variable group '" + ToUtf8(groupName) + "'", symbol->GetSpan(), symbol->SourceModuleId(), span, sourceModuleId);
    }
}

void ContainerSymbol::AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const
{
    for (const std::unique_ptr<Symbol>& member : members)
    {
        if (member->IsFunctionSymbol()) continue;
        if (member->IsClassTypeSymbol()) continue;
        if (member->GetSymbolType() == SymbolType::conceptSymbol) continue;
        if (member->GetSymbolType() == SymbolType::globalVariableSymbol) continue;
        if (member->GetSymbolType() != SymbolType::namespaceSymbol && !member->IsProject()) continue;
        if (member->GetSymbolType() == SymbolType::namespaceSymbol ||
            member->GetSymbolType() == SymbolType::classGroupTypeSymbol ||
            member->GetSymbolType() == SymbolType::functionGroupSymbol ||
            member->GetSymbolType() == SymbolType::conceptGroupSymbol ||
            member->GetSymbolType() == SymbolType::globalVariableGroupSymbol)
        {
            if (!member->HasProjectMembers()) continue;
        }
        std::unique_ptr<sngxml::dom::Element> memberElement = member->ToDomElement(typeMap);
        if (memberElement)
        {
            element->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberElement.release()));
        }
    }
}
bool ContainerSymbol::HasProjectMembers() const
{
    if (FullName() == U"System.Meta") return false;
    for (const std::unique_ptr<Symbol>& member : members)
    { 
        if (member->GetSymbolType() == SymbolType::namespaceSymbol || 
            member->GetSymbolType() == SymbolType::classGroupTypeSymbol || 
            member->GetSymbolType() == SymbolType::functionGroupSymbol || 
            member->GetSymbolType() == SymbolType::conceptGroupSymbol ||
            member->GetSymbolType() == SymbolType::globalVariableGroupSymbol)
        {
            if (member->HasProjectMembers()) return true;
        }
        else
        {
            if (member->IsFunctionSymbol())
            {
                FunctionSymbol* fun = static_cast<FunctionSymbol*>(member.get());
                if (fun->IsTemplateSpecialization()) continue;
            }
            if (member->IsProject()) return true;
        }
    }
    return false;
}

FunctionSymbol* ContainerSymbol::GetFunctionByIndex(int32_t functionIndex) const
{
    auto it = functionIndexMap.find(functionIndex);
    if (it != functionIndexMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void ContainerSymbol::Check()
{
    Symbol::Check();
    for (const auto& p : members)
    {
        p->Check();
    }
    for (const auto& p : functionIndexMap)
    {
        if (!p.second)
        {
            throw SymbolCheckException("container symbol has no function", GetSpan(), SourceModuleId());
        }
    }
}

void ContainerSymbol::CopyFrom(const Symbol* that) 
{
    Symbol::CopyFrom(that);
    const ContainerSymbol* thatContainer = static_cast<const ContainerSymbol*>(that);
    containerScope.SetParentScope(Parent()->GetContainerScope());
    for (const std::pair<std::u32string, Symbol*>& p : thatContainer->containerScope.SymbolMap())
    {
        containerScope.Install(p.second);
    }
}

DeclarationBlock::DeclarationBlock(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : ContainerSymbol(SymbolType::declarationBlock, span_, sourceModuleId_,name_)
{
}

void DeclarationBlock::AddMember(Symbol* member) 
{
    ContainerSymbol::AddMember(member);
    if (member->GetSymbolType() == SymbolType::localVariableSymbol)
    {
        FunctionSymbol* fun = Function();
        if (fun)
        {
            fun->AddLocalVariable(static_cast<LocalVariableSymbol*>(member));
        }
    }
}

} } // namespace cmajor::symbols
