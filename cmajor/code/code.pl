namespace cmajor.code
{
    grammar Declaration
    {
        BlockDeclaration: cmajor::codedom::CppObject*;
        SimpleDeclaration(var std::unique_ptr<SimpleDeclaration> sd): cmajor::codedom::SimpleDeclaration*;
        DeclSpecifierSeq(cmajor::codedom::SimpleDeclaration* declaration);
        DeclSpecifier: cmajor::codedom::DeclSpecifier*;
        StorageClassSpecifier: cmajor::codedom::StorageClassSpecifier*;
        TypeSpecifier: cmajor::codedom::TypeSpecifier*;
        SimpleTypeSpecifier: cmajor::codedom::TypeSpecifier*;
        TypeName: cmajor::codedom::TypeName*;
        TemplateArgumentList(cmajor::codedom::TypeName* typeName);
        TemplateArgument: cmajor::codedom::CppObject*;
        Typedef: cmajor::codedom::DeclSpecifier*;
        CVQualifier: cmajor::codedom::TypeSpecifier*;
        NamespaceAliasDefinition: cmajor::codedom::UsingObject*;
        UsingDeclaration: cmajor::codedom::UsingObject*;
        UsingDirective: cmajor::codedom::UsingObject*;
    }
    grammar Statement
    {
        Statement: cmajor::codedom::Statement*;
        LabeledStatement: cmajor::codedom::Statement*;
        Label: std::u32string;
        EmptyStatement: cmajor::codedom::Statement*;
        ExpressionStatement: cmajor::codedom::Statement*;
        CompoundStatement(var std::unique_ptr<CompoundStatement> cs): cmajor::codedom::CompoundStatement*;
        SelectionStatement: cmajor::codedom::Statement*;
        IfStatement: cmajor::codedom::Statement*;
        SwitchStatement: cmajor::codedom::Statement*;
        IterationStatement: cmajor::codedom::Statement*;
        WhileStatement: cmajor::codedom::Statement*;
        DoStatement: cmajor::codedom::Statement*;
        ForStatement: cmajor::codedom::Statement*;
        ForInitStatement: cmajor::codedom::CppObject*;
        JumpStatement: cmajor::codedom::Statement*;
        BreakStatement: cmajor::codedom::Statement*;
        ContinueStatement: cmajor::codedom::Statement*;
        ReturnStatement: cmajor::codedom::Statement*;
        GotoStatement: cmajor::codedom::Statement*;
        GotoTarget: std::u32string;
        DeclarationStatement: cmajor::codedom::Statement*;
        Condition(var std::unique_ptr<TypeId> ti): cmajor::codedom::CppObject*;
        TryStatement: cmajor::codedom::TryStatement*;
        HandlerSeq(TryStatement* st);
        Handler: cmajor::codedom::Handler*;
        ExceptionDeclaration(var std::unique_ptr<ExceptionDeclaration> ed): cmajor::codedom::ExceptionDeclaration*;
    }
    grammar Declarator
    {
        InitDeclaratorList(var std::unique_ptr<InitDeclaratorList> idl): cmajor::codedom::InitDeclaratorList*;
        InitDeclarator: cmajor::codedom::InitDeclarator*;
        Declarator: std::u32string;
        DirectDeclarator(var std::unique_ptr<CppObject> o);
        DeclaratorId(var std::unique_ptr<CppObject> o);
        TypeId(var std::unique_ptr<TypeId> ti): cmajor::codedom::TypeId*;
        Type(var std::unique_ptr<Type> t): cmajor::codedom::Type*;
        TypeSpecifierSeq(cmajor::codedom::TypeId* typeId);
        AbstractDeclarator: std::u32string;
        DirectAbstractDeclarator(var std::unique_ptr<CppObject> o);
        PtrOperator;
        CVQualifierSeq(var std::unique_ptr<CppObject> o);
        Initializer: cmajor::codedom::Initializer*;
        InitializerClause: cmajor::codedom::AssignInit*;
        InitializerList(cmajor::codedom::AssignInit* init);
    }
    grammar Expression
    {
        Expression: cmajor::codedom::CppObject*;
        ConstantExpression: cmajor::codedom::CppObject*;
        AssignmentExpression(var std::unique_ptr<CppObject> lor): cmajor::codedom::CppObject*;
        AssingmentOp: Operator;
        ThrowExpression: cmajor::codedom::CppObject*;
        ConditionalExpression: cmajor::codedom::CppObject*;
        LogicalOrExpression: cmajor::codedom::CppObject*;
        LogicalAndExpression: cmajor::codedom::CppObject*;
        InclusiveOrExpression: cmajor::codedom::CppObject*;
        ExclusiveOrExpression: cmajor::codedom::CppObject*;
        AndExpression: cmajor::codedom::CppObject*;
        EqualityExpression: cmajor::codedom::CppObject*;
        EqOp: Operator;
        RelationalExpression: cmajor::codedom::CppObject*;
        RelOp: Operator;
        ShiftExpression: cmajor::codedom::CppObject*;
        ShiftOp: Operator;
        AdditiveExpression: cmajor::codedom::CppObject*;
        AddOp: Operator;
        MultiplicativeExpression: cmajor::codedom::CppObject*;
        MulOp: Operator;
        PmExpression: cmajor::codedom::CppObject*;
        PmOp: Operator;
        CastExpression(var std::unique_ptr<CppObject> ce, var std::unique_ptr<CppObject> ti): cmajor::codedom::CppObject*;
        UnaryExpression(var std::unique_ptr<CppObject> ue): cmajor::codedom::CppObject*;
        UnaryOperator: Operator;
        NewExpression(var bool global, var TypeId* typeId, var bool parens): cmajor::codedom::CppObject*;
        NewTypeId(var std::unique_ptr<TypeId> ti): cmajor::codedom::TypeId*;
        NewDeclarator: std::u32string;
        DirectNewDeclarator(var std::unique_ptr<CppObject> e);
        NewPlacement: std::vector<cmajor::codedom::CppObject*>;
        NewInitializer: std::vector<cmajor::codedom::CppObject*>;
        DeleteExpression(var bool global, var bool array): cmajor::codedom::CppObject*;
        PostfixExpression(var std::unique_ptr<CppObject> pe): cmajor::codedom::CppObject*;
        TypeSpecifierOrTypeName: cmajor::codedom::CppObject*;
        PostCastExpression: cmajor::codedom::CppObject*;
        ExpressionList: std::vector<cmajor::codedom::CppObject*>;
        PrimaryExpression(var std::unique_ptr<CppObject> pe): cmajor::codedom::CppObject*;
        IdExpression: cmajor::codedom::IdExpr*;
        OperatorFunctionId;
        Operator;
    }
    grammar Literal
    {
        Literal: cmajor::codedom::Literal*;
        IntegerLiteral: cmajor::codedom::Literal*;
        DecimalLiteral;
        OctalLiteral;
        OctalDigit;
        HexadecimalLiteral;
        IntegerSuffix;
        UnsignedSuffix;
        LongLongSuffix;
        LongSuffix;
        CharacterLiteral: cmajor::codedom::Literal*;
        NarrowCharacterLiteral;
        UniversalCharacterLiteral;
        WideCharacterLiteral;
        CCharSequence: std::u32string;
        CChar;
        EscapeSequence;
        SimpleEscapeSequence;
        OctalEscapeSequence;
        HexadecimalEscapeSequence;
        FloatingLiteral: cmajor::codedom::Literal*;
        FractionalConstant;
        DigitSequence;
        ExponentPart;
        Sign;
        FloatingSuffix;
        StringLiteral: cmajor::codedom::Literal*;
        EncodingPrefix;
        SCharSequence;
        SChar;
        BooleanLiteral: cmajor::codedom::Literal*;
        PointerLiteral: cmajor::codedom::Literal*;
    }
    grammar Identifier
    {
        Identifier: std::u32string;
        QualifiedId: std::u32string;
    }
    grammar Keyword
    {
        Keyword;
    }
}
