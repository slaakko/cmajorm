#ifndef Json_hpp_8344
#define Json_hpp_8344

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/util/Json.hpp>
#include <sstream>

namespace cmajor { namespace parser {

using namespace cmajor::util;
class Json : public cmajor::parsing::Grammar
{
public:
    static Json* Create();
    static Json* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    JsonValue* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Json(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ValueRule;
    class StringRule;
    class NumberRule;
    class ObjectRule;
    class ArrayRule;
};

} } // namespace cmajor.parser

#endif // Json_hpp_8344
