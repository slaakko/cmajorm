#ifndef RULES_H
#define RULES_H
#include <system-x/assembler/Api.hpp>
#include <vector>

CMSX_ASSEMBLER_API std::vector<const char*>* GetRuleNameVecPtr();

const int AssemblyParser_AssemblyFile = 0;
const int AssemblyParser_AssemblyLine = 1;
const int AssemblyParser_CommentLine = 2;
const int AssemblyParser_Comment = 3;
const int AssemblyParser_EmptyLine = 4;
const int AssemblyParser_ModeLine = 5;
const int AssemblyParser_InstructionLine = 6;
const int AssemblyParser_Instruction = 7;
const int AssemblyParser_Label = 8;
const int AssemblyParser_SplitLine = 9;
const int AssemblyParser_OpCode = 10;
const int AssemblyParser_Operands = 11;
const int AssemblyParser_Expression = 12;
const int AssemblyParser_WeakOperator = 13;
const int AssemblyParser_Term = 14;
const int AssemblyParser_StrongOperator = 15;
const int AssemblyParser_PrimaryExpression = 16;
const int AssemblyParser_UnaryOperator = 17;
const int AssemblyParser_LocalSymbol = 18;
const int AssemblyParser_Constant = 19;
const int AssemblyParser_Symbol = 20;
const int AssemblyParser_At = 21;

#endif // RULES_H
