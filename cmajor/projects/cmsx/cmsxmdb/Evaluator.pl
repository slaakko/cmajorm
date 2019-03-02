namespace cmsx.debug
{
    grammar ExprGrammar
    {
        Expression : Node*;
        WeakOperator : Operator;
        StrongOperator : Operator;
        UnaryOperator : Operator;
        Format : ResultFormat;
        FormatExpression : Node*;
        BinaryExpression : Node*;
        Term : Node*;
        UnaryExpression : Node*;
        PrimaryExpression : Node*;
        Segment : SegmentNode*;
        RegNumber : RegNumberNode*;
        DecNumber : DecNumberNode*;
        HexNumber : HexNumberNode*;
        Symbol : SymbolNode*;
    }
    grammar StatementGrammar
    {
        Statement : Node*;
        PrintStatement : Node*;
        TraceStatement : Node*;
        Spaces;
    }
}
