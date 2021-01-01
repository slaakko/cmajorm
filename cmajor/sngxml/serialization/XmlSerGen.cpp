// =================================
// Copyright (c) 2021 Seppo Laakko
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
#include <fstream>
#include <iostream>

namespace sngxml { namespace xmlser {

using namespace soulng::util;
using namespace soulng::unicode;

class PointerMemberDetectorVisitor : public DefaultVisitor
{
public:
    PointerMemberDetectorVisitor();
    void Visit(PointerNode& node) override;
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

class HeaderGeneratorVisitor : public DefaultVisitor
{
public:
    HeaderGeneratorVisitor(CodeFormatter& formatter_, const std::string& includeGuard_);
    void Visit(SourceFileNode& node) override;
    void Visit(ClassNode& node) override;
    void Visit(MemberVariableNode& node) override;
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
    void Visit(TimePointNode& node) override;
    void Visit(DurationNode& node) override;
    void Visit(ClassIdNode& node) override;
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
    for (const std::string& includeDir : node.IncludeDirs())
    {
        formatter.WriteLine("#include " + includeDir);
    }
    formatter.WriteLine("#include <sngxml/dom/Element.hpp>");
    formatter.WriteLine("#include <soulng/util/Time.hpp>");
    formatter.WriteLine("#include <boost/uuid/uuid.hpp>");
    formatter.WriteLine("#include <chrono>");
    formatter.WriteLine("#include <vector>");
    formatter.WriteLine("#include <string>");
    formatter.WriteLine("#include <memory>");
    formatter.WriteLine("#include <stdint.h>");
    formatter.WriteLine();
    formatter.WriteLine("using date = soulng::util::Date;");
    formatter.WriteLine("using datetime = soulng::util::DateTime;");
    formatter.WriteLine("using time_point = std::chrono::steady_clock::time_point;");
    formatter.WriteLine("using duration = std::chrono::steady_clock::duration;");
    formatter.WriteLine("using uuid = boost::uuids::uuid;");
    DefaultVisitor::Visit(node);
    formatter.WriteLine();
    formatter.WriteLine("#endif // " + includeGuard);
}

void HeaderGeneratorVisitor::Visit(ClassNode& node)
{
    std::string api;
    if (!node.Api().empty())
    {
        api.append(1, ' ').append(node.Api());
    }
    std::string inheritance;
    if (!node.BaseClassId().empty())
    {
        inheritance.append(" : public ").append(node.BaseClassId());
    }
    formatter.WriteLine();
    formatter.WriteLine("class" + api + " " + node.Id() + inheritance);
    formatter.WriteLine("{");
    formatter.WriteLine("public:");
    formatter.IncIndent();
    formatter.WriteLine(node.Id() + "();");
    formatter.WriteLine(node.Id() + "(sngxml::dom::Element* element);");
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
    if (node.GetKey() == ClassNode::Key::baseClass)
    {
        formatter.WriteLine("virtual ~" + node.Id() + "();");
    }
    if (node.GetKey() == ClassNode::Key::baseClass)
    {
        formatter.Write("virtual ");
    }
    formatter.Write("std::unique_ptr<sngxml::dom::Element> ToXml(const std::string& fieldName) const");
    if (!node.BaseClassId().empty())
    {
        formatter.Write(" override");
    }
    formatter.WriteLine(";");
    formatter.DecIndent();
    formatter.WriteLine("public:");
    formatter.IncIndent();
    DefaultVisitor::Visit(node);
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
    InitializerGeneratorVisitor(CodeFormatter& formatter_, bool hasBaseInitializer_);
    void Visit(MemberVariableNode& node) override;
    bool HasMembers() const { return hasMembers; }
private:
    bool first;
    bool hasBaseInitializer;
    bool hasMembers;
    CodeFormatter& formatter;
};

InitializerGeneratorVisitor::InitializerGeneratorVisitor(CodeFormatter& formatter_, bool hasBaseInitializer_) : formatter(formatter_), first(true), hasBaseInitializer(hasBaseInitializer_), 
    hasMembers(false)
{
}

void InitializerGeneratorVisitor::Visit(MemberVariableNode& node)
{
    hasMembers = true;
    if (first)
    {
        if (hasBaseInitializer)
        {
            formatter.Write("    , ");
        }
        else
        {
            formatter.Write("    : ");
        }
        first = false;
    }
    else
    {
        formatter.Write(", ");
    }
    formatter.Write(node.Id() + "()");
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
    formatter.WriteLine("element->AppendChild(std::unique_ptr<sngxml::dom::Node>(sngxml::xmlser::ToXml(" + node.Id() + ", \"" + node.Id() + "\").release()));");
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

class SourceGeneratorVisitor : public DefaultVisitor
{
public:
    SourceGeneratorVisitor(CodeFormatter& formatter_, const std::string& headerFilePath_);
    void Visit(SourceFileNode& node) override;
    void Visit(ClassNode& node) override;
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
    formatter.WriteLine("#include <sngxml/serialization/XmlExport.hpp>");
    formatter.WriteLine("#include <sngxml/serialization/XmlImport.hpp>");
    formatter.WriteLine("#include <soulng/util/Unicode.hpp>");
    formatter.WriteLine();
    formatter.WriteLine("using namespace soulng::unicode;");
    formatter.WriteLine();
    DefaultVisitor::Visit(node);
}

void SourceGeneratorVisitor::Visit(ClassNode& node)
{
    formatter.WriteLine(node.Id() + "::" + node.Id() + "()");
    if (!node.BaseClassId().empty())
    {
        formatter.WriteLine("    : " + node.BaseClassId() + "()");
    }
    InitializerGeneratorVisitor defaultCtorInitializerGeneratorVisitor(formatter, !node.BaseClassId().empty());
    node.Accept(defaultCtorInitializerGeneratorVisitor);
    if (defaultCtorInitializerGeneratorVisitor.HasMembers())
    {
        formatter.WriteLine();
    }
    formatter.WriteLine("{");
    formatter.WriteLine("}");
    formatter.WriteLine();

    formatter.WriteLine(node.Id() + "::" + node.Id() + "(sngxml::dom::Element* element)");
    if (!node.BaseClassId().empty())
    {
        formatter.WriteLine("    : " + node.BaseClassId()  + "(sngxml::xmlser::GetXmlFieldElement(\"base\", element))");
    }
    InitializerGeneratorVisitor elementCtorInitializerGeneratorVisitor(formatter, !node.BaseClassId().empty());
    node.Accept(elementCtorInitializerGeneratorVisitor);
    if (elementCtorInitializerGeneratorVisitor.HasMembers())
    {
        formatter.WriteLine();
    }
    formatter.WriteLine("{");
    formatter.IncIndent();
    FromXmlMemberVariableGeneratorVisitor fromXmlGeneratorVisitor(formatter);
    node.Accept(fromXmlGeneratorVisitor);
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();

    if (node.GetKey() == sngxml::xmlser::ClassNode::Key::baseClass)
    {
        formatter.WriteLine(node.Id() + "::~" + node.Id() + "()");
        formatter.WriteLine("{");
        formatter.WriteLine("}");
        formatter.WriteLine();
    }

    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> " + node.Id() + "::ToXml(const std::string& fieldName) const");
    formatter.WriteLine("{");
    formatter.IncIndent();
    formatter.WriteLine("std::unique_ptr<sngxml::dom::Element> element(new sngxml::dom::Element(ToUtf32(fieldName)));");
    if (!node.BaseClassId().empty())
    {
        formatter.WriteLine("element->AppendChild(std::unique_ptr<sngxml::dom::Node>(" + node.BaseClassId() + "::ToXml(\"base\").release()));");
    }
    ToXmlMemberVariableGeneratorVisitor toXmlMemberVariableGeneratorVisitor(formatter);
    node.Accept(toXmlMemberVariableGeneratorVisitor);
    formatter.WriteLine("return element;");
    formatter.DecIndent();
    formatter.WriteLine("}");
    formatter.WriteLine();
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