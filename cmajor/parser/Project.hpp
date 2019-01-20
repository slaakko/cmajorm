#ifndef Project_hpp_8344
#define Project_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Project.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Project : public cmajor::parsing::Grammar
{
public:
    static Project* Create();
    static Project* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::ast::Project* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, std::string config);
private:
    Project(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ProjectRule;
    class DeclarationRule;
    class ReferenceDeclarationRule;
    class SourceFileDeclarationRule;
    class TextFileDeclarationRule;
    class TargetDeclarationRule;
    class TargetRule;
    class FilePathRule;
};

} } // namespace cmajor.parser

#endif // Project_hpp_8344
