// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
#include <cmajor/symbols/ClassTypeSymbol.hpp>

namespace cmajor { namespace symbols {

enum class ClassTemplateSpecializationFlags : uint8_t
{
    none = 0,
    prototype = 1 << 0,
    constraintChecked = 1 << 1
};

inline ClassTemplateSpecializationFlags operator|(ClassTemplateSpecializationFlags left, ClassTemplateSpecializationFlags right)
{
    return ClassTemplateSpecializationFlags(uint8_t(left) | uint8_t(right));
}

inline ClassTemplateSpecializationFlags operator&(ClassTemplateSpecializationFlags left, ClassTemplateSpecializationFlags right)
{
    return ClassTemplateSpecializationFlags(uint8_t(left) & uint8_t(right));
}

inline ClassTemplateSpecializationFlags operator~(ClassTemplateSpecializationFlags flags)
{
    return ClassTemplateSpecializationFlags(~uint8_t(flags));
}

std::u32string MakeClassTemplateSpecializationName(ClassTypeSymbol* classTemplate, const std::vector<TypeSymbol*>& templateArgumentTypes);

class SYMBOLS_API ClassTemplateSpecializationSymbol : public ClassTypeSymbol
{
public:
    ClassTemplateSpecializationSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassTemplateSpecializationSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, std::u32string& name_, ClassTypeSymbol* classTemplate_, const std::vector<TypeSymbol*>& templateArgumentTypes_);
    ~ClassTemplateSpecializationSymbol();
    std::u32string SimpleName() const override;
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    bool IsPrototypeTemplateSpecialization() const override;
    void* IrType(Emitter& emitter) override;
    ClassTypeSymbol* GetClassTemplate() { return classTemplate; }
    const ContainerScope* GetArrowScope() const override;
    ContainerScope* GetArrowScope() override;
    const std::vector<TypeSymbol*>& TemplateArgumentTypes() const { return templateArgumentTypes; }
    std::vector<TypeSymbol*>& TemplateArgumentTypes() { return templateArgumentTypes; }
    void SetGlobalNs(std::unique_ptr<Node>&& globalNs_);
    Node* GlobalNs() { return globalNs.get(); }
    void SetFileScope(FileScope* fileScope_);
    FileScope* ReleaseFileScope();
    void SetPrototype() { SetFlag(ClassTemplateSpecializationFlags::prototype); }
    bool IsPrototype() const { return GetFlag(ClassTemplateSpecializationFlags::prototype); }
    void SetConstraintChecked() { SetFlag(ClassTemplateSpecializationFlags::constraintChecked); }
    bool IsConstraintChecked() { return GetFlag(ClassTemplateSpecializationFlags::constraintChecked); }
    void SetFlag(ClassTemplateSpecializationFlags flag) { flags = flags | flag; }
    bool GetFlag(ClassTemplateSpecializationFlags flag) const { return (flags & flag) != ClassTemplateSpecializationFlags::none;  }
    void ResetFlag(ClassTemplateSpecializationFlags flag) { flags = flags & ~flag; }
    TypeSymbol* UnifyTemplateArgumentType(SymbolTable& symbolTable, const std::unordered_map<TemplateParameterSymbol*, TypeSymbol*>& templateParameterMap, const Span& span, const boost::uuids::uuid& moduleId) override;
    std::u32string Id() const override;
    const char* ClassName() const override { return "ClassTemplateSpecializationSymbol"; }
    void Check() override;
    std::string GetSymbolCategoryStr() const override { return "CL"; }
    std::string GetSymbolCategoryDescription() const override { return "class"; }
private:
    ClassTypeSymbol* classTemplate;
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Node> globalNs;
    std::unique_ptr<FileScope> fileScope;
    ClassTemplateSpecializationFlags flags;
};

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_CLASS_TEMPLATE_SPECIALIZATION_SYMBOL_INCLUDED
