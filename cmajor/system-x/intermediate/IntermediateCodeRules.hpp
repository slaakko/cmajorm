#ifndef RULES_H
#define RULES_H
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/IntermediateCodeTokens.hpp>
#include <system-x/intermediate/IntermediateCodeLexer.hpp>
#include <vector>

CMSX_INTERMEDIATE_API std::vector<const char*>* GetRuleNameVecPtr();

const int IntermediateCodeParser_IntermediateCodeFile = 0;
const int IntermediateCodeParser_CompileUnitHeader = 1;
const int IntermediateCodeParser_CompileUnitId = 2;
const int IntermediateCodeParser_TypeDeclarations = 3;
const int IntermediateCodeParser_TypeDeclaration = 4;
const int IntermediateCodeParser_TypeId = 5;
const int IntermediateCodeParser_FundamentalTypeId = 6;
const int IntermediateCodeParser_TypeRef = 7;
const int IntermediateCodeParser_StructureType = 8;
const int IntermediateCodeParser_ArrayType = 9;
const int IntermediateCodeParser_FunctionType = 10;
const int IntermediateCodeParser_DataDefinitions = 11;
const int IntermediateCodeParser_DataDefinition = 12;
const int IntermediateCodeParser_Constant = 13;
const int IntermediateCodeParser_BoolConstant = 14;
const int IntermediateCodeParser_SByteConstant = 15;
const int IntermediateCodeParser_ByteConstant = 16;
const int IntermediateCodeParser_ShortConstant = 17;
const int IntermediateCodeParser_UShortConstant = 18;
const int IntermediateCodeParser_IntConstant = 19;
const int IntermediateCodeParser_UIntConstant = 20;
const int IntermediateCodeParser_LongConstant = 21;
const int IntermediateCodeParser_ULongConstant = 22;
const int IntermediateCodeParser_FloatConstant = 23;
const int IntermediateCodeParser_DoubleConstant = 24;
const int IntermediateCodeParser_AddressConstant = 25;
const int IntermediateCodeParser_ArrayConstant = 26;
const int IntermediateCodeParser_StructureConstant = 27;
const int IntermediateCodeParser_StringConstant = 28;
const int IntermediateCodeParser_StringArrayConstant = 29;
const int IntermediateCodeParser_StringArrayPrefix = 30;
const int IntermediateCodeParser_ConversionConstant = 31;
const int IntermediateCodeParser_ClsIdConstant = 32;
const int IntermediateCodeParser_SymbolConstant = 33;
const int IntermediateCodeParser_Value = 34;
const int IntermediateCodeParser_RegValue = 35;
const int IntermediateCodeParser_ResultRegValue = 36;
const int IntermediateCodeParser_SymbolValue = 37;
const int IntermediateCodeParser_LiteralValue = 38;
const int IntermediateCodeParser_FunctionDefinitions = 39;
const int IntermediateCodeParser_FunctionDefinition = 40;
const int IntermediateCodeParser_FunctionHeader = 41;
const int IntermediateCodeParser_BasicBlock = 42;
const int IntermediateCodeParser_Label = 43;
const int IntermediateCodeParser_Operand = 44;
const int IntermediateCodeParser_Instructions = 45;
const int IntermediateCodeParser_Instruction = 46;
const int IntermediateCodeParser_StoreInstruction = 47;
const int IntermediateCodeParser_ArgInstruction = 48;
const int IntermediateCodeParser_JmpInstruction = 49;
const int IntermediateCodeParser_BranchInstruction = 50;
const int IntermediateCodeParser_ProcedureCallInstruction = 51;
const int IntermediateCodeParser_RetInstruction = 52;
const int IntermediateCodeParser_SwitchInstruction = 53;
const int IntermediateCodeParser_ValueInstruction = 54;
const int IntermediateCodeParser_Operation = 55;
const int IntermediateCodeParser_UnaryInstruction = 56;
const int IntermediateCodeParser_BinaryInstruction = 57;
const int IntermediateCodeParser_ParamInstruction = 58;
const int IntermediateCodeParser_LocalInstruction = 59;
const int IntermediateCodeParser_LoadInstruction = 60;
const int IntermediateCodeParser_ElemAddrInstruction = 61;
const int IntermediateCodeParser_PtrOffsetInstruction = 62;
const int IntermediateCodeParser_PtrDiffInstruction = 63;
const int IntermediateCodeParser_FunctionCallInstruction = 64;
const int IntermediateCodeParser_TrapInstruction = 65;
const int IntermediateCodeParser_PhiInstruction = 66;
const int IntermediateCodeParser_BlockValue = 67;
const int IntermediateCodeParser_NoOperationInstruction = 68;
const int IntermediateCodeParser_Metadata = 69;
const int IntermediateCodeParser_MetadataStruct = 70;
const int IntermediateCodeParser_MetadataField = 71;
const int IntermediateCodeParser_MetadataItem = 72;
const int IntermediateCodeParser_MetadataBool = 73;
const int IntermediateCodeParser_MetadataLong = 74;
const int IntermediateCodeParser_MetadataString = 75;
const int IntermediateCodeParser_MetadataRef = 76;

#endif // RULES_H
