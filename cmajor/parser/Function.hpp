#ifndef Function_hpp_4529
#define Function_hpp_4529

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Function.hpp>
#include <cmajor/parser/ParsingContext.hpp>
#include <cmajor/parser/Attribute.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Function : public cmajor::parsing::Grammar
{
public:
    static Function* Create();
    static Function* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    FunctionNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Function(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class FunctionRule;
    class FunctionGroupIdRule;
    class OperatorFunctionGroupIdRule;
};

} } // namespace cmajor.parser

#endif // Function_hpp_4529
