#ifndef CompileUnit_hpp_8376
#define CompileUnit_hpp_8376

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/ast/CompileUnit.hpp>
#include <cmajor/parser/ParsingContext.hpp>

namespace cmajor { namespace parser {

using namespace cmajor::ast;
class CompileUnit : public cmajor::parsing::Grammar
{
public:
    static CompileUnit* Create();
    static CompileUnit* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    CompileUnitNode* Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, ParsingContext* ctx);
private:
    CompileUnit(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class CompileUnitRule;
    class NamespaceContentRule;
    class UsingDirectivesRule;
    class UsingDirectiveRule;
    class UsingAliasDirectiveRule;
    class UsingNamespaceDirectiveRule;
    class DefinitionsRule;
    class DefinitionRule;
    class NamespaceDefinitionRule;
    class TypedefDeclarationRule;
    class ConceptDefinitionRule;
    class FunctionDefinitionRule;
    class ClassDefinitionRule;
    class InterfaceDefinitionRule;
    class EnumTypeDefinitionRule;
    class ConstantDefinitionRule;
    class DelegateDefinitionRule;
    class ClassDelegateDefinitionRule;
    class GlobalVariableDefinitionRule;
};

} } // namespace cmajor.parser

#endif // CompileUnit_hpp_8376
