// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_PARSING_OBJECT_INCLUDED
#define CMAJOR_PARSING_PARSING_OBJECT_INCLUDED
#include <cmajor/parsing/ParsingApi.hpp>
#include <cmajor/parsing/Scanner.hpp>
#include <memory>

namespace cmajor { namespace parsing {

class Visitor;
class Scope;

enum class ObjectKind : uint8_t
{
    none = 0,
    grammar = 1 << 0, 
    ns = 1 << 1, 
    usingObject = 1 << 2, 
    parser = 1 << 3, 
    parsingDomain = 1 << 4, 
    rule = 1 << 5, 
    ruleLink = 1 << 6, 
    scope = 1 << 7,
    parent = grammar | ns | scope
};

constexpr inline ObjectKind operator|(ObjectKind left, ObjectKind right)
{
    return  ObjectKind(uint8_t(left) | uint8_t(right));
}

constexpr inline ObjectKind operator&(ObjectKind left, ObjectKind right)
{
    return  ObjectKind(uint8_t(left) & uint8_t(right));
}

class PARSING_API ParsingObject
{
public:
    static const int external = -1;
    ParsingObject(const std::u32string& name_, ObjectKind kind_);
    ParsingObject(const std::u32string& name_, Scope* enclosingScope_, ObjectKind kind_);
    ParsingObject(const ParsingObject&) = delete;
    ParsingObject& operator=(const ParsingObject&) = delete;
    virtual ~ParsingObject();
    virtual void Accept(Visitor& visitor) = 0;
    void Own(ParsingObject* object);
    bool IsOwned() const { return isOwned; }
    void SetOwned() { isOwned = true; }
    void ResetOwned() { isOwned = false; }
    void SetOwner(int owner_) { owner = owner_; }
    int Owner() const { return owner; }
    void SetExternal() { owner = external; }
    bool IsExternal() const { return owner == external; }
    const std::u32string& Name() const { return name; }
    void SetName(const std::u32string& name_) { name = name_; }
    virtual std::u32string FullName() const;
    Scope* EnclosingScope() const { return enclosingScope; }
    void SetEnclosingScope(Scope* enclosingScope_) { enclosingScope = enclosingScope_; }
    void SetScope(Scope* scope_);
    Scope* GetScope() const { if (scope == nullptr) return enclosingScope; else return scope; }
    virtual bool IsActionParser() const { return false; }
    virtual bool IsNonterminalParser() const { return false; }
    virtual bool IsRule() const { return false; }
    virtual bool IsRuleLink() const { return false; }
    virtual bool IsNamespace() const { return false; }
    virtual void AddToScope();
    void SetSpan(const Span& span_) { span = span_; }
    const Span& GetSpan() const { return span; }
    ObjectKind Kind() const { return kind; }
private:
    ObjectKind kind;
    std::u32string name;
    std::vector<std::unique_ptr<ParsingObject>> ownedObjects;
    int owner;
    bool isOwned;
    Scope* enclosingScope;
    Scope* scope;
    Span span;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_PARSING_OBJECT_INCLUDED
