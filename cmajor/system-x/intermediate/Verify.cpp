// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/Verify.hpp>
#include <system-x/intermediate/Visitor.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/Util.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <soulng/util/Path.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

class VerifierVisitor : public Visitor
{
public:
    VerifierVisitor(Context* context_);
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
    void Visit(PhiInstruction& inst) override;
    void Visit(NoOperationInstruction& inst) override;
private:
    void CheckSameType(const std::string& typeDescription, Type* type, const std::string& expectedTypeDescription, Type* expected, const SourcePos& sourcePos);
    void CheckType(const std::string& typeDescription, Type* type, Type* assertedType, const std::string& expectedTypeDescription, Type* expected, const SourcePos& sourcePos);
    void CheckBinaryInstructionTypes(BinaryInstruction& inst);
    void CheckBooleanInstructionTypes(BinaryInstruction& inst);
    void CheckArithmeticType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckArithmeticOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckArithmeticOrPointerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckIntegerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckIntegerOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckFloatingPointType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckPointerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckArithmeticPointerOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos);
    void CheckRegValue(Value* value, const SourcePos& sourcePos);
    void CheckValueInstruction(ValueInstruction* valueInstruction);
    void CheckUnaryInstuction(UnaryInstruction* unaryInstruction);
    void CheckBinaryInstuction(BinaryInstruction* binaryInstruction);
    Function* currentFunction;
    BasicBlock* currentBasicBlock;
    int numParams;
    std::vector<Value*> arguments;
    int32_t regNumber;
    int index;
};

VerifierVisitor::VerifierVisitor(Context* context_) : Visitor(context_), currentFunction(nullptr), currentBasicBlock(nullptr), numParams(0), regNumber(0), index(0)
{
}

void VerifierVisitor::CheckSameType(const std::string& typeDescription, Type* type, const std::string& expectedTypeDescription, Type* expected, const SourcePos& sourcePos)
{
    if (type != expected)
    {
        Error("type check error: " + typeDescription + " is '" + type->Name() + "' but " + expectedTypeDescription + " '" + expected->Name() + "' expected", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckType(const std::string& typeDescription, Type* type, Type* assertedType, const std::string& expectedTypeDescription, Type* expected, const SourcePos& sourcePos)
{
    if (assertedType != expected)
    {
        Error("type check error: " + typeDescription + " is '" + type->Name() + "', and " + expectedTypeDescription + " is '" + assertedType->Name() + 
            "' but '" + expected->Name() + "' expected", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckBinaryInstructionTypes(BinaryInstruction& inst)
{
    if (inst.Result()->GetType() != inst.Left()->GetType() || inst.Result()->GetType() != inst.Right()->GetType())
    {
        Error("type check error: equal types expected: types are '" + inst.Result()->GetType()->Name() + "', '" + inst.Left()->GetType()->Name() + +"' and '" +
            inst.Right()->GetType()->Name() + "'", inst.GetSourcePos(), GetContext());
    }
}

void VerifierVisitor::CheckBooleanInstructionTypes(BinaryInstruction& inst)
{
    CheckSameType("result type", inst.Result()->GetType(), "Boolean type", GetContext()->GetTypes().GetBoolType(), inst.GetSourcePos());
    if (inst.Left()->GetType() != inst.Right()->GetType())
    {
        Error("type check error: operand types '" + inst.Left()->GetType()->Name() + "' and '" + inst.Right()->GetType()->Name() + "' differ", inst.GetSourcePos(), GetContext());
    }
}

void VerifierVisitor::CheckArithmeticType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsArithmeticType())
    {
        Error("type check error: arithmetic type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckArithmeticOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsArithmeticType() && !type->IsBooleanType())
    {
        Error("type check error: arithmetic or Boolean type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckArithmeticOrPointerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsArithmeticType() && !type->IsPointerType())
    {
        Error("type check error: arithmetic or pointer type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckIntegerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsIntegerType())
    {
        Error("type check error: integer type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckIntegerOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsIntegerType() && !type->IsBooleanType())
    {
        Error("type check error: integer or Boolean type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckFloatingPointType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsFloatingPointType())
    {
        Error("type check error: floating-point type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckPointerType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsPointerType())
    { 
        Error("type check error: pointer type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckArithmeticPointerOrBooleanType(Type* type, const std::string& typeDescription, const SourcePos& sourcePos)
{
    if (!type->IsBooleanType() && !type->IsArithmeticType() && !type->IsPointerType())
    {
        Error("type check exception: Boolean, arithmetic or pointer type expected, note: " + typeDescription + " is '" + type->Name() + "'", sourcePos, GetContext());
    }
}

void VerifierVisitor::CheckRegValue(Value* value, const SourcePos& sourcePos)
{
    if (value->IsRegValue())
    {
        RegValue* regValue = static_cast<RegValue*>(value);
        if (regValue->Reg() >= 0 && regValue->Reg() < regNumber)
        {
            Instruction* inst = currentFunction->GetInstruction(regValue->Reg());
            if (inst)
            {
                regValue->SetInst(inst);
            }
            else
            {
                Error("code verification error: instruction '" + std::to_string(regValue->Reg()) + " not found from function '" + currentFunction->Name() + "'",
                    sourcePos, GetContext());
            }
        }
        else
        {
            Error("code verification error: invalid register value " + std::to_string(regValue->Reg()) + 
                ": note: value must be greater than or equal to zero and less than " + std::to_string(regNumber), sourcePos, GetContext());
        }
    }
}

void VerifierVisitor::CheckValueInstruction(ValueInstruction* valueInstruction)
{
    RegValue* regValue = valueInstruction->Result();
    if (regValue->Reg() == regNumber)
    {
        regValue->SetInst(valueInstruction);
        ++regNumber;
    }
    else
    {
        Error("code verification error: result register value " + std::to_string(regNumber) + " expected", valueInstruction->GetSourcePos(), GetContext());
    }
}

void VerifierVisitor::CheckUnaryInstuction(UnaryInstruction* unaryInstruction)
{
    CheckValueInstruction(unaryInstruction);
    CheckRegValue(unaryInstruction->Operand(), unaryInstruction->GetSourcePos());
}

void VerifierVisitor::CheckBinaryInstuction(BinaryInstruction* binaryInstruction)
{
    CheckValueInstruction(binaryInstruction);
    CheckRegValue(binaryInstruction->Left(), binaryInstruction->GetSourcePos());
    CheckRegValue(binaryInstruction->Right(), binaryInstruction->GetSourcePos());
}

void VerifierVisitor::Visit(Function& function)
{
    if (!function.IsDefined()) return;
    index = 0;
    numParams = 0;
    regNumber = 0;
    currentFunction = &function;
    function.VisitBasicBlocks(*this);
    if (numParams != function.Arity())
    {
        Error("code verification error: function '" + function.Name() + "' has wrong number of parameters: " + std::to_string(numParams) + 
            " parameters, function arity=" + std::to_string(function.Arity()), function.GetSourcePos(), GetContext());
    }
    function.SetNextRegNumber(regNumber);
}

void VerifierVisitor::Visit(BasicBlock& basicBlock)
{
    currentBasicBlock = &basicBlock;
    if (basicBlock.IsEmpty())
    {
        Error("code verification error: basic block " + std::to_string(basicBlock.Id()) + " is empty", basicBlock.GetSourcePos(), GetContext());
    }
    if (!basicBlock.LastInstruction()->IsTerminator())
    {
        Error("code verification error: the last instruction '" + basicBlock.LastInstruction()->Name() + "' of basic block " + std::to_string(basicBlock.Id()) + 
            " is not a terminator", basicBlock.LastInstruction()->GetSourcePos(), GetContext());
    }
    basicBlock.VisitInstructions(*this);
}

void VerifierVisitor::Visit(StoreInstruction& inst)
{
    CheckArithmeticPointerOrBooleanType(inst.GetValue()->GetType(), "type of value", inst.GetSourcePos());
    CheckType("type of value", inst.GetValue()->GetType(), inst.GetValue()->GetType()->AddPointer(GetContext()), "pointer type", inst.GetPtr()->GetType(), inst.GetSourcePos());
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ArgInstruction& inst)
{
    CheckArithmeticPointerOrBooleanType(inst.Arg()->GetType(), "type of argument", inst.GetSourcePos());
    arguments.push_back(inst.Arg());
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(JmpInstruction& inst)
{
    int32_t target = inst.TargetLabelId();
    BasicBlock* basicBlock = inst.Parent();
    Function* function = basicBlock->Parent();
    BasicBlock* targetBasicBlock = function->GetBasicBlock(target);
    if (!targetBasicBlock)
    {
        Error("code verification error: jump target basic block " + std::to_string(target) + " not found from function '" + function->Name() + "'", inst.GetSourcePos(), GetContext());
    }
    inst.SetTargetBasicBlock(targetBasicBlock);
    if (&inst != basicBlock->LastInstruction())
    {
        Error("code verification error: terminator in the middle of basic block " + std::to_string(basicBlock->Id()), inst.GetSourcePos(), GetContext());
    }
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(BranchInstruction& inst)
{
    CheckSameType("condition type", inst.Cond()->GetType(), "Boolean type", GetContext()->GetTypes().GetBoolType(), inst.GetSourcePos());
    int32_t trueTarget = inst.TrueTargetLabelId();
    BasicBlock* trueBasicBlock = inst.Parent();
    Function* trueFunction = trueBasicBlock->Parent();
    BasicBlock* trueTargetBasicBlock = trueFunction->GetBasicBlock(trueTarget);
    if (!trueTargetBasicBlock)
    {
        Error("code verification error: branch true target basic block " + std::to_string(trueTarget) + " not found from function '" + trueFunction->Name() + "'", 
            inst.GetSourcePos(), GetContext());
    }
    inst.SetTrueTargetBasicBlock(trueTargetBasicBlock);
    int32_t falseTarget = inst.FalseTargetLabelId();
    BasicBlock* falseBasicBlock = inst.Parent();
    Function* falseFunction = falseBasicBlock->Parent();
    BasicBlock* falseTargetBasicBlock = falseFunction->GetBasicBlock(falseTarget);
    if (!falseTargetBasicBlock)
    {
        Error("code verification error: branch false target basic block " + std::to_string(falseTarget) + " not found from function '" + falseFunction->Name() + "'", 
            inst.GetSourcePos(), GetContext());
    }
    inst.SetFalseTargetBasicBlock(falseTargetBasicBlock);
    if (&inst != inst.Parent()->LastInstruction())
    {
        Error("code verification error: terminator in the middle of basic block " + std::to_string(inst.Parent()->Id()), inst.GetSourcePos(), GetContext());
    }
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ProcedureCallInstruction& inst)
{
    Type* calleeType = inst.Callee()->GetType();
    if (calleeType->IsPointerType())
    {
        calleeType = calleeType->RemovePointer(inst.GetSourcePos(), GetContext());
    }
    if (calleeType->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(calleeType);
        if (functionType->Arity() != arguments.size())
        {
            Error("code verification error: function call has wrong number of arguments: " + std::to_string(functionType->Arity()) + " arguments expected: note: " +
                std::to_string(arguments.size()) + " arguments passed", inst.GetSourcePos(), GetContext());
        }
        if (inst.Callee()->IsSymbolValue())
        {
            SymbolValue* calleeSymbolValue = static_cast<SymbolValue*>(inst.Callee());
            Function* function = GetContext()->AddFunctionDeclaration(inst.GetSourcePos(), calleeType, calleeSymbolValue->Symbol());
            if (function)
            {
                calleeSymbolValue->SetFunction(function);
            }
            else
            {
                Error("internal error: could not add function declaration", inst.GetSourcePos(), GetContext());
            }
        }
    }
    else
    {
        Error("type check error: callee in function call has invalid type: function type or function pointer type expected: note: type is " + inst.Callee()->GetType()->Name(),
            inst.GetSourcePos(), GetContext());
    }
    inst.SetArgs(std::move(arguments));
    inst.AddUse();
    arguments.clear();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(RetInstruction& inst) 
{
    FunctionType* functionType = currentFunction->GetType();
    if (inst.ReturnValue())
    {
        Type* returnValueType = inst.ReturnValue()->GetType();
        CheckSameType("instruction return type", returnValueType, "function return type", functionType->ReturnType(), inst.GetSourcePos());
        CheckArithmeticPointerOrBooleanType(returnValueType, "instruction return type", inst.GetSourcePos());
    }
    else
    {
        Type* returnValueType = GetContext()->GetTypes().GetVoidType();
        CheckSameType("instruction return type", returnValueType, "function return type", functionType->ReturnType(), inst.GetSourcePos());
    }
    if (&inst != inst.Parent()->LastInstruction())
    {
        Error("code verification error: terminator in the middle of basic block " + std::to_string(inst.Parent()->Id()), inst.GetSourcePos(), GetContext());
    }
    inst.AddUse();
    currentFunction->AddRetBlock(inst.Parent());
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(SwitchInstruction& inst)
{
    CheckIntegerOrBooleanType(inst.Cond()->GetType(), "condition type", inst.GetSourcePos());
    if (&inst != inst.Parent()->LastInstruction())
    {
        Error("code verification error: terminator in the middle of basic block " + std::to_string(inst.Parent()->Id()), inst.GetSourcePos(), GetContext());
    }
    int32_t defaultTarget = inst.DefaultTargetLabelId();
    BasicBlock* parent = inst.Parent();
    Function* function = parent->Parent();
    BasicBlock* defaultTargetBlock = function->GetBasicBlock(defaultTarget);
    if (!defaultTargetBlock)
    {
        Error("code verification error: switch default target basic block " + std::to_string(defaultTarget) + 
            " not found from function '" + function->Name() + "'", inst.GetSourcePos(), GetContext());
    }
    inst.SetDefaultTargetBlock(defaultTargetBlock);
    for (CaseTarget& caseTarget : inst.CaseTargets())
    {
        BasicBlock* caseTargetBlock = function->GetBasicBlock(caseTarget.targetLabelId);
        if (!caseTargetBlock)
        {
            Error("code verification error: switch case target basic block " + std::to_string(caseTarget.targetLabelId) +
                " not found from function '" + function->Name() + "'", inst.GetSourcePos(), GetContext());
        }
        caseTarget.targetBlock = caseTargetBlock;
    }
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(NotInstruction& inst)
{
    CheckSameType("result type", inst.Result()->GetType(), "Boolean type", GetContext()->GetTypes().GetBoolType(), inst.GetSourcePos());
    CheckSameType("operand type", inst.Operand()->GetType(), "Boolean type", GetContext()->GetTypes().GetBoolType(), inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(NegInstruction& inst)
{
    CheckSameType("result type", inst.Result()->GetType(), "operand type", inst.Operand()->GetType(), inst.GetSourcePos());
    CheckArithmeticType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(SignExtendInstruction& inst) 
{
    CheckArithmeticType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    if (inst.Result()->GetType()->Size() <= inst.Operand()->GetType()->Size())
    {
        Error("code verification error: result type width expected to be greater than operand type width", inst.GetSourcePos(), GetContext());
    }
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ZeroExtendInstruction& inst)
{
    CheckArithmeticType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    if (inst.Result()->GetType()->Size() <= inst.Operand()->GetType()->Size())
    {
        Error("code verification error: result type width expected to be greater than operand type width", inst.GetSourcePos(), GetContext());
    }
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(TruncateInstruction& inst)
{
    CheckArithmeticType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckArithmeticOrBooleanType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    if (inst.Result()->GetType()->Size() >= inst.Operand()->GetType()->Size() && !inst.Result()->GetType()->IsBooleanType())
    {
        Error("code verification error: result type width expected to be less than operand type width", inst.GetSourcePos(), GetContext());
    }
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(BitcastInstruction& inst)
{
    CheckArithmeticPointerOrBooleanType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckArithmeticPointerOrBooleanType(inst.Result()->GetType(), "operand type", inst.GetSourcePos());
    if (inst.Result()->GetType()->Size() != inst.Operand()->GetType()->Size())
    {
        Error("code verification error: result type width expected to be same as operand type width", inst.GetSourcePos(), GetContext());
    }
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(IntToFloatInstruction& inst)
{
    CheckIntegerType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckFloatingPointType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(FloatToIntInstruction& inst)
{
    CheckFloatingPointType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(IntToPtrInstruction& inst)
{
    CheckIntegerType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckPointerType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(PtrToIntInstruction& inst)
{
    CheckPointerType(inst.Operand()->GetType(), "operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckUnaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(AddInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckArithmeticType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(SubInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckArithmeticType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(MulInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckArithmeticType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(DivInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckArithmeticType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ModInstruction& inst) 
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(AndInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(OrInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(XorInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ShlInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ShrInstruction& inst)
{
    CheckBinaryInstructionTypes(inst);
    CheckIntegerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(EqualInstruction& inst)
{
    CheckBooleanInstructionTypes(inst);
    CheckArithmeticPointerOrBooleanType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticPointerOrBooleanType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(LessInstruction& inst)
{
    CheckBooleanInstructionTypes(inst);
    CheckArithmeticOrPointerType(inst.Left()->GetType(), "left operand type", inst.GetSourcePos());
    CheckArithmeticOrPointerType(inst.Right()->GetType(), "right operand type", inst.GetSourcePos());
    CheckBinaryInstuction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ParamInstruction& inst)
{
    CheckArithmeticPointerOrBooleanType(inst.Result()->GetType(), "parameter type", inst.GetSourcePos());
    CheckValueInstruction(&inst);
    ++numParams;
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(LocalInstruction& inst)
{
    CheckType("type of local", inst.LocalType(), inst.LocalType()->AddPointer(GetContext()), "instruction result type", inst.Result()->GetType(), inst.GetSourcePos());
    CheckValueInstruction(&inst);
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(LoadInstruction& inst)
{
    CheckPointerType(inst.Ptr()->GetType(), "operand type", inst.GetSourcePos());
    CheckType("result type", inst.Result()->GetType(), inst.Ptr()->GetType(), "pointer to result type", inst.Result()->GetType()->AddPointer(GetContext()), inst.GetSourcePos());
    CheckArithmeticPointerOrBooleanType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckValueInstruction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(ElemAddrInstruction& inst)
{
    Type* ptrType = inst.Ptr()->GetType();
    CheckPointerType(ptrType, "pointer type", inst.GetSourcePos());
    PointerType* pointerType = static_cast<PointerType*>(ptrType);
    Type* baseType = pointerType->BaseType();
    if (baseType->IsStructureType())
    {
        StructureType* structureType = static_cast<StructureType*>(baseType);
        CheckIntegerType(inst.Index()->GetType(), "integer type index", inst.GetSourcePos());
        CheckValueInstruction(&inst);
        if (inst.Index()->IsIntegerValue())
        {
            int64_t index = inst.Index()->GetIntegerValue();
            if (index < 0 || index >= structureType->FieldCount())
            {
                Error("code verification error: invalid index", inst.GetSourcePos(), GetContext());
            }
        }
    }
    else if (baseType->IsArrayType())
    {
        ArrayType* arrayType = static_cast<ArrayType*>(baseType);
        CheckIntegerType(inst.Index()->GetType(), "integer type index", inst.GetSourcePos());
        CheckValueInstruction(&inst);
        if (inst.Index()->IsIntegerValue())
        {
            int64_t index = inst.Index()->GetIntegerValue();
            if (index < 0 || index >= arrayType->ElementCount())
            {
                Error("code verification error: invalid index", inst.GetSourcePos(), GetContext());
            }
        }
    }
    else
    {
        Error("type check error: pointer to structure or array type expected", inst.GetSourcePos(), GetContext());
    }
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(PtrOffsetInstruction& inst)
{
    CheckPointerType(inst.Ptr()->GetType(), "pointer operand type", inst.GetSourcePos());
    CheckIntegerType(inst.Offset()->GetType(), "offset type", inst.GetSourcePos());
    CheckValueInstruction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(PtrDiffInstruction& inst)
{
    CheckIntegerType(inst.Result()->GetType(), "result type", inst.GetSourcePos());
    CheckPointerType(inst.LeftPtr()->GetType(), "left pointer operand type", inst.GetSourcePos());
    CheckPointerType(inst.RightPtr()->GetType(), "right pointer operand type", inst.GetSourcePos());
    CheckSameType("left pointer operand type", inst.LeftPtr()->GetType(), "right pointer operand type", inst.RightPtr()->GetType(), inst.GetSourcePos());
    CheckValueInstruction(&inst);
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(FunctionCallInstruction& inst)
{
    Type* calleeType = inst.Callee()->GetType();
    if (calleeType->IsPointerType())
    {
        calleeType = calleeType->RemovePointer(inst.GetSourcePos(), GetContext());
    }
    if (calleeType->IsFunctionType())
    {
        FunctionType* functionType = static_cast<FunctionType*>(calleeType);
        if (functionType->Arity() != arguments.size())
        {
            Error("code verification error: function call has wrong number of arguments: " + std::to_string(functionType->Arity()) + " arguments expected: note: " +
                std::to_string(arguments.size()) + " arguments passed", inst.GetSourcePos(), GetContext());
        }
        if (inst.Callee()->IsSymbolValue())
        {
            SymbolValue* calleeSymbolValue = static_cast<SymbolValue*>(inst.Callee());
            Function* function = GetContext()->AddFunctionDeclaration(inst.GetSourcePos(), calleeType, calleeSymbolValue->Symbol());
            if (function)
            {
                calleeSymbolValue->SetFunction(function);
            }
            else
            {
                Error("internal error: could not add function declaration", inst.GetSourcePos(), GetContext());
            }
        }
    }
    else
    {
        Error("type check error: callee in function call has invalid type: function type or function pointer type expected: note: type is " + inst.Callee()->GetType()->Name(),
            inst.GetSourcePos(), GetContext());
    }
    CheckValueInstruction(&inst);
    inst.SetArgs(std::move(arguments));
    inst.AddUse();
    arguments.clear();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(TrapInstruction& inst)
{
    Type* byteType = GetContext()->GetTypes().GetByteType();
    CheckSameType("operand 0 type", inst.Op1()->GetType(), "byte type", byteType, inst.GetSourcePos());
    CheckSameType("operand 1 type", inst.Op2()->GetType(), "byte type", byteType, inst.GetSourcePos());
    CheckSameType("operand 2 type", inst.Op3()->GetType(), "byte type", byteType, inst.GetSourcePos());
    CheckValueInstruction(&inst);
    inst.SetArgs(std::move(arguments));
    inst.AddUse();
    arguments.clear();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(PhiInstruction& inst)
{
    Type* type = inst.Result()->GetType();
    for (BlockValue& blockValue : inst.BlockValues())
    {
        CheckSameType("value type", blockValue.value->GetType(), "result type", type, inst.GetSourcePos());
        int32_t blockId = blockValue.blockId;
        BasicBlock* block = currentFunction->GetBasicBlock(blockId);
        if (!block)
        {
            Error("code verification error: phi source basic block " + std::to_string(blockId) + " not found from function '" + currentFunction->Name() + "'", 
                inst.GetSourcePos(), GetContext());
        }
        blockValue.block = block;
    }
    inst.AddUse();
    inst.SetIndex(index++);
}

void VerifierVisitor::Visit(NoOperationInstruction& inst)
{
    inst.SetIndex(index++);
}

void Verify(Context& context)
{
    VerifierVisitor visitor(&context);
    context.GetCode().VisitFunctions(visitor);
}

} // cmsx::intermediate
