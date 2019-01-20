namespace BigNumCalc
{
    grammar BigNumExpressionGrammar
    {
        Expr(EvaluationStack* s, var uchar op);
        Term(EvaluationStack* s, var uchar op);
        Factor(EvaluationStack* s);
        Number(EvaluationStack* s);
        ExponentPart;
        BigFloatLiteral : BigValue*;
        BigRationalLiteral : BigValue*;
        BigIntLiteral : BigValue*;
        Spaces;
    }
}
