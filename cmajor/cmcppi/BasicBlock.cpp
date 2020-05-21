// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmcppi/BasicBlock.hpp>
#include <soulng/util/TextUtils.hpp>

namespace cmcppi {

BasicBlock::BasicBlock(int id_, const std::string& name_) : id(id_), name(name_)
{
}

void BasicBlock::AddInstruction(Instruction* instruction)
{
    instructions.push_back(std::unique_ptr<Instruction>(instruction));
}

void BasicBlock::Write(CodeFormatter& formatter, Function& function, Context& context)
{
    int indent = formatter.IndentSize();
    bool indentDecremented = false;
    if (formatter.CurrentIndent() > 0)
    {
        formatter.DecIndent();
        indentDecremented = true;
    }
    formatter.Write(Format("__bb" + std::to_string(id) + ":", indent, FormatWidth::min));
    bool first = true;
    std::string n = "["+ name + "] ";
    int ni = instructions.size();
    for (int i = 0; i < ni; ++i)
    {
        Instruction* inst = instructions[i].get();
        formatter.WriteLine("// " + n + inst->IrName() + " : source line=" + std::to_string(inst->SourceLineNumber()) + ":");
        if (first)
        {
            if (indentDecremented)
            {
                formatter.IncIndent();
            }
            first = false;
        }
        inst->Write(formatter, function, context);
        formatter.WriteLine(";");
        n = "";
    }
}

} // namespace cmcppi
