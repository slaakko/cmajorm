#include "XmlSerRules.hpp"

std::vector<const char*> rules = {
"XmlSerParser.SourceFile"
, "XmlSerParser.IncludeDirective"
, "XmlSerParser.NamespaceContent"
, "XmlSerParser.Definition"
, "XmlSerParser.Namespace"
, "XmlSerParser.Class"
, "XmlSerParser.Api"
, "XmlSerParser.ClassKey"
, "XmlSerParser.Inheritance"
, "XmlSerParser.ClassContent"
, "XmlSerParser.MemberVariable"
, "XmlSerParser.Enum"
, "XmlSerParser.EnumContent"
, "XmlSerParser.EnumConstant"
, "XmlSerParser.Type"
, "XmlSerParser.ScalarType"
, "XmlSerParser.ClassType"
};

std::vector<const char*>* GetRuleNameVecPtr()
{
    return &rules;
}
