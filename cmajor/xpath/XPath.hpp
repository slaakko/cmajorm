#ifndef XPath_hpp_6600
#define XPath_hpp_6600

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/xml/XmlGrammar.hpp>
#include <cmajor/xpath/XPathExpr.hpp>

namespace cmajor { namespace xpath {

class XPath : public cmajor::parsing::Grammar
{
public:
    static XPath* Create();
    static XPath* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    XPathExpr* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    std::vector<std::u32string> keywords0;
    XPath(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ExprRule;
    class OrExprRule;
    class AndExprRule;
    class EqualityExprRule;
    class RelationalExprRule;
    class AdditiveExprRule;
    class MultiplicativeExprRule;
    class UnaryExprRule;
    class UnionExprRule;
    class PathExprRule;
    class FilterExprRule;
    class LocationPathRule;
    class AbsoluteLocationPathRule;
    class RelativeLocationPathRule;
    class AbbreviatedAbsoluteLocationPathRule;
    class StepRule;
    class AbbreviatedStepRule;
    class AxisSpecifierRule;
    class AxisNameRule;
    class AbbreviatedAxisSpecifierRule;
    class PredicateRule;
    class PredicateExprRule;
    class NodeTestRule;
    class NameTestRule;
    class NodeTypeRule;
    class PrimaryExprRule;
    class VariableReferenceRule;
    class LiteralRule;
    class NumberRule;
    class FunctionCallRule;
    class ArgumentRule;
    class FunctionNameRule;
    class QNameRule;
    class PrefixedNameRule;
    class UnprefixedNameRule;
    class PrefixRule;
    class LocalPartRule;
    class NCNameRule;
};

} } // namespace cmajor.xpath

#endif // XPath_hpp_6600
