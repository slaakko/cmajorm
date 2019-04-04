#ifndef GlobalVariable_hpp_8376
#define GlobalVariable_hpp_8376

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/GlobalVariable.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class GlobalVariable : public cmajor::parsing::Grammar
{
public:
    static GlobalVariable* Create();
    static GlobalVariable* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    GlobalVariableNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx, CompileUnitNode* cu);
private:
    GlobalVariable(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class GlobalVariableRule;
};

} } // namespace cmajor.parser

#endif // GlobalVariable_hpp_8376
