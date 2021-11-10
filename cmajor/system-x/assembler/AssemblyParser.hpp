#ifndef ASSEMBLYPARSER_HPP
#define ASSEMBLYPARSER_HPP
#include <system-x/assembler/Api.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyParser.parser' using soulng parser generator spg version 3.1.0

class AssemblyLexer;

struct CMSX_ASSEMBLER_API AssemblyParser
{
    static void Parse(AssemblyLexer& lexer);
    static soulng::parser::Match AssemblyFile(AssemblyLexer& lexer);
    static soulng::parser::Match AssemblyLine(AssemblyLexer& lexer);
    static soulng::parser::Match CommentLine(AssemblyLexer& lexer);
    static soulng::parser::Match Comment(AssemblyLexer& lexer);
    static soulng::parser::Match EmptyLine(AssemblyLexer& lexer);
    static soulng::parser::Match ModeLine(AssemblyLexer& lexer);
    static soulng::parser::Match InstructionLine(AssemblyLexer& lexer);
    static soulng::parser::Match Instruction(AssemblyLexer& lexer);
    static soulng::parser::Match Label(AssemblyLexer& lexer);
    static soulng::parser::Match SplitLine(AssemblyLexer& lexer, bool hasLabel);
    static soulng::parser::Match OpCode(AssemblyLexer& lexer);
    static soulng::parser::Match Operands(AssemblyLexer& lexer);
    static soulng::parser::Match Expression(AssemblyLexer& lexer);
    static soulng::parser::Match WeakOperator(AssemblyLexer& lexer);
    static soulng::parser::Match Term(AssemblyLexer& lexer);
    static soulng::parser::Match StrongOperator(AssemblyLexer& lexer);
    static soulng::parser::Match PrimaryExpression(AssemblyLexer& lexer);
    static soulng::parser::Match UnaryOperator(AssemblyLexer& lexer);
    static soulng::parser::Match LocalSymbol(AssemblyLexer& lexer);
    static soulng::parser::Match Constant(AssemblyLexer& lexer);
    static soulng::parser::Match Symbol(AssemblyLexer& lexer);
    static soulng::parser::Match At(AssemblyLexer& lexer);
};

#endif // ASSEMBLYPARSER_HPP
