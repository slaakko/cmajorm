namespace pp
{
    grammar PPFileGrammar
    {
        PPFile(PP* pp, ustring* result);
        Group(PP* pp, bool process, ustring* result);
        IfSection(PP* pp, bool parentProcess, ustring* result, var bool processed, var bool processGroup);
        IfGroup(PP* pp, bool parentProcess, ustring* result, var bool process) : bool;
        ElifGroup(PP* pp, bool process, ustring* result) : bool;
        ElseGroup(PP* pp, bool process, ustring* result);
        EndIfLine(PP* pp, ustring* result);
        ControlLine(PP* pp, bool process, ustring* result);
        IncludeDirective(PP* pp, bool process, ustring* result, var ustring headerNameText);
        DefineFunctionMacro(PP* pp, bool process, ustring* result, var ustring replacementList);
        MacroParams : List<ustring>;
        DefineObjectMacro(PP* pp, bool process, ustring* result, var ustring replacementList);
        UndefineMacro(PP* pp, bool process, ustring* result);
        PragmaDirective(ustring* result);
        ErrorDirective(bool process, ustring* result);
        LineDirective(ustring* result);
        NullDirective(ustring* result);
        ConstantExpression(bool process, PP* pp, var ustring constantExpressionText) : bool;
        TextLine(PP* pp, bool process, ustring* result, var ustring replacementText);
        PossiblyEmptyTokenLine(bool process, ustring* result, PP* pp, PPTokenFlags flags, var bool substituted, var int parenCount);
        NonemptyTokenLine(bool process, ustring* result, PP* pp, PPTokenFlags flags, var bool substituted, var int parenCount);
        PPToken(bool process, ustring* result, PP* pp, PPTokenFlags flags, bool* substituted, int* parenCount, var bool stringisize);
        FunctionMacroInvocation(bool process, PP* pp, PPTokenFlags flags, ustring* result, var FunctionMacro* macro);
        MacroArgumentList(bool process, PP* pp, PPTokenFlags flags) : List<ustring>;
        MacroArgument(bool process, PP* pp, PPTokenFlags flags, var bool substituted, var int parenCount) : ustring;
        PPTokenCatenation(PP* pp, PPTokenFlags flags, var ustring left, var ustring right, var bool substituted, var int parenCount) : ustring;
        PPTokenSequence(PP* pp, PPTokenFlags flags) : ustring;
        MacroReplacement(FunctionMacro* macro, List<ustring>* args, PP* pp, PPTokenFlags flags) : ustring;
        PPNumber : ustring;
        HeaderName : ustring;
        S;
    }
    grammar PPPossiblyEmptyTokenLineGrammar
    {
    }
    grammar PPMacroReplacementGrammar
    {
    }
    grammar PPNonemptyTokenLineGrammar
    {
    }
}
