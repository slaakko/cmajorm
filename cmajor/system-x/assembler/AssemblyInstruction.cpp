// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/assembler/AssemblyInstruction.hpp>
#include <system-x/assembler/Assembler.hpp>
#include <system-x/machine/OpCode.hpp>
#include <system-x/machine/Registers.hpp>

namespace cmsx::assembler {

AssemblyInstruction::AssemblyInstruction()
{
}

AssemblyInstruction::~AssemblyInstruction()
{
}

class ExternInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void ExternInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    for (auto& value : currentInst->Operands())
    {
        if (value.IsSymbolValue())
        {
            cmsx::object::Symbol* symbol = value.GetSymbol();
            symbol->SetLinkage(cmsx::object::Linkage::external);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs symbol operands", currentInst->GetSourcePos());
        }
    }
}

class LinkOnceInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void LinkOnceInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    for (auto& value : currentInst->Operands())
    {
        if (value.IsSymbolValue())
        {
            cmsx::object::Symbol* symbol = value.GetSymbol();
            symbol->SetLinkage(cmsx::object::Linkage::once);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs symbol operands", currentInst->GetSourcePos());
        }
    }
}

class FuncInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void FuncInstruction::Assemble(Assembler& assembler)
{
    cmsx::object::Symbol* functionSymbol = assembler.CurrentSymbol();
    functionSymbol->SetValue(
        cmsx::object::Value(assembler.CurrentSection()->Address(),
            cmsx::object::ValueFlags::definition | cmsx::object::ValueFlags::pure | cmsx::object::ValueFlags::address | cmsx::object::ValueFlags::function,
            functionSymbol));
    functionSymbol->SetStart(assembler.CurrentSection()->BaseAddress() + assembler.CurrentSection()->Address());
    functionSymbol->SetAlignment(4);
    assembler.SetParentIndex(functionSymbol->Index());
    assembler.SetCurrentFunctionSymbol(functionSymbol);
}

class EndFuncInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void EndFuncInstruction::Assemble(Assembler& assembler)
{
    cmsx::object::Symbol* functionSymbol = assembler.CurrentSymbol();
    functionSymbol->SetLength(assembler.CurrentSection()->BaseAddress() + assembler.CurrentSection()->Address() - functionSymbol->Start());
    assembler.SetParentIndex(-1);
    assembler.SetCurrentFunctionSymbol(nullptr);
}

class StructureInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void StructureInstruction::Assemble(Assembler& assembler)
{
    cmsx::object::Symbol* structureSymbol = assembler.CurrentSymbol();
    structureSymbol->SetValue(
        cmsx::object::Value(assembler.CurrentSection()->Address(),
            cmsx::object::ValueFlags::definition | cmsx::object::ValueFlags::pure | cmsx::object::ValueFlags::address | cmsx::object::ValueFlags::structure,
            structureSymbol));
    structureSymbol->SetStart(assembler.CurrentSection()->BaseAddress() + assembler.CurrentSection()->Address());
    structureSymbol->SetAlignment(8);
    assembler.SetParentIndex(structureSymbol->Index());
    assembler.SetCurrentStructureSymbol(structureSymbol);
};

class EndStructureInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void EndStructureInstruction::Assemble(Assembler& assembler)
{
    cmsx::object::Symbol* structureSymbol = assembler.CurrentSymbol();
    structureSymbol->SetLength(assembler.CurrentSection()->BaseAddress() + assembler.CurrentSection()->Address() - structureSymbol->Start());
    assembler.SetParentIndex(-1);
    assembler.SetCurrentStructureSymbol(nullptr);
}

class ByteInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void ByteInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (assembler.CurrentSection()->IsDataSection())
    {
        int n = currentInst->Operands().size();
        if (n > 0)
        {
            for (int i = 0; i < n; ++i)
            {
                assembler.EmitPureByteValue("byte", currentInst->Operands()[i]);
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs at least one operand", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " can appear only .DATA section", currentInst->GetSourcePos());
    }
}

class WydeInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void WydeInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (assembler.CurrentSection()->IsDataSection())
    {
        int n = currentInst->Operands().size();
        if (n > 0)
        {
            for (int i = 0; i < n; ++i)
            {
                assembler.EmitPureWydeValue("wyde", currentInst->Operands()[i]);
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs at least one operand", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " can appear only .DATA section", currentInst->GetSourcePos());
    }
}

class TetraInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void TetraInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (assembler.CurrentSection()->IsDataSection())
    {
        int n = currentInst->Operands().size();
        if (n > 0)
        {
            for (int i = 0; i < n; ++i)
            {
                assembler.EmitPureTetraValue("tetra", currentInst->Operands()[i]);
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs at least one operand", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " can appear only .DATA section", currentInst->GetSourcePos());
    }
}

class OctaInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void OctaInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (assembler.CurrentSection()->IsDataSection())
    {
        int n = currentInst->Operands().size();
        if (n > 0)
        {
            for (int i = 0; i < n; ++i)
            {
                cmsx::object::Value value = currentInst->Operands()[i];
                if (value.IsSymbolValue())
                {
                    assembler.EmitSymbolOcta(value);
                }
                else if (value.IsPureValue())
                {
                    assembler.EmitPureOctaValue("octa", value);
                }
                else if (value.IsTypeIdIndex())
                {
                    assembler.EmitClsIdCommmand(value.Val(), currentInst->GetSourcePos());
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " operand must be a symbol, pure or type id index", currentInst->GetSourcePos());
                }
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs at least one operand", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " can appear only .DATA section", currentInst->GetSourcePos());
    }
}

class SetInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void SetInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (currentInst->Operands().size() == 2)
    {
        if (currentInst->Operands()[0].IsRegValue())
        {
            if (currentInst->Operands()[1].IsRegValue())
            {
                assembler.EmitOpCode(cmsx::machine::ORI);
                assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
                assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
                assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
            }
            else if (currentInst->Operands()[1].IsPureValue())
            {
                assembler.EmitSetPureRegValue(currentInst->Operands()[0], currentInst->Operands()[1]);
            }
            else if (currentInst->Operands()[1].IsSymbolValue())
            {
                cmsx::object::Symbol* symbol = currentInst->Operands()[1].GetSymbol();
                assembler.EmitAbsoluteRegisterValue("X", currentInst->Operands()[0], symbol);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a register or pure value", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two operands", currentInst->GetSourcePos());
    }
}

class LoadInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void LoadInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        uint8_t opc = static_cast<uint8_t>(currentInst->GetOpCode()->Value());
        if (currentInst->Operands()[1].IsRegValue())
        {
            if (n == 2 || n == 3 && currentInst->Operands()[2].IsPureValue())
            {
                ++opc;
            }
            assembler.EmitOpCode(opc);
            if (currentInst->Operands()[0].IsRegValue())
            {
                assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
            if (n == 2)
            {
                assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
            }
            else if (n == 3)
            {
                if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
                }
                else if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
        }
        else if (currentInst->Operands()[1].IsSymbolValue())
        {
            assembler.EmitAbsoluteRegisterValue("X", currentInst->Operands()[0], currentInst->Operands()[1]);
            assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value() + 1));
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a register or a symbol", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class StoreInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void StoreInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        uint8_t opc = static_cast<uint8_t>(currentInst->GetOpCode()->Value());
        if (currentInst->Operands()[1].IsRegValue())
        {
            if (n == 2 || n == 3 && currentInst->Operands()[2].IsPureValue())
            {
                ++opc;
            }
            assembler.EmitOpCode(opc);
            if (currentInst->Operands()[0].IsRegValue())
            {
                assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
            if (n == 2)
            {
                assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
            }
            else if (n == 3)
            {
                if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
                }
                else if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
        }
        else if (currentInst->Operands()[1].IsSymbolValue())
        {
            assembler.EmitAbsoluteRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg), currentInst->Operands()[1]);
            assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value() + 1));
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            assembler.EmitRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg));
            assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a register or a symbol", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class StoreConstantInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void StoreConstantInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        uint8_t opc = static_cast<uint8_t>(currentInst->GetOpCode()->Value());
        if (n == 2 || n == 3 && currentInst->Operands()[2].IsPureValue())
        {
            ++opc;
        }
        if (currentInst->Operands()[1].IsRegValue())
        {
            assembler.EmitOpCode(opc);
            if (currentInst->Operands()[0].IsPureValue())
            {
                assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be pure", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
            if (n == 3)
            {
                if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
                }
                else if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else if (n == 2)
            {
                assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
        }
        else if (currentInst->Operands()[1].IsSymbolValue())
        {
            assembler.EmitAbsoluteRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg), currentInst->Operands()[1]);
            assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value() + 1));
            assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
            assembler.EmitRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg));
            assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a register or a symbol", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class RegisterInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void RegisterInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        if (currentInst->Operands()[0].IsRegValue() && currentInst->Operands()[1].IsRegValue())
        {
            int opc = currentInst->GetOpCode()->Value();
            if (opc == cmsx::machine::DIVU)
            {
                assembler.EmitOpCode(cmsx::machine::SETL);
                assembler.EmitRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg));
                assembler.EmitPureWydeValue("zero", cmsx::object::Value(static_cast<uint64_t>(0)));
                assembler.EmitOpCode(cmsx::machine::PUT);
                assembler.EmitPureByteValue("X", cmsx::object::Value(static_cast<uint64_t>(cmsx::machine::rD)));
                assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
                assembler.EmitRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg));
            }
            if (n == 2)
            {
                if (opc == LDA)
                {
                    opc = cmsx::machine::ADDU;
                }
                assembler.EmitOpCode(static_cast<uint8_t>(opc + 1));
            }
            else if (n == 3)
            {
                if (opc == LDA)
                {
                    opc = cmsx::machine::ADDU;
                }
                if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitOpCode(static_cast<uint8_t>(opc + 1));
                }
                else if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitOpCode(static_cast<uint8_t>(opc));
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X and Y operands must be registers", currentInst->GetSourcePos());
        }
        if (n == 2)
        {
            assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
        }
        else if (n == 3)
        {
            if (currentInst->Operands()[2].IsRegValue())
            {
                assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
            }
            else if (currentInst->Operands()[2].IsPureValue())
            {
                assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class NegateInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void NegateInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        if (currentInst->Operands()[0].IsRegValue())
        {
            uint8_t opc = currentInst->GetOpCode()->Value();
            if (n == 3)
            {
                if (currentInst->Operands()[2].IsPureValue())
                {
                    ++opc;
                }
                assembler.EmitOpCode(opc);
                assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
                if (currentInst->Operands()[1].IsPureValue())
                {
                    assembler.EmitPureByteValue("Y", currentInst->Operands()[1]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be pure", currentInst->GetSourcePos());
                }
                if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
                }
                else if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else if (n == 2)
            {
                if (currentInst->Operands()[1].IsPureValue())
                {
                    ++opc;
                }
                assembler.EmitOpCode(opc);
                assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
                assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
                if (currentInst->Operands()[1].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[1]);
                }
                else if (currentInst->Operands()[1].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[1]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class FloatingPointInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void FloatingPointInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 3)
    {
        if (currentInst->Operands()[0].IsRegValue() && currentInst->Operands()[1].IsRegValue() && currentInst->Operands()[2].IsRegValue())
        {
            assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value()));
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
            assembler.EmitRegisterValue("Y", currentInst->Operands()[1]);
            assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs three register operands", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs three operands", currentInst->GetSourcePos());
    }
}

class FloatingRoundingInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void FloatingRoundingInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        int opc = currentInst->GetOpCode()->Value();
        if (n == 2)
        {
            assembler.EmitOpCode(static_cast<uint8_t>(opc));
        }
        else if (n == 3)
        {
            if (currentInst->Operands()[2].IsRegValue())
            {
                assembler.EmitOpCode(static_cast<uint8_t>(opc));
            }
            else if (currentInst->Operands()[2].IsPureValue())
            {
                assembler.EmitOpCode(static_cast<uint8_t>(opc + 1));
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
            }
        }
        if (currentInst->Operands()[0].IsRegValue())
        {
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
        if (n == 2)
        {
            assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(1)));
        }
        else if (n == 3)
        {
            if (currentInst->Operands()[1].IsPureValue())
            {
                assembler.EmitPureByteValue("Y", currentInst->Operands()[1]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be pure", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
        }
        if (n == 2)
        {
            assembler.EmitRegisterValue("Z", currentInst->Operands()[1]);
        }
        else if (n == 3)
        {
            if (currentInst->Operands()[2].IsRegValue())
            {
                assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
            }
            else if (currentInst->Operands()[2].IsPureValue())
            {
                assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class WydeImmediateInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void WydeImmediateInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2)
    {
        assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value()));
        if (currentInst->Operands()[0].IsRegValue())
        {
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
        assembler.EmitPureWydeValue("Y", currentInst->Operands()[1]);
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two operands", currentInst->GetSourcePos());
    }
}

class JumpInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void JumpInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    if (currentInst->Operands().size() == 1)
    {
        cmsx::object::Value value = currentInst->Operands()[0];
        if (value.IsSymbolValue())
        {
            cmsx::object::Symbol* symbol = value.GetSymbol();
            if (symbol->GetLinkage() == cmsx::object::Linkage::internal || symbol->GetLinkage() == cmsx::object::Linkage::external)
            {
                if (value.GetFlag(cmsx::object::ValueFlags::address))
                {
                    uint64_t to = value.Val();
                    uint64_t at = assembler.CurrentSection()->Address();
                    if (at >= to)
                    {
                        uint64_t offset = (at - to) >> 2;
                        if (offset <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()) << 8)
                        {
                            assembler.EmitOpCode(cmsx::machine::JMPB);
                            assembler.EmitLongOffset(static_cast<uint32_t>(offset));
                        }
                        else
                        {
                            assembler.Error(currentInst->GetOpCode()->Name() + " too far", currentInst->GetSourcePos());
                        }
                    }
                    else
                    {
                        uint64_t offset = (to - at) >> 2;
                        if (offset <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()) << 8)
                        {
                            assembler.EmitOpCode(cmsx::machine::JMP);
                            assembler.EmitLongOffset(static_cast<uint32_t>(offset));
                        }
                        else
                        {
                            assembler.Error(currentInst->GetOpCode()->Name() + " too far", currentInst->GetSourcePos());
                        }
                    }
                }
                else if (value.GetFlag(cmsx::object::ValueFlags::undefined))
                {
                    assembler.EmitForwardLongJump(symbol);
                }
                else
                {
                    assembler.Error("invalid " + currentInst->GetOpCode()->Name() + " X operand", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " to undefined symbol " + symbol->FullName(), currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a symbol", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs one operand", currentInst->GetSourcePos());
    }
}

class BranchInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void BranchInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2)
    {
        uint8_t opc = static_cast<uint8_t>(currentInst->GetOpCode()->Value());
        if (currentInst->Operands()[0].IsRegValue())
        {
            cmsx::object::Value operand1 = currentInst->Operands()[1];
            if (operand1.IsSymbolValue())
            {
                cmsx::object::Symbol* symbol = operand1.GetSymbol();
                if (operand1.GetFlag(cmsx::object::ValueFlags::address))
                {
                    uint64_t to = operand1.Val();
                    uint64_t at = assembler.CurrentSection()->Address();
                    if (at >= to)
                    {
                        uint64_t offset = (at - to) >> 2;
                        if (offset <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
                        {
                            assembler.EmitOpCode(opc + 1);
                            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
                            assembler.EmitShortOffset(static_cast<uint16_t>(offset));
                        }
                        else
                        {
                            assembler.Error(currentInst->GetOpCode()->Name() + " too far", currentInst->GetSourcePos());
                        }
                    }
                    else
                    {
                        uint64_t offset = (to - at) >> 2;
                        if (offset <= static_cast<uint64_t>(std::numeric_limits<uint16_t>::max()))
                        {
                            assembler.EmitOpCode(opc);
                            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
                            assembler.EmitShortOffset(static_cast<uint16_t>(offset));
                        }
                        else
                        {
                            assembler.Error(currentInst->GetOpCode()->Name() + " too far", currentInst->GetSourcePos());
                        }
                    }
                }
                else if (operand1.GetFlag(cmsx::object::ValueFlags::undefined))
                {
                    assembler.EmitForwardShortJump("X", opc, currentInst->Operands()[0], symbol);
                }
                else
                {
                    assembler.Error("invalid " + currentInst->GetOpCode()->Name() + " Y operand", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a symbol", currentInst->GetSourcePos());
            }
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two operands", currentInst->GetSourcePos());
    }
}

class GetInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void GetInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2)
    {
        assembler.EmitOpCode(cmsx::machine::GET);
        if (currentInst->Operands()[0].IsRegValue())
        {
            assembler.EmitRegisterValue("X", currentInst->Operands()[0]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be a register", currentInst->GetSourcePos());
        }
        assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
        if (currentInst->Operands()[1].IsPureValue())
        {
            assembler.EmitPureByteValue("Z", currentInst->Operands()[1]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be pure", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two operands", currentInst->GetSourcePos());
    }
}

class PutInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void PutInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2)
    {
        assembler.EmitOpCode(cmsx::machine::PUT);
        if (currentInst->Operands()[0].IsPureValue())
        {
            assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be pure", currentInst->GetSourcePos());
        }
        assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
        if (currentInst->Operands()[1].IsRegValue())
        {
            assembler.EmitRegisterValue("Z", currentInst->Operands()[1]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two operands", currentInst->GetSourcePos());
    }
}

class CallInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void CallInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 2 || n == 3)
    {
        uint8_t opc = currentInst->GetOpCode()->Value();
        cmsx::object::Value operand1 = currentInst->Operands()[1];
        if (operand1.IsRegValue())
        {
            if (n == 2 || n == 3 && currentInst->Operands()[2].IsPureValue())
            {
                ++opc;
            }
            assembler.EmitOpCode(opc);
            if (currentInst->Operands()[0].IsPureValue())
            {
                assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be pure", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("Y", operand1);
            if (n == 2)
            {
                assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
            }
            else if (n == 3)
            {
                if (currentInst->Operands()[2].IsRegValue())
                {
                    assembler.EmitRegisterValue("Z", currentInst->Operands()[2]);
                }
                else if (currentInst->Operands()[2].IsPureValue())
                {
                    assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
                }
                else
                {
                    assembler.Error(currentInst->GetOpCode()->Name() + " Z operand must be a register or pure", currentInst->GetSourcePos());
                }
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
            }
        }
        else if (operand1.IsSymbolValue())
        {
            cmsx::object::Symbol* symbol = operand1.GetSymbol();
            assembler.EmitAbsoluteRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg), operand1);
            assembler.EmitOpCode(static_cast<uint8_t>(currentInst->GetOpCode()->Value() + 1));
            if (currentInst->Operands()[0].IsPureValue())
            {
                assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
            }
            else
            {
                assembler.Error(currentInst->GetOpCode()->Name() + " X operand must be pure", currentInst->GetSourcePos());
            }
            assembler.EmitRegisterValue("ex", cmsx::object::Value(cmsx::machine::regEX, cmsx::object::ValueFlags::reg));
            assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " Y operand must be a register or symbol", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs two or three operands", currentInst->GetSourcePos());
    }
}

class ReturnInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void ReturnInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 0)
    {
        assembler.EmitOpCode(cmsx::machine::RET);
        assembler.EmitPureByteValue("X", cmsx::object::Value(static_cast<uint64_t>(0)));
        assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
        assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " takes no operands", currentInst->GetSourcePos());
    }
}

class TrapInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void TrapInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 3)
    {
        if (currentInst->Operands()[0].IsPureValue() && currentInst->Operands()[1].IsPureValue() && currentInst->Operands()[2].IsPureValue())
        {
            assembler.EmitOpCode(cmsx::machine::TRAP);
            assembler.EmitPureByteValue("X", currentInst->Operands()[0]);
            assembler.EmitPureByteValue("Y", currentInst->Operands()[1]);
            assembler.EmitPureByteValue("Z", currentInst->Operands()[2]);
        }
        else
        {
            assembler.Error(currentInst->GetOpCode()->Name() + " needs three pure operands", currentInst->GetSourcePos());
        }
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " needs three operands", currentInst->GetSourcePos());
    }
}

class SwymInstruction : public AssemblyInstruction
{
public:
    void Assemble(Assembler& assembler) override;
};

void SwymInstruction::Assemble(Assembler& assembler)
{
    Instruction* currentInst = assembler.CurrentInstruction();
    int n = currentInst->Operands().size();
    if (n == 0)
    {
        assembler.EmitOpCode(cmsx::machine::SWYM);
        assembler.EmitPureByteValue("X", cmsx::object::Value(static_cast<uint64_t>(0)));
        assembler.EmitPureByteValue("Y", cmsx::object::Value(static_cast<uint64_t>(0)));
        assembler.EmitPureByteValue("Z", cmsx::object::Value(static_cast<uint64_t>(0)));
    }
    else
    {
        assembler.Error(currentInst->GetOpCode()->Name() + " takes no operands", currentInst->GetSourcePos());
    }
}

class AssemblyInstructionMap
{
public:
    static void Init();
    static void Done();
    static AssemblyInstructionMap& Instance() { return *instance; }
    AssemblyInstruction* GetAssemblyInstruction(int opCode) const;
private:
    AssemblyInstructionMap();
    AssemblyInstruction* AddInstruction(AssemblyInstruction* instruction);
    void MapInstruction(int opc, AssemblyInstruction* instruction);
    static std::unique_ptr<AssemblyInstructionMap> instance;
    std::vector<std::unique_ptr<AssemblyInstruction>> instructions;
    std::map<int, AssemblyInstruction*> instructionMap;
};

std::unique_ptr<AssemblyInstructionMap> AssemblyInstructionMap::instance;

void AssemblyInstructionMap::Init()
{
    instance.reset(new AssemblyInstructionMap());
}

void AssemblyInstructionMap::Done()
{
    instance.reset();
}

AssemblyInstructionMap::AssemblyInstructionMap()
{
    AssemblyInstruction* externInstruction = AddInstruction(new ExternInstruction());
    MapInstruction(cmsx::assembler::EXTERN, externInstruction);
    AssemblyInstruction* linkOnceInstruction = AddInstruction(new LinkOnceInstruction());
    MapInstruction(cmsx::assembler::LINKONCE, linkOnceInstruction);
    AssemblyInstruction* funcInstruction = AddInstruction(new FuncInstruction());
    MapInstruction(cmsx::assembler::FUNC, funcInstruction);
    AssemblyInstruction* endFuncInstruction = AddInstruction(new EndFuncInstruction());
    MapInstruction(cmsx::assembler::ENDF, endFuncInstruction);
    AssemblyInstruction* structInstruction = AddInstruction(new StructureInstruction());
    MapInstruction(cmsx::assembler::STRUCT, structInstruction);
    AssemblyInstruction* endStructInstruction = AddInstruction(new EndStructureInstruction());
    MapInstruction(cmsx::assembler::ENDS, endStructInstruction);
    AssemblyInstruction* byteInstruction = AddInstruction(new ByteInstruction());
    MapInstruction(cmsx::assembler::BYTE, byteInstruction);
    AssemblyInstruction* wydeInstruction = AddInstruction(new WydeInstruction());
    MapInstruction(cmsx::assembler::WYDE, wydeInstruction);
    AssemblyInstruction* tetraInstruction = AddInstruction(new TetraInstruction());
    MapInstruction(cmsx::assembler::TETRA, tetraInstruction);
    AssemblyInstruction* octaInstruction = AddInstruction(new OctaInstruction());
    MapInstruction(cmsx::assembler::OCTA, octaInstruction);
    AssemblyInstruction* setInstruction = AddInstruction(new SetInstruction());
    MapInstruction(cmsx::assembler::SET, setInstruction);
    AssemblyInstruction* loadInstruction = AddInstruction(new LoadInstruction());
    MapInstruction(cmsx::machine::LDB, loadInstruction);
    MapInstruction(cmsx::machine::LDBU, loadInstruction);
    MapInstruction(cmsx::machine::LDW, loadInstruction);
    MapInstruction(cmsx::machine::LDWU, loadInstruction);
    MapInstruction(cmsx::machine::LDT, loadInstruction);
    MapInstruction(cmsx::machine::LDTU, loadInstruction);
    MapInstruction(cmsx::machine::LDO, loadInstruction);
    MapInstruction(cmsx::machine::LDOU, loadInstruction);
    MapInstruction(cmsx::machine::LDHT, loadInstruction);
    MapInstruction(cmsx::machine::LDSF, loadInstruction);
    AssemblyInstruction* storeInstruction = AddInstruction(new StoreInstruction());
    MapInstruction(cmsx::machine::STB, storeInstruction);
    MapInstruction(cmsx::machine::STBU, storeInstruction);
    MapInstruction(cmsx::machine::STW, storeInstruction);
    MapInstruction(cmsx::machine::STWU, storeInstruction);
    MapInstruction(cmsx::machine::STT, storeInstruction);
    MapInstruction(cmsx::machine::STTU, storeInstruction);
    MapInstruction(cmsx::machine::STO, storeInstruction);
    MapInstruction(cmsx::machine::STOU, storeInstruction);
    MapInstruction(cmsx::machine::STHT, storeInstruction);
    MapInstruction(cmsx::machine::STSF, storeInstruction);
    AssemblyInstruction* storeConstantInstruction = AddInstruction(new StoreConstantInstruction());
    MapInstruction(cmsx::machine::STCO, storeInstruction);
    AssemblyInstruction* registerInstruction = AddInstruction(new RegisterInstruction());
    MapInstruction(cmsx::assembler::LDA, registerInstruction);
    MapInstruction(cmsx::machine::ADD, registerInstruction);
    MapInstruction(cmsx::machine::ADDU, registerInstruction);
    MapInstruction(cmsx::machine::SUB, registerInstruction);
    MapInstruction(cmsx::machine::SUBU, registerInstruction);
    MapInstruction(cmsx::machine::MUL, registerInstruction);
    MapInstruction(cmsx::machine::MULU, registerInstruction);
    MapInstruction(cmsx::machine::DIV, registerInstruction);
    MapInstruction(cmsx::machine::DIVU, registerInstruction);
    MapInstruction(cmsx::machine::I2ADDU, registerInstruction);
    MapInstruction(cmsx::machine::I4ADDU, registerInstruction);
    MapInstruction(cmsx::machine::I8ADDU, registerInstruction);
    MapInstruction(cmsx::machine::I16ADDU, registerInstruction);
    MapInstruction(cmsx::machine::SL, registerInstruction);
    MapInstruction(cmsx::machine::SLU, registerInstruction);
    MapInstruction(cmsx::machine::SR, registerInstruction);
    MapInstruction(cmsx::machine::SRU, registerInstruction);
    MapInstruction(cmsx::machine::CMP, registerInstruction);
    MapInstruction(cmsx::machine::CMPU, registerInstruction);
    MapInstruction(cmsx::machine::CSN, registerInstruction);
    MapInstruction(cmsx::machine::CSZ, registerInstruction);
    MapInstruction(cmsx::machine::CSP, registerInstruction);
    MapInstruction(cmsx::machine::CSOD, registerInstruction);
    MapInstruction(cmsx::machine::CSNN, registerInstruction);
    MapInstruction(cmsx::machine::CSNZ, registerInstruction);
    MapInstruction(cmsx::machine::CSNP, registerInstruction);
    MapInstruction(cmsx::machine::CSEV, registerInstruction);
    MapInstruction(cmsx::machine::ZSN, registerInstruction);
    MapInstruction(cmsx::machine::ZSZ, registerInstruction);
    MapInstruction(cmsx::machine::ZSP, registerInstruction);
    MapInstruction(cmsx::machine::ZSOD, registerInstruction);
    MapInstruction(cmsx::machine::ZSNN, registerInstruction);
    MapInstruction(cmsx::machine::ZSNZ, registerInstruction);
    MapInstruction(cmsx::machine::ZSNP, registerInstruction);
    MapInstruction(cmsx::machine::ZSEV, registerInstruction);
    MapInstruction(cmsx::machine::AND, registerInstruction);
    MapInstruction(cmsx::machine::OR, registerInstruction);
    MapInstruction(cmsx::machine::XOR, registerInstruction);
    MapInstruction(cmsx::machine::ANDN, registerInstruction);
    MapInstruction(cmsx::machine::ORN, registerInstruction);
    MapInstruction(cmsx::machine::NAND, registerInstruction);
    MapInstruction(cmsx::machine::NOR, registerInstruction);
    MapInstruction(cmsx::machine::NXOR, registerInstruction);
    MapInstruction(cmsx::machine::MUX, registerInstruction);
    MapInstruction(cmsx::machine::SADD, registerInstruction);
    MapInstruction(cmsx::machine::BDIF, registerInstruction);
    MapInstruction(cmsx::machine::WDIF, registerInstruction);
    MapInstruction(cmsx::machine::TDIF, registerInstruction);
    MapInstruction(cmsx::machine::ODIF, registerInstruction);
    AssemblyInstruction* negateInstruction = AddInstruction(new NegateInstruction());
    MapInstruction(cmsx::machine::NEG, negateInstruction);
    MapInstruction(cmsx::machine::NEGU, negateInstruction);
    AssemblyInstruction* floatingPointInstruction = AddInstruction(new FloatingPointInstruction());
    MapInstruction(cmsx::machine::FADD, floatingPointInstruction);
    MapInstruction(cmsx::machine::FSUB, floatingPointInstruction);
    MapInstruction(cmsx::machine::FMUL, floatingPointInstruction);
    MapInstruction(cmsx::machine::FDIV, floatingPointInstruction);
    MapInstruction(cmsx::machine::FCMP, floatingPointInstruction);
    AssemblyInstruction* floatingRoundingInstruction = AddInstruction(new FloatingRoundingInstruction());
    MapInstruction(cmsx::machine::FINT, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::FIX, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::FIXU, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::FLOT, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::FLOTU, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::SFLOT, floatingRoundingInstruction);
    MapInstruction(cmsx::machine::SFLOTU, floatingRoundingInstruction);
    AssemblyInstruction* wydeImmediateInstruction = AddInstruction(new WydeImmediateInstruction());
    MapInstruction(cmsx::machine::SETH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::SETMH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::SETML, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::SETL, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::INCH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::INCMH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::INCML, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::INCL, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ORH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ORMH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ORML, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ORL, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ANDNH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ANDNMH, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ANDNML, wydeImmediateInstruction);
    MapInstruction(cmsx::machine::ANDNL, wydeImmediateInstruction);
    AssemblyInstruction* jumpInstruction = AddInstruction(new JumpInstruction());
    MapInstruction(cmsx::machine::JMP, jumpInstruction);
    AssemblyInstruction* branchInstruction = AddInstruction(new BranchInstruction());
    MapInstruction(cmsx::machine::BN, branchInstruction);
    MapInstruction(cmsx::machine::BZ, branchInstruction);
    MapInstruction(cmsx::machine::BP, branchInstruction);
    MapInstruction(cmsx::machine::BOD, branchInstruction);
    MapInstruction(cmsx::machine::BNN, branchInstruction);
    MapInstruction(cmsx::machine::BNZ, branchInstruction);
    MapInstruction(cmsx::machine::BNP, branchInstruction);
    MapInstruction(cmsx::machine::BEV, branchInstruction);
    MapInstruction(cmsx::machine::PBN, branchInstruction);
    MapInstruction(cmsx::machine::PBZ, branchInstruction);
    MapInstruction(cmsx::machine::PBP, branchInstruction);
    MapInstruction(cmsx::machine::PBOD, branchInstruction);
    MapInstruction(cmsx::machine::PBNN, branchInstruction);
    MapInstruction(cmsx::machine::PBNZ, branchInstruction);
    MapInstruction(cmsx::machine::PBNP, branchInstruction);
    MapInstruction(cmsx::machine::PBEV, branchInstruction);
    AssemblyInstruction* getInstruction = AddInstruction(new GetInstruction());
    MapInstruction(cmsx::machine::GET, getInstruction);
    AssemblyInstruction* putInstruction = AddInstruction(new PutInstruction());
    MapInstruction(cmsx::machine::PUT, putInstruction);
    AssemblyInstruction* callInstruction = AddInstruction(new CallInstruction());
    MapInstruction(cmsx::machine::CALL, callInstruction);
    AssemblyInstruction* returnInstruction = AddInstruction(new ReturnInstruction());
    MapInstruction(cmsx::machine::RET, returnInstruction);
    AssemblyInstruction* trapInstruction = AddInstruction(new TrapInstruction());
    MapInstruction(cmsx::machine::TRAP, trapInstruction);
    AssemblyInstruction* swymInstruction = AddInstruction(new SwymInstruction());
    MapInstruction(cmsx::machine::SWYM, swymInstruction);
}

AssemblyInstruction* AssemblyInstructionMap::AddInstruction(AssemblyInstruction* instruction)
{
    instructions.push_back(std::unique_ptr<AssemblyInstruction>(instruction));
    return instruction;
}

void AssemblyInstructionMap::MapInstruction(int opc, AssemblyInstruction* instruction)
{
    instructionMap[opc] = instruction;
}

AssemblyInstruction* AssemblyInstructionMap::GetAssemblyInstruction(int opCode) const
{
    auto it = instructionMap.find(opCode);
    if (it != instructionMap.cend())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

AssemblyInstruction* GetAssemblyInstruction(int opc)
{
    return AssemblyInstructionMap::Instance().GetAssemblyInstruction(opc);
}

void InitAssemblyInstruction()
{
    AssemblyInstructionMap::Init();
}

void DoneAssemblyInstruction()
{
    AssemblyInstructionMap::Done();
}

} // namespace cmsx::assembler