namespace System.Xml
{
    grammar XmlGrammar
    {
        Document(XmlProcessor* processor);
        Char;
        S;
        NameStartChar;
        NameChar;
        Name : ustring;
        Names;
        Nmtoken;
        Nmtokens;
        EntityValue(XmlProcessor* processor) : ustring;
        AttValue(XmlProcessor* processor) : ustring;
        SystemLiteral : ustring;
        PubidLiteral : ustring;
        PubidChar;
        CharData(XmlProcessor* processor);
        Comment(XmlProcessor* processor);
        PI(XmlProcessor* processor, var ustring data);
        PITarget : ustring;
        CDSect(XmlProcessor* processor);
        CDStart;
        CData : ustring;
        CDEnd;
        Prolog(XmlProcessor* processor);
        XMLDecl(XmlProcessor* processor);
        VersionInfo(XmlProcessor* processor);
        Eq;
        VersionNum;
        Misc(XmlProcessor* processor);
        DocTypeDecl(XmlProcessor* processor);
        DeclSep(XmlProcessor* processor);
        IntSubset(XmlProcessor* processor);
        MarkupDecl(XmlProcessor* processor);
        ExtSubset(XmlProcessor* processor);
        ExtSubsetDecl(XmlProcessor* processor);
        SDDecl(XmlProcessor* processor, var bool standalone);
        Element(XmlProcessor* processor, var ustring tagName);
        Attribute(XmlProcessor* processor);
        ETag(XmlProcessor* processor);
        Content(XmlProcessor* processor);
        ElementDecl(XmlProcessor* processor);
        ContentSpec;
        Children;
        CP;
        Choice;
        Seq;
        Mixed;
        AttlistDecl(XmlProcessor* processor);
        AttDef(XmlProcessor* processor);
        AttType;
        StringType;
        TokenizedType;
        EnumeratedType;
        NotationType;
        Enumeration;
        DefaultDecl(XmlProcessor* processor);
        ConditionalSect(XmlProcessor* processor);
        IncludeSect(XmlProcessor* processor);
        IgnoreSect;
        IgnoreSectContents;
        Ignore;
        CharRef : uchar;
        Reference(XmlProcessor* processor);
        EntityRef(XmlProcessor* processor);
        PEReference(XmlProcessor* processor);
        EntityDecl(XmlProcessor* processor);
        GEDecl(XmlProcessor* processor);
        PEDecl(XmlProcessor* processor);
        EntityDef(XmlProcessor* processor);
        PEDef(XmlProcessor* processor);
        ExternalID;
        NDataDecl : ustring;
        TextDecl(XmlProcessor* processor);
        ExtParsedEnt(XmlProcessor* processor);
        EncodingDecl(XmlProcessor* processor);
        EncName : ustring;
        NotationDecl(XmlProcessor* processor);
        PublicID;
    }
}
