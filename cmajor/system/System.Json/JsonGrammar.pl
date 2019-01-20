namespace System.Json
{
    grammar JsonGrammar
    {
        Value : JsonValue*;
        Object : JsonObject*;
        Field(JsonObject* o);
        Array : JsonArray*;
        String : JsonString*;
        Escape : uchar;
        Number : JsonNumber*;
    }
}
