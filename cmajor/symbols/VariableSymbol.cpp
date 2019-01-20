// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::unicode;

VariableSymbol::VariableSymbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_) : Symbol(symbolType_, span_, name_), type()
{
}

void VariableSymbol::Write(SymbolWriter& writer)
{
    Symbol::Write(writer);
    writer.GetBinaryWriter().Write(type->TypeId());
}

void VariableSymbol::Read(SymbolReader& reader)
{
    Symbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 0);
}

void VariableSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    type = typeSymbol;
}

void VariableSymbol::Check()
{
    Symbol::Check();
    if (!type)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "variable symbol contains null type pointer", GetSpan());
    }
}

ParameterSymbol::ParameterSymbol(const Span& span_, const std::u32string& name_) : VariableSymbol(SymbolType::parameterSymbol, span_, name_), artificialName(false)
{
}

void ParameterSymbol::Write(SymbolWriter& writer)
{
    VariableSymbol::Write(writer);
    writer.GetBinaryWriter().Write(artificialName);
}

void ParameterSymbol::Read(SymbolReader& reader)
{
    VariableSymbol::Read(reader);
    artificialName = reader.GetBinaryReader().ReadBool();
}

std::u32string ParameterSymbol::CodeName() const
{
    if (artificialName)
    {
        return std::u32string();
    }
    return VariableSymbol::CodeName();
}

std::unique_ptr<dom::Element> ParameterSymbol::CreateDomElement(TypeMap& typeMap) 
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"ParameterSymbol"));
    if (GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    return element;
}

LocalVariableSymbol::LocalVariableSymbol(const Span& span_, const std::u32string& name_) : VariableSymbol(SymbolType::localVariableSymbol, span_, name_)
{
}

std::unique_ptr<dom::Element> LocalVariableSymbol::CreateDomElement(TypeMap& typeMap)
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"LocalVariableSymbol"));
    if (GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    return element;
}

MemberVariableSymbol::MemberVariableSymbol(const Span& span_, const std::u32string& name_) : VariableSymbol(SymbolType::memberVariableSymbol, span_, name_), layoutIndex(-1)
{
}

void MemberVariableSymbol::Write(SymbolWriter& writer)
{
    VariableSymbol::Write(writer);
    writer.GetBinaryWriter().Write(layoutIndex);
}

void MemberVariableSymbol::Read(SymbolReader& reader)
{
    VariableSymbol::Read(reader);
    layoutIndex = reader.GetBinaryReader().ReadInt();
}

bool MemberVariableSymbol::IsExportSymbol() const
{
    return VariableSymbol::IsExportSymbol();
}

void MemberVariableSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddMemberVariable(this);
    }
}

void MemberVariableSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("mangled name: " + ToUtf8(MangledName()));
    formatter.WriteLine("type: " + ToUtf8(GetType()->FullName()));
    formatter.WriteLine("layout index: " + std::to_string(layoutIndex));
}

std::string MemberVariableSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append(ToUtf8(GetType()->DocName()));
    syntax.append(1, ' ');
    syntax.append(ToUtf8(DocName()));
    syntax.append(1, ';');
    return syntax;
}

void MemberVariableSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        SetStatic();
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "member variable cannot be unit_test", GetSpan());
    }
}

llvm::DIDerivedType* MemberVariableSymbol::GetDIMemberType(Emitter& emitter, uint64_t offsetInBits)
{
    Assert(layoutIndex != -1, "invalid layout index");
    Assert(Parent() && Parent()->IsClassTypeSymbol(), "parent class type expected");
    ClassTypeSymbol* parentClassType = static_cast<ClassTypeSymbol*>(Parent());
    std::pair<boost::uuids::uuid, int32_t> memberVariableId = std::make_pair(parentClassType->TypeId(), layoutIndex);
    llvm::DIDerivedType* localDIType = emitter.GetDIMemberType(memberVariableId);
    if (!localDIType)
    {
        uint64_t sizeInBits = GetType()->SizeInBits(emitter);
        uint32_t alignInBits = GetType()->AlignmentInBits(emitter);
        llvm::DINode::DIFlags flags = llvm::DINode::DIFlags::FlagZero;
        llvm::DIType* scope = parentClassType->GetDIType(emitter);
        localDIType = emitter.DIBuilder()->createMemberType(scope, ToUtf8(Name()), emitter.GetFile(GetSpan().FileIndex()), GetSpan().LineNumber(), sizeInBits, alignInBits, 
            offsetInBits, flags, GetType()->GetDIType(emitter));
        emitter.SetDIMemberType(memberVariableId, localDIType);
    }
    return localDIType;
}

std::unique_ptr<dom::Element> MemberVariableSymbol::CreateDomElement(TypeMap& typeMap)
{
    std::unique_ptr<dom::Element> element(new dom::Element(U"MemberVariableSymbol"));
    if (GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        element->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    return element;
}

void MemberVariableSymbol::Check()
{
    VariableSymbol::Check();
    if (layoutIndex == -1)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "member variable symbol contains invalid layout index", GetSpan());
    }
}

} } // namespace cmajor::symbols
