// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngxml/serialization/XmlSerGen.hpp>
#include <sngxml/serialization/XmlSerLexer.hpp>
#include <sngxml/serialization/XmlSerParser.hpp>
#include <sngxml/serialization/XmlSerVisitor.hpp>
#include <sngxml/serialization/XmlSerAst.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/MappedInputFile.hpp>
#include <soulng/util/Sha1.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <fstream>
#include <iostream>

namespace sngxml { namespace xmlser {

using namespace soulng::util;
using namespace soulng::unicode;

void WriteIndentedText(CodeFormatter& formatter, const std::string& cppText)
{
    std::string line;
    bool startOfLine = true;
    bool lbraceSeen = false;
    for (char c : cppText)
    {
        if (c == '\n')
        {
            formatter.WriteLine(line);
            line.clear();
            startOfLine = true;
            if (lbraceSeen)
            {
                formatter.IncIndent();
                lbraceSeen = false;
            }
        }
        else if (c == ' ')
        {
            if (!startOfLine)
            {
                line.append(1, c);
            }
        }
        else if (c == '{')
        {
            line.append(1, c);
            lbraceSeen = true;
        }
        else if (c == '}')
        {
            line.append(1, c);
            formatter.DecIndent();
        }
        else if (c != '\r')
        {
            startOfLine = false;
            line.append(1, c);
        }
    }
    if (!line.empty())
    {
        formatter.WriteLine(line);
    }
}

class PointerMemberDetectorVisitor : public DefaultVisitor
{
public:
    PointerMemberDetectorVisitor();
    void Visit(PointerNode& node) override;
    void Visit(TemplateIdNode& node) override;
    bool HasPointerMember() const { return hasPointerMember; }
private:
    bool hasPointerMember;
};

PointerMemberDetectorVisitor::PointerMemberDetectorVisitor() : hasPointerMember(false)
{
}

void PointerMemberDetectorVisitor::Visit(PointerNode& node)
{
    hasPointerMember = true;
}

void PointerMemberDetectorVisitor::Visit(TemplateIdNode& node)
{
    if (node.TypeId() == "xml_ptr" || node.TypeId() == "unique_xml_ptr")
    {
        hasPointerMember = true;
    }
}

class HeaderGeneratorVisitor : public DefaultVisitor
{
public:
    HeaderGeneratorVisitor(CodeFormatter& formatter_, const std::string& includeGuard_);
    void Visit(SourceFileNode& node) override;
    void Visit(NamespaceNode& node) override;
    void Visit(ClassNode& node) override;
    void Visit(MemberVariableNode& node) override;
    void Visit(EnumTypeNode& node) override;
    void Visit(PointerNode& node) override;
    void Visit(ArrayNode& node) override;
    void Visit(BoolNode& node) override;
    void Visit(SByteNode& node) override;
    void Visit(ByteNode& node) override;
    void Visit(ShortNode& node) override;
    void Visit(UShortNode& node) override;
    void Visit(IntNode& node) override;
    void Visit(UIntNode& node) override;
    void Visit(LongNode& node) override;
    void Visit(ULongNode& node) override;
    void Visit(FloatNode& node) override;
    void Visit(DoubleNode& node) override;
    void Visit(CharNode& node) override;
    void Visit(WCharNode& node) override;
    void Visit(UCharNode& node) override;
    void Visit(UuidNode& node) override;
    void Visit(StringNode& node) override;
    void Visit(WStringNode& node) override;
    void Visit(UStringNode& node) override;
    void Visit(DateNode& node) override;
    void Visit(DateTimeNode& node) override;
    void Visit(TimestampNode& node) override;
    void Visit(TimePointNode& node) override;
    void Visit(DurationNode& node) override;
    void Visit(ClassIdNode& node) override;
    void Visit(TemplateIdNode& node) override;
    void Visit(ForwardClassDeclarationNode& node) override;
    void Visit(AliasDeclarationNode& node) override;
    void Visit(CppBlockNode& node) override;
private:
    CodeFormatter& formatter;
    std::string includeGuard;
};

HeaderGeneratorVisitor::HeaderGeneratorVisitor(CodeFormatter& formatter_, const std::string& includeGuard_) : formatter(formatter_), includeGuard(includeGuard_)
{
}

void HeaderGeneratorVisitor::Visit(SourceFileNode& node)
{
    formatter.WriteLine("#ifndef " + includeGuard);
    formatter.WriteLine("#define " + includeGuard);
    for (const auto& includeDirective : node.IncludeDirectives())
    {
        if (includeDirective->FileTag() == "hpp" || includeDirective->FileTag().empty())
        {
            formatter.WriteLine("#include " + includeDirective->FilePath());
        }
    }
    formatter.WriteLine("#include <sngxml/serialization/XmlPtr.hpp>");
    formatter.WriteLine("#include <soulng/util/Time.hpp>");
    formatter.WriteLine("#include <chrono>");
    formatter.WriteLine("#include <memory>");
    formatter.WriteLine("#include <stdint.h>");
    DefaultVisitor::Visit(node);
    formatter.WriteLine("#endif // " + includeGuard);

}

void HeaderGeneratorVisitor::Visit(NamespaceNode& node)
{
    if (!node.Id().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("namespace " + node.Id() + " {");
        formatter.WriteLine();
        formatter.WriteLine("using date = soulng::util::Date;");
        formatter.WriteLine("using datetime = soulng::util::DateTime;");
        formatter.WriteLine("using timestamp = soulng::util::Timestamp;");
        formatter.WriteLine("using time_point = std::chrono::steady_clock::time_point;");
        formatter.WriteLine("using duration = std::chrono::steady_clock::duration;");
        formatter.WriteLine("using uuid = boost::uuids::uuid;");
        formatter.WriteLine("template<class T> using xml_ptr = sngxml::xmlser::XmlPtr<T>;");
        formatter.WriteLine("template<class T> using unique_xml_ptr = sngxml::xmlser::UniqueXmlPtr<T>;");
    }
    else
    {
        if (!node.ContainsNamespaces())
        {
            formatter.WriteLine();
            formatter.WriteLine("using date = soulng::util::Date;");
            formatter.WriteLine("using datetime = soulng::util::DateTime;");
            formatter.WriteLine("using timestamp = soulng::util::Timestamp;");
            formatter.WriteLine("using time_point = std::chrono::steady_clock::time_point;");
            formatter.WriteLine("using duration = std::chrono::steady_clock::duration;");
            formatter.WriteLine("using uuid = boost::uuids::uuid;");
            formatter.WriteLine("template<class T> using xml_ptr = sngxml::xmlser::XmlPtr<T>;");
            formatter.WriteLine("template<class T> using unique_xml_ptr = sngxml::xmlser::UniqueXmlPtr<T>;");
        }
    }
    DefaultVisitor::Visit(node);
    if (!node.Id().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("} // namespace  " + node.Id());
    }
    formatter.WriteLine();
}

void HeaderGeneratorVisitor::Visit(ClassNode& node)
{
    std::string api;
    if (!node.Api().empty())
    {
        api.append(1, ' ').append(node.Api());
    }
    std::string inheritance;
    std::vector<BaseClassNode*> externalBases = node.ExternalBaseClasses();
    if (node.InternalBaseClass())
    {
        inheritance.append(" : public ").append(node.InternalBaseClass()->Id());
        for (BaseClassNode* externalBase : externalBases)
        {
            inheritance.append(", public ").append(externalBase->Id());
        }
    }
    else
    {
        bool first = true;
        for (BaseClassNode* externalBase : externalBases)
        {
            if (first)
            {
                inheritance.append(" : public ").append(externalBase->Id());
                first = false;
            }
            else
            {
                inheritance.append(", public ").append(externalBase->Id());
            }
        }
        if (externalBases.empty())
        {
            inheritance.append(" : public sngxml::xmlser::XmlSerializable");
        }
        else
        {
            inheritance.append(", public sngxml::xmlser::XmlSerializable");
        }
    }
    formatter.WriteLine();
    formatter.WriteLine("class" + api + " " + node.Id() + inheritance);
    formatter.WriteLine("{");
    formatter.WriteLine("public:");
    formatter.IncIndent();
    formatter.WriteLine(node.Id() + "();");
    PointerMemberDetectorVisitor pointerMemberDetectorVisitor;
    node.Accept(pointerMemberDetectorVisitor);
    bool hasPointerMembers = pointerMemberDetectorVisitor.HasPointerMember();
    if (hasPointerMembers)
    {
        formatter.WriteLine(node.Id() + "(const " + node.Id() + "&) = delete;");
        formatter.WriteLine(node.Id() + "(" + node.Id() + "&&) = delete;");
        formatter.WriteLine(node.Id() + "& operator=(const " + node.Id() + "&) = delete;");
        formatter.WriteLine(node.Id() + "& operator=(" + node.Id() + "&&) = delete;");
    }
    formatter.WriteLine("virtual ~" + node.Id() + "();");
    formatter.WriteLine("static void* Create();");
    formatter.WriteLine("static std::string StaticClassName();");
    formatter.WriteLine("static void Register(int classId_);");
    formatter.WriteLine("void DestroyObject() override { delete this; }");
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("const boost::uuids::uuid& ObjectId() const override { return objectId; }");
        formatter.WriteLine("void SetObjectId(const boost::uuids::uuid& objectId_) override { objectId = objectId_; }");
    }
    formatter.WriteLine("int ClassId() const override { return classId; }");
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("bool IsOwned() const override { return isOwned; }");
        formatter.WriteLine("void SetOwned() override { isOwned = true; }");
        formatter.WriteLine("void ResetOwned() override { isOwned = false; }");
    }
    formatter.WriteLine("std::string ClassName() const override;");
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("sngxml::xmlser::XmlContainer* Container() const override { return container; }");
        formatter.WriteLine("void SetContainer(sngxml::xmlser::XmlContainer* container_) override { container = container_; }");
    }
    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const;");
    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const override;");
    formatter.WriteLine("void FromXml(sngxml::dom::Element* element) override;");
    formatter.WriteLine("std::vector<sngxml::xmlser::XmlPtrBase*> GetPtrs() const override;");
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("virtual void SetObjectXmlAttributes(sngxml::dom::Element* element) const;");
    }
    else
    {
        formatter.WriteLine("void SetObjectXmlAttributes(sngxml::dom::Element* element) const override;");
    }
    formatter.DecIndent();
    formatter.WriteLine("public:");
    formatter.IncIndent();
    DefaultVisitor::Visit(node);
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("void* dataPtr;");
    }
    formatter.DecIndent();
    formatter.WriteLine("private:");
    formatter.IncIndent();
    formatter.WriteLine("static int classId;");
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("boost::uuids::uuid objectId;");
        formatter.WriteLine("sngxml::xmlser::XmlContainer* container;");
        formatter.WriteLine("bool isOwned;");
    }
    formatter.DecIndent();
    formatter.WriteLine("};");
}

void HeaderGeneratorVisitor::Visit(MemberVariableNode& node)
{
    DefaultVisitor::Visit(node);
    formatter.Write(" ");
    formatter.Write(node.Id());
    formatter.WriteLine(";");
}

void HeaderGeneratorVisitor::Visit(EnumTypeNode& node)
{
    formatter.WriteLine();
    formatter.WriteLine("enum class " + node.Id());
    formatter.WriteLine("{");
    formatter.IncIndent();
    bool first = true;
    for (const auto& enumConstant : node.EnumConstants())
    {
        if (first)
        {
            first = false;
        }
        else
        {
            formatter.Write(", ");
        }
        formatter.WriteLine(enumConstant->Id());
    }
    formatter.DecIndent();
    formatter.WriteLine("};");
}

void HeaderGeneratorVisitor::Visit(PointerNode& node)
{
    formatter.Write("std::unique_ptr<");
    DefaultVisitor::Visit(node);
    formatter.Write(">");
}

void HeaderGeneratorVisitor::Visit(ArrayNode& node)
{
    formatter.Write("std::vector<");
    DefaultVisitor::Visit(node);
    formatter.Write(">");
}

void HeaderGeneratorVisitor::Visit(BoolNode& node)
{
    formatter.Write("bool");
}

void HeaderGeneratorVisitor::Visit(SByteNode& node) 
{
    formatter.Write("int8_t");
}

void HeaderGeneratorVisitor::Visit(ByteNode& node) 
{
    formatter.Write("uint8_t");
}

void HeaderGeneratorVisitor::Visit(ShortNode& node) 
{
    formatter.Write("int16_t");
}

void HeaderGeneratorVisitor::Visit(UShortNode& node) 
{
    formatter.Write("uint16_t");
}

void HeaderGeneratorVisitor::Visit(IntNode& node) 
{
    formatter.Write("int32_t");
}

void HeaderGeneratorVisitor::Visit(UIntNode& node) 
{
    formatter.Write("uint32_t");
}

void HeaderGeneratorVisitor::Visit(LongNode& node) 
{
    formatter.Write("int64_t");
}

void HeaderGeneratorVisitor::Visit(ULongNode& node) 
{
    formatter.Write("uint64_t");
}

void HeaderGeneratorVisitor::Visit(FloatNode& node) 
{
    formatter.Write("float");
}

void HeaderGeneratorVisitor::Visit(DoubleNode& node) 
{
    formatter.Write("double");
}

void HeaderGeneratorVisitor::Visit(CharNode& node) 
{
    formatter.Write("char");
}

void HeaderGeneratorVisitor::Visit(WCharNode& node) 
{
    formatter.Write("char16_t");
}

void HeaderGeneratorVisitor::Visit(UCharNode& node) 
{
    formatter.Write("char32_t");
}

void HeaderGeneratorVisitor::Visit(UuidNode& node) 
{
    formatter.Write("uuid");
}

void HeaderGeneratorVisitor::Visit(StringNode& node) 
{
    formatter.Write("std::string");
}

void HeaderGeneratorVisitor::Visit(WStringNode& node) 
{
    formatter.Write("std::u16string");
}

void HeaderGeneratorVisitor::Visit(UStringNode& node) 
{
    formatter.Write("std::u32string");
}

void HeaderGeneratorVisitor::Visit(DateNode& node) 
{
    formatter.Write("date");
}

void HeaderGeneratorVisitor::Visit(DateTimeNode& node) 
{
    formatter.Write("datetime");
}

void HeaderGeneratorVisitor::Visit(TimestampNode& node)
{
    formatter.Write("timestamp");
}

void HeaderGeneratorVisitor::Visit(TimePointNode& node) 
{
    formatter.Write("time_point");
}

void HeaderGeneratorVisitor::Visit(DurationNode& node) 
{
    formatter.Write("duration");
}

void HeaderGeneratorVisitor::Visit(ClassIdNode& node) 
{
    formatter.Write(node.ClassId());
}

void HeaderGeneratorVisitor::Visit(TemplateIdNode& node)
{
    formatter.Write(node.TypeId() + "<" + node.TypeParamId() + ">");
}

void HeaderGeneratorVisitor::Visit(ForwardClassDeclarationNode& node)
{
    formatter.WriteLine();
    formatter.WriteLine("class " + node.ClassId() + ";");
}

void HeaderGeneratorVisitor::Visit(AliasDeclarationNode& node)
{
    formatter.WriteLine();
    formatter.WriteLine("using " + node.Name() + " = " + node.Subject() + ";");
}

void HeaderGeneratorVisitor::Visit(CppBlockNode& node)
{
    if (node.Source()) return;
    const std::string& cppText = node.CppText();
    WriteIndentedText(formatter, cppText);
}

void GenerateXmlSerializationHeaderFile(SourceFileNode* sourceFileNode, const std::string& headerFilePath)
{
    std::string headerName = Path::GetFileNameWithoutExtension(headerFilePath);
    std::string hash = GetSha1MessageDigest(headerFilePath);
    std::string includeGuard = ToUtf8(ToUpper(ToUtf32(headerName + "_HPP_" + hash)));
    std::ofstream headerFile(headerFilePath);
    CodeFormatter formatter(headerFile);
    HeaderGeneratorVisitor visitor(formatter, includeGuard);
    sourceFileNode->Accept(visitor);
}

class InitializerGeneratorVisitor : public DefaultVisitor
{
public:
    InitializerGeneratorVisitor(CodeFormatter& formatter_);
    void Visit(MemberVariableNode& node) override;
    bool HasMembers() const { return hasMembers; }
private:
    bool first;
    bool hasMembers;
    CodeFormatter& formatter;
};

InitializerGeneratorVisitor::InitializerGeneratorVisitor(CodeFormatter& formatter_) : formatter(formatter_), first(true), hasMembers(false)
{
}

void InitializerGeneratorVisitor::Visit(MemberVariableNode& node)
{
    hasMembers = true;
    formatter.Write(", " + node.Id() + "()");
}

class ToXmlMemberVariableGeneratorVisitor : public DefaultVisitor
{
public:
    ToXmlMemberVariableGeneratorVisitor(CodeFormatter& formatter_);
    void Visit(MemberVariableNode& node) override;
private:
    CodeFormatter& formatter;
};

ToXmlMemberVariableGeneratorVisitor::ToXmlMemberVariableGeneratorVisitor(CodeFormatter& formatter_) : formatter(formatter_)
{
}

void ToXmlMemberVariableGeneratorVisitor::Visit(MemberVariableNode& node)
{
    formatter.WriteLine("element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(" + node.Id() + ", \"" + node.Id() + "\", ctx).release()));");
}

class FromXmlMemberVariableGeneratorVisitor : public DefaultVisitor
{
public:
    FromXmlMemberVariableGeneratorVisitor(CodeFormatter& formatter_);
    void Visit(MemberVariableNode& node) override;
private:
    CodeFormatter& formatter;
};

FromXmlMemberVariableGeneratorVisitor::FromXmlMemberVariableGeneratorVisitor(CodeFormatter& formatter_) : formatter(formatter_)
{
}

void FromXmlMemberVariableGeneratorVisitor::Visit(MemberVariableNode& node)
{
    formatter.WriteLine("sngxml::xmlser::FromXml(element, \"" + node.Id() + "\", " + node.Id() + ");");
}

class GetPtrsGeneratorVisitor : public DefaultVisitor
{
public:
    GetPtrsGeneratorVisitor(CodeFormatter& formatter_);
    void Visit(MemberVariableNode& node) override;
private:
    CodeFormatter& formatter;
};

GetPtrsGeneratorVisitor::GetPtrsGeneratorVisitor(CodeFormatter& formatter_) : formatter(formatter_)
{
}

void GetPtrsGeneratorVisitor::Visit(MemberVariableNode& node)
{
    TypeNode* type = node.Type();
    if (type->IsTemplateIdNode())
    {
        TemplateIdNode* templateIdNode = static_cast<TemplateIdNode*>(type);
        if (templateIdNode->TypeId() == "xml_ptr")
        {
            formatter.WriteLine("ptrs.push_back(const_cast<xml_ptr<" + templateIdNode->TypeParamId() + ">*>(&" + node.Id() + ")); ");
        }
        else if (templateIdNode->TypeId() == "unique_xml_ptr")
        {
            formatter.WriteLine("ptrs.push_back(const_cast<unique_xml_ptr<" + templateIdNode->TypeParamId() + ">*>(&" + node.Id() + ")); ");
        }
    }
    else if (type->IsArrayNode())
    {
        ArrayNode* arrayNode = static_cast<ArrayNode*>(type);
        TypeNode* typeNode = arrayNode->Type();
        if (typeNode->IsTemplateIdNode())
        {
            TemplateIdNode* templateIdNode = static_cast<TemplateIdNode*>(typeNode);
            if (templateIdNode->TypeId() == "xml_ptr")
            {
                formatter.WriteLine("for (const auto& ptr : " + node.Id() + ")");
                formatter.WriteLine("{");
                formatter.IncIndent();
                formatter.WriteLine("ptrs.push_back(const_cast<xml_ptr<" + templateIdNode->TypeParamId() + ">*>(&ptr));");
                formatter.DecIndent();
                formatter.WriteLine("}");
            }
            else if (templateIdNode->TypeId() == "unique_xml_ptr")
            {
                formatter.WriteLine("for (const auto& ptr : " + node.Id() + ")");
                formatter.WriteLine("{");
                formatter.IncIndent();
                formatter.WriteLine("ptrs.push_back(const_cast<unique_xml_ptr<" + templateIdNode->TypeParamId() + ">*>(&ptr));");
                formatter.DecIndent();
                formatter.WriteLine("}");
            }
        }
    }
}

class SourceGeneratorVisitor : public DefaultVisitor
{
public:
    SourceGeneratorVisitor(CodeFormatter& formatter_, const std::string& headerFilePath_);
    void Visit(SourceFileNode& node) override;
    void Visit(NamespaceNode& noide) override;
    void Visit(ClassNode& node) override;
    void Visit(CppBlockNode& node) override;
private:
    CodeFormatter& formatter;
    std::string headerFilePath;
};

SourceGeneratorVisitor::SourceGeneratorVisitor(CodeFormatter& formatter_, const std::string& headerFilePath_) : formatter(formatter_), headerFilePath(headerFilePath_)
{
}

void SourceGeneratorVisitor::Visit(SourceFileNode& node)
{
    std::string headerInclude = "#include \"" + Path::GetFileName(headerFilePath) + "\"";
    formatter.WriteLine(headerInclude);
    for (const auto& includeDirective : node.IncludeDirectives())
    {
        if (includeDirective->FileTag() == "cpp")
        {
            formatter.WriteLine("#include " + includeDirective->FilePath());
        }
    }
    formatter.WriteLine("#include <boost/uuid/nil_generator.hpp>");
    formatter.WriteLine("#include <boost/uuid/uuid_io.hpp>");
    formatter.WriteLine("#include <sngxml/serialization/XmlExport.hpp>");
    formatter.WriteLine("#include <sngxml/serialization/XmlImport.hpp>");
    formatter.WriteLine("#include <soulng/util/Unicode.hpp>");
    formatter.WriteLine();
    formatter.WriteLine("using namespace soulng::unicode;");
    formatter.WriteLine();
    DefaultVisitor::Visit(node);
}
void SourceGeneratorVisitor::Visit(NamespaceNode& node)
{
    if (!node.Id().empty())
    {
        formatter.WriteLine("namespace " + node.Id() + " {");
        formatter.WriteLine();
        formatter.WriteLine("using namespace soulng::unicode;");
        formatter.WriteLine();
    }
    for (const std::unique_ptr<Node>& n : node.Nodes())
    {
        n->Accept(*this);
    }
    if (!node.Id().empty())
    {
        formatter.WriteLine();
        formatter.WriteLine("} // namespace " + node.Id());
    }
}

void SourceGeneratorVisitor::Visit(ClassNode& node)
{
    formatter.WriteLine("int " + node.Id() + "::classId = -1;");
    formatter.WriteLine();
    formatter.WriteLine(node.Id() + "::" + node.Id() + "()");
    formatter.IncIndent();
    std::vector<BaseClassNode*> externalBases = node.ExternalBaseClasses();
    if (node.InternalBaseClass())
    {
        formatter.Write(" : " + node.InternalBaseClass()->Id() + "()");
        for (BaseClassNode* externalBase : externalBases)
        {
            formatter.Write(", " + externalBase->Id() + "()");
        }
    }
    else
    {
        if (externalBases.empty())
        {
            formatter.Write(" : objectId(boost::uuids::nil_uuid()), container(nullptr)");
        }
        else
        {
            bool first = true;
            for (BaseClassNode* externalBase : externalBases)
            {
                if (first)
                {
                    formatter.Write(" : " + externalBase->Id() + "()");
                    first = false;
                }
                else
                {
                    formatter.Write(", " + externalBase->Id() + "()");
                }
            }
            formatter.Write(", objectId(boost::uuids::nil_uuid()), container(nullptr)");
        }
    }
    InitializerGeneratorVisitor defaultCtorInitializerGeneratorVisitor(formatter);
    node.Accept(defaultCtorInitializerGeneratorVisitor);
    if (defaultCtorInitializerGeneratorVisitor.HasMembers())
    {
        if (!node.InternalBaseClass())
        {
            formatter.WriteLine(", dataPtr(nullptr), isOwned(false)");
        }
        else
        {
            formatter.WriteLine();
        }
    }
    else
    {
        if (!node.InternalBaseClass())
        {
            formatter.WriteLine(", dataPtr(nullptr), isOwned(false)");
        }
        else
        {
            formatter.WriteLine();
        }
    }
    formatter.DecIndent();
    formatter.WriteLine("{");
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("void* " + node.Id() + "::Create()");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("return new " + node.Id() + "();");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("void " + node.Id() + "::Register(int classId_)");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("classId = classId_;");
    formatter.WriteLine("sngxml::xmlser::XmlRegister<" + node.Id() + ">(classId, &" + node.Id() + "::Create);");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("std::string " + node.Id() + "::StaticClassName()");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("return \"" + node.Id() + "\";");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("std::string " + node.Id() + "::ClassName() const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("return \"" + node.Id() + "\";");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("void " + node.Id() + "::FromXml(sngxml::dom::Element* element)");
    formatter.WriteLine("{");
    formatter.IncIndent();
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("std::u32string objectIdAttr = element->GetAttribute(U\"objectId\");");
        formatter.WriteLine("if (!objectIdAttr.empty())");
        formatter.WriteLine("{");
        formatter.IncIndent();
        formatter.WriteLine("objectId = boost::lexical_cast<boost::uuids::uuid>(ToUtf8(objectIdAttr));");
        formatter.DecIndent();
        formatter.WriteLine("}");
    }
    else
    {
        formatter.WriteLine(node.InternalBaseClass()->Id() + "::FromXml(element);");
    }
    FromXmlMemberVariableGeneratorVisitor fromXmlGeneratorVisitor(formatter);
    node.Accept(fromXmlGeneratorVisitor);
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("std::vector<sngxml::xmlser::XmlPtrBase*> " + node.Id() + "::GetPtrs() const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    GetPtrsGeneratorVisitor getPtrsGeneratorVisitor(formatter);
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("std::vector<sngxml::xmlser::XmlPtrBase*> ptrs;");
    }
    else
    {
        formatter.WriteLine("std::vector<sngxml::xmlser::XmlPtrBase*> ptrs = " + node.InternalBaseClass()->Id() + "::GetPtrs();");
    }
    node.Accept(getPtrsGeneratorVisitor);
    formatter.WriteLine("return ptrs;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("void " + node.Id() + "::SetObjectXmlAttributes(sngxml::dom::Element* element) const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("element->SetAttribute(U\"classId\", ToUtf32(std::to_string(classId)));");
    formatter.WriteLine("element->SetAttribute(U\"objectId\", ToUtf32(boost::uuids::to_string(ObjectId())));");
    formatter.WriteLine("element->SetAttribute(U\"className\", ToUtf32(ClassName()));");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine(node.Id() + "::~" + node.Id() + "()");
    formatter.WriteLine("{");
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> " + node.Id() + "::ToXml(const std::string& fieldName) const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("sngxml::xmlser::XmlSerializationContext ctx;");
    formatter.WriteLine("return ToXml(fieldName, ctx);");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> " + node.Id() + "::ToXml(const std::string& fieldName, sngxml::xmlser::XmlSerializationContext& ctx) const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    if (!node.InternalBaseClass())
    {
        formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));");
        formatter.WriteLine("if (!ctx.GetFlag(sngxml::xmlser::XmlSerializationFlags::suppressMetadata))");
        formatter.WriteLine("{");
        formatter.IncIndent();
        formatter.WriteLine("SetObjectXmlAttributes(element.get());");
        formatter.DecIndent();
        formatter.WriteLine("}");
    }
    else
    {
        formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> element = " + node.InternalBaseClass()->Id() + "::ToXml(fieldName, ctx);");
    }
    ToXmlMemberVariableGeneratorVisitor toXmlMemberVariableGeneratorVisitor(formatter);
    node.Accept(toXmlMemberVariableGeneratorVisitor);
    formatter.WriteLine("return element;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
}

void SourceGeneratorVisitor::Visit(CppBlockNode& node)
{
    if (!node.Source()) return;
    WriteIndentedText(formatter, node.CppText());
}

void GenerateXmlSerializationSourceFile(SourceFileNode* sourceFileNode, const std::string& sourceFilePath, const std::string& headerFilePath)
{
    std::ofstream sourceFile(sourceFilePath);
    CodeFormatter formatter(sourceFile);
    SourceGeneratorVisitor visitor(formatter, headerFilePath);
    sourceFileNode->Accept(visitor);
}

void GenerateXmlSerializationCode(const std::string& serializationDefinitionFilePath, bool verbose)
{
    std::u32string content = ToUtf32(ReadFile(serializationDefinitionFilePath));
    XmlSerLexer lexer(content, serializationDefinitionFilePath, 0);
    lexer.SetFlag(soulng::lexer::LexerFlags::farthestError);
    std::unique_ptr<SourceFileNode> sourceFile = XmlSerParser::Parse(lexer);
    std::string headerFilePath = Path::ChangeExtension(serializationDefinitionFilePath, ".hpp");
    std::string sourceFilePath = Path::ChangeExtension(serializationDefinitionFilePath, ".cpp");
    GenerateXmlSerializationHeaderFile(sourceFile.get(), headerFilePath);
    if (verbose)
    {
        std::cout << "==> " << headerFilePath << std::endl;
    }
    GenerateXmlSerializationSourceFile(sourceFile.get(), sourceFilePath, headerFilePath);
    if (verbose)
    {
        std::cout << "==> " << sourceFilePath << std::endl;
    }
}

} } // namespace sngxml::xmlser