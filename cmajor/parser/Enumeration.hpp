#ifndef Enumeration_hpp_3397
#define Enumeration_hpp_3397

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Enumeration.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Enumeration : public cmajor::parsing::Grammar
{
public:
    static Enumeration* Create();
    static Enumeration* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    EnumTypeNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Enumeration(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class EnumTypeRule;
    class UnderlyingTypeRule;
    class EnumConstantsRule;
    class EnumConstantRule;
};

} } // namespace cmajor.parser

#endif // Enumeration_hpp_3397
