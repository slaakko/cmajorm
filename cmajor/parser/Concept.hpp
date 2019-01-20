#ifndef Concept_hpp_26948
#define Concept_hpp_26948

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/Concept.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class Concept : public cmajor::parsing::Grammar
{
public:
    static Concept* Create();
    static Concept* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    ConceptNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    Concept(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class ConceptRule;
    class RefinementRule;
    class ConceptBodyRule;
    class ConceptBodyConstraintRule;
    class TypeNameConstraintRule;
    class SignatureConstraintRule;
    class ConstructorConstraintRule;
    class DestructorConstraintRule;
    class MemberFunctionConstraintRule;
    class FunctionConstraintRule;
    class EmbeddedConstraintRule;
    class WhereConstraintRule;
    class ConstraintExprRule;
    class DisjunctiveConstraintExprRule;
    class ConjunctiveConstraintExprRule;
    class PrimaryConstraintExprRule;
    class AtomicConstraintExprRule;
    class PredicateConstraintRule;
    class IsConstraintRule;
    class ConceptOrTypeNameRule;
    class MultiParamConstraintRule;
    class AxiomRule;
    class AxiomBodyRule;
    class AxiomStatementRule;
};

} } // namespace cmajor.parser

#endif // Concept_hpp_26948
