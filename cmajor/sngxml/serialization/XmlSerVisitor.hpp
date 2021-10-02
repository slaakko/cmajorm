// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XMLSER_VISITOR_INCLUDED
#define SNGXML_XMLSER_VISITOR_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>

namespace sngxml { namespace xmlser {

class BoolNode;
class SByteNode;
class ByteNode;
class ShortNode;
class UShortNode;
class IntNode;
class UIntNode;
class LongNode;
class ULongNode;
class FloatNode;
class DoubleNode;
class CharNode;
class WCharNode;
class UCharNode;
class UuidNode;
class StringNode;
class WStringNode;
class UStringNode;
class DateNode;
class DateTimeNode;
class TimestampNode;
class TimePointNode;
class DurationNode;
class ClassIdNode;
class PointerNode;
class ArrayNode;
class MemberVariableNode;
class NamespaceNode;
class ClassNode;
class TemplateIdNode;
class EnumTypeNode;
class EnumConstantNode;
class SourceFileNode;
class ForwardClassDeclarationNode;
class IncludeDirectiveNode;
class CppBlockNode;

class Visitor
{
public:
    virtual ~Visitor();
    virtual void Visit(BoolNode& node) {}
    virtual void Visit(SByteNode& node) {}
    virtual void Visit(ByteNode& node) {}
    virtual void Visit(ShortNode& node) {}
    virtual void Visit(UShortNode& node) {}
    virtual void Visit(IntNode& node) {}
    virtual void Visit(UIntNode& node) {}
    virtual void Visit(LongNode& node) {}
    virtual void Visit(ULongNode& node) {}
    virtual void Visit(FloatNode& node) {}
    virtual void Visit(DoubleNode& node) {}
    virtual void Visit(CharNode& node) {}
    virtual void Visit(WCharNode& node) {}
    virtual void Visit(UCharNode& node) {}
    virtual void Visit(UuidNode& node) {}
    virtual void Visit(StringNode& node) {}
    virtual void Visit(WStringNode& node) {}
    virtual void Visit(UStringNode& node) {}
    virtual void Visit(DateNode& node) {}
    virtual void Visit(DateTimeNode& node) {}
    virtual void Visit(TimestampNode& node) {}
    virtual void Visit(TimePointNode& node) {}
    virtual void Visit(DurationNode& node) {}
    virtual void Visit(ClassIdNode& node) {}
    virtual void Visit(PointerNode& node) {}
    virtual void Visit(ArrayNode& node) {}
    virtual void Visit(MemberVariableNode& node) {}
    virtual void Visit(NamespaceNode& node) {}
    virtual void Visit(ClassNode& node) {}
    virtual void Visit(TemplateIdNode& node) {}
    virtual void Visit(EnumTypeNode& node) {}
    virtual void Visit(EnumConstantNode& node) {}
    virtual void Visit(SourceFileNode& node) {}
    virtual void Visit(ForwardClassDeclarationNode& node) {}
    virtual void Visit(IncludeDirectiveNode& node) {}
    virtual void Visit(CppBlockNode& node) {}
};

class DefaultVisitor : public Visitor
{
public:
    void Visit(SourceFileNode& node) override;
    void Visit(NamespaceNode& node) override;
    void Visit(ClassNode& node) override;
    void Visit(MemberVariableNode& node) override;
    void Visit(EnumTypeNode& node) override;
    void Visit(ArrayNode& node) override;
    void Visit(PointerNode& node) override;
};

} } // namespace sngxml::xmlser

#endif // SNGXML_XMLSER_VISITOR_INCLUDED
