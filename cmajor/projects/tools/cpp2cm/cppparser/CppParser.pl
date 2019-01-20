namespace cppparser
{
    grammar ConstantExpressionGrammar
    {
    }
    grammar DeclarationGrammar
    {
        Declarations(ParsingContext* ctx, CompositeNode<Node>* parent);
        Declaration(ParsingContext* ctx) : Node*;
        NamespaceDefinition(ParsingContext* ctx) : NamespaceNode*;
        NamedNamespaceDefinition(ParsingContext* ctx) : NamespaceNode*;
        UnnamedNamespaceDefinition(ParsingContext* ctx) : NamespaceNode*;
        BlockDeclaration(ParsingContext* ctx) : Node*;
        SimpleDeclaration(ParsingContext* ctx) : SimpleDeclarationNode*;
        DeclSpecifiers : Specifiers;
        DeclSpecifier : Specifiers;
        StorageClassSpecifier : Specifiers;
        FunctionSpecifier : Specifiers;
        AliasDeclaration(ParsingContext* ctx) : UsingTypeDeclarationNode*;
        UsingDirective(ParsingContext* ctx) : UsingDirectiveNode*;
        UsingDeclaration(ParsingContext* ctx) : UsingDeclarationNode*;
        StaticAssertDeclaration(ParsingContext* ctx) : StaticAssertDeclarationNode*;
        TypedefDeclaration(ParsingContext* ctx) : TypedefDeclarationNode*;
        LinkageSpecification(ParsingContext* ctx) : LinkageSpecificationNode*;
    }
    grammar SimpleTypeGrammar
    {
        SimpleType(var SimpleTypeSpecifiers simpleTypeSpecifiers) : SimpleTypeNode*;
        SimpleTypeSpecifier : SimpleTypeSpecifiers;
    }
    grammar EnumerationGrammar
    {
        EnumDeclaration(ParsingContext* ctx) : EnumTypeNode*;
        OpaqueEnumDeclaration(ParsingContext* ctx) : EnumTypeNode*;
        EnumSpecifier(ParsingContext* ctx) : EnumTypeNode*;
        EnumHead(ParsingContext* ctx) : EnumTypeNode*;
        EnumKey : EnumKey;
        EnumName(ParsingContext* ctx) : Node*;
        EnumBase(ParsingContext* ctx) : Node*;
        Enumerators(ParsingContext* ctx, EnumTypeNode* enumType);
        EnumeratorDefinition(ParsingContext* ctx) : EnumeratorNode*;
        Enumerator : IdentifierNode*;
    }
    grammar FunctionGrammar
    {
        FunctionDefinition(ParsingContext* ctx, var Specifiers specifiers, var UniquePtr<Node> returnTypeExpr, var UniquePtr<Node> declarator, var UniquePtr<CompoundStatementNode> body) : FunctionNode*;
        ParameterList(ParsingContext* ctx, var UniquePtr<ParameterListNode> node) : ParameterListNode*;
        ParameterDeclaration(ParsingContext* ctx) : ParameterNode*;
        FunctionBody(ParsingContext* ctx) : CompoundStatementNode*;
    }
    grammar SourceFileGrammar
    {
        SourceFile(var ParsingContext ctx) : SourceFileNode*;
    }
    grammar IdentifierGrammar
    {
        Identifier : IdentifierNode*;
        Id(ParsingContext* ctx) : Node*;
        NestedNameSpecifier(ParsingContext* ctx) : Node*;
        QualifiedId(ParsingContext* ctx) : Node*;
    }
    grammar TypeExprGrammar
    {
        TypeExpr(ParsingContext* ctx) : Node*;
        CVSpecifierSequence : CVSpecifiers;
        CVSpecifier : CVSpecifiers;
        PrefixTypeExpr(ParsingContext* ctx) : Node*;
        PostfixTypeExpr(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        PrimaryTypeExpr(ParsingContext* ctx) : Node*;
    }
    grammar TemplateGrammar
    {
        TemplateDeclaration(ParsingContext* ctx) : TemplateDeclarationNode*;
        TemplateParameterList(ParsingContext* ctx, TemplateDeclarationNode* templateDeclaration);
        RuleTemplateParameter(ParsingContext* ctx) : Node*;
        TemplateParameter(ParsingContext* ctx) : Node*;
        TypeParameter(ParsingContext* ctx) : TemplateParameterNode*;
        SimpleTemplateId(ParsingContext* ctx, var UniquePtr<TemplateIdNode> templateIdNode) : TemplateIdNode*;
        TemplateId(ParsingContext* ctx) : TemplateIdNode*;
        TemplateName : IdentifierNode*;
        TemplateArgumentList(ParsingContext* ctx, TemplateIdNode* templateIdNode);
        TemplateArgument(ParsingContext* ctx) : Node*;
        ExplicitInstantiation(ParsingContext* ctx) : ExplicitInstantiationNode*;
        ExplicitSpecialization(ParsingContext* ctx) : ExplicitSpecializationNode*;
    }
    grammar DeclaratorGrammar
    {
        InitDeclarator(ParsingContext* ctx) : Node*;
        Declarator(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        Initializer(ParsingContext* ctx) : Node*;
        BraceOrEqualInitializer(ParsingContext* ctx) : Node*;
        BracedInitializerList(ParsingContext* ctx, var UniquePtr<BracedInitializerListNode> node) : BracedInitializerListNode*;
        InitializerClause(ParsingContext* ctx) : Node*;
        InitializerList(ParsingContext* ctx, var UniquePtr<InitializerListNode> node) : InitializerListNode*;
    }
    grammar ClassGrammar
    {
        ClassDeclaration(ParsingContext* ctx) : ClassNode*;
        ForwardClassDeclaration(ParsingContext* ctx, var UniquePtr<Node> className) : ForwardClassDeclarationNode*;
        ClassSpecifier(ParsingContext* ctx, var Specifiers defaultAccess) : ClassNode*;
        MemberSpecifications(ParsingContext* ctx, ClassNode* classNode);
        MemberSpecification(ParsingContext* ctx, ClassNode* classNode);
        MemberDeclaration(ParsingContext* ctx, Specifiers accessSpecifier, var Specifiers specifiers, var bool functionDeclarator) : Node*;
        SpecialMemberFunctionDeclaration(ParsingContext* ctx, var UniquePtr<Node> declarator) : MemberDeclarationNode*;
        ClassHead(ParsingContext* ctx) : ClassNode*;
        ClassKey : ClassKey;
        ClassName(ParsingContext* ctx) : Node*;
        ClassVirtSpecifiers : Specifiers;
        ClassVirtSpecifier : Specifiers;
        BaseClause(ParsingContext* ctx) : BaseClassListNode*;
        BaseClassSpecifierList(ParsingContext* ctx, BaseClassListNode* baseClassList);
        BaseClassSpecifier(ParsingContext* ctx) : BaseClassNode*;
        BaseSpecifiers : Specifiers;
        BaseSpecifier : Specifiers;
        AccessSpecifier : Specifiers;
        VirtPureSpecifiers(bool functionMember) : Specifiers;
        VirtSpecifier(bool functionMember) : Specifiers;
        PureSpecifier(bool functionMember) : Specifiers;
        SpecialMemberFunctionDefinition(ParsingContext* ctx, var UniquePtr<Node> declarator, var UniquePtr<Node> ctorInitializer, var Specifiers defaultOrDeleteSpecifier) : SpecialMemberFunctionNode*;
        CtorInitializer(ParsingContext* ctx) : CtorInitializerNode*;
        MemberInitializerList(ParsingContext* ctx, CtorInitializerNode* ctorInitializer);
        MemberInitializer(ParsingContext* ctx) : MemberInitializerNode*;
        MemberInitializerId(ParsingContext* ctx) : Node*;
    }
    grammar LiteralGrammar
    {
        Literal : LiteralNode*;
        FloatingLiteral(var double val) : FloatingLiteralNode*;
        FractionalConstant;
        ExponentPart;
        FloatingSuffix;
        IntegerLiteral(var ulong val) : IntegerLiteralNode*;
        DecimalLiteral : ulong;
        OctalLiteral : ulong;
        HexadecimalLiteral : ulong;
        IntegerSuffix;
        CharacterLiteral(var uchar val) : CharacterLiteralNode*;
        CChar : uchar;
        HexDigit4 : uint;
        HexDigit8 : uint;
        StringLiteral(var ustring val) : StringLiteralNode*;
        EncodingPrefix;
        SChar : uchar;
        BooleanLiteral : BooleanLiteralNode*;
        PointerLiteral : PointerLiteralNode*;
    }
    grammar ExpressionGrammar
    {
        ExpressionList(ParsingContext* ctx, var UniquePtr<ExpressionListNode> node) : ExpressionListNode*;
        PossiblyEmptyArgumentList(ParsingContext* ctx) : ExpressionListNode*;
        Expression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        ConstantExpression(ParsingContext* ctx) : Node*;
        AssignmentExpression(ParsingContext* ctx) : Node*;
        ConcreteAssignmentExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        AssignmentOperator : Operator;
        ConditionalExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        ThrowExpression(ParsingContext* ctx) : Node*;
        LogicalOrExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        LogicalAndExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        InclusiveOrExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        ExclusiveOrExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        AndExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        EqualityExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        RelationalExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        ShiftExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        AdditiveExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        MultiplicativeExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        PMExpression(ParsingContext* ctx, var UniquePtr<Node> node, var Operator op) : Node*;
        CastExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        UnaryExpression(ParsingContext* ctx) : Node*;
        UnaryOperator : Operator;
        NewExpression(ParsingContext* ctx) : NewExpressionNode*;
        NewPlacement(ParsingContext* ctx) : ExpressionListNode*;
        NewTypeId(ParsingContext* ctx) : Node*;
        NewDeclarator(ParsingContext* ctx) : Node*;
        NewInitializer(ParsingContext* ctx) : ExpressionListNode*;
        DeleteExpression(ParsingContext* ctx, var bool arrayDelete) : DeleteExpressionNode*;
        PostfixExpression(ParsingContext* ctx, var UniquePtr<Node> node) : Node*;
        PrimaryExpression(ParsingContext* ctx) : Node*;
        CppCastExpression(ParsingContext* ctx, var CppCast cast_) : Node*;
        DefinedExpr : Node*;
        TypeIdExpression(ParsingContext* ctx) : Node*;
        IdExpression(ParsingContext* ctx) : Node*;
        UnqualifiedIdExpr(ParsingContext* ctx) : Node*;
        QualifiedIdExpr(ParsingContext* ctx) : Node*;
        OperatorFunctionId : OperatorFunctionIdNode*;
        Operator : Operator;
        ConversionFunctionId(ParsingContext* ctx) : ConversionFunctionIdNode*;
        LambdaExpression(ParsingContext* ctx) : LambdaExpressionNode*;
        LambdaCaptures(LambdaExpressionNode* lambdaExpression);
        CaptureDefault : Node*;
        Captures(LambdaExpressionNode* lambdaExpression);
        Capture : Node*;
    }
    grammar KeywordGrammar
    {
        Keyword;
    }
    grammar StatementGrammar
    {
        Statement(ParsingContext* ctx) : StatementNode*;
        LabeledStatement(ParsingContext* ctx) : StatementNode*;
        ExpressionStatement(ParsingContext* ctx) : StatementNode*;
        CompoundStatement(ParsingContext* ctx) : CompoundStatementNode*;
        SelectionStatement(ParsingContext* ctx) : StatementNode*;
        Condition(ParsingContext* ctx, var UniquePtr<Node> typeExpr, var UniquePtr<Node> declarator) : Node*;
        IterationStatement(ParsingContext* ctx) : StatementNode*;
        ForInitialization(ParsingContext* ctx) : Node*;
        ForRangeDeclaration(ParsingContext* ctx) : SimpleDeclarationNode*;
        ForRangeInitializer(ParsingContext* ctx) : Node*;
        JumpStatement(ParsingContext* ctx) : StatementNode*;
        DeclarationStatement(ParsingContext* ctx) : StatementNode*;
        TryBlock(ParsingContext* ctx) : TryStatementNode*;
        HandlerSequence(ParsingContext* ctx, TryStatementNode* tryStatement);
        Handler(ParsingContext* ctx) : ExceptionHandlerNode*;
        ExceptionDeclaration(ParsingContext* ctx) : Node*;
    }
}
