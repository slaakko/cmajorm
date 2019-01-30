// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/ir/Emitter.hpp>
#include <cmajor/util/Unicode.hpp>

namespace cmajor { namespace symbols {

using namespace cmajor::unicode;

ConstantSymbol::ConstantSymbol(const Span& span_, const std::u32string& name_) : Symbol(SymbolType::constantSymbol, span_, name_), type(), evaluating(false), sizeOfValue(0), valuePos(0)
{
}

void ConstantSymbol::Write(SymbolWriter& writer)
{
    Symbol::Write(writer);
    writer.GetBinaryWriter().Write(type->TypeId());
    bool hasComplexValue = value->IsComplexValue();
    writer.GetBinaryWriter().Write(hasComplexValue);
    if (hasComplexValue)
    {
        uint32_t sizePos = writer.GetBinaryWriter().Pos();
        uint32_t sizeOfValue = 0;
        writer.GetBinaryWriter().Write(sizeOfValue);
        uint32_t startPos = writer.GetBinaryWriter().Pos();
        value->Write(writer.GetBinaryWriter());
        uint32_t endPos = writer.GetBinaryWriter().Pos();
        sizeOfValue = endPos - startPos;
        writer.GetBinaryWriter().Seek(sizePos);
        writer.GetBinaryWriter().Write(sizeOfValue);
        writer.GetBinaryWriter().Seek(endPos);
    }
    else
    {
        WriteValue(value.get(), writer.GetBinaryWriter());
    }
    writer.GetBinaryWriter().Write(strValue);
}

void ConstantSymbol::Read(SymbolReader& reader)
{
    Symbol::Read(reader);
    boost::uuids::uuid typeId;
    reader.GetBinaryReader().ReadUuid(typeId);
    reader.GetSymbolTable()->EmplaceTypeRequest(reader, this, typeId, 0);
    bool hasComplexValue = reader.GetBinaryReader().ReadBool();
    if (hasComplexValue)
    {
        sizeOfValue = reader.GetBinaryReader().ReadUInt();
        valuePos = reader.GetBinaryReader().Pos();
        reader.GetBinaryReader().Skip(sizeOfValue);
        filePathReadFrom = reader.GetBinaryReader().FileName();
    }
    else
    {
        value = ReadValue(reader.GetBinaryReader(), GetSpan());
    }
    strValue = reader.GetBinaryReader().ReadUtf32String();
}

Value* ConstantSymbol::GetValue() 
{
    if (!value)
    {
        if (filePathReadFrom.empty())
        {
            throw Exception(GetRootModuleForCurrentThread(), "internal error: could not read value: value file name not set", GetSpan());
        }
        BinaryReader reader(filePathReadFrom);
        reader.Skip(valuePos);
        value.reset(type->MakeValue());
        if (!value)
        {
            throw Exception(GetRootModuleForCurrentThread(), "internal error: could not read value because could not create value of type '" + ToUtf8(type->FullName()) + "'", GetSpan());
        }
        value->Read(reader);
    }
    return value.get(); 
}

void ConstantSymbol::EmplaceType(TypeSymbol* typeSymbol, int index)
{
    Assert(index == 0, "invalid emplace type index");
    type = typeSymbol;
}

void ConstantSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject() && Access() == SymbolAccess::public_)
    {
        collector->AddConstant(this);
    }
}

void ConstantSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("full name: " + ToUtf8(FullNameWithSpecifiers()));
    formatter.WriteLine("mangled name: " + ToUtf8(MangledName()));
    formatter.WriteLine("type: " + ToUtf8(type->FullName()));
    formatter.WriteLine("value: " + value->ToString());
}

void ConstantSymbol::SetSpecifiers(Specifiers specifiers)
{
    Specifiers accessSpecifiers = specifiers & Specifiers::access_;
    SetAccess(accessSpecifiers);
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be static", GetSpan());
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be virtual", GetSpan());
    }
    if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be override", GetSpan());
    }
    if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be abstract", GetSpan());
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be inline", GetSpan());
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be explicit", GetSpan());
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be external", GetSpan());
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be suppressed", GetSpan());
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be default", GetSpan());
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be constexpr", GetSpan());
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be cdecl", GetSpan());
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be nothrow", GetSpan());
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be throw", GetSpan());
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be new", GetSpan());
    }
    if ((specifiers & Specifiers::const_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be const", GetSpan());
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        throw Exception(GetRootModuleForCurrentThread(), "constant cannot be unit_test", GetSpan());
    }
}

void ConstantSymbol::ComputeMangledName()
{
    Symbol::ComputeMangledName();
}


std::string ConstantSymbol::Syntax() const
{
    std::string syntax = GetSpecifierStr();
    if (!syntax.empty())
    {
        syntax.append(1, ' ');
    }
    syntax.append("const ");
    syntax.append(ToUtf8(GetType()->DocName()));
    syntax.append(1, ' ');
    syntax.append(ToUtf8(DocName()));
    if (value)
    {
        syntax.append(" = ");
        std::string valueStr = value->ToString();
        if (GetType()->IsUnsignedType())
        {
            valueStr.append(1, 'u');
        }
        syntax.append(valueStr);
    }
    syntax.append(1, ';');
    return syntax;
}

void ConstantSymbol::SetValue(Value* value_)
{
    value.reset(value_);
}

void* ConstantSymbol::ArrayIrObject(Emitter& emitter, bool create)
{
    if (!type->IsArrayType())
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: array object expected", GetSpan());
    }
    if (!value)
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: array value missing", GetSpan());
    }
    if (value->GetValueType() != ValueType::arrayValue)
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: array value expected", GetSpan());
    }
    ArrayValue* arrayValue = static_cast<ArrayValue*>(value.get());
    void* irArrayType = type->IrType(emitter);
    void* irArrayObject = emitter.GetOrInsertGlobal(ToUtf8(MangledName()), irArrayType);
    if (create)
    {
        void* arrayObjectGlobal = irArrayObject;
        emitter.SetInitializer(arrayObjectGlobal, arrayValue->IrValue(emitter));
    }
    return irArrayObject;
}

void* ConstantSymbol::StructureIrObject(Emitter& emitter, bool create)
{
    if (!type->IsClassTypeSymbol())
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: class type object expected", GetSpan());
    }
    if (!value)
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: structured value missing", GetSpan());
    }
    if (value->GetValueType() != ValueType::structuredValue)
    {
        throw Exception(GetRootModuleForCurrentThread(), "internal error: structured value expected", GetSpan());
    }
    StructuredValue* structuredValue = static_cast<StructuredValue*>(value.get());
    void* irStructureType = type->IrType(emitter);
    void* irStructureObject = emitter.GetOrInsertGlobal(ToUtf8(MangledName()), irStructureType);
    if (create)
    {
        void* structureObjectGlobal = irStructureObject;
        emitter.SetInitializer(structureObjectGlobal, structuredValue->IrValue(emitter));
    }
    return irStructureObject;
}

void ConstantSymbol::Check()
{
    Symbol::Check();
    if (!type)
    {
        throw SymbolCheckException(GetRootModuleForCurrentThread(), "constant symbol has no type", GetSpan());
    }
}

} } // namespace cmajor::symbols
