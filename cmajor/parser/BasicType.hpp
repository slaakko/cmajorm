#ifndef BasicType_hpp_8344
#define BasicType_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/BasicType.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class BasicType : public cmajor::parsing::Grammar
{
public:
    static BasicType* Create();
    static BasicType* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    Node* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    BasicType(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class BasicTypeRule;
};

} } // namespace cmajor.parser

#endif // BasicType_hpp_8344
