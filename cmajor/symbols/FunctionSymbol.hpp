// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_SYMBOLS_FUNCTION_SYMBOL_INCLUDED
#define CMAJOR_SYMBOLS_FUNCTION_SYMBOL_INCLUDED
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/Meta.hpp>
#include <cmajor/ast/Function.hpp>
#include <cmajor/ir/GenObject.hpp>
#include <unordered_set>

namespace cmajor {  namespace symbols {

using namespace cmajor::ir;
class Value;
class IntrinsicFunction;

struct FunctionSymbolsEqual
{
    bool operator()(FunctionSymbol* left, FunctionSymbol* right) const;
};

struct FunctionSymbolHash
{
    size_t operator()(FunctionSymbol* fun) const;
};

class ViableFunctionSet
{
public:
    const std::unordered_set<FunctionSymbol*, FunctionSymbolHash, FunctionSymbolsEqual>& Get() const { return set; }
    void Insert(FunctionSymbol* fun);
private:
    std::unordered_set<FunctionSymbol*, FunctionSymbolHash, FunctionSymbolsEqual> set;
};

class FunctionGroupSymbol : public Symbol
{
public:
    FunctionGroupSymbol(const Span& span_, const std::u32string& name_);
    bool IsExportSymbol() const override { return false; }
    std::string TypeString() const override { return "function_group"; }
    void ComputeMangledName() override;
    void AddFunction(FunctionSymbol* function);
    void RemoveFunction(FunctionSymbol* function);
    void CollectViableFunctions(int arity, ViableFunctionSet& viableFunctions, Module* module);
    bool HasProjectMembers() const override;
    void AppendChildElements(dom::Element* element, TypeMap& typeMap) const override;
    std::u32string Info() const override { return Name(); }
    const char* ClassName() const override { return "FunctionGroupSymbol"; }
    void Check() override;
private:
    std::unordered_map<int, std::vector<FunctionSymbol*>> arityFunctionListMap;
};

enum class ConversionType : uint8_t
{
    implicit_, explicit_
};

enum class FunctionSymbolFlags : uint32_t
{
    none = 0,
    inline_ = 1 << 0,
    constExpr = 1 << 1,
    cdecl_ = 1 << 2,
    suppress = 1 << 3,
    default_ = 1 << 4,
    explicit_ = 1 << 5,
    virtual_ = 1 << 6,
    override_ = 1 << 7,
    abstract_ = 1 << 8,
    new_ = 1 << 9,
    const_ = 1 << 10,
    conversion = 1 << 11,
    linkOnceOdrLinkage = 1 << 12,
    templateSpecialization = 1 << 13,
    hasTry = 1 << 14,
    hasSource = 1 << 15,
    includeConstraint = 1 << 16,
    copy = 1 << 17
};

inline FunctionSymbolFlags operator|(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint32_t(left) | uint32_t(right));
}

inline FunctionSymbolFlags operator&(FunctionSymbolFlags left, FunctionSymbolFlags right)
{
    return FunctionSymbolFlags(uint32_t(left) & uint32_t(right));
}

std::string FunctionSymbolFlagStr(FunctionSymbolFlags flags);

class ParameterSymbol;
class LocalVariableSymbol;
class TypeSymbol;
class TemplateParameterSymbol;
class BoundTemplateParameterSymbol;

class FunctionSymbol : public ContainerSymbol
{
public:
    FunctionSymbol(const Span& span_, const std::u32string& name_);
    FunctionSymbol(SymbolType symbolType_, const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    void EmplaceFunction(FunctionSymbol* functionSymbol, int index) override;
    void Accept(SymbolCollector* collector) override;
    const NodeList<Node>& UsingNodes() const { return usingNodes; }
    FunctionNode* GetFunctionNode() { return functionNode.get(); }
    ConstraintNode* Constraint() { return constraint.get(); }
    void SetConstraint(ConstraintNode* constraint_) { constraint.reset(constraint_); }
    void EmplaceType(TypeSymbol* typeSymbol, int index) override;
    void AddMember(Symbol* member) override;
    bool IsFunctionSymbol() const override { return true; }
    std::string TypeString() const override { return "function"; }
    bool IsExportSymbol() const override;
    virtual void ComputeName();
    std::u32string FullName() const override;
    std::u32string FullNameWithSpecifiers() const override;
    std::u32string DocName() const override;
    std::u32string CodeName() const override { return groupName; }
    std::string GetSpecifierStr() const override;
    std::string Syntax() const override;
    int32_t GetIndex() const { return index; }
    void SetIndex(int32_t index_) { index = index_; }
    virtual ConversionType GetConversionType() const { return ConversionType::implicit_; }
    virtual uint8_t ConversionDistance() const { return 0; }
    virtual TypeSymbol* ConversionSourceType() const { return nullptr; }
    virtual TypeSymbol* ConversionTargetType() const { return nullptr; }
    virtual bool IsBasicTypeOperation() const { return false; }
    virtual bool IsGeneratedFunction() const { return false; }
    virtual bool IsLvalueReferenceCopyAssignment() const { return false; }
    virtual bool IsArrayElementAccess() const { return false; }
    virtual bool IsCompileTimePrimitiveFunction() const { return false; }
    virtual bool IsClassToInterfaceTypeConversion() const { return false; }
    virtual bool IsMemberFunctionToClassDelegateConversion() const { return false; }
    virtual void GenerateCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span);
    void GenerateVirtualCall(Emitter& emitter, std::vector<GenObject*>& genObjects, OperationFlags flags, const Span& span);
    virtual std::unique_ptr<Value> ConstructValue(const std::vector<std::unique_ptr<Value>>& argumentValues, const Span& span) const;
    virtual std::unique_ptr<Value> ConvertValue(const std::unique_ptr<Value>& value) const;
    virtual ParameterSymbol* GetThisParam() const { return nullptr; }
    virtual bool IsConstructorDestructorOrNonstaticMemberFunction() const { return false; }
    virtual bool IsClassDelegateCopyConstructor() const { return false; }
    void Dump(CodeFormatter& formatter) override;
    bool IsDefaultConstructor() const;
    bool IsCopyConstructor() const;
    bool IsMoveConstructor() const;
    bool IsCopyAssignment() const;
    bool IsMoveAssignment() const;
    const boost::uuids::uuid& FunctionId() const { Assert(!functionId.is_nil(), "function id not initialized");  return functionId; }
    void SetFunctionId(const boost::uuids::uuid& functionId_) { functionId = functionId_; }
    const std::u32string& GroupName() const { return groupName; }
    void SetGroupName(const std::u32string& groupName_);
    const std::vector<TemplateParameterSymbol*>& TemplateParameters() const { return templateParameters; }
    void SetSpecifiers(Specifiers specifiers);
    bool IsInline() const { return GetFlag(FunctionSymbolFlags::inline_); }
    void SetInline() { SetFlag(FunctionSymbolFlags::inline_); }
    bool IsConstExpr() const { return GetFlag(FunctionSymbolFlags::constExpr); }
    void SetConstExpr() { SetFlag(FunctionSymbolFlags::constExpr); }
    bool IsCDecl() const { return GetFlag(FunctionSymbolFlags::cdecl_); }
    void SetCDecl() { SetFlag(FunctionSymbolFlags::cdecl_); }
    bool IsDefault() const { return GetFlag(FunctionSymbolFlags::default_); }
    void SetDefault() { SetFlag(FunctionSymbolFlags::default_); }
    bool IsSuppressed() const { return GetFlag(FunctionSymbolFlags::suppress); }
    void SetSuppressed() { SetFlag(FunctionSymbolFlags::suppress); }
    bool IsExplicit() const { return GetFlag(FunctionSymbolFlags::explicit_); }
    void SetExplicit() { SetFlag(FunctionSymbolFlags::explicit_); }
    bool IsVirtual() const { return GetFlag(FunctionSymbolFlags::virtual_); }
    void SetVirtual() { SetFlag(FunctionSymbolFlags::virtual_); }
    bool IsOverride() const { return GetFlag(FunctionSymbolFlags::override_); }
    void SetOverride() { SetFlag(FunctionSymbolFlags::override_); }
    bool IsAbstract() const { return GetFlag(FunctionSymbolFlags::abstract_); }
    void SetAbstract() { SetFlag(FunctionSymbolFlags::abstract_); }
    bool IsVirtualAbstractOrOverride() const { return GetFlag(FunctionSymbolFlags::virtual_ | FunctionSymbolFlags::abstract_ | FunctionSymbolFlags::override_); }
    bool IsNew() const { return GetFlag(FunctionSymbolFlags::new_); }
    void SetNew() { SetFlag(FunctionSymbolFlags::new_); }
    bool IsConst() const { return GetFlag(FunctionSymbolFlags::const_); }
    void SetConst() { SetFlag(FunctionSymbolFlags::const_); }
    bool IsConversion() const { return GetFlag(FunctionSymbolFlags::conversion); }
    void SetConversion() { SetFlag(FunctionSymbolFlags::conversion); }
    bool HasLinkOnceOdrLinkage() const { return GetFlag(FunctionSymbolFlags::linkOnceOdrLinkage);  }
    void SetLinkOnceOdrLinkage() { SetFlag(FunctionSymbolFlags::linkOnceOdrLinkage); }
    bool IsTemplateSpecialization() const { return GetFlag(FunctionSymbolFlags::templateSpecialization); }
    void SetTemplateSpecialization() { SetFlag(FunctionSymbolFlags::templateSpecialization); }
    bool HasTry() const { return GetFlag(FunctionSymbolFlags::hasTry); }
    void SetHasTry() { SetFlag(FunctionSymbolFlags::hasTry); }
    bool IsCopy() const { return GetFlag(FunctionSymbolFlags::copy); }
    void SetCopy() { SetFlag(FunctionSymbolFlags::copy); }
    virtual bool DontThrow() const { return IsNothrow() || IsBasicTypeOperation(); }
    FunctionSymbolFlags GetFunctionSymbolFlags() const { return flags; }
    bool GetFlag(FunctionSymbolFlags flag) const { return (flags & flag) != FunctionSymbolFlags::none; }
    void SetFlag(FunctionSymbolFlags flag) { flags = flags | flag; }
    void ComputeMangledName() override;
    int Arity() const { return parameters.size(); }
    const std::vector<ParameterSymbol*>& Parameters() const { return parameters; }
    void AddLocalVariable(LocalVariableSymbol* localVariable);
    const std::vector<LocalVariableSymbol*>& LocalVariables() const { return localVariables; }
    void SetReturnType(TypeSymbol* returnType_) { returnType = returnType_; }
    TypeSymbol* ReturnType() const { return returnType; }
    ParameterSymbol* ReturnParam() { return returnParam.get(); }
    void SetReturnParam(ParameterSymbol* returnParam_);
    bool ReturnsClassInterfaceOrClassDelegateByValue() const;
    bool IsFunctionTemplate() const { return !templateParameters.empty(); }
    void CloneUsingNodes(const std::vector<Node*>& usingNodes_);
    LocalVariableSymbol* CreateTemporary(TypeSymbol* type, const Span& span);
    virtual std::vector<LocalVariableSymbol*> CreateTemporariesTo(FunctionSymbol* currentFunction);
    llvm::FunctionType* IrType(Emitter& emitter);
    int32_t VmtIndex() const { return vmtIndex; }
    void SetVmtIndex(int32_t vmtIndex_) { vmtIndex = vmtIndex_; }
    int32_t ImtIndex() const { return imtIndex; }
    void SetImtIndex(int32_t imtIndex_) { imtIndex = imtIndex_; }
    FunctionGroupSymbol* FunctionGroup() { return functionGroup; }
    void SetFunctionGroup(FunctionGroupSymbol* functionGroup_) { functionGroup = functionGroup_; }
    IntrinsicFunction* GetIntrinsic() { return intrinsic.get(); }
    void SetIntrinsic(IntrinsicFunction* intrinsic_) { intrinsic.reset(intrinsic_); }
    bool IsProgramMain() const { return isProgramMain; }
    void SetProgramMain() { isProgramMain = true; }
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    std::u32string Info() const override { return groupName; }
    const char* ClassName() const override { return "FunctionSymbol"; }
    virtual int ClassArity() const { return 0; }
    bool HasSource() const { return GetFlag(FunctionSymbolFlags::hasSource); }
    void SetHasSource() { SetFlag(FunctionSymbolFlags::hasSource); }
    std::u32string Id() const override;
    FunctionSymbol* FunctionTemplate() { return functionTemplate; }
    void SetFunctionTemplate(FunctionSymbol* functionTemplate_) { functionTemplate = functionTemplate_; }
    void SetTemplateArgumentTypes(const std::vector<TypeSymbol*>& templateArgumentTypes_);
    const std::vector<TypeSymbol*>& TemplateArgumentTypes() const { return templateArgumentTypes; }
    void Check() override;
    FunctionSymbol* Master() const { return master; }
    void SetMaster(FunctionSymbol* master_) { master = master_; }
private:
    FunctionSymbol* functionTemplate;
    FunctionSymbol* master;
    boost::uuids::uuid functionId;
    std::u32string groupName;
    std::vector<TemplateParameterSymbol*> templateParameters;
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::vector<ParameterSymbol*> parameters;
    std::unique_ptr<ParameterSymbol> returnParam;
    std::vector<LocalVariableSymbol*> localVariables;
    TypeSymbol* returnType;
    FunctionSymbolFlags flags;
    int32_t index;
    int32_t vmtIndex;
    int32_t imtIndex;
    NodeList<Node> usingNodes;
    std::unique_ptr<FunctionNode> functionNode;
    std::unique_ptr<ConstraintNode> constraint;
    int nextTemporaryIndex;
    FunctionGroupSymbol* functionGroup;
    std::unique_ptr<IntrinsicFunction> intrinsic;
    bool isProgramMain;
};

class StaticConstructorSymbol : public FunctionSymbol
{
public:
    StaticConstructorSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override { return "static_constructor"; }
    void Accept(SymbolCollector* collector) override {}
    void SetSpecifiers(Specifiers specifiers);
    std::u32string FullNameWithSpecifiers() const override;
    std::u32string CodeName() const override;
    std::u32string Info() const override { return std::u32string(); }
    const char* ClassName() const override { return "StaticConstructorSymbol"; }
};
   
class ConstructorSymbol : public FunctionSymbol
{
public:
    ConstructorSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override;
    std::u32string DocName() const override;
    std::u32string CodeName() const override;
    ParameterSymbol* GetThisParam() const override { return Parameters()[0]; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    void SetSpecifiers(Specifiers specifiers);
    uint8_t ConversionDistance() const override;
    TypeSymbol* ConversionSourceType() const override;
    TypeSymbol* ConversionTargetType() const override;
    std::u32string Info() const override { return std::u32string(); }
    const char* ClassName() const override { return "ConstructorSymbol"; }
};

class DestructorSymbol : public FunctionSymbol
{
public:
    DestructorSymbol(const Span& span_, const std::u32string& name_);
    void Write(SymbolWriter& writer) override;
    void Read(SymbolReader& reader) override;
    bool IsExportSymbol() const override;
    void Accept(SymbolCollector* collector) override {}
    std::string TypeString() const override { return "destructor"; }
    ParameterSymbol* GetThisParam() const override { return Parameters()[0]; }
    std::u32string CodeName() const override;
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    bool IsGeneratedFunction() const { return generated; }
    bool DontThrow() const override { return true; }
    void SetSpecifiers(Specifiers specifiers);
    void SetGenerated() { generated = true; }
    std::u32string Info() const override { return std::u32string(); }
    const char* ClassName() const override { return "DestructorSymbol"; }
private:
    bool generated;
};

class MemberFunctionSymbol : public FunctionSymbol
{
public:
    MemberFunctionSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override;
    std::u32string DocName() const override;
    ParameterSymbol* GetThisParam() const override { if (IsStatic()) return nullptr; else return Parameters()[0]; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return !IsStatic(); }
    void SetSpecifiers(Specifiers specifiers);
    const char* ClassName() const override { return "MemberFunctionSymbol"; }
};

class ConversionFunctionSymbol : public FunctionSymbol
{
public:
    ConversionFunctionSymbol(const Span& span_, const std::u32string& name_);
    std::string TypeString() const override { return "conversion_function";  }
    std::u32string DocName() const override;
    ParameterSymbol* GetThisParam() const override { return Parameters()[0]; }
    bool IsConstructorDestructorOrNonstaticMemberFunction() const override { return true; }
    ConversionType GetConversionType() const override { return ConversionType::implicit_; }
    uint8_t ConversionDistance() const override { return 255; }
    TypeSymbol* ConversionSourceType() const override;
    TypeSymbol* ConversionTargetType() const override;
    void SetSpecifiers(Specifiers specifiers);
    std::unique_ptr<dom::Element> CreateDomElement(TypeMap& typeMap) override;
    std::u32string Info() const override { return std::u32string(); }
    const char* ClassName() const override { return "ConversionFunctionSymbol"; }
};

class FunctionGroupTypeSymbol : public TypeSymbol
{
public:
    FunctionGroupTypeSymbol(FunctionGroupSymbol* functionGroup_, void* boundFunctionGroup_);
    bool IsExportSymbol() const override { return false; }
    llvm::Type* IrType(Emitter& emitter) override { Assert(false, "tried to get ir type of function group type"); return nullptr; }
    llvm::Constant* CreateDefaultIrValue(Emitter& emitter) override { Assert(false, "tried to get default ir value of function group type"); return nullptr; }
    const FunctionGroupSymbol* FunctionGroup() const { return functionGroup; }
    FunctionGroupSymbol* FunctionGroup() { return functionGroup; }
    void* BoundFunctionGroup() const { return boundFunctionGroup; }
    const char* ClassName() const override { return "FunctionGroupTypeSymbol"; }
private:
    FunctionGroupSymbol* functionGroup;
    void* boundFunctionGroup;
};

class MemberExpressionTypeSymbol : public TypeSymbol
{
public:
    MemberExpressionTypeSymbol(const Span& span_, const std::u32string& name_, void* boundMemberExpression_);
    bool IsExportSymbol() const override { return false; }
    llvm::Type* IrType(Emitter& emitter) override { Assert(false, "tried to get ir type of member expression type");  return nullptr; }
    llvm::Constant* CreateDefaultIrValue(Emitter& emitter) override { Assert(false, "tried to get default ir value of member expression type"); return nullptr; }
    std::string TypeString() const override { return "member_expression_type"; }
    void* BoundMemberExpression() const { return boundMemberExpression; }
    const char* ClassName() const override { return "MemberExpressionTypeSymbol"; }
private:
    void* boundMemberExpression;
};

void InitFunctionSymbol();
void DoneFunctionSymbol();

} } // namespace cmajor::symbols

#endif // CMAJOR_SYMBOLS_FUNCTION_SYMBOL_INCLUDED
