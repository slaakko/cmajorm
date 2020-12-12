#ifndef PROJECTFILE_HPP
#define PROJECTFILE_HPP
#include <sngcm/cmparser/ParserApi.hpp>
#include <sngcm/ast/Project.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/ProjectFile.parser' using soulng parser generator spg version 3.0.0

class ContainerFileLexer;

struct SNGCM_PARSER_API ProjectFileParser
{
    static std::unique_ptr<sngcm::ast::Project> Parse(ContainerFileLexer& lexer, std::string config, sngcm::ast::BackEnd backend, std::string toolChain, sngcm::ast::SystemDirKind systemDirKind);
    static soulng::parser::Match ProjectFile(ContainerFileLexer& lexer, std::string config, sngcm::ast::BackEnd backend, std::string toolChain, sngcm::ast::SystemDirKind systemDirKind);
    static soulng::parser::Match QualifiedId(ContainerFileLexer& lexer);
    static soulng::parser::Match Declaration(ContainerFileLexer& lexer);
    static soulng::parser::Match ReferenceDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match SourceFileDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match ResourceFileDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match TextFileDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match TargetDeclaration(ContainerFileLexer& lexer);
    static soulng::parser::Match Target(ContainerFileLexer& lexer);
};

#endif // PROJECTFILE_HPP
