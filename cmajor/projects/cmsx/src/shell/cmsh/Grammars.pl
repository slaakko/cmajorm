namespace cmsh
{
    grammar EnvironmentGrammar
    {
        EnvironmentVariables : List<Pair<ustring, ustring>>;
        Line(List<Pair<ustring, ustring>>* environmentVariables);
        EnvironmentVariableLine : Pair<ustring, ustring>;
        Value : ustring;
        Space;
        CommentLine;
    }
}
