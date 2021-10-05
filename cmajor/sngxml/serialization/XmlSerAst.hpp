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
class CppBlockNode;

class SNGXML_SERIALIZATION_API Node
{
public:
    virtual ~Node();
    virtual void Accept(Visitor& visitor) = 0;
    virtual bool IsNamespaceNode() const { return false; }
    virtual bool ContainsNamespaces() const { return false; }
    virtual bool IsTemplateIdNode() const { return false; }
    virtual bool IsArrayNode() const { return false; }
};

class SNGXML_SERIALIZATION_API IncludeDirectiveNode : public Node
{
public:
    IncludeDirectiveNode(const std::string& fileTag_, const std::string& filePath_);
    void Accept(Visitor& visitor) override;
    const std::string& FileTag() const { return fileTag; }
    const std::string& FilePath() const { return filePath; }
private:
    std::string fileTag;
    std::string filePath;
};

class SNGXML_SERIALIZATION_API ForwardClassDeclarationNode : public Node
{
public:
    ForwardClassDeclarationNode(const std::string& classId_);
    void Accept(Visitor& visitor) override;
    const std::string& ClassId() const { return classId; }
private:
    std::string classId;
};

class SNGXML_SERIALIZATION_API AliasDeclarationNode : public Node
{
public:
    AliasDeclarationNode(const std::string& name_, const std::string& subject_);
    void Accept(Visitor& visitor) override;
    const std::string& Name() const { return name; }
    const std::string& Subject() const { return subject; }
private:
    std::string name;
    std::string subject;
};

class SNGXML_SERIALIZATION_API BaseClassNode : public Node
{
public:
    BaseClassNode(const std::string& id_);
    const std::string& Id() const { return id; }
    virtual bool IsExternal() const { return false; }
    virtual bool IsInternal() const { return false; }
private:
    std::string id;
};

class SNGXML_SERIALIZATION_API ExternalBaseClassNode : public BaseClassNode
{
public:
    ExternalBaseClassNode(const std::string& id_);
    void Accept(Visitor& visitor) override;
    bool IsExternal() const override { return true; }
};

class SNGXML_SERIALIZATION_API InternalBaseClassNode : public BaseClassNode
{
public:
    InternalBaseClassNode(const std::string& id_);
    void Accept(Visitor& visitor) override;
    bool IsInternal() const override { return true; }
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

class SNGXML_SERIALIZATION_API TimestampNode : public TypeNode
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
    bool IsArrayNode() const override { return true; }
    void Accept(Visitor& visitor) override;
    TypeNode* Type() const { return type.get(); }
private:
    std::unique_ptr<TypeNode> type;
};

class SNGXML_SERIALIZATION_API TemplateIdNode : public TypeNode
{
public:
    TemplateIdNode(const std::string& typeId_, const std::string& typeParamId_);
    bool IsTemplateIdNode() const override { return true; }
    void Accept(Visitor& visitor);
    const std::string& TypeId() const { return typeId; }
    const std::string& TypeParamId() const { return typeParamId; }
private:
    std::string typeId;
    std::string typeParamId;
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

class SNGXML_SERIALIZATION_API NamespaceNode : public Node
{
public:
    NamespaceNode(const std::string& id_);
    NamespaceNode(const NamespaceNode&) = delete;
    NamespaceNode(NamespaceNode&&) = delete;
    NamespaceNode& operator=(const NamespaceNode&) = delete;
    NamespaceNode& operator=(NamespaceNode&&) = delete;
    void Accept(Visitor& visitor) override;
    const std::string& Id() const { return id; }
    bool IsNamespaceNode() const override { return true; }
    void AddNode(Node* node);
    const std::vector<std::unique_ptr<Node>>& Nodes() const { return nodes; }
    bool ContainsNamespaces() const override;
private:
    std::string id;
    std::vector<std::unique_ptr<Node>> nodes;
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
    void AddBaseClass(BaseClassNode* baseClass);
    BaseClassNode* InternalBaseClass() const { return internalBaseClass; }
    std::vector<BaseClassNode*> ExternalBaseClasses() const;
    void AddMemberVariable(MemberVariableNode* memberVariable);
    void AddCppBlock(CppBlockNode* cppBlock);
    const std::vector<std::unique_ptr<Node>>& Nodes() const { return nodes; }
private:
    Key key;
    std::string api;
    std::string id;
    std::vector<std::unique_ptr<BaseClassNode>> bases;
    BaseClassNode* internalBaseClass;
    std::vector<std::unique_ptr<Node>> nodes;
};

class SNGXML_SERIALIZATION_API EnumConstantNode : public Node
{
public:
    EnumConstantNode(const std::string& id_);
    EnumConstantNode(const EnumConstantNode&) = delete;
    EnumConstantNode(EnumConstantNode&&) = delete;
    EnumConstantNode& operator=(const EnumConstantNode&) = delete;
    EnumConstantNode& operator=(EnumConstantNode&&) = delete;
    const std::string& Id() const { return id; }
    void Accept(Visitor& visitor) override;
private:
    std::string id;
};

class SNGXML_SERIALIZATION_API EnumTypeNode : public Node
{
public:
    EnumTypeNode(const std::string& id_);
    EnumTypeNode(const EnumTypeNode&) = delete;
    EnumTypeNode(EnumTypeNode&&) = delete;
    EnumTypeNode& operator=(const EnumTypeNode&) = delete;
    EnumTypeNode& operator=(EnumTypeNode&&) = delete;
    const std::string& Id() const { return id; }
    void AddEnumConstant(EnumConstantNode* enumConstant);
    const std::vector<std::unique_ptr<EnumConstantNode>>& EnumConstants() const { return enumConstants; }
    void Accept(Visitor& visitor) override;
private:
    std::string id;
    std::vector<std::unique_ptr<EnumConstantNode>> enumConstants;
};

class SNGXML_SERIALIZATION_API CppBlockNode : public Node
{
public:
    CppBlockNode(const std::string& cppText_);
    void Accept(Visitor& visitor) override;
    const std::string& CppText() const { return cppText; }
    bool Source() const { return source; }
    void SetSource() { source = true; }
private:
    std::string cppText;
    bool source;
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
    void AddIncludeDirective(IncludeDirectiveNode* includeDirectiveNode);
    const std::vector<std::unique_ptr<IncludeDirectiveNode>>& IncludeDirectives() const { return includeDirectives; }
    NamespaceNode* GlobalNs();
private:
    std::vector<std::unique_ptr<IncludeDirectiveNode>> includeDirectives;
    NamespaceNode globalNs;
};

} } // namespace sngxml::xmlser

#endif // SNGXML_XMLSER_AST_INCLUDED
