// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmsystemx/Function.hpp>
#include <cmajor/cmsystemx/Context.hpp>
#include <cmajor/cmsystemx/OpCode.hpp>
#include <cmajor/util/TextUtils.hpp>
#include <cmajor/util/Error.hpp>

namespace cmsystemx {

using namespace cmajor::util;

void Function::IncFrameOffset(Type* type)
{
    uint64_t sizeInBytes = type->SizeInBits() / 8;
    uint64_t offset = 8 * ((sizeInBytes - 1) / 8 + 1);
    frameOffset += offset;
}

Function::Function(Context& context, const std::string& mangledName_, FunctionType* type_) :
    Value(Kind::function), mangledName(mangledName_), type(type_), nextBasicBlockId(0), nextLocalRegNumber(0), frameOffset(8), linkOnce(false)
{
    int index = 0;
    for (Type* paramType : type->ParamTypes())
    {
        Value* store = nullptr;
        if (index < context.GetSymbolicRegisters().Count())
        {
            store = context.GetSymbolicRegisters().GetSymbolicRegister(index);
        }
        else
        {
            FrameLocation* frameLocation = new FrameLocation(frameOffset, context.FP(), paramType);
            IncFrameOffset(paramType);
            store = frameLocation;
            frameArguments.push_back(std::unique_ptr<FrameLocation>(frameLocation));
        }
        Argument* argument = new Argument(paramType, store);
        arguments.push_back(std::unique_ptr<Argument>(argument));
        ++index;
    }
}

BasicBlock* Function::CreateBasicBlock(const std::string& name)
{
    BasicBlock* bb = new BasicBlock(nextBasicBlockId++, name, this);
    basicBlocks.push_back(std::unique_ptr<BasicBlock>(bb));
    return bb;
}

void Function::Write(std::ostream& s)
{
    Assert(false, "tried to write a function");
}

Type* Function::GetType(Context& context)
{
    return type;
}

bool Function::Defined() const
{
    return !basicBlocks.empty();
}

void Function::WriteDeclaration(std::ostream& s)
{
    if (!Defined()) return;
    std::string labelField = Format(" ", labelFieldWidth, FormatWidth::min);
    if (linkOnce)
    {
        s << labelField << " LINKONCE " << mangledName << std::endl;
    }
    else
    {
        s << labelField << " EXTERN " << mangledName << std::endl;
    }
}

void Function::WriteDefinition(Context& context, std::ostream& s)
{
    if (!Defined()) return;
    std::string labelField = Format(mangledName, labelFieldWidth, FormatWidth::min);
    s << labelField << " FUNC" << std::endl;
    CreateProlog(context);
    for (const auto& bb : basicBlocks)
    {
        bb->Write(s);
    }
    s << labelField << " ENDF" << std::endl;
    s << std::endl;
}

void Function::CreateProlog(Context& context)
{
    Assert(!basicBlocks.empty(), "basic blocks is empty");
    BasicBlock& entryBlock = *basicBlocks[0];
    CompositeInstruction* prolog = new CompositeInstruction();
    Instruction* stoInst = new Instruction(STO);
    stoInst->AddOperand(context.FP());
    stoInst->AddOperand(context.SP());
    stoInst->AddOperand(context.GetULongType()->DefaultValue());
    prolog->AddInstruction(stoInst);
    Instruction* setSPInst = new Instruction(SET);
    setSPInst->AddOperand(context.FP());
    setSPInst->AddOperand(context.SP());
    prolog->AddInstruction(setSPInst);
    Instruction* inclInst = new Instruction(INCL);
    inclInst->AddOperand(context.SP());
    inclInst->AddOperand(context.GetULongValue(frameOffset & 0xFFFF));
    prolog->AddInstruction(inclInst);
    if (frameOffset > std::numeric_limits<int16_t>::max())
    {
        if (frameOffset <= std::numeric_limits<int32_t>::max())
        {
            Instruction* incMLInst = new Instruction(INCML);
            incMLInst->AddOperand(context.SP());
            incMLInst->AddOperand(context.GetULongValue((frameOffset >> 16) & 0xFFFF));
            prolog->AddInstruction(incMLInst);
        }
        else
        {
            throw std::runtime_error("frame too big");
        }
    }
    entryBlock.InsertFront(prolog);
}

LocalRegister* Function::GetReg(Type* type)
{
    LocalRegister* localRegister = new LocalRegister(nextLocalRegNumber++, type);
    localRegisters.push_back(std::unique_ptr<LocalRegister>(localRegister));
    return localRegister;
}

LocalVariable* Function::CreateLocalVariable(Type* type, Context& context)
{
    Type* ptrType = context.GetPtrType(type);
    Value* store = nullptr;
    if (nextLocalRegNumber < maxLocalVariableReg && type->GetKind() != TypeKind::classType)
    {
        store = GetReg(ptrType);
    }
    else
    {
        FrameLocation* frameLocation = new FrameLocation(frameOffset, context.FP(), ptrType);
        IncFrameOffset(type);
        frameLocalVars.push_back(std::unique_ptr<FrameLocation>(frameLocation));
        store = frameLocation;
    }
    LocalVariable* localVariable = new LocalVariable(ptrType, store);
    localVariables.push_back(std::unique_ptr<LocalVariable>(localVariable));
    return localVariable;
}

Value* Function::GetArgument(int argumentIndex)
{
    Assert(argumentIndex >= 0 && argumentIndex < arguments.size(), "invalid argument index");
    return arguments[argumentIndex].get();
}

Value* Function::CreateLoad(Value* ptr, Context& context)
{
    return ptr->Load(context);
}

void Function::CreateStore(Value* value, Value* ptr, Context& context)
{
    ptr->Store(value, context);
}

Value* Function::CreateCall(Value* callee, const std::vector<Value*>& args, Context& context)
{
    BasicBlock* bb = context.GetCurrentBasicBlock();
    int numLocalRegs = nextLocalRegNumber;
    int symbolicRegCount = context.GetSymbolicRegisters().Count();
    int n = args.size();
    if (n > symbolicRegCount)
    {
        Instruction* setInst = bb->CreateInstruction(SET);
        setInst->AddOperand(context.Ix());
        setInst->AddOperand(context.CreateBinaryExpr(Operator::mul, context.GetEight(), context.CreateParenExpr(context.CreateBinaryExpr(Operator::add, context.GetIntValue(numLocalRegs), context.GetIntValue(3)))));
        Instruction* addSPInst = bb->CreateInstruction(ADDU);
        addSPInst->AddOperand(context.SP());
        addSPInst->AddOperand(context.SP());
        addSPInst->AddOperand(context.Ix());
        Instruction* storeInst = bb->CreateInstruction(STOU);
        storeInst->AddOperand(context.Ix());
        storeInst->AddOperand(context.SP());
        storeInst->AddOperand(context.GetZero());
    }
    for (int i = 0; i < n; ++i)
    {
        Value* arg = args[i];
        if (arg->GetType(context)->GetKind() == TypeKind::classType)
        {
            arg = arg->Load(context);
        }
        if (i < symbolicRegCount)
        {
            Instruction* setInst = bb->CreateInstruction(SET);
            setInst->AddOperand(context.GetSymbolicRegisters().GetSymbolicRegister(i));
            setInst->AddOperand(arg);
        }
        else if (i < maxDirectFrameLoc + symbolicRegCount)
        {
            int index = i - symbolicRegCount;
            if (arg->GetKind() != Kind::register_)
            {
                Value* reg = GetReg(arg->GetType(context));
                arg->StoreTo(reg, context);
                arg = reg;
            }
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(arg);
            storeInst->AddOperand(context.SP());
            storeInst->AddOperand(context.CreateBinaryExpr(Operator::mul, context.GetEight(), context.CreateParenExpr(context.CreateBinaryExpr(Operator::add, context.GetIntValue(index), context.GetIntValue(1)))));
        }
        else if (i < maxIxFrameLoc + symbolicRegCount)
        {
            int index = i - symbolicRegCount;
            if (arg->GetKind() != Kind::register_)
            {
                Value* reg = GetReg(arg->GetType(context));
                arg->StoreTo(reg, context);
                arg = reg;
            }
            Instruction* setInst = bb->CreateInstruction(SET);
            setInst->AddOperand(context.Ix());
            setInst->AddOperand(context.CreateBinaryExpr(Operator::mul, context.GetEight(), context.CreateParenExpr(context.CreateBinaryExpr(Operator::add, context.GetIntValue(index), context.GetOne()))));
            Instruction* storeInst = arg->GetType(context)->CreateStore(context);
            storeInst->AddOperand(arg);
            storeInst->AddOperand(context.SP());
            storeInst->AddOperand(context.Ix());
        }
        else
        {
            throw std::runtime_error("too many arguments");
        }
    }
    if (n > symbolicRegCount)
    {
        Instruction* loadInst = bb->CreateInstruction(LDOU);
        loadInst->AddOperand(context.Ix());
        loadInst->AddOperand(context.SP());
        loadInst->AddOperand(context.GetZero());
        Instruction* subSPInst = bb->CreateInstruction(SUBU);
        subSPInst->AddOperand(context.SP());
        subSPInst->AddOperand(context.SP());
        subSPInst->AddOperand(context.Ix());
    }
    return callee->CreateCallInst(numLocalRegs, context);
}

Value* Function::CreateCallInst(int numLocalRegs, Context& context)
{
    Instruction* callInst = context.GetCurrentBasicBlock()->CreateInstruction(CALL);
    callInst->AddOperand(context.CreateUnaryExpr(Operator::registerize, context.GetIntValue(numLocalRegs)));
    Symbol* symbol = new Symbol(mangledName);
    context.AddValue(symbol);
    callInst->AddOperand(symbol);
    return type->ReturnType()->CreateReturnValue(context);
}

} // namespace cmsystemx
