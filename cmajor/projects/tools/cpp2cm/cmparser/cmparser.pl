namespace cmparser
{
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx) : Node*;
        PrefixTypeExpr(ParsingContext* ctx) : Node*;
        PostfixTypeExpr(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        PrimaryTypeExpr(ParsingContext* ctx) : Node*;
    }
    grammar TemplateGrammar
    {
        RuleTemplateParameter(ParsingContext* ctx) : Node*;
        TemplateId(ParsingContext* ctx, var UniquePtr<TemplateIdNode> templateId) : TemplateIdNode*;
    }
    grammar IdentifierGrammar
    {
        Identifier : IdentifierNode*;
        QualifiedId(var UniquePtr<Node> node) : Node*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar BasicTypeGrammar
    {
        BasicType : Node*;
    }
}
