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
const int XmlSerParser_QualifiedId = 5;
const int XmlSerParser_Definition = 6;
const int XmlSerParser_Namespace = 7;
const int XmlSerParser_Class = 8;
const int XmlSerParser_Api = 9;
const int XmlSerParser_ClassKey = 10;
const int XmlSerParser_Inheritance = 11;
const int XmlSerParser_BaseClass = 12;
const int XmlSerParser_ClassContent = 13;
const int XmlSerParser_MemberVariable = 14;
const int XmlSerParser_Enum = 15;
const int XmlSerParser_EnumContent = 16;
const int XmlSerParser_EnumConstant = 17;
const int XmlSerParser_Type = 18;
const int XmlSerParser_ScalarType = 19;
const int XmlSerParser_ClassType = 20;
const int XmlSerParser_TemplateType = 21;
const int XmlSerParser_CppBlock = 22;

#endif // RULES_H
