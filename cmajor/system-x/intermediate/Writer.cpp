// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Writer.hpp>
#include <system-x/intermediate/Context.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmsx::intermediate {

namespace {

void WriteTypeRef(Type* type, CodeFormatter& formatter, int w)
{
    std::string s;
    int8_t pointerCount = GetPointerCount(type->Id());
    int32_t baseTypeId = GetBaseTypeId(type->Id());
    switch (baseTypeId)
    {
        case voidTypeId: s = "void"; break;
        case boolTypeId: s = "bool"; break;
        case sbyteTypeId: s = "sbyte";  break;
        case byteTypeId: s = "byte"; break;
        case shortTypeId: s = "short"; break;
        case ushortTypeId: s = "ushort"; break;
        case intTypeId: s = "int"; break;
        case uintTypeId: s = "uint"; break;
        case longTypeId: s = "long"; break;
        case ulongTypeId: s = "ulong"; break;
        case floatTypeId: s = "float"; break;
        case doubleTypeId: s = "double";  break;
        default:
        {
            s = "$T" + std::to_string(MakeUserTypeId(baseTypeId));
            break;
        }
    }
    s.append(pointerCount, '*');
    formatter.Write(Format(s, w, FormatWidth::min));
}

void Write(Value* value, CodeFormatter& formatter, int w)
{
    WriteTypeRef(value->GetType(), formatter, w);
    formatter.Write(" ");
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        formatter.Write("$" + std::to_string(regValue->Reg()));
    }
    else if (value->IsSymbolValue())
    {
        SymbolValue* symbolValue = static_cast<SymbolValue*>(value);
        formatter.Write("@" + symbolValue->Symbol());
    }
    else
    {
        switch (value->Kind())
        {
        case ValueKind::boolValue:
        {
            BoolValue* boolValue = static_cast<BoolValue*>(value);
            if (boolValue->GetValue())
            {
                formatter.Write("true");
            }
            else
            {
                formatter.Write("false");
            }
            break;
        }
        case ValueKind::sbyteValue:
        case ValueKind::byteValue:
        case ValueKind::shortValue:
        case ValueKind::ushortValue:
        case ValueKind::intValue:
        case ValueKind::uintValue:
        case ValueKind::longValue:
        {
            formatter.Write(std::to_string(value->GetIntegerValue()));
            break;
        }
        case ValueKind::ulongValue:
        {
            ULongValue* ulongValue = static_cast<ULongValue*>(value);
            formatter.Write(std::to_string(ulongValue->GetValue()));
            break;
        }
        case ValueKind::floatValue:
        {
            FloatValue* floatValue = static_cast<FloatValue*>(value);
            formatter.Write(std::to_string(floatValue->GetValue()));
            break;
        }
        case ValueKind::doubleValue:
        {
            DoubleValue* doubleValue = static_cast<DoubleValue*>(value);
            formatter.Write(std::to_string(doubleValue->GetValue()));
            break;
        }
        case ValueKind::nullValue:
        {
            formatter.Write("null");
            break;
        }
        case ValueKind::arrayValue:
        {
            formatter.Write("[ARRAY]");
            break;
        }
        case ValueKind::structureValue:
        {
            formatter.Write("{STRUCTURE}");
            break;
        }
        case ValueKind::stringValue:
        {
            formatter.Write("\"STRING\"");
            break;
        }
        case ValueKind::stringArrayValue:
        {
            StringArrayValue* stringArrayValue = static_cast<StringArrayValue*>(value);
            switch (stringArrayValue->Prefix())
            {
            case 'b': formatter.Write("b");
            case 'w': formatter.Write("w");
            case 'u': formatter.Write("u");
            }
            formatter.Write("[\"STRING_ARRAY\"]");
            break;
        }
        case ValueKind::conversionValue:
        {
            formatter.Write("CONV");
            break;
        }
        case ValueKind::clsIdValue:
        {
            formatter.Write("$CLSID");
            break;
        }
        }
    }
}

void WriteLabel(int32_t id, CodeFormatter& formatter, int w)
{
    formatter.Write(Format("@" + std::to_string(id), w, FormatWidth::min));
}

} // namespace

Writer::Writer(Context* context_, const std::string& fileName) : 
    Visitor(context_), file(fileName), fileFormatter(new CodeFormatter(file)), formatter(*fileFormatter), newLines(false)
{
    formatter.SetIndentSize(8);
}

Writer::Writer(Context* context_, CodeFormatter& formatter_) : Visitor(context_), formatter(formatter_), newLines(true)
{
}

void Writer::Visit(Function& function)
{
    if (function.Parent()->FirstFunction() != &function)
    {
        formatter.WriteLine();
    }
    formatter.WriteLine("function " + function.Name());
    formatter.WriteLine("{");
    formatter.IncIndent();
    function.VisitBasicBlocks(*this);
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void Writer::Visit(BasicBlock& basicBlock)
{
    if (basicBlock.Parent()->FirstBasicBlock() != &basicBlock)
    {
        formatter.WriteLine();
    }
    formatter.DecIndent();
    formatter.Write(Format("@" + basicBlock.Name(), 8));
    Instruction* inst = basicBlock.FirstInstruction();
    if (!inst)
    {
        formatter.IncIndent();
        formatter.WriteLine();
    }
    bool first = true;
    while (inst)
    {
        inst->Accept(*this);
        if (first)
        {
            formatter.IncIndent();
            first = false;
        }
        formatter.WriteLine();
        inst = inst->Next();
    }
}

void Writer::Visit(StoreInstruction& inst)
{
    formatter.Write(Format("store ", 8));
    Write(inst.GetValue(), formatter, 0);
    formatter.Write(", ");
    Write(inst.GetPtr(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ArgInstruction& inst)
{
    formatter.Write(Format("arg ", 8));
    Write(inst.Arg(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(JmpInstruction& inst)
{
    formatter.Write(Format("jmp ", 8));
    WriteLabel(inst.TargetLabelId(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(BranchInstruction& inst)
{
    formatter.Write(Format("branch ", 8));
    Write(inst.Cond(), formatter, 0);
    formatter.Write(", ");
    WriteLabel(inst.TrueTargetLabelId(), formatter, 0);
    formatter.Write(", ");
    WriteLabel(inst.FalseTargetLabelId(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ProcedureCallInstruction& inst)
{
    formatter.Write(Format("call ", 8));
    Write(inst.Callee(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(RetInstruction& inst)
{
    formatter.Write(Format("ret ", 8));
    if (inst.ReturnValue())
    {
        Write(inst.ReturnValue(), formatter, 0);
    }
    else
    {
        formatter.Write("void");
    }
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(SwitchInstruction& inst)
{
    formatter.Write(Format("switch ", 8));
    Write(inst.Cond(), formatter, 0);
    WriteLabel(inst.DefaultTargetLabelId(), formatter, 0);
    formatter.Write(", [");
    int n = inst.CaseTargets().size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(" : ");
        }
        Write(inst.CaseTargets()[i].caseValue, formatter, 0);
        formatter.Write(", ");
        WriteLabel(inst.CaseTargets()[i].targetLabelId, formatter, 0);
    }
    formatter.Write("]");
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(NotInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = not ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(NegInstruction& inst) 
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = neg ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(SignExtendInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = signextend ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ZeroExtendInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = zeroextend ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(TruncateInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = truncate ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(BitcastInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = bitcast ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(IntToFloatInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = inttofloat ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(FloatToIntInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = floattoint ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(IntToPtrInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = inttoptr ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(PtrToIntInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = ptrtoint ");
    Write(inst.Operand(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(AddInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = add ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(SubInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = sub ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(MulInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = mul ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(DivInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = div ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ModInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = mod ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(AndInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = and ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(OrInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = or ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(XorInstruction& inst) 
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = xor ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ShlInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = shl ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ShrInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = shr ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(EqualInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = equal ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(LessInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = less ");
    Write(inst.Left(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Right(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ParamInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = param");
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(LocalInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = local ");
    WriteTypeRef(inst.LocalType(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(LoadInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = load ");
    Write(inst.Ptr(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(ElemAddrInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = elemaddr ");
    Write(inst.Ptr(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Index(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(PtrOffsetInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = ptroffset ");
    Write(inst.Ptr(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Offset(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(PtrDiffInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = ptrdiff ");
    Write(inst.LeftPtr(), formatter, 0);
    formatter.Write(", ");
    Write(inst.RightPtr(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(FunctionCallInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = call ");
    Write(inst.Callee(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(TrapInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = trap ");
    Write(inst.Op1(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Op2(), formatter, 0);
    formatter.Write(", ");
    Write(inst.Op3(), formatter, 0);
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(PhiInstruction& inst)
{
    Write(inst.Result(), formatter, 7);
    formatter.Write(" = phi ");
    bool first = true;
    for (const BlockValue& blockValue : inst.BlockValues())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.Write("[");
        Write(blockValue.value, formatter, 0);
        formatter.Write(", ");
        WriteLabel(blockValue.blockId, formatter, 0);
        formatter.Write("]");
    }
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Writer::Visit(NoOperationInstruction& inst) 
{
    formatter.Write("nop");
    if (newLines)
    {
        formatter.WriteLine();
    }
}

void Write(Context& context, const std::string& fileName)
{
    Writer writer(&context, fileName);
    context.GetCode().VisitFunctions(writer);
}

} // cmsx::intermediate
