#include "XmlSerRules.hpp"

std::vector<const char*> rules = {
"XmlSerParser.SourceFile"
, "XmlSerParser.IncludeDirective"
, "XmlSerParser.FileTag"
, "XmlSerParser.NamespaceContent"
, "XmlSerParser.Declaration"
, "XmlSerParser.QualifiedId"
, "XmlSerParser.Definition"
, "XmlSerParser.Namespace"
, "XmlSerParser.Class"
, "XmlSerParser.Api"
, "XmlSerParser.ClassKey"
, "XmlSerParser.Inheritance"
, "XmlSerParser.BaseClass"
, "XmlSerParser.ClassContent"
, "XmlSerParser.MemberVariable"
, "XmlSerParser.Enum"
, "XmlSerParser.EnumContent"
, "XmlSerParser.EnumConstant"
, "XmlSerParser.Type"
, "XmlSerParser.ScalarType"
, "XmlSerParser.ClassType"
, "XmlSerParser.TemplateType"
, "XmlSerParser.CppBlock"
};

std::vector<const char*>* GetRuleNameVecPtr()
{
    return &rules;
}
