// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/intermediate/OptCodeGen.hpp>
#include <system-x/intermediate/Code.hpp>
#include <system-x/intermediate/Data.hpp>
#include <system-x/intermediate/LinearScanRegisterAllocator.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Registers.hpp>
#include <system-x/assembler/Constant.hpp>
#include <soulng/util/Sha1.hpp>

namespace cmsx::intermediate {

using namespace soulng::util;

struct ByCaseValue
{
    bool operator()(const CaseTarget& left, const CaseTarget& right) const
    {
        int64_t leftCaseValue = left.caseValue->GetIntegerValue();
        int64_t rightCaseValue = right.caseValue->GetIntegerValue();
        return leftCaseValue < rightCaseValue;
    }
};

const CaseTarget* GetCaseTarget(const std::vector<CaseTarget>& caseTargets, int64_t value)
{
    LongValue val(value, nullptr);
    CaseTarget caseValue(&val, -1);
    auto it = std::lower_bound(caseTargets.begin(), caseTargets.end(), caseValue, ByCaseValue());
    if (it == caseTargets.end())
    {
        --it;
    }
    if (it->caseValue->GetIntegerValue() == value)
    {
        return &(*it);
    }
    return nullptr;
}

void EmitOptSwitch(SwitchInstruction& inst, CodeGenerator& codeGen)
{
    if (inst.CaseTargets().size() <= 4)
    {
        EmitSwitch(inst, codeGen);
    }
    else
    {
        std::vector<CaseTarget> caseTargets = inst.CaseTargets();
        for (const CaseTarget& caseTarget : caseTargets)
        {
            if (!caseTarget.caseValue->IsIntegerValue())
            {
                EmitSwitch(inst, codeGen);
                return;
            }
        }
        std::sort(caseTargets.begin(), caseTargets.end(), ByCaseValue());
        const CaseTarget& first = caseTargets.front();
        const CaseTarget& last = caseTargets.back();
        int64_t n = last.caseValue->GetIntegerValue() - first.caseValue->GetIntegerValue() + 1;
        int64_t start = first.caseValue->GetIntegerValue();
        int64_t end = last.caseValue->GetIntegerValue();

        cmsx::assembler::Instruction* setStartInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setStartInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regCX)));
        setStartInst->AddOperand(cmsx::assembler::MakeConstantExpr(start));
        codeGen.Emit(setStartInst);

        cmsx::assembler::Instruction* cmpStartInst = new cmsx::assembler::Instruction(cmsx::machine::CMP);
        cmpStartInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regAX)));
        cmpStartInst->AddOperand(MakeRegOperand(inst.Cond(), GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regBX), codeGen));
        cmpStartInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regCX)));
        codeGen.Emit(cmpStartInst);

        cmsx::assembler::Instruction* bnInst = new cmsx::assembler::Instruction(cmsx::machine::BN);
        bnInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regAX)));
        bnInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.DefaultTargetBlock()->Id()));
        codeGen.Emit(bnInst);

        cmsx::assembler::Instruction* setEndInst = new cmsx::assembler::Instruction(cmsx::assembler::SET);
        setEndInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regCX)));
        setEndInst->AddOperand(cmsx::assembler::MakeConstantExpr(end));
        codeGen.Emit(setEndInst);

        cmsx::assembler::Instruction* cmpEndInst = new cmsx::assembler::Instruction(cmsx::machine::CMP);
        cmpEndInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regAX)));
        cmpEndInst->AddOperand(MakeRegOperand(inst.Cond(), GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regBX), codeGen));
        cmpEndInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regCX)));
        codeGen.Emit(cmpEndInst);

        cmsx::assembler::Instruction* bpInst = new cmsx::assembler::Instruction(cmsx::machine::BP);
        bpInst->AddOperand(MakeRegOperand(GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regAX)));
        bpInst->AddOperand(cmsx::assembler::MakeLocalSymbol(inst.DefaultTargetBlock()->Id()));
        codeGen.Emit(bpInst);

        std::string jumpTableStructName = "jmptab@" + codeGen.CurrentFunction()->Name() + "@" + std::to_string(codeGen.CurrentLineNumber());

        cmsx::assembler::Instruction* ldoInst = new cmsx::assembler::Instruction(cmsx::machine::LDOU);
        ldoInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regCX));
        ldoInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(jumpTableStructName));
        codeGen.Emit(ldoInst);

        cmsx::assembler::Instruction* subInst = new cmsx::assembler::Instruction(cmsx::machine::SUB);
        subInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regBX));
        subInst->AddOperand(MakeRegOperand(inst.Cond(), GetGlobalRegister(codeGen.Ctx(), cmsx::machine::regBX), codeGen));
        subInst->AddOperand(cmsx::assembler::MakeConstantExpr(start));
        codeGen.Emit(subInst);

        cmsx::assembler::Instruction* i8AdduInst = new cmsx::assembler::Instruction(cmsx::machine::I8ADDU);
        i8AdduInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regAX));
        i8AdduInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regBX));
        i8AdduInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regCX));
        codeGen.Emit(i8AdduInst);

        cmsx::assembler::Instruction* ldouInst = new cmsx::assembler::Instruction(cmsx::machine::LDOU);
        ldouInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regAX));
        ldouInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regAX));
        codeGen.Emit(ldouInst);

        cmsx::assembler::Instruction* goInst = new cmsx::assembler::Instruction(cmsx::machine::GO);
        goInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regAX));
        goInst->AddOperand(cmsx::assembler::MakeGlobalRegOperand(cmsx::machine::regAX));
        codeGen.Emit(goInst);

        std::string dataSymbolName = jumpTableStructName + "_data";
        cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
        octaInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(dataSymbolName));
        codeGen.AssemblyFile()->GetLinkSection()->GetOrCreateExternObject()->AddExternSymbol(cmsx::assembler::MakeGlobalSymbol(jumpTableStructName));
        cmsx::assembler::AssemblyStruct* jumpTableStruct = codeGen.AssemblyFile()->GetDataSection()->CreateStructure(jumpTableStructName);
        jumpTableStruct->AddInstruction(octaInst);
        for (int64_t i = 0; i < n; ++i)
        {
            cmsx::assembler::Instruction* octaInst = new cmsx::assembler::Instruction(cmsx::assembler::OCTA);
            if (i == 0)
            {
                octaInst->SetLabel(cmsx::assembler::MakeGlobalSymbol(dataSymbolName));
            }
            const CaseTarget* target = GetCaseTarget(caseTargets, start + i);
            BasicBlock* targetBlock = inst.DefaultTargetBlock();
            if (target)
            {
                targetBlock = target->targetBlock;
            }
            octaInst->AddOperand(cmsx::assembler::MakeGlobalSymbol(codeGen.CurrentFunction()->Name() + "@" + std::to_string(targetBlock->Id())));
            jumpTableStruct->AddInstruction(octaInst);
        }
    }
}

} // cmsx::intermediate
