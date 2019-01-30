// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_CODEDOM_DECLARATION_INCLUDED
#define CMAJOR_CODEDOM_DECLARATION_INCLUDED
#include <cmajor/codedom/Object.hpp>

namespace cmajor { namespace codedom {

class CODEDOM_API AssignInit : public CppObject
{
public:
    AssignInit(CppObject* assignmentExpr_);
    void Add(AssignInit* subInit);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    CppObject* assignmentExpr;
    std::vector<AssignInit*> subInits;
};

class CODEDOM_API Initializer : public CppObject
{
public:
    Initializer(AssignInit* assignInit_, const std::vector<CppObject*>& expressionList_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    AssignInit* assignInit;
    std::vector<CppObject*> expressionList;
};

class CODEDOM_API InitDeclarator : public CppObject
{
public:
    InitDeclarator(const std::u32string& declarator_, Initializer* initializer_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string declarator;
    Initializer* initializer;
};

class CODEDOM_API InitDeclaratorList : public CppObject
{
public:
    InitDeclaratorList();
    void Add(InitDeclarator* initDeclarator);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::vector<InitDeclarator*> initDeclarators;
};

class DeclSpecifier;

class CODEDOM_API SimpleDeclaration : public CppObject
{
public:
    SimpleDeclaration();
    void Add(DeclSpecifier* declSpecifier);
    void SetInitDeclaratorList(InitDeclaratorList* initDeclaratorList_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::vector<DeclSpecifier*> declSpecifiers;
    InitDeclaratorList* initDeclaratorList;
};

class CODEDOM_API UsingObject : public CppObject
{
public:
    UsingObject(const std::u32string& name_);
    const std::u32string& FileAttr() const { return fileAttr; }
    void SetFileAttr(const std::u32string& fileAttr_) { fileAttr = fileAttr_; }
private:
    std::u32string fileAttr;
};

class CODEDOM_API NamespaceAlias : public UsingObject
{
public:
    NamespaceAlias(const std::u32string& aliasName_, const std::u32string& namespaceName_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string aliasName;
    std::u32string namespaceName;
};

class CODEDOM_API UsingDeclaration : public UsingObject
{
public:
    UsingDeclaration(const std::u32string& usingId_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string usingId;
};

class CODEDOM_API UsingDirective : public UsingObject
{
public:
    UsingDirective(const std::u32string& usingNs_);
    void Print(CodeFormatter& formatter) override;
    void Accept(Visitor& visitor) override;
private:
    std::u32string usingNs;
};

inline std::vector<CppObject*> GetEmptyVector() { return std::vector<CppObject*>(); }

} } // namespace cmajor::codedom

#endif // CMAJOR_CODEDOM_DECLARATION_INCLUDED
