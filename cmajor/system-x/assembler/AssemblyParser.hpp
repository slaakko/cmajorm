#ifndef ASSEMBLYPARSER_HPP
#define ASSEMBLYPARSER_HPP
#include <system-x/assembler/Assembler.hpp>
#include <system-x/assembler/Expression.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyParser.parser' using soulng parser generator spg version 3.1.0

class AssemblyLexer;

struct CMSX_ASSEMBLER_API AssemblyParser
{
    static void Parse(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match AssemblyFile(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match AssemblyLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match CommentLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Comment(AssemblyLexer& lexer);
    static soulng::parser::Match EmptyLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match ModeLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match InstructionLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Instruction(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Label(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match SplitLine(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler, bool hasLabel);
    static soulng::parser::Match OpCode(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Operands(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler, cmsx::assembler::Instruction* instruction);
    static soulng::parser::Match Expression(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match WeakOperator(AssemblyLexer& lexer);
    static soulng::parser::Match Term(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match StrongOperator(AssemblyLexer& lexer);
    static soulng::parser::Match PrimaryExpression(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match UnaryOperator(AssemblyLexer& lexer);
    static soulng::parser::Match LocalSymbol(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Constant(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match Symbol(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
    static soulng::parser::Match At(AssemblyLexer& lexer, cmsx::assembler::Assembler* assembler);
};

#endif // ASSEMBLYPARSER_HPP
