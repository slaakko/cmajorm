#ifndef Delegate_hpp_8344
#define Delegate_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Delegate.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Delegate : public cmajor::parsing::Grammar
{
public:
    static Delegate* Create();
    static Delegate* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    DelegateNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Delegate(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class DelegateRule;
    class ClassDelegateRule;
};

} } // namespace cmajor.parser

#endif // Delegate_hpp_8344
