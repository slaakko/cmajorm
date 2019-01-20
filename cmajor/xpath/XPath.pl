namespace cmajor.xpath
{
    grammar XPath
    {
        Expr: XPathExpr*;
        OrExpr: XPathExpr*;
        AndExpr: XPathExpr*;
        EqualityExpr(var Operator op): XPathExpr*;
        RelationalExpr(var Operator op): XPathExpr*;
        AdditiveExpr(var Operator op): XPathExpr*;
        MultiplicativeExpr(var Operator op): XPathExpr*;
        UnaryExpr: XPathExpr*;
        UnionExpr: XPathExpr*;
        PathExpr(var Operator op, var std::unique_ptr<XPathExpr> fnc): XPathExpr*;
        FilterExpr: XPathExpr*;
        LocationPath: XPathExpr*;
        AbsoluteLocationPath: XPathExpr*;
        RelativeLocationPath(var Operator op): XPathExpr*;
        AbbreviatedAbsoluteLocationPath: XPathExpr*;
        Step: XPathLocationStepExpr*;
        AbbreviatedStep: XPathLocationStepExpr*;
        AxisSpecifier: Axis;
        AxisName: Axis;
        AbbreviatedAxisSpecifier: Axis;
        Predicate: XPathExpr*;
        PredicateExpr: XPathExpr*;
        NodeTest: XPathNodeTestExpr*;
        NameTest: XPathNodeTestExpr*;
        NodeType: XPathNodeTestExpr*;
        PrimaryExpr: XPathExpr*;
        VariableReference: XPathExpr*;
        Literal: XPathExpr*;
        Number: XPathExpr*;
        Digits;
        FunctionCall: XPathFunctionCall*;
        Argument: XPathExpr*;
        FunctionName(var std::unique_ptr<XPathExpr> nodeType): std::u32string;
        QName: std::u32string;
        PrefixedName: std::u32string;
        UnprefixedName: std::u32string;
        Prefix: std::u32string;
        LocalPart: std::u32string;
        NCName: std::u32string;
    }
}
