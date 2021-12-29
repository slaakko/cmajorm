#ifndef INTERMEDIATECODEPARSER_HPP
#define INTERMEDIATECODEPARSER_HPP
#include <system-x/intermediate/Context.hpp>
#include <system-x/intermediate/Error.hpp>
#include <system-x/intermediate/IntermediateCodeTokens.hpp>
#include <system-x/intermediate/IntermediateCodeLexer.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/intermediate/IntermediateCodeParser.parser' using soulng parser generator spg version 3.1.0

class IntermediateCodeLexer;

struct CMSX_INTERMEDIATE_API IntermediateCodeParser
{
    static void Parse(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match IntermediateCodeFile(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match CompileUnitHeader(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match CompileUnitId(IntermediateCodeLexer& lexer);
    static soulng::parser::Match TypeDeclarations(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match TypeDeclaration(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match TypeId(IntermediateCodeLexer& lexer);
    static soulng::parser::Match FundamentalTypeId(IntermediateCodeLexer& lexer);
    static soulng::parser::Match TypeRef(IntermediateCodeLexer& lexer);
    static soulng::parser::Match StructureType(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, int32_t typeId);
    static soulng::parser::Match ArrayType(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, int32_t typeId);
    static soulng::parser::Match FunctionType(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, int32_t typeId);
    static soulng::parser::Match DataDefinitions(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match DataDefinition(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match Constant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match BoolConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match SByteConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ByteConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ShortConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match UShortConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match IntConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match UIntConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match LongConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ULongConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match FloatConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match DoubleConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match AddressConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ArrayConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match StructureConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match StringConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match StringArrayConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match StringArrayPrefix(IntermediateCodeLexer& lexer);
    static soulng::parser::Match ConversionConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ClsIdConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match SymbolConstant(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match Value(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Type* type);
    static soulng::parser::Match RegValue(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Type* type);
    static soulng::parser::Match ResultRegValue(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Type* type);
    static soulng::parser::Match SymbolValue(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Type* type);
    static soulng::parser::Match LiteralValue(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Type* type);
    static soulng::parser::Match FunctionDefinitions(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match FunctionDefinition(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match FunctionHeader(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match BasicBlock(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::Function* function);
    static soulng::parser::Match Label(IntermediateCodeLexer& lexer);
    static soulng::parser::Match Operand(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match Instructions(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::BasicBlock* basicBlock);
    static soulng::parser::Match Instruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match StoreInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ArgInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match JmpInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match BranchInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ProcedureCallInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match RetInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match SwitchInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match ValueInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match Operation(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match UnaryInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match BinaryInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match ParamInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match LocalInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match LoadInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match ElemAddrInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match PtrOffsetInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match PtrDiffInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match FunctionCallInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match TrapInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match PhiInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::RegValue* result);
    static soulng::parser::Match BlockValue(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match NoOperationInstruction(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match Metadata(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataStruct(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataField(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context, cmsx::intermediate::MetadataStruct* parent);
    static soulng::parser::Match MetadataItem(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataBool(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataLong(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataString(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
    static soulng::parser::Match MetadataRef(IntermediateCodeLexer& lexer, cmsx::intermediate::Context* context);
};

#endif // INTERMEDIATECODEPARSER_HPP
