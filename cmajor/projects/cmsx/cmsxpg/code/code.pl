namespace Code
{
    grammar IdentifierGrammar
    {
        Identifier : Identifier*;
        QualifiedId : Identifier*;
        TemplateId(ParsingContext* ctx, var UniquePtr<Identifier> qid, var UniquePtr<TemplateId> templateId) : TemplateId*;
        UsingAlias(var UniquePtr<Identifier> id, var UniquePtr<Identifier> qid) : ustring;
        UsingImport(var UniquePtr<Identifier> qid) : ustring;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar ExpressionGrammar
    {
        Expression(ParsingContext* ctx) : DomObject*;
        Disjunction(ParsingContext* ctx, var UniquePtr<DomObject> expr) : DomObject*;
        Conjunction(ParsingContext* ctx, var UniquePtr<DomObject> expr) : DomObject*;
        BitOr(ParsingContext* ctx, var UniquePtr<DomObject> expr) : DomObject*;
        BitXor(ParsingContext* ctx, var UniquePtr<DomObject> expr) : DomObject*;
        BitAnd(ParsingContext* ctx, var UniquePtr<DomObject> expr) : DomObject*;
        Equality(ParsingContext* ctx, var UniquePtr<DomObject> expr, var Operator op) : DomObject*;
        Relational(ParsingContext* ctx, var UniquePtr<DomObject> expr, var Operator op) : DomObject*;
        Shift(ParsingContext* ctx, var UniquePtr<DomObject> expr, var Operator op) : DomObject*;
        Additive(ParsingContext* ctx, var UniquePtr<DomObject> expr, var Operator op) : DomObject*;
        Multiplicative(ParsingContext* ctx, var UniquePtr<DomObject> expr, var Operator op) : DomObject*;
        Prefix(ParsingContext* ctx, var Operator op) : DomObject*;
        Postfix(ParsingContext* ctx, var UniquePtr<Identifier> id, var UniquePtr<DomObject> expr) : DomObject*;
        Primary(ParsingContext* ctx) : DomObject*;
        SizeOfExpr(ParsingContext* ctx) : DomObject*;
        TypeNameExpr(ParsingContext* ctx) : DomObject*;
        CastExpr(ParsingContext* ctx) : DomObject*;
        ConstructExpr(ParsingContext* ctx) : DomObject*;
        NewExpr(ParsingContext* ctx) : DomObject*;
        ArgumentList(ParsingContext* ctx, DomObject* owner);
        ExpressionList(ParsingContext* ctx, DomObject* owner);
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx) : DomObject*;
        PrefixTypeExpr(ParsingContext* ctx) : DomObject*;
        PostfixTypeExpr(ParsingContext* ctx, var UniquePtr<Identifier> id, var UniquePtr<DomObject> typeExpr) : DomObject*;
        PrimaryTypeExpr(ParsingContext* ctx) : DomObject*;
    }
    grammar BasicTypeGrammar
    {
        BasicType : Type*;
    }
    grammar LiteralGrammar
    {
        Literal : Literal*;
        BooleanLiteral : Literal*;
        FloatingLiteral : Literal*;
        FloatingLiteralValue;
        FractionalFloatingLiteral;
        ExponentFloatingLiteral;
        ExponentPart;
        IntegerLiteral : Literal*;
        IntegerLiteralValue;
        HexIntegerLiteral;
        DecIntegerLiteral;
        CharLiteral : Literal*;
        StringLiteral : Literal*;
        NullLiteral : Literal*;
        CharEscape;
    }
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx) : Statement*;
        LabelId : ustring;
        LabeledStatement(ParsingContext* ctx) : Statement*;
        ControlStatement(ParsingContext* ctx) : Statement*;
        CompoundStatement(ParsingContext* ctx) : CompoundStatement*;
        ReturnStatement(ParsingContext* ctx) : Statement*;
        IfStatement(ParsingContext* ctx) : Statement*;
        WhileStatement(ParsingContext* ctx) : Statement*;
        DoStatement(ParsingContext* ctx) : Statement*;
        ForStatement(ParsingContext* ctx) : Statement*;
        ForInitStatement(ParsingContext* ctx) : Statement*;
        ForLoopStatementExpr(ParsingContext* ctx) : Statement*;
        RangeForStatement(ParsingContext* ctx) : Statement*;
        BreakStatement(ParsingContext* ctx) : Statement*;
        ContinueStatement(ParsingContext* ctx) : Statement*;
        GotoStatement(ParsingContext* ctx, var UniquePtr<Identifier> id) : Statement*;
        SwitchStatement(ParsingContext* ctx) : SwitchStatement*;
        CaseStatement(ParsingContext* ctx, var UniquePtr<CaseStatement> caseS) : CaseStatement*;
        DefaultStatement(ParsingContext* ctx) : DefaultStatement*;
        GotoCaseStatement(ParsingContext* ctx) : Statement*;
        GotoDefaultStatement(ParsingContext* ctx) : Statement*;
        AssignmentStatementExpr(ParsingContext* ctx, var UniquePtr<DomObject> targetExpr) : Statement*;
        AssignmentStatement(ParsingContext* ctx) : Statement*;
        ConstructionStatement(ParsingContext* ctx) : ConstructionStatement*;
        DeleteStatement(ParsingContext* ctx) : Statement*;
        DestroyStatement(ParsingContext* ctx) : Statement*;
        ExpressionStatement(ParsingContext* ctx, var UniquePtr<DomObject> expr) : Statement*;
        EmptyStatement(ParsingContext* ctx) : Statement*;
        ThrowStatement(ParsingContext* ctx) : Statement*;
        TryStatement(ParsingContext* ctx) : TryStatement*;
        Catch(ParsingContext* ctx) : Catch*;
        AssertStatement(ParsingContext* ctx) : Statement*;
    }
}
