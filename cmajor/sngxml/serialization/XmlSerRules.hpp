#ifndef RULES_H
#define RULES_H
#include <sngxml/serialization/XmlSerLexer.hpp>
#include <sngxml/serialization/XmlSerAst.hpp>
#include <vector>

SNGXML_SERIALIZATION_API std::vector<const char*>* GetRuleNameVecPtr();

const int XmlSerParser_SourceFile = 0;
const int XmlSerParser_IncludeDirective = 1;
const int XmlSerParser_NamespaceContent = 2;
const int XmlSerParser_Definition = 3;
const int XmlSerParser_Namespace = 4;
const int XmlSerParser_Class = 5;
const int XmlSerParser_Api = 6;
const int XmlSerParser_ClassKey = 7;
const int XmlSerParser_Inheritance = 8;
const int XmlSerParser_ClassContent = 9;
const int XmlSerParser_MemberVariable = 10;
const int XmlSerParser_Enum = 11;
const int XmlSerParser_EnumContent = 12;
const int XmlSerParser_EnumConstant = 13;
const int XmlSerParser_Type = 14;
const int XmlSerParser_ScalarType = 15;
const int XmlSerParser_ClassType = 16;

#endif // RULES_H
