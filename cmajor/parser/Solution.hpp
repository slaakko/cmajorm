#ifndef Solution_hpp_8344
#define Solution_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Solution.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Solution : public cmajor::parsing::Grammar
{
public:
    static Solution* Create();
    static Solution* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::ast::Solution* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Solution(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class SolutionRule;
    class DeclarationRule;
    class SolutionProjectDeclarationRule;
    class ActiveProjectDeclarationRule;
    class FilePathRule;
};

} } // namespace cmajor.parser

#endif // Solution_hpp_8344
