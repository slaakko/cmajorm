namespace Syntax
{
    grammar PrimitiveGrammar
    {
        Primitive : System.Text.Parsing.Parser*;
        Char : System.Text.Parsing.Parser*;
        String : System.Text.Parsing.Parser*;
        CharSet(var bool inverse, var ustring s) : System.Text.Parsing.Parser*;
        CharSetRange : ustring;
        CharSetChar : uchar;
        Keyword : System.Text.Parsing.Parser*;
        KeywordBody(var UniquePtr<Identifier> qid) : System.Text.Parsing.Parser*;
        KeywordList : System.Text.Parsing.Parser*;
        KeywordListBody(var UniquePtr<Identifier> qid, var List<ustring> keywords) : System.Text.Parsing.Parser*;
        Range : System.Text.Parsing.Parser*;
        CodePoint : uint;
        Empty : System.Text.Parsing.Parser*;
        Space : System.Text.Parsing.Parser*;
        AnyChar : System.Text.Parsing.Parser*;
        Letter : System.Text.Parsing.Parser*;
        Digit : System.Text.Parsing.Parser*;
        HexDigit : System.Text.Parsing.Parser*;
        UpperLetter : System.Text.Parsing.Parser*;
        LowerLetter : System.Text.Parsing.Parser*;
        TitleLetter : System.Text.Parsing.Parser*;
        ModifierLetter : System.Text.Parsing.Parser*;
        OtherLetter : System.Text.Parsing.Parser*;
        CasedLetter : System.Text.Parsing.Parser*;
        Mark : System.Text.Parsing.Parser*;
        NonspacingMark : System.Text.Parsing.Parser*;
        SpacingMark : System.Text.Parsing.Parser*;
        EnclosingMark : System.Text.Parsing.Parser*;
        Number : System.Text.Parsing.Parser*;
        DecimalNumber : System.Text.Parsing.Parser*;
        LetterNumber : System.Text.Parsing.Parser*;
        OtherNumber : System.Text.Parsing.Parser*;
        Punctuation : System.Text.Parsing.Parser*;
        ConnectorPunctuation : System.Text.Parsing.Parser*;
        DashPunctuation : System.Text.Parsing.Parser*;
        OpenPunctuation : System.Text.Parsing.Parser*;
        ClosePunctuation : System.Text.Parsing.Parser*;
        InitialPunctuation : System.Text.Parsing.Parser*;
        FinalPunctuation : System.Text.Parsing.Parser*;
        OtherPunctuation : System.Text.Parsing.Parser*;
        Symbol : System.Text.Parsing.Parser*;
        MathSymbol : System.Text.Parsing.Parser*;
        CurrencySymbol : System.Text.Parsing.Parser*;
        ModifierSymbol : System.Text.Parsing.Parser*;
        OtherSymbol : System.Text.Parsing.Parser*;
        Separator : System.Text.Parsing.Parser*;
        SpaceSeparator : System.Text.Parsing.Parser*;
        LineSeparator : System.Text.Parsing.Parser*;
        ParagraphSeparator : System.Text.Parsing.Parser*;
        Other : System.Text.Parsing.Parser*;
        Control : System.Text.Parsing.Parser*;
        Format : System.Text.Parsing.Parser*;
        Surrogate : System.Text.Parsing.Parser*;
        PrivateUse : System.Text.Parsing.Parser*;
        Unassigned : System.Text.Parsing.Parser*;
        Graphic : System.Text.Parsing.Parser*;
        BaseChar : System.Text.Parsing.Parser*;
        Alphabetic : System.Text.Parsing.Parser*;
        IdStart : System.Text.Parsing.Parser*;
        IdCont : System.Text.Parsing.Parser*;
    }
    grammar GrammarGrammar
    {
        Grammar(ParsingContext* ctx, System.Text.Parsing.Scope* enclosingScope) : System.Text.Parsing.Grammar*;
        GrammarContent(ParsingContext* ctx, System.Text.Parsing.Grammar* grammar);
        StartClause(System.Text.Parsing.Grammar* grammar);
        SkipClause(System.Text.Parsing.Grammar* grammar);
    }
    grammar ParserFileGrammar
    {
        ParserFile(ParsingContext* ctx, int id, System.Text.Parsing.ParsingDomain* parsingDomain) : ParserFileContent*;
        NamespaceContent(ParsingContext* ctx, ParserFileContent* parserFileContent);
        Namespace(ParsingContext* ctx, ParserFileContent* parserFileContent);
    }
    grammar PrimaryGrammar
    {
        Primary(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        RuleCall(ParsingContext* ctx, var ArgumentCollector argumentCollector) : System.Text.Parsing.Parser*;
        Nonterminal : ustring;
        Alias : ustring;
        Grouping(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Token(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Expectation(System.Text.Parsing.Parser* child) : System.Text.Parsing.Parser*;
        Action(ParsingContext* ctx, System.Text.Parsing.Parser* child) : System.Text.Parsing.ActionParser*;
    }
    grammar ProjectFileGrammar
    {
        ProjectFile : Project*;
        ProjectFileContent(Project* project);
        Source : string;
        Reference : string;
        FilePath : string;
    }
    grammar CompositeGrammar
    {
        Alternative(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Sequence(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Difference(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        ExclusiveOr(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Intersection(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        List(ParsingContext* ctx) : System.Text.Parsing.Parser*;
        Postfix(ParsingContext* ctx) : System.Text.Parsing.Parser*;
    }
    grammar LibraryFileGrammar
    {
        LibraryFile(ParsingContext* ctx, System.Text.Parsing.ParsingDomain* parsingDomain);
        NamespaceContent(ParsingContext* ctx, System.Text.Parsing.ParsingDomain* parsingDomain);
        Namespace(ParsingContext* ctx, System.Text.Parsing.ParsingDomain* parsingDomain);
        Grammar(ParsingContext* ctx, System.Text.Parsing.Scope* enclosingScope) : System.Text.Parsing.Grammar*;
        GrammarContent(ParsingContext* ctx, System.Text.Parsing.Grammar* grammar);
        Rule(ParsingContext* ctx, System.Text.Parsing.Scope* enclosingScope) : System.Text.Parsing.RuleParser*;
    }
    grammar RuleGrammar
    {
        Rule(ParsingContext* ctx, System.Text.Parsing.Scope* enclosingScope) : System.Text.Parsing.RuleParser*;
        RuleHeader(ParsingContext* ctx, System.Text.Parsing.Scope* enclosingScope) : System.Text.Parsing.RuleParser*;
        RuleBody(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule);
    }
    grammar ElementGrammar
    {
        RuleLink(System.Text.Parsing.Grammar* grammar);
        Signature(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule);
        ParameterList(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule);
        Variable(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule, var UniquePtr<DomObject> type, var UniquePtr<DomObject> id);
        Parameter(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule, var UniquePtr<DomObject> type, var UniquePtr<DomObject> id);
        ReturnType(ParsingContext* ctx, System.Text.Parsing.RuleParser* rule, var UniquePtr<DomObject> type);
        Keyword;
        Identifier : ustring;
        QualifiedId : ustring;
        StringArray(List<ustring>* array);
    }
}
