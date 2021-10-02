#ifndef RULES_H
#define RULES_H
#include <sngxml/serialization/XmlSerLexer.hpp>
#include <sngxml/serialization/XmlSerAst.hpp>
#include <vector>

SNGXML_SERIALIZATION_API std::vector<const char*>* GetRuleNameVecPtr();

const int XmlSerParser_SourceFile = 0;
const int XmlSerParser_IncludeDirective = 1;
const int XmlSerParser_FileTag = 2;
const int XmlSerParser_NamespaceContent = 3;
const int XmlSerParser_Declaration = 4;
const int XmlSerParser_Definition = 5;
const int XmlSerParser_Namespace = 6;
const int XmlSerParser_Class = 7;
const int XmlSerParser_Api = 8;
const int XmlSerParser_ClassKey = 9;
const int XmlSerParser_Inheritance = 10;
const int XmlSerParser_ClassContent = 11;
const int XmlSerParser_MemberVariable = 12;
const int XmlSerParser_Enum = 13;
const int XmlSerParser_EnumContent = 14;
const int XmlSerParser_EnumConstant = 15;
const int XmlSerParser_Type = 16;
const int XmlSerParser_ScalarType = 17;
const int XmlSerParser_ClassType = 18;
const int XmlSerParser_TemplateType = 19;
const int XmlSerParser_CppBlock = 20;

#endif // RULES_H
