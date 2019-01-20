#ifndef Statement_hpp_5555
#define Statement_hpp_5555

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/codedom/Statement.hpp>

namespace cmajor { namespace code {

class Statement : public cmajor::parsing::Grammar
{
public:
    static Statement* Create();
    static Statement* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    cmajor::codedom::CompoundStatement* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName);
private:
    Statement(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class StatementRule;
    class LabeledStatementRule;
    class LabelRule;
    class EmptyStatementRule;
    class ExpressionStatementRule;
    class CompoundStatementRule;
    class SelectionStatementRule;
    class IfStatementRule;
    class SwitchStatementRule;
    class IterationStatementRule;
    class WhileStatementRule;
    class DoStatementRule;
    class ForStatementRule;
    class ForInitStatementRule;
    class JumpStatementRule;
    class BreakStatementRule;
    class ContinueStatementRule;
    class ReturnStatementRule;
    class GotoStatementRule;
    class GotoTargetRule;
    class DeclarationStatementRule;
    class ConditionRule;
    class TryStatementRule;
    class HandlerSeqRule;
    class HandlerRule;
    class ExceptionDeclarationRule;
};

} } // namespace cmajor.code

#endif // Statement_hpp_5555
