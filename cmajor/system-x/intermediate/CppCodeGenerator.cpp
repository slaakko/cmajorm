// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/CppCodeGenerator.hpp>
#include <system-x/intermediate/Visitor.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Writer.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/Path.hpp>
#include <fstream>

namespace cmsx::intermediate {

using namespace soulng::util;
    
std::vector<Type*> TopologicalSort(const std::vector<Type*>& types, std::set<Type*>& visited);
void WriteForwardDeclaration(Type* type, CodeFormatter& formatter);
void WriteTypeDeclaration(Type* type, CodeFormatter& formatter);
void WriteTypeRef(Type* type, CodeFormatter& formatter);
void WriteFunctionHeader(Function* function, CodeFormatter& formatter);
void WriteFunctionDeclaration(Function* function, CodeFormatter& formatter);
void WriteLabel(int32_t labelId, CodeFormatter& formatter);
void WriteResult(RegValue* result, CodeFormatter& formatter);
void WriteRegValue(RegValue* regValue, CodeFormatter& formatter);
void WriteValue(Value* value, CodeFormatter& formatter);
void WriteVariableDeclarations(Function* function, CodeFormatter& formatter);
void WriteInst(Instruction& inst, Writer& writer);

class CppCodeGeneratorVisitor : public Visitor
{
public:
    CppCodeGeneratorVisitor(Context* context_);
    void WriteTypes();
    void WriteFunctions();
    void Visit(StructureType& structureType) override;
    void Visit(ArrayType& arrayType) override;
    void Visit(FunctionType& functionType) override;
    void Visit(GlobalVariable& globalVariable) override;
    void Visit(Function& function) override;
    void Visit(BasicBlock& basicBlock) override;
    void Visit(StoreInstruction& inst) override;
    void Visit(ArgInstruction& inst) override;
    void Visit(JmpInstruction& inst) override;
    void Visit(BranchInstruction& inst) override;
    void Visit(ProcedureCallInstruction& inst) override;
    void Visit(RetInstruction& inst) override;
    void Visit(SwitchInstruction& inst) override;
    void Visit(NotInstruction& inst) override;
    void Visit(NegInstruction& inst) override;
    void Visit(SignExtendInstruction& inst) override;
    void Visit(ZeroExtendInstruction& inst) override;
    void Visit(TruncateInstruction& inst) override;
    void Visit(BitcastInstruction& inst) override;
    void Visit(IntToFloatInstruction& inst) override;
    void Visit(FloatToIntInstruction& inst) override;
    void Visit(IntToPtrInstruction& inst) override;
    void Visit(PtrToIntInstruction& inst) override;
    void Visit(AddInstruction& inst) override;
    void Visit(SubInstruction& inst) override;
    void Visit(MulInstruction& inst) override;
    void Visit(DivInstruction& inst) override;
    void Visit(ModInstruction& inst) override;
    void Visit(AndInstruction& inst) override;
    void Visit(OrInstruction& inst) override;
    void Visit(XorInstruction& inst) override;
    void Visit(ShlInstruction& inst) override;
    void Visit(ShrInstruction& inst) override;
    void Visit(EqualInstruction& inst) override;
    void Visit(LessInstruction& inst) override;
    void Visit(ParamInstruction& inst) override;
    void Visit(LocalInstruction& inst) override;
    void Visit(LoadInstruction& inst) override;
    void Visit(ElemAddrInstruction& inst) override;
    void Visit(PtrOffsetInstruction& inst) override;
    void Visit(PtrDiffInstruction& inst) override;
    void Visit(FunctionCallInstruction& inst) override;
    void Visit(TrapInstruction& inst) override;
    void Visit(NoOperationInstruction& inst) override;
private:
    std::ofstream cppFile;
    CodeFormatter formatter;
    std::vector<Type*> weakTypes;
    std::vector<Type*> strongTypes;
    std::vector<Function*> functions;
    std::vector<Function*> definedFunctions;
    int paramNum;
    Writer writer;
};

CppCodeGeneratorVisitor::CppCodeGeneratorVisitor(Context* context_) : 
    Visitor(context_), 
    cppFile(Path::ChangeExtension(GetContext()->FilePath(), ".cpp")), 
    formatter(cppFile), paramNum(0), writer(context_, formatter)
{
    formatter.WriteLine("// C++ code generated from " + GetContext()->FilePath());
    formatter.WriteLine();
    formatter.WriteLine("#include <stdint.h>");
    formatter.WriteLine();
}

void CppCodeGeneratorVisitor::WriteTypes()
{
    GetContext()->GetTypes().VisitTypeDeclarations(*this);
    std::set<Type*> visited;
    std::vector<Type*> wtypes = TopologicalSort(weakTypes, visited);
    std::vector<Type*> stypes = TopologicalSort(strongTypes, visited);
    for (Type* type : wtypes)
    {
        WriteForwardDeclaration(type, formatter);
    }
    for (Type* type : stypes)
    {
        WriteForwardDeclaration(type, formatter);
    }
    for (Type* type : wtypes)
    {
        WriteTypeDeclaration(type, formatter);
    }
    for (Type* type : stypes)
    {
        WriteTypeDeclaration(type, formatter);
    }
}

void CppCodeGeneratorVisitor::Visit(StructureType& structureType)
{
    if (structureType.IsWeakType())
    {
        weakTypes.push_back(&structureType);
    }
    else
    {
        strongTypes.push_back(&structureType);
    }
}

void CppCodeGeneratorVisitor::Visit(ArrayType& arrayType)
{
    if (arrayType.IsWeakType())
    {
        weakTypes.push_back(&arrayType);
    }
    else
    {
        strongTypes.push_back(&arrayType);
    }
}

void CppCodeGeneratorVisitor::Visit(FunctionType& functionType)
{
    if (functionType.IsWeakType())
    {
        weakTypes.push_back(&functionType);
    }
    else
    {
        strongTypes.push_back(&functionType);
    }
}

void CppCodeGeneratorVisitor::Visit(GlobalVariable& globalVariable)
{
    // todo
}

void CppCodeGeneratorVisitor::WriteFunctions()
{
    GetContext()->GetCode().VisitFunctions(*this);
    if (!functions.empty())
    {
        formatter.WriteLine();
        for (Function* function : functions)
        {
            WriteFunctionDeclaration(function, formatter);
        }
    }
    if (!definedFunctions.empty())
    {
        for (Function* function : definedFunctions)
        {
            formatter.WriteLine();
            WriteFunctionHeader(function, formatter);
            formatter.WriteLine();
            formatter.WriteLine("{");
            formatter.IncIndent();
            WriteVariableDeclarations(function, formatter);
            paramNum = 0;
            function->VisitBasicBlocks(*this);
            formatter.DecIndent();
            formatter.WriteLine("}");
        }
    }
}

void CppCodeGeneratorVisitor::Visit(Function& function)
{
    functions.push_back(&function);
    if (function.IsDefined())
    {
        definedFunctions.push_back(&function);
    }
}

void CppCodeGeneratorVisitor::Visit(BasicBlock& basicBlock)
{
    formatter.DecIndent();
    WriteLabel(basicBlock.Id(), formatter);
    formatter.WriteLine(":");
    formatter.IncIndent();
    basicBlock.VisitInstructions(*this);
}

void CppCodeGeneratorVisitor::Visit(StoreInstruction& inst)
{
    WriteInst(inst, writer);
    formatter.Write("*");
    WriteValue(inst.GetPtr(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.GetValue(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ArgInstruction& inst)
{
    WriteInst(inst, writer);
}

void CppCodeGeneratorVisitor::Visit(JmpInstruction& inst)
{
    WriteInst(inst, writer);
    formatter.Write("goto ");
    WriteLabel(inst.TargetLabelId(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(BranchInstruction& inst)
{
    WriteInst(inst, writer);
    formatter.Write("if (");
    WriteValue(inst.Cond(), formatter);
    formatter.Write(") goto ");
    WriteLabel(inst.TrueTargetLabelId(), formatter);
    formatter.Write("; else goto ");
    WriteLabel(inst.FalseTargetLabelId(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ProcedureCallInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Callee(), formatter);
    formatter.Write("(");
    int n = inst.Args().size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        WriteValue(inst.Args()[i], formatter);
    }
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(RetInstruction& inst)
{
    WriteInst(inst, writer);
    if (inst.ReturnValue())
    {
        formatter.Write("return ");
        WriteValue(inst.ReturnValue(), formatter);
        formatter.WriteLine(";");
    }
    else
    {
        formatter.WriteLine("return;");
    }
}

void CppCodeGeneratorVisitor::Visit(SwitchInstruction& inst)
{
    WriteInst(inst, writer);
    formatter.Write("switch (");
    WriteValue(inst.Cond(), formatter);
    formatter.WriteLine(")");
    formatter.WriteLine("{");
    formatter.IncIndent();
    int n = inst.CaseTargets().size();
    for (int i = 0; i < n; ++i)
    {
        formatter.Write("case ");
        WriteValue(inst.CaseTargets()[i].caseValue, formatter);
        formatter.Write(": goto ");
        WriteLabel(inst.CaseTargets()[i].targetLabelId, formatter);
        formatter.WriteLine(";");
    }
    formatter.Write("default: goto ");
    WriteLabel(inst.DefaultTargetLabelId(), formatter);
    formatter.WriteLine(";");
    formatter.DecIndent();
    formatter.WriteLine("}");
}

void CppCodeGeneratorVisitor::Visit(NotInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = !");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(NegInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = -");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(SignExtendInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ZeroExtendInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(TruncateInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = static_cast<");
    WriteTypeRef(inst.Result()->GetType(), formatter);
    formatter.Write(">(");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(BitcastInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = static_cast<");
    WriteTypeRef(inst.Result()->GetType(), formatter);
    formatter.Write(">(");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(IntToFloatInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(FloatToIntInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = static_cast<");
    WriteTypeRef(inst.Result()->GetType(), formatter);
    formatter.Write(">(");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(IntToPtrInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = reinterpret_cast<");
    WriteTypeRef(inst.Result()->GetType(), formatter);
    formatter.Write(">(");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(PtrToIntInstruction& inst) 
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = reinterpret_cast<");
    WriteTypeRef(inst.Result()->GetType(), formatter);
    formatter.Write(">(");
    WriteValue(inst.Operand(), formatter);
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(AddInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" + ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(SubInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" - ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(MulInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" * ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(DivInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" / ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ModInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" % ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(AndInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" & ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(OrInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" | ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(XorInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" ^ ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ShlInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" << ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ShrInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" >> ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(EqualInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" == ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(LessInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Left(), formatter);
    formatter.Write(" < ");
    WriteValue(inst.Right(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ParamInstruction& inst)
{
    WriteInst(inst, writer);
    WriteResult(inst.Result(), formatter);
    formatter.WriteLine(" = _p" + std::to_string(paramNum++) +";");
}

void CppCodeGeneratorVisitor::Visit(LocalInstruction& inst)
{
    WriteInst(inst, writer);
    WriteRegValue(inst.Result(), formatter);
    formatter.WriteLine(" = &_local" + std::to_string(inst.Result()->Reg()) + ";");
}

void CppCodeGeneratorVisitor::Visit(LoadInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = *");
    WriteValue(inst.Ptr(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(ElemAddrInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = &");
    WriteValue(inst.Ptr(), formatter);
    formatter.Write("->m" + std::to_string(inst.Index()->GetIntegerValue()));
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(PtrOffsetInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Ptr(), formatter);
    formatter.Write(" + ");
    WriteValue(inst.Offset(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(PtrDiffInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.LeftPtr(), formatter);
    formatter.Write(" - ");
    WriteValue(inst.RightPtr(), formatter);
    formatter.WriteLine(";");
}

void CppCodeGeneratorVisitor::Visit(FunctionCallInstruction& inst)
{
    WriteInst(inst, writer);
    WriteValue(inst.Result(), formatter);
    formatter.Write(" = ");
    WriteValue(inst.Callee(), formatter);
    formatter.Write("(");
    int n = inst.Args().size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        WriteValue(inst.Args()[i], formatter);
    }
    formatter.WriteLine(");");
}

void CppCodeGeneratorVisitor::Visit(TrapInstruction& inst)
{
    WriteInst(inst, writer);
    // todo
}

void CppCodeGeneratorVisitor::Visit(NoOperationInstruction& inst)
{
    WriteInst(inst, writer);
    formatter.WriteLine("_nop = 0;");
}

std::set<Type*> GetDependsOnTypes(Type* type)
{
    std::set<Type*> dependsOnTypes;
    if (type->IsStructureType())
    {
        StructureType* structureType = static_cast<StructureType*>(type);
        int n = structureType->FieldCount();
        for (int i = 0; i < n; ++i)
        {
            Type* type = structureType->FieldType(i);
            dependsOnTypes.insert(type);
        }
    }
    else if (type->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(type);
        dependsOnTypes.insert(arrayType->ElementType());
    }
    else if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        dependsOnTypes.insert(functionType->ReturnType());
        int n = functionType->Arity();
        for (int i = 0; i < n; ++i)
        {
            dependsOnTypes.insert(functionType->ParamType(i));
        }
    }
    else if (type->IsPointerType())
    {
        PointerType* pointerType = static_cast<PointerType*>(type);
        dependsOnTypes.insert(pointerType->BaseType());
    }
    return dependsOnTypes;
}

void Visit(Type* type, std::vector<Type*>& sortedTypes, std::set<Type*>& visited, std::set<Type*>& tempVisit)
{
    if (type->IsFundamentalType()) return;
    if (tempVisit.find(type) == tempVisit.cend())
    {
        if (visited.find(type) == visited.end())
        {
            tempVisit.insert(type);
            std::set<Type*> dependsOn = GetDependsOnTypes(type);
            for (Type* type : dependsOn)
            {
                Visit(type, sortedTypes, visited, tempVisit);
            }
            tempVisit.erase(type);
            visited.insert(type);
            sortedTypes.push_back(type);
        }
    }
    else
    {
        throw std::runtime_error("circular type dependency '" + type->Name() + "' detected");
    }
}

std::vector<Type*> TopologicalSort(const std::vector<Type*>& types, std::set<Type*>& visited)
{
    std::vector<Type*> sortedTypes;
    std::set<Type*> tempVisit;
    for (Type* type : types)
    {
        Visit(type, sortedTypes, visited, tempVisit);
    }
    return sortedTypes;
}

void WriteForwardDeclaration(Type* type, CodeFormatter& formatter)
{
    if (type->IsStructureType())
    {
        formatter.WriteLine("struct _S" + std::to_string(MakeUserTypeId(type->Id())) + ";");
    }
}

void WriteTypeRef(Type* type, CodeFormatter& formatter)
{
    int8_t pointerCount = GetPointerCount(type->Id());
    int32_t baseTypeId = GetBaseTypeId(type->Id());
    switch (baseTypeId)
    {
        case voidTypeId: formatter.Write("void"); break;
        case boolTypeId: formatter.Write("bool"); break;
        case sbyteTypeId: formatter.Write("int8_t"); break;
        case byteTypeId: formatter.Write("uint8_t"); break;
        case shortTypeId: formatter.Write("int16_t"); break;
        case ushortTypeId: formatter.Write("uint16_t"); break;
        case intTypeId: formatter.Write("int32_t"); break;
        case uintTypeId: formatter.Write("uint32_t"); break;
        case longTypeId: formatter.Write("int64_t"); break;
        case ulongTypeId: formatter.Write("uint64_t"); break;
        case floatTypeId: formatter.Write("float"); break;
        case doubleTypeId: formatter.Write("double"); break;
        default:
        {
            if (type->IsStructureType())
            {
                formatter.Write("_S" + std::to_string(MakeUserTypeId(baseTypeId)));
            }
            else if (type->IsArrayType())
            {
                formatter.Write("_A" + std::to_string(MakeUserTypeId(baseTypeId)));
            }
            else if (type->IsFunctionType())
            {
                formatter.Write("_F" + std::to_string(MakeUserTypeId(baseTypeId)));
            }
            break;
        }
    }
    for (int8_t i = 0; i < pointerCount; ++i)
    {
        formatter.Write("*");
    }
}

void WriteTypeDeclaration(Type* type, CodeFormatter& formatter)
{
    if (type->IsStructureType())
    {
        StructureType* structureType = static_cast<StructureType*>(type);
        formatter.WriteLine("struct _S" + std::to_string(MakeUserTypeId(type->Id())));
        formatter.WriteLine("{");
        formatter.IncIndent();
        int n = structureType->FieldCount();
        for (int i = 0; i < n; ++i)
        {
            Type* type = structureType->FieldType(i);
            WriteTypeRef(type, formatter);
            formatter.WriteLine(" m" + std::to_string(i) + ";");
        }
        formatter.DecIndent();
        formatter.WriteLine("};");
    }
    else if (type->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(type);
        formatter.Write("using _A" + std::to_string(MakeUserTypeId(type->Id())) + " = ");
        WriteTypeRef(arrayType->ElementType(), formatter);
        formatter.Write("[");
        formatter.Write(std::to_string(arrayType->ElementCount()));
        formatter.WriteLine("];");
    }
    else if (type->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(type);
        formatter.Write("using _F" + std::to_string(MakeUserTypeId(type->Id())) + " = ");
        WriteTypeRef(functionType->ReturnType(), formatter);
        formatter.Write("(");
        int n = functionType->Arity();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                formatter.Write(", ");
            }
            Type* paramType = functionType->ParamType(i);
            WriteTypeRef(paramType, formatter);
        }
        formatter.WriteLine(");");
    }
}

void WriteFunctionHeader(Function* function, CodeFormatter& formatter)
{
    WriteTypeRef(function->GetType()->ReturnType(), formatter);
    formatter.Write(" ");
    formatter.Write(function->Name());
    formatter.Write("(");
    int n = function->Arity();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            formatter.Write(", ");
        }
        WriteTypeRef(function->GetType()->ParamType(i), formatter);
        formatter.Write(" _p" + std::to_string(i));
    }
    formatter.Write(")");
}

void WriteFunctionDeclaration(Function* function, CodeFormatter& formatter)
{
    WriteFunctionHeader(function, formatter);
    formatter.WriteLine(";");
}

void WriteLabel(int32_t labelId, CodeFormatter& formatter)
{
    formatter.Write("_L" + std::to_string(labelId));
}

void WriteResult(RegValue* regValue, CodeFormatter& formatter)
{
    WriteRegValue(regValue, formatter);
}

void WriteRegValue(RegValue* regValue, CodeFormatter& formatter)
{
    formatter.Write("_v" + std::to_string(regValue->Reg()));
}

void WriteValue(Value* value, CodeFormatter& formatter)
{
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        WriteRegValue(regValue, formatter);
    }
    else if (value->IsSymbolValue())
    {
        SymbolValue* symbolValue = static_cast<SymbolValue*>(value);
        formatter.Write(symbolValue->Symbol());
    }
    else if (value->IsGlobalVariable())
    {
        GlobalVariable* globalVariable = static_cast<GlobalVariable*>(value);
        formatter.Write(globalVariable->Name());
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
            {
                SByteValue* sbyteValue = static_cast<SByteValue*>(value);
                formatter.Write(std::to_string(static_cast<int>(sbyteValue->GetValue())));
                break;
            }
            case ValueKind::byteValue:
            {
                ByteValue* byteValue = static_cast<ByteValue*>(value);
                formatter.Write(std::to_string(static_cast<int>(byteValue->GetValue())));
                break;
            }
            case ValueKind::shortValue:
            {
                ShortValue* shortValue = static_cast<ShortValue*>(value);
                formatter.Write(std::to_string(static_cast<int>(shortValue->GetValue())));
                break;
            }
            case ValueKind::ushortValue:
            {
                UShortValue* ushortValue = static_cast<UShortValue*>(value);
                formatter.Write(std::to_string(static_cast<int>(ushortValue->GetValue())));
                break;
            }
            case ValueKind::intValue:
            {
                IntValue* intValue = static_cast<IntValue*>(value);
                formatter.Write(std::to_string(intValue->GetValue()));
                break;
            }
            case ValueKind::uintValue:
            {
                UIntValue* uintValue = static_cast<UIntValue*>(value);
                formatter.Write(std::to_string(uintValue->GetValue()));
                break;
            }
            case ValueKind::longValue:
            {
                LongValue* longValue = static_cast<LongValue*>(value);
                formatter.Write(std::to_string(longValue->GetValue()));
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
                formatter.Write("nullptr");
                break;
            }
        }
    }
}

void WriteVariableDeclarations(Function* function, CodeFormatter& formatter)
{
    formatter.WriteLine("int _nop;");
    BasicBlock* basicBlock = function->FirstBasicBlock();
    while (basicBlock)
    {
        Instruction* inst = basicBlock->FirstInstruction();
        while (inst)
        {
            if (inst->IsLocalInstruction())
            {
                LocalInstruction* localInst = static_cast<LocalInstruction*>(inst);
                WriteTypeRef(localInst->LocalType(), formatter);
                formatter.WriteLine(" _local" + std::to_string(localInst->Result()->Reg()) + ";");
            }
            if (inst->IsValueInstruction())
            {
                ValueInstruction* valueInst = static_cast<ValueInstruction*>(inst);
                RegValue* result = valueInst->Result();
                WriteTypeRef(result->GetType(), formatter);
                formatter.Write(" ");
                WriteRegValue(result, formatter);
                formatter.WriteLine(";");
            }
            inst = inst->Next();
        }
        basicBlock = basicBlock->Next();
    }
}

void WriteInst(Instruction& inst, Writer& writer)
{
    writer.Formatter().Write("// ");
    inst.Accept(writer);
}

void GenerateCppCode(Context& context)
{
    CppCodeGeneratorVisitor visitor(&context);
    visitor.WriteTypes();
    visitor.WriteFunctions();
}

} // cmsx::intermediate
