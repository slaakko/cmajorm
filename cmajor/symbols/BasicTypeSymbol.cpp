// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/symbols/BasicTypeSymbol.hpp>
#include <cmajor/symbols/Exception.hpp>
#include <cmajor/symbols/Module.hpp>
#include <cmajor/symbols/SymbolWriter.hpp>
#include <cmajor/symbols/SymbolReader.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/SymbolCollector.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace cmajor { namespace symbols {

using namespace soulng::unicode;

BasicTypeSymbol::BasicTypeSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) :
    TypeSymbol(symbolType_, span_, sourceModuleId_, name_), 
    defaultConstructor(nullptr), copyConstructor(nullptr), moveConstructor(nullptr), copyAssignment(nullptr), moveAssignment(nullptr), returnFun(nullptr), equalityOp(nullptr)
{
}

void BasicTypeSymbol::Write(SymbolWriter& writer)
{
    TypeSymbol::Write(writer);
    boost::uuids::uuid defaultConstructorId = boost::uuids::nil_uuid();
    if (defaultConstructor)
    {
        defaultConstructorId = defaultConstructor->FunctionId();
    }
    writer.GetBinaryWriter().Write(defaultConstructorId);
    boost::uuids::uuid copyConstructorId = boost::uuids::nil_uuid();
    if (copyConstructor)
    {
        copyConstructorId = copyConstructor->FunctionId();
    }
    writer.GetBinaryWriter().Write(copyConstructorId);
    boost::uuids::uuid moveConstructorId = boost::uuids::nil_uuid();
    if (moveConstructor)
    {
        moveConstructorId = moveConstructor->FunctionId();
    }
    writer.GetBinaryWriter().Write(moveConstructorId);
    boost::uuids::uuid copyAssignmentId = boost::uuids::nil_uuid();
    if (copyAssignment)
    {
        copyAssignmentId = copyAssignment->FunctionId();
    }
    writer.GetBinaryWriter().Write(copyAssignmentId);
    boost::uuids::uuid moveAssignmentId = boost::uuids::nil_uuid();
    if (moveAssignment)
    {
        moveAssignmentId = moveAssignment->FunctionId();
    }
    writer.GetBinaryWriter().Write(moveAssignmentId);
    boost::uuids::uuid returnId = boost::uuids::nil_uuid();
    if (returnFun)
    {
        returnId = returnFun->FunctionId();
    }
    writer.GetBinaryWriter().Write(returnId);
    boost::uuids::uuid equalityOpId = boost::uuids::nil_uuid();
    if (equalityOp)
    {
        equalityOpId = equalityOp->FunctionId();
    }
    writer.GetBinaryWriter().Write(equalityOpId);
}

void BasicTypeSymbol::Read(SymbolReader& reader)
{
    TypeSymbol::Read(reader);
    boost::uuids::uuid defaultConstructorId;
    reader.GetBinaryReader().ReadUuid(defaultConstructorId);
    if (!defaultConstructorId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, defaultConstructorId, 0);
    }
    boost::uuids::uuid copyConstructorId;
    reader.GetBinaryReader().ReadUuid(copyConstructorId);
    if (!copyConstructorId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyConstructorId, 1);
    }
    boost::uuids::uuid moveConstructorId;
    reader.GetBinaryReader().ReadUuid(moveConstructorId);
    if (!moveConstructorId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveConstructorId, 2);
    }
    boost::uuids::uuid copyAssignmentId;
    reader.GetBinaryReader().ReadUuid(copyAssignmentId);
    if (!copyAssignmentId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, copyAssignmentId, 3);
    }
    boost::uuids::uuid moveAssignmentId;
    reader.GetBinaryReader().ReadUuid(moveAssignmentId);
    if (!moveAssignmentId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, moveAssignmentId, 4);
    }
    boost::uuids::uuid returnId;
    reader.GetBinaryReader().ReadUuid(returnId);
    if (!returnId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, returnId, 5);
    }
    boost::uuids::uuid equalityOpId;
    reader.GetBinaryReader().ReadUuid(equalityOpId);
    if (!equalityOpId.is_nil())
    {
        reader.GetSymbolTable()->EmplaceFunctionRequest(reader, this, equalityOpId, 6);
    }
}

void BasicTypeSymbol::EmplaceFunction(FunctionSymbol* functionSymbol, int index)
{
    switch (index)
    {
        case 0: defaultConstructor = functionSymbol; break;
        case 1: copyConstructor = functionSymbol; break;
        case 2: moveConstructor = functionSymbol; break;
        case 3: copyAssignment = functionSymbol; break;
        case 4: moveAssignment = functionSymbol; break;
        case 5: returnFun = functionSymbol; break;
        case 6: equalityOp = functionSymbol; break;
        default:
        {
            Assert(false, "invalid emplace function index");
        }
    }
}

void BasicTypeSymbol::Accept(SymbolCollector* collector)
{
    if (IsProject())
    {
        collector->AddBasicType(this);
    }
}

void BasicTypeSymbol::Dump(CodeFormatter& formatter)
{
    formatter.WriteLine(ToUtf8(Name()));
    formatter.WriteLine("typeid: " + boost::uuids::to_string(TypeId()));
}

void BasicTypeSymbol::Check()
{
    TypeSymbol::Check();
    if (!defaultConstructor && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no default constructor", GetSpan(), SourceModuleId());
    }
    if (!copyConstructor && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no copy constructor", GetSpan(), SourceModuleId());
    }
    if (!moveConstructor && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no move constructor", GetSpan(), SourceModuleId());
    }
    if (!copyAssignment && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no copy assignment", GetSpan(), SourceModuleId());
    }
    if (!moveAssignment && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no move assignment", GetSpan(), SourceModuleId());
    }
    if (!returnFun && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no return function", GetSpan(), SourceModuleId());
    }
    if (!equalityOp && !IsVoidType())
    {
        throw SymbolCheckException("basic type symbol has no equality comparison operation", GetSpan(), SourceModuleId());
    }
}

BoolTypeSymbol::BoolTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::boolTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType BoolTypeSymbol::GetValueType() const
{
    return ValueType::boolValue;
}

Value* BoolTypeSymbol::MakeValue() const
{
    return new BoolValue(GetSpan(), SourceModuleId(), false);
}

void* BoolTypeSymbol::CreateDIType(Emitter& emitter) 
{
    return emitter.CreateDITypeForBool();
}

SByteTypeSymbol::SByteTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::sbyteTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType SByteTypeSymbol::GetValueType() const
{
    return ValueType::sbyteValue;
}

Value* SByteTypeSymbol::MakeValue() const
{
    return new SByteValue(GetSpan(), SourceModuleId(), 0);
}

void* SByteTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForSByte();
}

ByteTypeSymbol::ByteTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::byteTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType ByteTypeSymbol::GetValueType() const
{
    return ValueType::byteValue;
}

Value* ByteTypeSymbol::MakeValue() const
{
    return new ByteValue(GetSpan(), SourceModuleId(), 0);
}

void* ByteTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForByte();
}

ShortTypeSymbol::ShortTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::shortTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType ShortTypeSymbol::GetValueType() const
{
    return ValueType::shortValue;
}

Value* ShortTypeSymbol::MakeValue() const
{
    return new ShortValue(GetSpan(), SourceModuleId(), 0);
}

void* ShortTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForShort();
}

UShortTypeSymbol::UShortTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::ushortTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType UShortTypeSymbol::GetValueType() const
{
    return ValueType::ushortValue;
}

Value* UShortTypeSymbol::MakeValue() const
{
    return new UShortValue(GetSpan(), SourceModuleId(), 0);
}

void* UShortTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForUShort();
}

IntTypeSymbol::IntTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::intTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType IntTypeSymbol::GetValueType() const
{
    return ValueType::intValue;
}

Value* IntTypeSymbol::MakeValue() const
{
    return new IntValue(GetSpan(), SourceModuleId(), 0);
}

void* IntTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForInt();
}

UIntTypeSymbol::UIntTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::uintTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType UIntTypeSymbol::GetValueType() const
{
    return ValueType::uintValue;
}

Value* UIntTypeSymbol::MakeValue() const
{
    return new UIntValue(GetSpan(), SourceModuleId(), 0);
}

void* UIntTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForUInt();
}

LongTypeSymbol::LongTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::longTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType LongTypeSymbol::GetValueType() const
{
    return ValueType::longValue;
}

Value* LongTypeSymbol::MakeValue() const
{
    return new LongValue(GetSpan(), SourceModuleId(), 0);
}

void* LongTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForLong();
}

ULongTypeSymbol::ULongTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::ulongTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType ULongTypeSymbol::GetValueType() const
{
    return ValueType::ulongValue;
}

Value* ULongTypeSymbol::MakeValue() const
{
    return new ULongValue(GetSpan(), SourceModuleId(), 0);
}

void* ULongTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForULong();
}

FloatTypeSymbol::FloatTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::floatTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType FloatTypeSymbol::GetValueType() const
{
    return ValueType::floatValue;
}

Value* FloatTypeSymbol::MakeValue() const
{
    return new FloatValue(GetSpan(), SourceModuleId(), 0.0);
}

void* FloatTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForFloat();
}

DoubleTypeSymbol::DoubleTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::doubleTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType DoubleTypeSymbol::GetValueType() const
{
    return ValueType::doubleValue;
}

Value* DoubleTypeSymbol::MakeValue() const
{
    return new DoubleValue(GetSpan(), SourceModuleId(), 0.0);
}

void* DoubleTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForDouble();
}

CharTypeSymbol::CharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::charTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType CharTypeSymbol::GetValueType() const
{
    return ValueType::charValue;
}

Value* CharTypeSymbol::MakeValue() const
{
    return new CharValue(GetSpan(), SourceModuleId(), '\0');
}

void* CharTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForChar();
}

WCharTypeSymbol::WCharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::wcharTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType WCharTypeSymbol::GetValueType() const
{
    return ValueType::wcharValue;
}

Value* WCharTypeSymbol::MakeValue() const
{
    return new WCharValue(GetSpan(), SourceModuleId(), '\0');
}

void* WCharTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForWChar();
}

UCharTypeSymbol::UCharTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::ucharTypeSymbol, span_, sourceModuleId_, name_)
{
}

ValueType UCharTypeSymbol::GetValueType() const
{
    return ValueType::ucharValue;
}

Value* UCharTypeSymbol::MakeValue() const
{
    return new UCharValue(GetSpan(), SourceModuleId(), '\0');
}

void* UCharTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForUChar();
}

VoidTypeSymbol::VoidTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_) : BasicTypeSymbol(SymbolType::voidTypeSymbol, span_, sourceModuleId_, name_)
{
}

void* VoidTypeSymbol::CreateDIType(Emitter& emitter)
{
    return emitter.CreateDITypeForVoid();
}

} } // namespace cmajor::symbols
