#include "AssemblyRules.hpp"

std::vector<const char*> rules = {
"AssemblyParser.AssemblyFile"
, "AssemblyParser.AssemblyLine"
, "AssemblyParser.CommentLine"
, "AssemblyParser.Comment"
, "AssemblyParser.EmptyLine"
, "AssemblyParser.ModeLine"
, "AssemblyParser.InstructionLine"
, "AssemblyParser.Instruction"
, "AssemblyParser.Label"
, "AssemblyParser.SplitLine"
, "AssemblyParser.OpCode"
, "AssemblyParser.Operands"
, "AssemblyParser.Expression"
, "AssemblyParser.WeakOperator"
, "AssemblyParser.Term"
, "AssemblyParser.StrongOperator"
, "AssemblyParser.PrimaryExpression"
, "AssemblyParser.UnaryOperator"
, "AssemblyParser.LocalSymbol"
, "AssemblyParser.Constant"
, "AssemblyParser.Symbol"
, "AssemblyParser.At"
};

std::vector<const char*>* GetRuleNameVecPtr()
{
    return &rules;
}
