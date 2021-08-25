#ifndef RULES_H
#define RULES_H
#include <sngxml/xml/XmlProcessor.hpp>
#include <vector>

SNGXML_XML_API std::vector<const char*>* GetRuleNameVecPtr();

const int XmlParser_Document = 0;
const int XmlParser_Char = 1;
const int XmlParser_S = 2;
const int XmlParser_NameStartChar = 3;
const int XmlParser_NameChar = 4;
const int XmlParser_Name = 5;
const int XmlParser_Names = 6;
const int XmlParser_Nmtoken = 7;
const int XmlParser_Nmtokens = 8;
const int XmlParser_Prolog = 9;
const int XmlParser_XMLDecl = 10;
const int XmlParser_VersionInfo = 11;
const int XmlParser_VersionNum = 12;
const int XmlParser_VersionNumDQ = 13;
const int XmlParser_VersionNumSQ = 14;
const int XmlParser_VersionNumber = 15;
const int XmlParser_EncodingDecl = 16;
const int XmlParser_EncodingName = 17;
const int XmlParser_EncNameDQ = 18;
const int XmlParser_EncNameSQ = 19;
const int XmlParser_EncName = 20;
const int XmlParser_SDDecl = 21;
const int XmlParser_DocTypeDecl = 22;
const int XmlParser_ExternalID = 23;
const int XmlParser_SystemLiteral = 24;
const int XmlParser_PubidLiteral = 25;
const int XmlParser_PubidChar = 26;
const int XmlParser_IntSubset = 27;
const int XmlParser_MarkupDecl = 28;
const int XmlParser_DeclSep = 29;
const int XmlParser_ElementDecl = 30;
const int XmlParser_ContentSpec = 31;
const int XmlParser_Children = 32;
const int XmlParser_CP = 33;
const int XmlParser_Choice = 34;
const int XmlParser_Seq = 35;
const int XmlParser_Mixed = 36;
const int XmlParser_AttlistDecl = 37;
const int XmlParser_AttDef = 38;
const int XmlParser_AttType = 39;
const int XmlParser_StringType = 40;
const int XmlParser_TokenizedType = 41;
const int XmlParser_EnumeratedType = 42;
const int XmlParser_NotationType = 43;
const int XmlParser_Enumeration = 44;
const int XmlParser_DefaultDecl = 45;
const int XmlParser_EntityDecl = 46;
const int XmlParser_GEDecl = 47;
const int XmlParser_PEDecl = 48;
const int XmlParser_EntityDef = 49;
const int XmlParser_PEDef = 50;
const int XmlParser_EntityValue = 51;
const int XmlParser_NDataDecl = 52;
const int XmlParser_PEReference = 53;
const int XmlParser_NotationDecl = 54;
const int XmlParser_PublicID = 55;
const int XmlParser_Element = 56;
const int XmlParser_ETag = 57;
const int XmlParser_Content = 58;
const int XmlParser_CharDataChar = 59;
const int XmlParser_CharData = 60;
const int XmlParser_CDSect = 61;
const int XmlParser_Attribute = 62;
const int XmlParser_AttValueDQ = 63;
const int XmlParser_AttValueSQ = 64;
const int XmlParser_AttValue = 65;
const int XmlParser_EntityRef = 66;
const int XmlParser_DecCodePoint = 67;
const int XmlParser_HexCodePoint = 68;
const int XmlParser_CharRef = 69;
const int XmlParser_Reference = 70;
const int XmlParser_Misc = 71;
const int XmlParser_Comment = 72;
const int XmlParser_PI = 73;
const int XmlParser_PITarget = 74;
const int XmlParser_Xml = 75;
const int XmlParser_Eq = 76;
const int XmlParser_YesNo = 77;

#endif // RULES_H
