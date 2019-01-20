namespace cmajor.syntax
{
    grammar Composite
    {
        Alternative(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Sequence(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Difference(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        ExclusiveOr(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Intersection(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        List(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Postfix(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
    }
    grammar Rule
    {
        Rule(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Rule*;
        RuleHeader(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Rule*;
        RuleBody(cmajor::parsing::Rule* rule);
    }
    grammar Element
    {
        RuleLink(cmajor::parsing::Grammar* grammar);
        Signature(cmajor::parsing::Rule* rule);
        ParameterList(cmajor::parsing::Rule* rule);
        Variable(cmajor::parsing::Rule* rule);
        Parameter(cmajor::parsing::Rule* rule);
        ReturnType(cmajor::parsing::Rule* rule);
        Keyword;
        Identifier: std::u32string;
        QualifiedId: std::u32string;
        StringArray(std::vector<std::u32string>* array);
    }
    grammar GrammarGrammar
    {
        Grammar(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Grammar*;
        GrammarContent(cmajor::parsing::Grammar* grammar);
        StartClause(cmajor::parsing::Grammar* grammar);
        SkipClause(cmajor::parsing::Grammar* grammar);
    }
    grammar Primitive
    {
        Primitive: cmajor::parsing::Parser*;
        Char: cmajor::parsing::Parser*;
        String: cmajor::parsing::Parser*;
        CharSet(var bool inverse, var std::u32string s): cmajor::parsing::Parser*;
        CharSetRange: std::u32string;
        CharSetChar: char32_t;
        Keyword: cmajor::parsing::Parser*;
        KeywordBody: cmajor::parsing::Parser*;
        KeywordList: cmajor::parsing::Parser*;
        KeywordListBody(var std::vector<std::u32string> keywords): cmajor::parsing::Parser*;
        Range: cmajor::parsing::Parser*;
        CodePoint: uint32_t;
        Empty: cmajor::parsing::Parser*;
        Space: cmajor::parsing::Parser*;
        AnyChar: cmajor::parsing::Parser*;
        Letter: cmajor::parsing::Parser*;
        Digit: cmajor::parsing::Parser*;
        HexDigit: cmajor::parsing::Parser*;
        UpperLetter: cmajor::parsing::Parser*;
        LowerLetter: cmajor::parsing::Parser*;
        TitleLetter: cmajor::parsing::Parser*;
        ModifierLetter: cmajor::parsing::Parser*;
        OtherLetter: cmajor::parsing::Parser*;
        CasedLetter: cmajor::parsing::Parser*;
        Mark: cmajor::parsing::Parser*;
        NonspacingMark: cmajor::parsing::Parser*;
        SpacingMark: cmajor::parsing::Parser*;
        EnclosingMark: cmajor::parsing::Parser*;
        Number: cmajor::parsing::Parser*;
        DecimalNumber: cmajor::parsing::Parser*;
        LetterNumber: cmajor::parsing::Parser*;
        OtherNumber: cmajor::parsing::Parser*;
        Punctuation: cmajor::parsing::Parser*;
        ConnectorPunctuation: cmajor::parsing::Parser*;
        DashPunctuation: cmajor::parsing::Parser*;
        OpenPunctuation: cmajor::parsing::Parser*;
        ClosePunctuation: cmajor::parsing::Parser*;
        InitialPunctuation: cmajor::parsing::Parser*;
        FinalPunctuation: cmajor::parsing::Parser*;
        OtherPunctuation: cmajor::parsing::Parser*;
        Symbol: cmajor::parsing::Parser*;
        MathSymbol: cmajor::parsing::Parser*;
        CurrencySymbol: cmajor::parsing::Parser*;
        ModifierSymbol: cmajor::parsing::Parser*;
        OtherSymbol: cmajor::parsing::Parser*;
        Separator: cmajor::parsing::Parser*;
        SpaceSeparator: cmajor::parsing::Parser*;
        LineSeparator: cmajor::parsing::Parser*;
        ParagraphSeparator: cmajor::parsing::Parser*;
        Other: cmajor::parsing::Parser*;
        Control: cmajor::parsing::Parser*;
        Format: cmajor::parsing::Parser*;
        Surrogate: cmajor::parsing::Parser*;
        PrivateUse: cmajor::parsing::Parser*;
        Unassigned: cmajor::parsing::Parser*;
        Graphic: cmajor::parsing::Parser*;
        BaseChar: cmajor::parsing::Parser*;
        Alphabetic: cmajor::parsing::Parser*;
        IdStart: cmajor::parsing::Parser*;
        IdCont: cmajor::parsing::Parser*;
    }
    grammar ParserFile
    {
        ParserFile(int id_, cmajor::parsing::ParsingDomain* parsingDomain_): ParserFileContent*;
        IncludeDirectives(ParserFileContent* parserFileContent);
        IncludeDirective(ParserFileContent* parserFileContent);
        FileAttribute: std::u32string;
        IncludeFileName: std::u32string;
        NamespaceContent(ParserFileContent* parserFileContent);
        Namespace(ParserFileContent* parserFileContent);
    }
    grammar LibraryFile
    {
        LibraryFile(cmajor::parsing::ParsingDomain* parsingDomain);
        NamespaceContent(cmajor::parsing::ParsingDomain* parsingDomain);
        Namespace(cmajor::parsing::ParsingDomain* parsingDomain);
        Grammar(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Grammar*;
        GrammarContent(cmajor::parsing::Grammar* grammar);
        Rule(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Rule*;
    }
    grammar Primary
    {
        Primary(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        RuleCall: cmajor::parsing::Parser*;
        Nonterminal: std::u32string;
        Alias: std::u32string;
        Grouping(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Token(cmajor::parsing::Scope* enclosingScope): cmajor::parsing::Parser*;
        Expectation(cmajor::parsing::Parser* child): cmajor::parsing::Parser*;
        Action(cmajor::parsing::Parser* child): cmajor::parsing::Parser*;
    }
    grammar ProjectFile
    {
        ProjectFile: Project*;
        ProjectFileContent(Project* project);
        Source: std::u32string;
        Reference: std::u32string;
        FilePath: std::u32string;
    }
}
