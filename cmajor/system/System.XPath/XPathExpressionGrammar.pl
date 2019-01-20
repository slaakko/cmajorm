namespace System.XPath
{
    grammar XPathExpressionGrammar
    {
        Expr : XPathExpr*;
        OrExpr : XPathExpr*;
        AndExpr : XPathExpr*;
        EqualityExpr(var Operator op) : XPathExpr*;
        RelationalExpr(var Operator op) : XPathExpr*;
        AdditiveExpr(var Operator op) : XPathExpr*;
        MultiplicativeExpr(var Operator op) : XPathExpr*;
        UnaryExpr : XPathExpr*;
        UnionExpr : XPathExpr*;
        PathExpr(var Operator op, var UniquePtr<XPathExpr> fnc) : XPathExpr*;
        FilterExpr : XPathExpr*;
        LocationPath : XPathExpr*;
        AbsoluteLocationPath : XPathExpr*;
        RelativeLocationPath(var Operator op) : XPathExpr*;
        AbbreviatedAbsoluteLocationPath : XPathExpr*;
        Step : XPathLocationStepExpr*;
        AbbreviatedStep : XPathLocationStepExpr*;
        AxisSpecifier : Axis;
        AxisName : Axis;
        AbbreviatedAxisSpecifier : Axis;
        Predicate : XPathExpr*;
        PredicateExpr : XPathExpr*;
        NodeTest : XPathNodeTestExpr*;
        NameTest : XPathNodeTestExpr*;
        NodeType : XPathNodeTestExpr*;
        PrimaryExpr : XPathExpr*;
        VariableReference : XPathExpr*;
        Literal : XPathExpr*;
        Number : XPathExpr*;
        Digits;
        FunctionCall : XPathFunctionCall*;
        Argument : XPathExpr*;
        FunctionName(var UniquePtr<XPathExpr> nodeType) : ustring;
        QName : ustring;
        PrefixedName : ustring;
        UnprefixedName : ustring;
        Prefix : ustring;
        LocalPart : ustring;
        NCName : ustring;
    }
}
