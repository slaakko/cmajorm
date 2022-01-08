// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/Assembler.hpp>
#include <system-x/assembler/AssemblyInstruction.hpp>
#include <system-x/assembler/Constant.hpp>
#include <system-x/assembler/Expression.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/object/Link.hpp>
#include <system-x/machine/Memory.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Uuid.hpp>

namespace cmsx::assembler {

using namespace soulng::unicode;
using namespace soulng::util;

Assembler::Assembler(soulng::lexer::Lexer* lexer_, const std::string& assemblyFilePath_, const std::string& objectFilePath_) :
    stage(Stage::none), lexer(lexer_), assemblyFilePath(assemblyFilePath_), objectFile(new cmsx::object::ObjectFile(objectFilePath_)), currentInstruction(nullptr),
    currentSegment(cmsx::object::Segment::unknown), currentSection(nullptr), currentSymbol(nullptr), currentFunctionSymbol(nullptr), currentStructureSymbol(nullptr),
    parentIndex(-1), line(1), prevWasEmptyLine(false), currentAlignment(1), inSpec(false)
{
    objectFile->CreateSections();
    objectFile->GetSymbolTable().InstallDefaultSymbols();
    objectFile->GetCodeSection()->SetBaseAddress(cmsx::machine::textSegmentBaseAddress);
    objectFile->GetDataSection()->SetBaseAddress(cmsx::machine::dataSegmentBaseAddress);
}

void Assembler::Assemble()
{
    Resolve();
    GenerateCode();
    cmsx::object::LinkInternal(objectFile.get());
    objectFile->Finalize();
    objectFile->WriteFile();
}

void Assembler::Resolve()
{
    currentSegment = cmsx::object::Segment::text;
    currentSection = objectFile->GetCodeSection();
    stage = Stage::resolve;
    VisitInstructions();
}

void Assembler::GenerateCode()
{
    currentSegment = cmsx::object::Segment::text;
    currentSection = objectFile->GetCodeSection();
    stage = Stage::generateCode;
    VisitInstructions();
}

void Assembler::VisitInstructions()
{
    for (const auto& instruction : instructions)
    {
        instruction->Accept(*this);
    }
}

void Assembler::AlignAt()
{
    int64_t alignment = 1;
    if (currentInstruction->GetOpCode()->Value() == WYDE)
    {
        alignment = 2;
        currentAlignment = 2;
    }
    else if (currentInstruction->GetOpCode()->Value() == TETRA || currentInstruction->GetOpCode()->Value() < 256)
    {
        alignment = 4;
        currentAlignment = 4;
    }
    else if (currentInstruction->GetOpCode()->Value() == OCTA || currentInstruction->GetOpCode()->Value() == STRUCT)
    {
        alignment = 8;
        currentAlignment = 8;
    }
    int64_t at = currentSection->Address();
    int64_t a = at & (alignment - 1);
    if (a != 0)
    {
        int64_t offset = alignment - a;
        for (int64_t i = 0; i < offset; ++i)
        {
            currentSection->EmitByte(0);
        }
    }
}

void Assembler::Visit(DecimalConstant& node)
{
    value = cmsx::object::Value(node.Value());
}

void Assembler::Visit(HexadecimalConstant& node)
{
    value = cmsx::object::Value(node.Value());
}

void Assembler::Visit(ByteConstant& node)
{
    value = cmsx::object::Value(node.Value());
}

void Assembler::Visit(WydeConstant& node)
{
    value = cmsx::object::Value(node.Value());
}

void Assembler::Visit(TetraConstant& node)
{
    value = cmsx::object::Value(node.Value());
}

void Assembler::Visit(CharacterConstant& node)
{
    value = cmsx::object::Value(static_cast<uint64_t>(node.Value()));
}

void Assembler::Visit(StringConstant& node)
{
    if (currentSection->IsDataSection())
    {
        std::string s = ToUtf8(node.Value());
        for (char c : s)
        {
            currentSection->EmitByte(static_cast<uint8_t>(c));
        }
    }
    else
    {
        // todo: debug section
    }
}

void Assembler::Visit(ClsIdConstant& node)
{
    boost::uuids::uuid typeId = boost::lexical_cast<boost::uuids::uuid>(node.TypeId());
    value = cmsx::object::Value(static_cast<uint64_t>(typeIds.size()), cmsx::object::ValueFlags::typeIdIndex);
    typeIds.push_back(typeId);
}

void Assembler::Visit(UnaryExpression& node)
{
    value = cmsx::object::Value();
    node.Operand()->Accept(*this);
    if (value.GetFlag(cmsx::object::ValueFlags::undefined))
    {
        Error("value of operand is undefined", node.GetSourcePos());
    }
    else
    {
        switch (node.Op())
        {
            case Operator::unaryPlus:
            {
                break;
            }
            case Operator::unaryMinus:
            {
                if (value.GetFlag(cmsx::object::ValueFlags::pure))
                {
                    if (value.Val() == 0)
                    {
                        Error("unary minus operator with operand 0 not defined", node.GetSourcePos());
                    }
                    else
                    {
                        value.SetVal(-value.Val());
                    }
                }
                else
                {
                    Error("unary minus operator needs pure operand", node.GetSourcePos());
                }
                break;
            }
            case Operator::complement:
            {
                if (value.GetFlag(cmsx::object::ValueFlags::pure))
                {
                    value.SetVal(~value.Val());
                }
                else
                {
                    Error("complement operator needs pure operand", node.GetSourcePos());
                }
                break;
            }
            case Operator::reg:
            {
                if (value.GetFlag(cmsx::object::ValueFlags::pure))
                {
                    if (value.Val() >= 0 && value.Val() < 256)
                    {
                        value.ResetFlag(cmsx::object::ValueFlags::pure);
                        value.SetFlag(cmsx::object::ValueFlags::reg);
                    }
                    else
                    {
                        Error("invalid registerize operand (not in range 0...255)", node.GetSourcePos());
                    }
                }
                else
                {
                    Error("registerize operator needs pure operand", node.GetSourcePos());
                }
                break;
            }
            case Operator::serial:
            {
                Error("serial operator not implemented", node.GetSourcePos());
                break;
            }
            default:
            {
                Error("unknown unary operator", node.GetSourcePos());
                break;
            }
        }
    }
}

void Assembler::Visit(BinaryExpression& node)
{
    value = cmsx::object::Value();
    node.Left()->Accept(*this);
    cmsx::object::Value left = value;
    if (left.GetFlag(cmsx::object::ValueFlags::undefined))
    {
        Error("value of left operand is undefined", node.GetSourcePos());
    }
    value = cmsx::object::Value();
    node.Right()->Accept(*this);
    cmsx::object::Value right = value;
    if (right.GetFlag(cmsx::object::ValueFlags::undefined))
    {
        Error("value of right operand is undefined", node.GetSourcePos());
    }
    switch (node.Op())
    {
        case Operator::add:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() + right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::reg))
            {
                value = cmsx::object::Value(left.Val() + right.Val(), cmsx::object::ValueFlags::reg);
                if (value.Val() > 256)
                {
                    Error("invalid add register operand (not in range 0...255)", node.GetSourcePos());
                }
            }
            else if (left.GetFlag(cmsx::object::ValueFlags::reg) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() + right.Val(), cmsx::object::ValueFlags::reg);
                if (value.Val() > 256)
                {
                    Error("invalid add register operand (not in range 0...255)", node.GetSourcePos());
                }
            }
            else
            {
                Error("add operator needs operands that are both pure or either one is register", node.GetSourcePos());
            }
            break;
        }
        case Operator::subtract:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() - right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else if (left.GetFlag(cmsx::object::ValueFlags::reg))
            {
                if (right.GetFlag(cmsx::object::ValueFlags::pure))
                {
                    value = cmsx::object::Value(left.Val() - right.Val(), cmsx::object::ValueFlags::reg);
                    if (value.Val() > 256)
                    {
                        Error("invalid subtract register operand (not in range 0...255)", node.GetSourcePos());
                    }
                }
                else if (right.GetFlag(cmsx::object::ValueFlags::reg))
                {
                    value = cmsx::object::Value(left.Val() - right.Val());
                    if (value.Val() > 256)
                    {
                        Error("invalid subtract register operand (not in range 0...255)", node.GetSourcePos());
                    }
                }
                else
                {
                    Error("subtract operator needs operands that are both pure, or left is register number and right is pure or register number", node.GetSourcePos());
                }
            }
            else
            {
                Error("subtract operator needs operands that are both pure, or left is register number and right is pure or register number", node.GetSourcePos());
            }
            break;
        }
        case Operator::multiply:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() * right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("multiply operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::divide:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                if (right.Val() == 0)
                {
                    Error("division by zero", node.GetSourcePos());
                }
                value = cmsx::object::Value(left.Val() / right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("division operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::modulus:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                if (right.Val() == 0)
                {
                    Error("modulo by zero", node.GetSourcePos());
                }
                value = cmsx::object::Value(left.Val() % right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("modulo operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::shift_left:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() << right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("left shift operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::shift_right:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() >> right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("right shift operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::bitwise_and:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() & right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("bitwise and operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::bitwise_or:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() | right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("bitwise or operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::bitwise_xor:
        {
            if (left.GetFlag(cmsx::object::ValueFlags::pure) && right.GetFlag(cmsx::object::ValueFlags::pure))
            {
                value = cmsx::object::Value(left.Val() ^ right.Val());
                if (left.GetFlag(cmsx::object::ValueFlags::address) || right.GetFlag(cmsx::object::ValueFlags::address))
                {
                    value.SetFlag(cmsx::object::ValueFlags::address);
                }
            }
            else
            {
                Error("bitwise xor operator needs pure operands", node.GetSourcePos());
            }
            break;
        }
        case Operator::fractional_divide:
        {
            Error("fractional division operator not implemented", node.GetSourcePos());
            break;
        }
        default:
        {
            Error("unknown binary operator", node.GetSourcePos());
            break;
        }
    }
}

void Assembler::Visit(ParenthesizedExpression& node)
{
    value = cmsx::object::Value();
    node.Expr()->Accept(*this);
    if (value.GetFlag(cmsx::object::ValueFlags::undefined))
    {
        Error("value of parenthesized expression is undefined", node.GetSourcePos());
    }
}

void Assembler::Visit(OpCode& node)
{
    if (stage == Stage::resolve)
    {
        int opCode = GetOpCode(node.Name());
        if (opCode != -1)
        {
            node.SetValue(opCode);
        }
        else
        {
            Error("unknown opcode '" + node.Name() + "'", node.GetSourcePos());
        }
    }
}

void Assembler::Visit(LocalSymbol& node)
{
    int opc = currentInstruction->GetOpCode()->Value();
    if (stage == Stage::resolve)
    {
        cmsx::object::Symbol* symbol = objectFile->GetSymbolTable().GetSymbol(node.Name());
        if (symbol)
        {
            switch (opc)
            {
                case ENDF:
                {
                    Error("ENDF symbol is local", node.GetSourcePos());
                    break;
                }
                case ENDS:
                {
                    Error("ENDS symbol is local", node.GetSourcePos());
                    break;
                }
                default:
                {
                    Error("symbol '" + node.Name() + "' already defined", node.GetSourcePos());
                    break;
                }
            }
        }
        else
        {
            switch (opc)
            {
                case ENDF:
                {
                    Error("ENDF symbol is local", node.GetSourcePos());
                    break;
                }
                case ENDS:
                {
                    Error("ENDS symbol is local", node.GetSourcePos());
                    break;
                }
                default:
                {
                    if (currentFunctionSymbol)
                    {
                        symbol = new cmsx::object::Symbol(cmsx::object::SymbolKind::local, node.Name(), currentFunctionSymbol->FullName() + node.Name());
                    }
                    else if (currentStructureSymbol)
                    {
                        symbol = new cmsx::object::Symbol(cmsx::object::SymbolKind::local, node.Name(), currentStructureSymbol->FullName() + node.Name());
                    }
                    else
                    {
                        Error("local symbols can be used only inside FUNC or STRUCT", node.GetSourcePos());
                    }
                    break;
                }
            }
            if (symbol)
            {
                symbol->SetSegment(currentSegment);
                symbol->SetLinkage(cmsx::object::Linkage::internal);
                symbol->SetSection(currentSection);
                symbol->SetValue(cmsx::object::Value(symbol));
                objectFile->GetSymbolTable().AddSymbol(symbol);
            }
        }
    }
    else if (stage == Stage::generateCode)
    {
        if (currentFunctionSymbol)
        {
            currentSymbol = objectFile->GetSymbolTable().GetSymbol(currentFunctionSymbol->FullName() + node.Name());
        }
        else if (currentStructureSymbol)
        {
            currentSymbol = objectFile->GetSymbolTable().GetSymbol(currentStructureSymbol->FullName() + node.Name());
        }
        if (currentSymbol)
        {
            value = currentSymbol->GetValue();
        }
        else
        {
            Error("label '" + node.Name() + " not found", node.GetSourcePos());
        }
    }
}

void Assembler::Visit(GlobalSymbol& node)
{
    int opc = currentInstruction->GetOpCode()->Value();
    if (stage == Stage::resolve)
    {
        cmsx::object::Symbol* symbol = objectFile->GetSymbolTable().GetSymbol(node.Name());
        if (symbol)
        {
            switch (opc)
            {
                case ENDF:
                {
                    currentFunctionSymbol = nullptr;
                    break;
                }
                case ENDS:
                {
                    currentStructureSymbol = nullptr;
                    break;
                }
                default:
                {
                    Error("symbol '" + node.Name() + "' already defined", node.GetSourcePos());
                    break;
                }
            }
        }
        else
        {
            symbol = new cmsx::object::Symbol(cmsx::object::SymbolKind::global, std::string(), node.Name());
            switch (opc)
            {
                case FUNC:
                {
                    currentFunctionSymbol = symbol;
                    break;
                }
                case STRUCT:
                {
                    currentStructureSymbol = symbol;
                    break;
                }
                case ENDF:
                {
                    Error("no corresponding FUNC '" + node.Name() + "' not seen", node.GetSourcePos());
                    break;
                }
                case ENDS:
                {
                    Error("no corresponding STRUCT '" + node.Name() + "' not seen", node.GetSourcePos());
                    break;
                }
            }
            symbol->SetSegment(currentSegment);
            symbol->SetLinkage(cmsx::object::Linkage::internal);
            symbol->SetSection(currentSection);
            symbol->SetValue(cmsx::object::Value(symbol));
            objectFile->GetSymbolTable().AddSymbol(symbol);
            if (currentSegment == cmsx::object::Segment::data)
            {
                currentSection->AddSymbol(symbol);
            }
        }
    }
    else if (stage == Stage::generateCode)
    {
        currentSymbol = objectFile->GetSymbolTable().GetSymbol(node.Name());
        if (!currentSymbol)
        {
            currentSymbol = new cmsx::object::Symbol(cmsx::object::SymbolKind::global, std::string(), node.Name());
            currentSymbol->SetSection(currentSection);
            currentSymbol->SetLinkage(cmsx::object::Linkage::undefined);
            currentSymbol->SetValue(cmsx::object::Value(currentSymbol));
            objectFile->GetSymbolTable().AddSymbol(currentSymbol);
        }
        value = currentSymbol->GetValue();
    }
}

void Assembler::Visit(At& node)
{
    if (stage == Stage::generateCode)
    {
        value = cmsx::object::Value(currentSection->Address(), cmsx::object::ValueFlags::pure | cmsx::object::ValueFlags::address);
    }
}

void Assembler::Visit(OperandList& node)
{
    if (stage == Stage::generateCode)
    {
        operands.clear();
        int operandCount = node.OperandCount();
        for (int i = 0; i < operandCount; ++i)
        {
            value = cmsx::object::Value();
            Node* operand = node.GetOperand(i);
            operand->Accept(*this);
            operands.push_back(value);
        }
        currentInstruction->SetOperands(std::move(operands));
    }
}

void Assembler::Visit(Instruction& node)
{
    currentInstruction = &node;
    if (stage == Stage::resolve)
    {
        node.GetOpCode()->Accept(*this);
        switch (node.GetOpCode()->Value())
        {
            case LINK:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetLinkSection();
                return;
            }
            case CODE:
            {
                currentSegment = cmsx::object::Segment::text;
                currentSection = objectFile->GetCodeSection();
                return;
            }
            case DATA:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetDataSection();
                return;
            }
            case DEBUG:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetDebugSection();
                return;
            }
        }
        if (node.Label())
        {
            int opc = node.GetOpCode()->Value();
            switch (opc)
            {
                case EXTERN:
                case LINKONCE:
                {
                    Error("no label field allowed for EXTERN or LINKONCE instruction", node.GetSourcePos());
                    break;
                }
                case IS:
                {
                    break;
                }
                default:
                {
                    node.Label()->Accept(*this);
                    break;
                }
            }
        }
    }
    else if (stage == Stage::generateCode)
    {
        switch (node.GetOpCode()->Value())
        {
            case LINK:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetLinkSection();
                return;
            }
            case CODE:
            {
                currentSegment = cmsx::object::Segment::text;
                currentSection = objectFile->GetCodeSection();
                return;
            }
            case DATA:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetDataSection();
                return;
            }
            case DEBUG:
            {
                currentSegment = cmsx::object::Segment::data;
                currentSection = objectFile->GetDebugSection();
                return;
            }
        }
        currentSymbol = nullptr;
        currentAlignment = 1;
        if (currentSection != objectFile->GetDebugSection() && !inSpec)
        {
            AlignAt();
        }
        if (node.Label())
        {
            node.Label()->Accept(*this);
        }
        int opc = node.GetOpCode()->Value();
        if (currentSymbol)
        {
            switch (opc)
            {
                case FUNC:
                case ENDF:
                case STRUCT:
                case ENDS:
                {
                    AssemblyInstruction* assemblyInstruction = GetAssemblyInstruction(opc);
                    if (!assemblyInstruction)
                    {
                        Error("assembly instruction for opcode " + node.GetOpCode()->Name() + "(" + std::to_string(opc) + ") not found", node.GetSourcePos());
                    }
                    assemblyInstruction->Assemble(*this);
                    break;
                }
                default:
                {
                    currentSymbol->SetValue(cmsx::object::Value(currentSection->Address(), cmsx::object::ValueFlags::pure | cmsx::object::ValueFlags::address, currentSymbol));
                    currentSymbol->SetStart(currentSection->BaseAddress() + currentSection->Address());
                    if (currentAlignment >= 0 && currentAlignment < 256)
                    {
                        currentSymbol->SetAlignment(static_cast<uint8_t>(currentAlignment));
                    }
                    else
                    {
                        Error("invalid alignment", node.GetSourcePos());
                    }
                    if (parentIndex != -1)
                    {
                        currentSymbol->SetParentIndex(parentIndex);
                    }
                    break;
                }
            }
            objectFile->GetSymbolTable().AddSymbolToAddressMap(currentSymbol, true);
        }
        node.GetOperandList()->Accept(*this);
        switch (opc)
        {
            case FUNC:
            case ENDF:
            case STRUCT:
            case ENDS:
            {
                break;
            }
            default:
            {
                AssemblyInstruction* assemblyInstruction = GetAssemblyInstruction(opc);
                if (!assemblyInstruction)
                {
                    Error("assembly instruction for opcode " + node.GetOpCode()->Name() + "(" + std::to_string(opc) + ") not found", node.GetSourcePos());
                }
                assemblyInstruction->Assemble(*this);
                break;
            }
        }
    }
}

void Assembler::AddInstruction(Instruction* instruction)
{
    instructions.push_back(std::unique_ptr<Instruction>(instruction));
}

void Assembler::AddModeInstruction(const std::string& mode, const SourcePos& sourcePos)
{
    if (mode == "LINK")
    {
        AddInstruction(new Instruction(sourcePos, nullptr, new OpCode(sourcePos, ".LINK")));
    }
    else if (mode == "CODE")
    {
        AddInstruction(new Instruction(sourcePos, nullptr, new OpCode(sourcePos, ".CODE")));
    }
    else if (mode == "DATA")
    {
        AddInstruction(new Instruction(sourcePos, nullptr, new OpCode(sourcePos, ".DATA")));
    }
    else if (mode == "DEBUG")
    {
        AddInstruction(new Instruction(sourcePos, nullptr, new OpCode(sourcePos, ".DEBUG")));
    }
    else
    {
        Error("unknown mode '." + mode + "'", sourcePos);
    }
    prevWasEmptyLine = false;
}

void Assembler::Error(const std::string& message, const SourcePos& sourcePos)
{
    throw std::runtime_error("error assembling file '" + assemblyFilePath + "', line " + std::to_string(sourcePos.line) + ": " + message + ":\n" +
        ToUtf8(lexer->ErrorLines(sourcePos)));
}

Node* Assembler::MakeDecimalConstant(const SourcePos& sourcePos, const std::string& s)
{
    try
    {
        return new DecimalConstant(sourcePos, boost::lexical_cast<uint64_t>(s));
    }
    catch (const std::exception& ex)
    {
        Error("error parsing decimal constant " + s + ": " + PlatformStringToUtf8(ex.what()), sourcePos);
    }
    return nullptr;
}

Node* Assembler::MakeHexConstant(const SourcePos& sourcePos, const std::string& s)
{
    try
    {
        return new HexadecimalConstant(sourcePos, ParseHexULong(s.substr(1)));
    }
    catch (const std::exception& ex)
    {
        Error("error parsing hexadecimal constant " + s + ": " + PlatformStringToUtf8(ex.what()), sourcePos);
    }
    return nullptr;
}

Node* Assembler::MakeCharConstant(const SourcePos& sourcePos, const std::u32string& s)
{
    try
    {
        return new CharacterConstant(sourcePos, s[1]);
    }
    catch (const std::exception& ex)
    {
        Error("error parsing character constant " + ToUtf8(s) + ": " + PlatformStringToUtf8(ex.what()), sourcePos);
    }
    return nullptr;
}

Node* Assembler::MakeStringConstant(const SourcePos& sourcePos, const std::u32string& s)
{
    try
    {
        return new StringConstant(sourcePos, s.substr(1, s.length() - 2));
    }
    catch (const std::exception& ex)
    {
        Error("error parsing string constant " + ToUtf8(s) + ": " + PlatformStringToUtf8(ex.what()), sourcePos);
    }
    return nullptr;
}

Node* Assembler::MakeClsIdConstant(const SourcePos& sourcePos, const std::u32string& s)
{
    try
    {
        std::u32string::size_type start = s.find(U'(');
        std::u32string::size_type end = s.find(U')');
        std::string typeId = ToUtf8(s.substr(start + 1, end - (start + 1)));
        return new ClsIdConstant(sourcePos, typeId);
    }
    catch (const std::exception& ex)
    {
        Error("error parsing string constant " + ToUtf8(s) + ": " + PlatformStringToUtf8(ex.what()), sourcePos);
    }
    return nullptr;
}

void Assembler::EmptyLine()
{
    if (!prevWasEmptyLine)
    {
        prevWasEmptyLine = true;
    }
    else
    {
        ++line;
    }
}

void Assembler::InstructionLine()
{
    IncLine();
    prevWasEmptyLine = false;
}

SourcePos Assembler::GetSourcePos(soulng::lexer::Lexer& lexer, int64_t pos)
{
    int64_t np = (static_cast<int64_t>(line) << 32) | (pos & 0xFFFF);
    SourcePos sp = lexer.GetSourcePos(np);
    return sp;
}

void Assembler::EmitOpCode(uint8_t opCode)
{
    if (currentSection->IsCodeSection())
    {
        currentSection->EmitByte(opCode);
    }
    else
    {
        Error(currentInstruction->GetOpCode()->Name() + " instruction can appear only in a .CODE section", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitRegisterValue(const std::string& paramName, const cmsx::object::Value& registerValue)
{
    if (registerValue.IsRegValue())
    {
        if (registerValue.Val() < 256)
        {
            currentSection->EmitByte(static_cast<uint8_t>(registerValue.Val()));
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " register operand (not in range 0...255)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error(currentInstruction->GetOpCode()->Name() + " " + paramName + " operand must be a register", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitPureByteValue(const std::string& paramName, const cmsx::object::Value& byteValue)
{
    if (byteValue.IsPureValue())
    {
        if (byteValue.Val() < 256)
        {
            currentSection->EmitByte(static_cast<uint8_t>(byteValue.Val()));
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " pure byte operand (not in range 0...255)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " operand (not pure value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitPureWydeValue(const std::string& paramName, const cmsx::object::Value& wydeValue)
{
    if (wydeValue.IsPureValue())
    {
        uint64_t value = wydeValue.Val();
        if (value <= std::numeric_limits<uint16_t>::max())
        {
            currentSection->EmitWyde(static_cast<uint16_t>(value));
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " pure wyde operand (not in range 0...65535)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " operand (not pure value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitPureTetraValue(const std::string& paramName, const cmsx::object::Value& tetraValue)
{
    if (tetraValue.IsPureValue())
    {
        uint64_t value = tetraValue.Val();
        if (value <= std::numeric_limits<int32_t>::max())
        {
            currentSection->EmitTetra(static_cast<uint32_t>(value));
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " pure tetra operand (not in range)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " operand (not pure value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitPureOctaValue(const std::string& paramName, const cmsx::object::Value& octaValue)
{
    if (octaValue.IsPureValue())
    {
        uint64_t value = octaValue.Val();
        if (value <= std::numeric_limits<uint64_t>::max())
        {
            currentSection->EmitOcta(static_cast<uint64_t>(value));
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " pure octa operand (not in range)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + paramName + " operand (not pure value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitSymbolOcta(const cmsx::object::Value& symbolValue)
{
    if (symbolValue.IsSymbolValue())
    {
        cmsx::object::Symbol* symbol = symbolValue.GetSymbol();
        cmsx::object::LinkFarOctaCommand* linkCommand = new cmsx::object::LinkFarOctaCommand(objectFile->GetDataSection()->Address(), symbol->Index());
        int32_t linkCommandId = objectFile->GetLinkSection()->AddLinkCommand(linkCommand, true);
        if (currentStructureSymbol)
        {
            currentStructureSymbol->AddLinkCommandId(linkCommandId);
        }
        currentSection->EmitOcta(static_cast<uint64_t>(-1));
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " " + " operand (not symbol value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitSetPureRegValue(const cmsx::object::Value& registerValue, const cmsx::object::Value& pureValue)
{
    if (registerValue.IsRegValue())
    {
        if (pureValue.IsPureValue())
        {
            uint64_t value = pureValue.Val();
            if (value <= std::numeric_limits<uint16_t>().max())
            {
                uint8_t b0 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b1 = static_cast<uint8_t>(value);
                EmitOpCode(cmsx::machine::SETL);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b1);
                EmitPureByteValue("Z", b0);
            }
            else if (value <= std::numeric_limits<uint32_t>().max())
            {
                uint8_t b0 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b1 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b2 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b3 = static_cast<uint8_t>(value);
                EmitOpCode(cmsx::machine::SETML);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b3);
                EmitPureByteValue("Z", b2);
                EmitOpCode(cmsx::machine::ORL);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b1);
                EmitPureByteValue("Z", b0);
            }
            else if (value <= std::numeric_limits<uint64_t>().max())
            {
                uint8_t b0 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b1 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b2 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b3 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b4 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b5 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b6 = static_cast<uint8_t>(value);
                value = value >> 8;
                uint8_t b7 = static_cast<uint8_t>(value);
                EmitOpCode(cmsx::machine::SETH);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b7);
                EmitPureByteValue("Z", b6);
                EmitOpCode(cmsx::machine::ORMH);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b5);
                EmitPureByteValue("Z", b4);
                EmitOpCode(cmsx::machine::ORML);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b3);
                EmitPureByteValue("Z", b2);
                EmitOpCode(cmsx::machine::ORL);
                EmitRegisterValue("X", registerValue);
                EmitPureByteValue("Y", b1);
                EmitPureByteValue("Z", b0);
            }
            else
            {
                Error("invalid " + currentInstruction->GetOpCode()->Name() + " Y operand (not in range)", currentInstruction->GetSourcePos());
            }
        }
        else
        {
            Error("invalid " + currentInstruction->GetOpCode()->Name() + " Y operand (not pure value)", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error("invalid " + currentInstruction->GetOpCode()->Name() + " X operand (not register value)", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitAbsoluteRegisterValue(const std::string& paramName, const cmsx::object::Value& registerValue, const cmsx::object::Value& symbolValue)
{
    cmsx::object::Symbol* symbol = symbolValue.GetSymbol();
    cmsx::object::LinkAbsoluteAddressCommand* linkCommand = new cmsx::object::LinkAbsoluteAddressCommand(objectFile->GetCodeSection()->Address(), symbol->Index());
    int32_t linkCommandId = objectFile->GetLinkSection()->AddLinkCommand(linkCommand, true);
    if (currentFunctionSymbol)
    {
        currentFunctionSymbol->AddLinkCommandId(linkCommandId);
    }
    if (currentSection->IsCodeSection())
    {
        if (registerValue.IsRegValue())
        {
            EmitOpCode(cmsx::machine::SETH);
            EmitRegisterValue(paramName, registerValue);
            currentSection->EmitShortOffset(static_cast<uint16_t>(-1));
            EmitOpCode(cmsx::machine::ORMH);
            EmitRegisterValue(paramName, registerValue);
            currentSection->EmitShortOffset(static_cast<uint16_t>(-1));
            EmitOpCode(cmsx::machine::ORML);
            EmitRegisterValue(paramName, registerValue);
            currentSection->EmitShortOffset(static_cast<uint16_t>(-1));
            EmitOpCode(cmsx::machine::ORL);
            EmitRegisterValue(paramName, registerValue);
            currentSection->EmitShortOffset(static_cast<uint16_t>(-1));
        }
        else
        {
            Error(currentInstruction->GetOpCode()->Name() + " " + paramName + " operand must be a register", currentInstruction->GetSourcePos());
        }
    }
    else
    {
        Error(currentInstruction->GetOpCode()->Name() + " instruction can appear only in a .CODE section", currentInstruction->GetSourcePos());
    }
}

void Assembler::EmitForwardLongJump(cmsx::object::Symbol* symbol)
{
    cmsx::object::LinkForwardLongJumpCommand* linkCommand = new cmsx::object::LinkForwardLongJumpCommand(objectFile->GetCodeSection()->Address(), symbol->Index());
    objectFile->GetLinkSection()->AddLinkCommand(linkCommand, false);
    EmitOpCode(cmsx::machine::JMP);
    currentSection->EmitLongOffset(static_cast<uint32_t>(-1));
}

void Assembler::EmitForwardShortJump(const std::string& paramName, uint8_t opcode, const cmsx::object::Value& registerValue, cmsx::object::Symbol* symbol)
{
    cmsx::object::LinkForwardShortJumpCommand* linkCommand = new cmsx::object::LinkForwardShortJumpCommand(objectFile->GetCodeSection()->Address(), symbol->Index());
    objectFile->GetLinkSection()->AddLinkCommand(linkCommand, false);
    EmitOpCode(opcode);
    EmitRegisterValue(paramName, registerValue);
    currentSection->EmitShortOffset(static_cast<uint16_t>(-1));
}

void Assembler::EmitLongOffset(uint32_t offset)
{
    currentSection->EmitLongOffset(offset);
}

void Assembler::EmitShortOffset(uint16_t offset)
{
    currentSection->EmitShortOffset(offset);
}

void Assembler::EmitClsIdCommmand(uint64_t typeIdIndex, const SourcePos& sourcePos)
{
    if (typeIdIndex >= 0 && typeIdIndex < typeIds.size())
    {
        boost::uuids::uuid typeId = typeIds[typeIdIndex];
        uint64_t t1;
        uint64_t t2;
        UuidToInts(typeId, t1, t2);
        cmsx::object::LinkClsIdCommand* linkCommand = new cmsx::object::LinkClsIdCommand(objectFile->GetDataSection()->Address(), t1, t2);
        int32_t linkCommandId = objectFile->GetLinkSection()->AddLinkCommand(linkCommand, true);
        if (currentStructureSymbol)
        {
            currentStructureSymbol->AddLinkCommandId(linkCommandId);
        }
        objectFile->GetDataSection()->EmitOcta(static_cast<uint64_t>(cmsx::object::undefinedValue));
    }
    else
    {
        Error("invalid type id index " + std::to_string(typeIdIndex), sourcePos);
    }
}

} // namespace cmsx::assembler
