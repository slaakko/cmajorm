namespace cmajor.parser
{
    grammar Attribute
    {
        Attributes: cmajor::ast::Attributes*;
        Attribute(cmajor::ast::Attributes* attributes);
    }
    grammar BasicType
    {
        BasicType: Node*;
    }
    grammar Constant
    {
        Constant(ParsingContext* ctx, var std::u32string strValue): ConstantNode*;
    }
    grammar Class
    {
        Class(ParsingContext* ctx, var std::unique_ptr<Attributes> attributes, var Span specifierSpan, var Span classSpan, var Span beginBraceSpan, var Span endBraceSpan): ClassNode*;
        InheritanceAndInterfaces(ParsingContext* ctx, ClassNode* classNode);
        BaseClassOrInterface(ParsingContext* ctx): Node*;
        ClassContent(ParsingContext* ctx, ClassNode* classNode);
        ClassMember(ParsingContext* ctx, ClassNode* classNode): Node*;
        StaticConstructor(ParsingContext* ctx, ClassNode* classNode, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<Attributes> attributes, var Span specifierSpan): StaticConstructorNode*;
        Constructor(ParsingContext* ctx, ClassNode* classNode, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<ConstructorNode> ctor, var std::unique_ptr<Attributes> attributes, var Span specifierSpan): Node*;
        Destructor(ParsingContext* ctx, ClassNode* classNode, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<DestructorNode> dtor, var std::unique_ptr<Attributes> attributes, var Span specifierSpan): Node*;
        Initializer(ParsingContext* ctx): InitializerNode*;
        MemberFunction(ParsingContext* ctx, var std::unique_ptr<MemberFunctionNode> memFun, var std::unique_ptr<IdentifierNode> qid, var std::unique_ptr<Attributes> attributes, var Span specifierSpan, var Span groupIdSpan): Node*;
        ConversionFunction(ParsingContext* ctx, var std::unique_ptr<ConversionFunctionNode> conversionFun, var std::unique_ptr<Attributes> attributes, var Span specifierSpan): Node*;
        MemberVariable(ParsingContext* ctx, var std::unique_ptr<Attributes> attributes, var Span specifierSpan): MemberVariableNode*;
    }
    grammar Solution
    {
        Solution: cmajor::ast::Solution*;
        Declaration: SolutionDeclaration*;
        SolutionProjectDeclaration: SolutionDeclaration*;
        ActiveProjectDeclaration: SolutionDeclaration*;
        FilePath: std::string;
    }
    grammar Enumeration
    {
        EnumType(ParsingContext* ctx, var Span beginBraceSpan, var Span endBraceSpan): EnumTypeNode*;
        UnderlyingType(ParsingContext* ctx): Node*;
        EnumConstants(ParsingContext* ctx, EnumTypeNode* enumType);
        EnumConstant(ParsingContext* ctx, EnumTypeNode* enumType, var Span s): EnumConstantNode*;
    }
    grammar CommandLine
    {
        CommandLine: std::vector<std::string>;
        Spaces;
        Argument: std::string;
        ArgElement: std::string;
        OddBackslashesAndLiteralQuotationMark: std::string;
        EvenBackslashesAndQuotationMark: std::string;
        StringChar: std::string;
    }
    grammar Delegate
    {
        Delegate(ParsingContext* ctx): DelegateNode*;
        ClassDelegate(ParsingContext* ctx): ClassDelegateNode*;
    }
    grammar CompileUnit
    {
        CompileUnit(ParsingContext* ctx): CompileUnitNode*;
        NamespaceContent(ParsingContext* ctx, NamespaceNode* ns);
        UsingDirectives(ParsingContext* ctx, NamespaceNode* ns);
        UsingDirective(ParsingContext* ctx, NamespaceNode* ns);
        UsingAliasDirective(var std::unique_ptr<IdentifierNode> id): Node*;
        UsingNamespaceDirective: Node*;
        Definitions(ParsingContext* ctx, NamespaceNode* ns);
        Definition(ParsingContext* ctx, NamespaceNode* ns): Node*;
        NamespaceDefinition(ParsingContext* ctx, NamespaceNode* ns): NamespaceNode*;
        TypedefDeclaration(ParsingContext* ctx): TypedefNode*;
        ConceptDefinition(ParsingContext* ctx): ConceptNode*;
        FunctionDefinition(ParsingContext* ctx): FunctionNode*;
        ClassDefinition(ParsingContext* ctx): ClassNode*;
        InterfaceDefinition(ParsingContext* ctx): InterfaceNode*;
        EnumTypeDefinition(ParsingContext* ctx): EnumTypeNode*;
        ConstantDefinition(ParsingContext* ctx): ConstantNode*;
        DelegateDefinition(ParsingContext* ctx): DelegateNode*;
        ClassDelegateDefinition(ParsingContext* ctx): ClassDelegateNode*;
    }
    grammar Concept
    {
        Concept(ParsingContext* ctx, var Span beginBraceSpan, var Span endBraceSpan): ConceptNode*;
        Refinement: ConceptIdNode*;
        ConceptBody(ParsingContext* ctx, ConceptNode* concept);
        ConceptBodyConstraint(ParsingContext* ctx, ConceptNode* concept);
        TypeNameConstraint(ParsingContext* ctx): ConstraintNode*;
        SignatureConstraint(ParsingContext* ctx, IdentifierNode* firstTypeParameter): ConstraintNode*;
        ConstructorConstraint(ParsingContext* ctx, IdentifierNode* firstTypeParameter, var std::unique_ptr<IdentifierNode> id, var std::unique_ptr<ConstraintNode> ctorConstraint): ConstraintNode*;
        DestructorConstraint(ParsingContext* ctx, IdentifierNode* firstTypeParameter, var std::unique_ptr<IdentifierNode> id): ConstraintNode*;
        MemberFunctionConstraint(ParsingContext* ctx, var std::unique_ptr<Node> returnType, var std::unique_ptr<IdentifierNode> typeParam): ConstraintNode*;
        FunctionConstraint(ParsingContext* ctx): ConstraintNode*;
        EmbeddedConstraint(ParsingContext* ctx): WhereConstraintNode*;
        WhereConstraint(ParsingContext* ctx): WhereConstraintNode*;
        ConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        DisjunctiveConstraintExpr(ParsingContext* ctx, var Span s): ConstraintNode*;
        ConjunctiveConstraintExpr(ParsingContext* ctx, var Span s): ConstraintNode*;
        PrimaryConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        AtomicConstraintExpr(ParsingContext* ctx): ConstraintNode*;
        PredicateConstraint(ParsingContext* ctx): ConstraintNode*;
        IsConstraint(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): ConstraintNode*;
        ConceptOrTypeName(ParsingContext* ctx): Node*;
        MultiParamConstraint(ParsingContext* ctx, var std::unique_ptr<MultiParamConstraintNode> constraint): ConstraintNode*;
        Axiom(ParsingContext* ctx, ConceptNode* concept, var std::unique_ptr<AxiomNode> axiom, var Span axiomSpan, var Span beginBraceSpan, var Span endBraceSpan);
        AxiomBody(ParsingContext* ctx, AxiomNode* axiom);
        AxiomStatement(ParsingContext* ctx): AxiomStatementNode*;
    }
    grammar Expression
    {
        Expression(ParsingContext* ctx): Node*;
        Equivalence(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        Implication(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        Disjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        Conjunction(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        BitOr(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        BitXor(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        BitAnd(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        Equality(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        Relational(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        Shift(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        Additive(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        Multiplicative(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s, var Operator op): Node*;
        Prefix(ParsingContext* ctx, var Span s, var Operator op): Node*;
        Postfix(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
        Primary(ParsingContext* ctx): Node*;
        SizeOfExpr(ParsingContext* ctx): Node*;
        TypeNameExpr(ParsingContext* ctx): Node*;
        TypeIdExpr(ParsingContext* ctx): Node*;
        CastExpr(ParsingContext* ctx): Node*;
        ConstructExpr(ParsingContext* ctx): Node*;
        NewExpr(ParsingContext* ctx): Node*;
        ArgumentList(ParsingContext* ctx, Node* node);
        ExpressionList(ParsingContext* ctx, Node* node);
        InvokeExpr(ParsingContext* ctx, var std::unique_ptr<Node> expr, var Span s): Node*;
    }
    grammar Function
    {
        Function(ParsingContext* ctx, var std::unique_ptr<FunctionNode> fun, var Span s, var Span specifierSpan, var Span groupIdSpan, var std::unique_ptr<Attributes> attributes): FunctionNode*;
        FunctionGroupId(ParsingContext* ctx, var std::unique_ptr<IdentifierNode> id): std::u32string;
        OperatorFunctionGroupId(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr): std::u32string;
    }
    grammar Identifier
    {
        Identifier: IdentifierNode*;
        QualifiedId: IdentifierNode*;
    }
    grammar Interface
    {
        Interface(ParsingContext* ctx, var std::unique_ptr<Attributes> attributes, var Span specifierSpan, var Span beginBraceSpan, var Span endBraceSpan): InterfaceNode*;
        InterfaceContent(ParsingContext* ctx, InterfaceNode* intf);
        InterfaceMemFun(ParsingContext* ctx, var std::unique_ptr<MemberFunctionNode> memFun, var std::unique_ptr<Attributes> attributes): Node*;
        InterfaceFunctionGroupId(var std::unique_ptr<IdentifierNode> id): std::u32string;
    }
    grammar Json
    {
        Value: JsonValue*;
        String: JsonString*;
        Number: JsonNumber*;
        Object(var std::unique_ptr<JsonString> js, var std::unique_ptr<JsonValue> jv): JsonObject*;
        Array(var std::unique_ptr<JsonValue> item): JsonArray*;
    }
    grammar Keyword
    {
        Keyword;
        PPKeyword;
        S;
    }
    grammar Literal
    {
        Literal(ParsingContext* ctx): LiteralNode*;
        BooleanLiteral: LiteralNode*;
        FloatingLiteral(var Span s): LiteralNode*;
        FloatingLiteralValue: double;
        FractionalFloatingLiteral;
        ExponentFloatingLiteral;
        ExponentPart;
        IntegerLiteral(var Span s): LiteralNode*;
        IntegerLiteralValue: uint64_t;
        HexIntegerLiteral: uint64_t;
        DecIntegerLiteral: uint64_t;
        CharLiteral(var char32_t litValue): LiteralNode*;
        StringLiteral(var std::u32string s): LiteralNode*;
        NullLiteral: LiteralNode*;
        ArrayLiteral(ParsingContext* ctx): ArrayLiteralNode*;
        StructuredLiteral(ParsingContext* ctx): StructuredLiteralNode*;
        CharEscape: char32_t;
        DecDigitSequence: uint64_t;
        HexDigitSequence: uint64_t;
        HexDigit4: uint16_t;
        HexDigit8: uint32_t;
        OctalDigitSequence: uint64_t;
        Sign;
    }
    grammar Parameter
    {
        ParameterList(ParsingContext* ctx, Node* owner);
        Parameter(ParsingContext* ctx): ParameterNode*;
    }
    grammar Project
    {
        Project(std::string config, BackEnd backend): cmajor::ast::Project*;
        Declaration: ProjectDeclaration*;
        ReferenceDeclaration: ProjectDeclaration*;
        SourceFileDeclaration: ProjectDeclaration*;
        TextFileDeclaration: ProjectDeclaration*;
        TargetDeclaration: ProjectDeclaration*;
        Target: Target;
        FilePath: std::string;
    }
    grammar SourceToken
    {
        SourceTokens(SourceTokenFormatter* formatter);
        SourceToken(SourceTokenFormatter* formatter);
        Spaces: std::u32string;
        LineComment;
        Other: std::u32string;
    }
    grammar Specifier
    {
        Specifiers: Specifiers;
        Specifier: Specifiers;
    }
    grammar Statement
    {
        Statement(ParsingContext* ctx): StatementNode*;
        LabelId: std::u32string;
        Label(var std::u32string label): LabelNode*;
        LabeledStatement(ParsingContext* ctx): StatementNode*;
        ControlStatement(ParsingContext* ctx): StatementNode*;
        CompoundStatement(ParsingContext* ctx): CompoundStatementNode*;
        ReturnStatement(ParsingContext* ctx): StatementNode*;
        IfStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan, var Span elseSpan): IfStatementNode*;
        WhileStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan): WhileStatementNode*;
        DoStatement(ParsingContext* ctx, var Span whileSpan, var Span leftParenSpan, var Span rightParenSpan): DoStatementNode*;
        ForStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan): ForStatementNode*;
        ForInitStatement(ParsingContext* ctx): StatementNode*;
        ForLoopStatementExpr(ParsingContext* ctx): StatementNode*;
        RangeForStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan, var Span colonSpan): RangeForStatementNode*;
        BreakStatement(ParsingContext* ctx): StatementNode*;
        ContinueStatement(ParsingContext* ctx): StatementNode*;
        GotoStatement(ParsingContext* ctx): StatementNode*;
        SwitchStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan, var Span beginBraceSpan, var Span endBraceSpan): SwitchStatementNode*;
        CaseStatement(ParsingContext* ctx, var std::unique_ptr<CaseStatementNode> caseS, var Span caseSpan): CaseStatementNode*;
        DefaultStatement(ParsingContext* ctx): DefaultStatementNode*;
        GotoCaseStatement(ParsingContext* ctx): StatementNode*;
        GotoDefaultStatement(ParsingContext* ctx): StatementNode*;
        AssignmentStatementExpr(ParsingContext* ctx, var std::unique_ptr<Node> targetExpr): StatementNode*;
        AssignmentStatement(ParsingContext* ctx): StatementNode*;
        ConstructionStatement(ParsingContext* ctx): ConstructionStatementNode*;
        DeleteStatement(ParsingContext* ctx): StatementNode*;
        DestroyStatement(ParsingContext* ctx): StatementNode*;
        ExpressionStatement(ParsingContext* ctx, var std::unique_ptr<Node> expr): StatementNode*;
        EmptyStatement(ParsingContext* ctx): StatementNode*;
        ThrowStatement(ParsingContext* ctx): StatementNode*;
        TryStatement(ParsingContext* ctx): TryStatementNode*;
        Catch(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan): CatchNode*;
        AssertStatement(ParsingContext* ctx): StatementNode*;
        ConditionalCompilationStatement(ParsingContext* ctx, var Span leftParenSpan, var Span rightParenSpan, var Span keywordSpan): ConditionalCompilationStatementNode*;
        ConditionalCompilationExpression: ConditionalCompilationExpressionNode*;
        ConditionalCompilationDisjunction(var Span s): ConditionalCompilationExpressionNode*;
        ConditionalCompilationConjunction(var Span s): ConditionalCompilationExpressionNode*;
        ConditionalCompilationPrefix: ConditionalCompilationExpressionNode*;
        ConditionalCompilationPrimary: ConditionalCompilationExpressionNode*;
        Symbol: std::u32string;
    }
    grammar Template
    {
        TemplateId(ParsingContext* ctx, var std::unique_ptr<TemplateIdNode> templateId): Node*;
        TemplateParameter(ParsingContext* ctx): TemplateParameterNode*;
        TemplateParameterList(ParsingContext* ctx, Node* owner);
    }
    grammar Typedef
    {
        Typedef(ParsingContext* ctx): TypedefNode*;
    }
    grammar TypeExpr
    {
        TypeExpr(ParsingContext* ctx): Node*;
        PrefixTypeExpr(ParsingContext* ctx): Node*;
        PostfixTypeExpr(ParsingContext* ctx, var std::unique_ptr<Node> typeExpr, var Span s): Node*;
        PrimaryTypeExpr(ParsingContext* ctx): Node*;
    }
}
