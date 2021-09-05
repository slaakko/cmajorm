// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_CLASS_TYPE_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_CLASS_TYPE_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Constant.hpp>

namespace cmajor { namespace symbols {

class InterfaceTypeSymbol;
class TemplateParameterSymbol;
class MemberVariableSymbol;
class StaticConstructorSymbol;
class ConstructorSymbol;
class DestructorSymbol;
class MemberFunctionSymbol;
class ClassTemplateSpecializationSymbol;

class SYMBOLS_API ClassGroupTypeSymbol : public TypeSymbol
{
public:
    ClassGroupTypeSymbol(const Span& span_, const boost::uuids::uuid& moduleId_, const std::u32string& name_);
    bool IsExportSymbol() const override { return false; }
    std::string TypeString() const override { return "class_group"; }
    bool IsInComplete() const override { return true; }
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    void AddClass(ClassTypeSymbol* classTypeSymbol);
    void RemoveClass(ClassTypeSymbol* classTypeSymbol);
    const ContainerScope* GetTypeScope() const override;
    ContainerScope* GetTypeScope() override;
    bool IsEmpty() const;
    ClassTypeSymbol* GetClass(int arity) const;
    bool HasProjectMembers() const override;
    void AppendChildElements(sngxml::dom::Element* element, TypeMap& typeMap) const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "ClassGroupTypeSymbol"; }
    void Check() override;
    std::string GetSymbolCategoryStr() const override { return "CL"; }
    std::string GetSymbolCategoryDescription() const override { return "class"; }
    std::string GetSymbolHelp() const override;
    bool IsValidCCClassGroup(Module* module, FunctionSymbol* fromFunction) const;
    std::vector<Symbol*> GetParamHelpSymbols() const override;
private:
    std::unordered_map<int, ClassTypeSymbol*> arityClassMap;
};

enum class ClassTypeSymbolFlags : uint16_t
{
    none = 0,
    abstract_ = 1 << 0,
    polymorphic = 1 << 1,
    vmtInitialized = 1 << 2,
    imtsInitialized = 1 << 3,
    layoutsComputed = 1 << 4,
    statementsNotBound = 1 << 5,
    recursiveComputed = 1 << 6,
    recursive = 1 << 7,
    vmtEmitted = 1 << 8
};

inline ClassTypeSymbolFlags operator|(ClassTypeSymbolFlags left, ClassTypeSymbolFlags right)
{
    return ClassTypeSymbolFlags(uint16_t(left) | uint16_t(right));
}

inline ClassTypeSymbolFlags operator&(ClassTypeSymbolFlags left, ClassTypeSymbolFlags right)
{
    return ClassTypeSymbolFlags(uint16_t(left) & uint16_t(right));
}

inline ClassTypeSymbolFlags operator~(ClassTypeSymbolFlags operand)
{
    return ClassTypeSymbolFlags(~uint16_t(operand));
}

SYMBOLS_API int32_t GetClassIdVmtIndexOffset();     // 128-bit class id, was 64-bit before
SYMBOLS_API int32_t GetTypeIdVmtIndexOffset();      // 16-byte type id
SYMBOLS_API int32_t GetClassNameVmtIndexOffset();   // class name pointer
SYMBOLS_API int32_t GetImtsVmtIndexOffset();        // interface method table pointer
SYMBOLS_API int32_t GetFunctionVmtIndexOffset();    // virtual method table

/* Indeces for LLVM backend:

const int32_t classIdVmtIndexOffset = 0;    // 128-bit class id, was 64-bit before
const int32_t typeIdVmtIndexOffset = 2;     // 16-byte type id
const int32_t classNameVmtIndexOffset = 4;  // class name pointer
const int32_t imtsVmtIndexOffset = 5;       // interface method table pointer
const int32_t functionVmtIndexOffset = 6;   // virtual method table

*/

class SYMBOLS_API ClassTypeSymbol : public TypeSymbol
{
public:
    ClassTypeSymbol(const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    ClassTypeSymbol(SymbolType symbolType_, const Span& span_, const boost::uuids::uuid& sourceModuleId_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    const NodeList<Node>& UsingNodes() const { return usingNodes; }
    ClassNode* GetClassNode() { return classNode.get(); }
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void AddMember(Symbol* member) override;
    bool IsClassTypeSymbol() const override { return true; }
    bool IsParentSymbol() const override { return true; }
    std::string TypeString() const override { return "class"; }
    std::u32string SimpleName() const override { return groupName; }
    std::string GetSpecifierStr() const override;
    bool HasNontrivialDestructor() const override;
    void Accept(SymbolCollector* collector) override;
    const ContainerScope* GetArrowScope() const override;
    ContainerScope* GetArrowScope() override;
    void CollectMembers(SymbolCollector* collector);
    void Dump(CodeFormatter& formatter) override;
    bool IsRecursive(TypeSymbol* type, std::unordered_set<boost::uuids::uuid, boost::hash<boost::uuids::uuid>>& tested) override;
    virtual bool IsPrototypeTemplateSpecialization() const { return false; }
    bool CompletelyBound() const override { return IsBound() && !StatementsNotBound(); }
    void CreateDestructorSymbol();
    const std::u32string& GroupName() const { return groupName; }
    void SetGroupName(const std::u32string& groupName_);
    std::u32string CodeName() const override { return groupName; }
    int MinArity() const { return minArity; }
    int MaxArity() const { return templateParameters.size(); }
    void ComputeMinArity();
    const ClassTypeSymbol* BaseClass() const { return baseClass; }
    ClassTypeSymbol* BaseClass() { return baseClass; }
    void SetBaseClass(ClassTypeSymbol* baseClass_) { baseClass = baseClass_; }
    bool HasBaseClass(ClassTypeSymbol* cls) const;
    bool HasBaseClass(ClassTypeSymbol* cls, uint8_t& distance) const;
    const std::vector<InterfaceTypeSymbol*>& ImplementedInterfaces() const { return implementedInterfaces; }
    void AddImplementedInterface(InterfaceTypeSymbol* interfaceTypeSymbol);
    const std::vector<TemplateParameterSymbol*>& TemplateParameters() const { return templateParameters; }
    bool IsClassTemplate() const { return !templateParameters.empty(); }
    void CloneUsingNodes(const std::vector<Node*>& usingNodes_);
    void SetSpecifiers(Specifiers specifiers);
    void ComputeName();
    void ComputeMangledName() override;
    bool IsPolymorphicType() const override { return IsPolymorphic(); }
    void SetConstraint(ConstraintNode* constraint_) { constraint.reset(constraint_); }
    ConstraintNode* Constraint() { return constraint.get(); }
    bool IsLiteralClassType() const;
    StaticConstructorSymbol* StaticConstructor() { return staticConstructor; }
    ConstructorSymbol* DefaultConstructor() { return defaultConstructor; }
    void SetDefaultConstructor(ConstructorSymbol* defaultConstructor_) { defaultConstructor = defaultConstructor_; }
    ConstructorSymbol* CopyConstructor() { return copyConstructor; }
    void SetCopyConstructor(ConstructorSymbol* copyConstructor_) { copyConstructor = copyConstructor_; }
    ConstructorSymbol* MoveConstructor() { return moveConstructor; }
    void SetMoveConstructor(ConstructorSymbol* moveConstructor_) { moveConstructor = moveConstructor_; }
    DestructorSymbol* Destructor() { return destructor; }
    MemberFunctionSymbol* CopyAssignment() { return copyAssignment; }
    void SetCopyAssignment(MemberFunctionSymbol* copyAssignment_) { copyAssignment = copyAssignment_; }
    MemberFunctionSymbol* MoveAssignment() { return moveAssignment; }
    void SetMoveAssignment(MemberFunctionSymbol* moveAssignment_) { moveAssignment = moveAssignment_; }
    void SetSpecialMemberFunctions();
    const std::vector<MemberVariableSymbol*>& MemberVariables() const { return memberVariables; }
    const std::vector<MemberVariableSymbol*>& StaticMemberVariables() const { return staticMemberVariables; }
    const std::vector<MemberFunctionSymbol*>& MemberFunctions() const { return memberFunctions; }
    MemberVariableSymbol* InitializedVar() { return initializedVar.get(); }
    void SetInitializedVar(MemberVariableSymbol* initializedVar_);
    bool IsAbstract() const { return GetFlag(ClassTypeSymbolFlags::abstract_); }
    void SetAbstract() { SetFlag(ClassTypeSymbolFlags::abstract_); }
    bool IsPolymorphic() const { return GetFlag(ClassTypeSymbolFlags::polymorphic); }
    void SetPolymorphic() { SetFlag(ClassTypeSymbolFlags::polymorphic); }
    bool IsVmtInitialized() const { return GetFlag(ClassTypeSymbolFlags::vmtInitialized);  }
    void SetVmtInitialized() { SetFlag(ClassTypeSymbolFlags::vmtInitialized); }
    bool IsImtsInitialized() const { return GetFlag(ClassTypeSymbolFlags::imtsInitialized); }
    void SetImtsInitialized() { SetFlag(ClassTypeSymbolFlags::imtsInitialized); }
    bool IsLayoutsComputed() const { return GetFlag(ClassTypeSymbolFlags::layoutsComputed); }
    void SetLayoutsComputed() { SetFlag(ClassTypeSymbolFlags::layoutsComputed); }
    bool StatementsNotBound() const { return GetFlag(ClassTypeSymbolFlags::statementsNotBound); }
    void SetStatementsNotBound() { SetFlag(ClassTypeSymbolFlags::statementsNotBound); }
    void ResetStatementsNotBound() { ResetFlag(ClassTypeSymbolFlags::statementsNotBound); }
    bool RecursiveComputed() const { return GetFlag(ClassTypeSymbolFlags::recursiveComputed); }
    void SetRecursiveComputed() { SetFlag(ClassTypeSymbolFlags::recursiveComputed); }
    bool IsRecursive();
    bool Recursive() const { return GetFlag(ClassTypeSymbolFlags::recursive); }
    void SetRecursive() { SetFlag(ClassTypeSymbolFlags::recursive); }
    bool VmtEmitted() const { return GetFlag(ClassTypeSymbolFlags::vmtEmitted); }
    void SetVmtEmitted() { SetFlag(ClassTypeSymbolFlags::vmtEmitted); }
    ClassTypeSymbolFlags GetClassTypeSymbolFlags() const { return flags; }
    bool GetFlag(ClassTypeSymbolFlags flag) const { return (flags & flag) != ClassTypeSymbolFlags::none; }
    void SetFlag(ClassTypeSymbolFlags flag) { flags = flags | flag; }
    void ResetFlag(ClassTypeSymbolFlags flag) { flags = flags & (~flag); }
    void InitVmt();
    void InitImts();
    void CreateLayouts();
    const std::vector<TypeSymbol*>& ObjectLayout() const { return objectLayout; }
    const std::vector<FunctionSymbol*>& Vmt() const { return vmt; }
    void* IrType(Emitter& emitter) override;
    void* CreateDefaultIrValue(Emitter& emitter) override;
    void* CreateDIType(Emitter& emitter) override; 
    void* CreateDIForwardDeclaration(Emitter& emitter);
    void* VmtObject(Emitter& emitter, bool create);
    void* VmtPtrType(Emitter& emitter);
    std::string VmtObjectName(Emitter& emitter);
    std::string VmtObjectNameStr();
    std::string ImtArrayObjectName(Emitter& emitter);
    std::string ImtObjectName(int index);
    int32_t VmtPtrIndex() const { return vmtPtrIndex; }
    ClassTypeSymbol* VmtPtrHolderClass();
    void* StaticObject(Emitter& emitter, bool create);
    void* StaticObjectType(Emitter& emitter);
    std::string StaticObjectName(Emitter& emitter);
    void SetPrototype(ClassTemplateSpecializationSymbol* prototype_) { prototype = prototype_; }
    ClassTemplateSpecializationSymbol* Prototype() const { return prototype; }
    ValueType GetValueType() const override;
    Value* MakeValue() const override;
    std::u32string Id() const override;
    std::u32string Info() const override { return groupName; }
    const char* ClassName() const override { return "ClassTypeSymbol"; }
    void Check() override;
    void SetClassGroup(ClassGroupTypeSymbol* classGroup_) { classGroup = classGroup_; }
    std::unique_ptr<Symbol> RemoveFromParent() override;
    std::string GetSymbolCategoryStr() const override { return "CL"; }
    std::string GetSymbolCategoryDescription() const override { return "class"; }
private:
    std::u32string groupName;
    int minArity;
    ClassTypeSymbol* baseClass;
    ClassTypeSymbolFlags flags;
    std::vector<InterfaceTypeSymbol*> implementedInterfaces;
    std::vector<TemplateParameterSymbol*> templateParameters;
    std::vector<MemberVariableSymbol*> memberVariables;
    std::vector<MemberVariableSymbol*> staticMemberVariables;
    std::unique_ptr<MemberVariableSymbol> initializedVar;
    StaticConstructorSymbol* staticConstructor;
    std::vector<ConstructorSymbol*> constructors;
    ConstructorSymbol* defaultConstructor;
    ConstructorSymbol* copyConstructor;
    ConstructorSymbol* moveConstructor;
    DestructorSymbol* destructor;
    MemberFunctionSymbol* copyAssignment;
    MemberFunctionSymbol* moveAssignment;
    std::vector<MemberFunctionSymbol*> memberFunctions;
    std::vector<FunctionSymbol*> vmt;
    std::vector<std::vector<FunctionSymbol*>> imts;
    std::vector<TypeSymbol*> objectLayout;
    std::vector<TypeSymbol*> staticLayout;
    int32_t vmtPtrIndex;
    NodeList<Node> usingNodes;
    std::unique_ptr<ClassNode> classNode;
    std::unique_ptr<ConstraintNode> constraint;
    ClassTemplateSpecializationSymbol* prototype;
    ClassGroupTypeSymbol* classGroup;
    void InitVmt(std::vector<FunctionSymbol*>& vmtToInit);
    void* CreateImt(Emitter& emitter, int index);
    void* CreateImts(Emitter& emitter);
};

SYMBOLS_API ConstantNode* MakePolymorphicClassArray(const std::unordered_set<ClassTypeSymbol*>& polymorphicClasses, const std::u32string& arrayName);
SYMBOLS_API ConstantNode* MakeStaticClassArray(const std::unordered_set<ClassTypeSymbol*>& classesHavingStaticConstructor, const std::u32string& arrayName);
SYMBOLS_API void MakeClassIdFile(const std::unordered_set<ClassTypeSymbol*>& polymorphicClasses, const std::string& classIdFileName);

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_CLASS_TYPE_SYMBOL_INCLUDED
