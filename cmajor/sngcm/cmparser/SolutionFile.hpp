#ifndef SOLUTIONFILE_HPP
#define SOLUTIONFILE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Solution.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/SolutionFile.parser' using soulng parser generator spg version 3.0.0

class ContainerFileLexer;

struct SNGCM_PARSER_API SolutionFileParser
{
    static std::unique_ptr<sngcm::ast::Solution> Parse(ContainerFileLexer& lexer);
    static soulng::parser::Match SolutionFile(ContainerFileLexer& lexer);
    static soulng::parser::Match QualifiedId(ContainerFileLexer& lexer);
    static soulng::parser::Match Declaration(ContainerFileLexer& lexer);
    static soulng::parser::Match SolutionProjectDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match ActiveProjectDeclaration(ContainerFileLexer& lexer);
};

#endif // SOLUTIONFILE_HPP
