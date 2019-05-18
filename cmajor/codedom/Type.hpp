// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_TYPE_INCLUDED
#define CMAJOR_CODEDOM_TYPE_INCLUDED
#include <cmajor/codedom/Object.hpp>

namespace cmajor { namespace codedom {

class CODEDOM_API DeclSpecifier : public CppObject
{
public:
    DeclSpecifier(const std::u32string& name_);
};

class CODEDOM_API Typedef : public DeclSpecifier
{
public:
    Typedef();
    void Accept(Visitor& visitor) override;
};

class CODEDOM_API TypeSpecifier : public DeclSpecifier
{
public:
    TypeSpecifier(const std::u32string& name_);
    void Accept(Visitor& visitor) override;
};

class CODEDOM_API Const : public TypeSpecifier
{
public:
    Const();
};

class CODEDOM_API Volatile: public TypeSpecifier
{
public:
    Volatile();
};

class CODEDOM_API TypeName : public TypeSpecifier
{
public:
    TypeName(const std::u32string& name_);
    void AddTemplateArgument(CppObject* templateArgument);
    bool IsTemplate() const { return isTemplate; }
    bool& IsTemplate() { return isTemplate; }
    std::u32string ToString() const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    bool isTemplate;
    std::vector<CppObject*> templateArguments;
};

class CODEDOM_API Type : public CppObject
{
public:
    Type();
    void Add(TypeSpecifier* typeSpecifier);
    std::u32string ToString() const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::vector<TypeSpecifier*> typeSpecifiers;
};

typedef std::shared_ptr<Type> TypePtr;

class CODEDOM_API TypeId : public CppObject
{
public:
    typedef std::vector<TypeSpecifier*> TypeSpecifierVec;
    TypeId();
    void Add(TypeSpecifier* typeSpecifier);
    const TypeSpecifierVec& TypeSpecifiers() const { return typeSpecifiers; }
    TypeSpecifierVec& TypeSpecifiers() { return typeSpecifiers; }
    const std::u32string& Declarator() const { return declarator; }
    std::u32string& Declarator() { return declarator; }
    std::u32string ToString() const override;
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    TypeSpecifierVec typeSpecifiers;
    std::u32string declarator;
};

class CODEDOM_API StorageClassSpecifier : public DeclSpecifier
{
public:
    StorageClassSpecifier(const std::u32string& name_): DeclSpecifier(name_) {}
    void Accept(Visitor& visitor) override;
};

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_TYPE_INCLUDED