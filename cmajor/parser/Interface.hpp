#ifndef Interface_hpp_23665
#define Interface_hpp_23665

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Interface.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Interface : public cmajor::parsing::Grammar
{
public:
    static Interface* Create();
    static Interface* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    InterfaceNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Interface(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class InterfaceRule;
    class InterfaceContentRule;
    class InterfaceMemFunRule;
    class InterfaceFunctionGroupIdRule;
};

} } // namespace cmajor.parser

#endif // Interface_hpp_23665
