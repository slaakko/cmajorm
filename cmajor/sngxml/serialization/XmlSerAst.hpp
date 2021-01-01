// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SNGXML_XMLSER_AST_INCLUDED
#define SNGXML_XMLSER_AST_INCLUDED
#include <sngxml/serialization/XmlSerApi.hpp>
#include <memory>
#include <vector>
#include <string>

namespace sngxml { namespace xmlser {

class Visitor;

class SNGXML_SERIALIZATION_API Node
{
public:
    virtual ~Node();
    virtual void Accept(Visitor& visitor) = 0;
};

class SNGXML_SERIALIZATION_API TypeNode : public Node
{
};

class SNGXML_SERIALIZATION_API BoolNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API SByteNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API ByteNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API ShortNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API UShortNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API IntNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API UIntNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API LongNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API ULongNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API FloatNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API DoubleNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API CharNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API WCharNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API UCharNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API UuidNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API StringNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API WStringNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API UStringNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API DateNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API DateTimeNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API TimePointNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API DurationNode : public TypeNode
{
public:
    void Accept(Visitor& visitor) override;
};

class SNGXML_SERIALIZATION_API ClassIdNode : public TypeNode
{
public:
    ClassIdNode(const std::string& classId_);
    void Accept(Visitor& visitor) override;
    const std::string& ClassId() const { return classId; }
private:
    std::string classId;
};

class SNGXML_SERIALIZATION_API PointerNode : public TypeNode
{
public:
    PointerNode(TypeNode* type_);
    void Accept(Visitor& visitor) override;
    TypeNode* Type() const { return type.get(); }
private:
    std::unique_ptr<TypeNode> type;
};

class SNGXML_SERIALIZATION_API ArrayNode : public TypeNode
{
public:
    ArrayNode(TypeNode* type_);
    void Accept(Visitor& visitor) override;
    TypeNode* Type() const { return type.get(); }
private:
    std::unique_ptr<TypeNode> type;
};

class SNGXML_SERIALIZATION_API MemberVariableNode : public Node
{
public:
    MemberVariableNode(TypeNode* type_, const std::string& id_);
    void Accept(Visitor& visitor) override;
    TypeNode* Type() const { return type.get(); }
    const std::string& Id() const { return id; }
private:
    std::unique_ptr<TypeNode> type;
    std::string id;
};

class SNGXML_SERIALIZATION_API ClassNode : public Node
{
public:
    enum class Key
    {
        class_, baseClass
    };
    ClassNode(const ClassNode&) = delete;
    ClassNode(ClassNode&&) = delete;
    ClassNode& operator=(const ClassNode&) = delete;
    ClassNode& operator=(ClassNode&&) = delete;
    ClassNode(Key key_, const std::string& api_, const std::string& id_);
    void Accept(Visitor& visitor) override;
    const Key& GetKey() const { return key; }
    const std::string& Api() const { return api; }
    const std::string& Id() const { return id; }
    void SetBaseClassId(const std::string& baseClassId_);
    const std::string& BaseClassId() const { return baseClassId; }
    void AddMemberVariable(MemberVariableNode* memberVariable);
    const std::vector<std::unique_ptr<MemberVariableNode>>& MemberVariables() const { return memberVariables; }
private:
    Key key;
    std::string api;
    std::string id;
    std::string baseClassId;
    std::vector<std::unique_ptr<MemberVariableNode>> memberVariables;
};

class SNGXML_SERIALIZATION_API SourceFileNode : public Node
{
public:
    SourceFileNode();
    SourceFileNode(const SourceFileNode&) = delete;
    SourceFileNode(SourceFileNode&&) = delete;
    SourceFileNode& operator=(const SourceFileNode&) = delete;
    SourceFileNode& operator=(SourceFileNode&&) = delete;
    void Accept(Visitor& visitor) override;
    void AddInclude(const std::string& includeDir);
    const std::vector<std::string>& IncludeDirs() const { return includeDirs; }
    void AddClass(ClassNode* classNode);
    const std::vector<std::unique_ptr<ClassNode>>& Classes() const { return classes; }
private:
    std::vector<std::string> includeDirs;
    std::vector<std::unique_ptr<ClassNode>> classes;
};

} } // namespace sngxml::xmlser

#endif // SNGXML_XMLSER_AST_INCLUDED
