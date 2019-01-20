namespace System.Text.RegularExpressions
{
    grammar RegularExpressionGrammar
    {
        RegularExpression(PtrNfaStateFactory* factory) : PtrNfa;
        AlternativeExpression(PtrNfaStateFactory* factory) : PtrNfa;
        SequenceExpression(PtrNfaStateFactory* factory) : PtrNfa;
        PostfixExpression(PtrNfaStateFactory* factory) : PtrNfa;
        PrimaryExpression(PtrNfaStateFactory* factory) : PtrNfa;
        Char : uchar;
        CharClass(var bool inverse, var ustring s) : CharClass;
        Class : Class;
        CharRange : ustring;
        CharClassChar : uchar;
    }
}
