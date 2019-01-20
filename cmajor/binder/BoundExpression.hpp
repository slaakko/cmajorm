// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_BOUND_EXPRESSION_INCLUDED
#define CMAJOR_BINDER_BOUND_EXPRESSION_INCLUDED
#include <cmajor/binder/BoundNode.hpp>
#include <cmajor/symbols/TypeSymbol.hpp>
#include <cmajor/symbols/VariableSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/DelegateSymbol.hpp>

namespace cmajor { namespace binder {

using namespace cmajor::symbols;
class BoundCompileUnit;

enum class BoundExpressionFlags : uint8_t
{
    none = 0, 
    argIsExplicitThisOrBasePtr = 1 << 0,
    bindToRvalueReference = 1 << 1,
    virtualCall = 1 << 2,
    deref = 1 << 3,
    exceptionCapture = 1 << 4
};

inline BoundExpressionFlags operator|(BoundExpressionFlags left, BoundExpressionFlags right)
{
    return BoundExpressionFlags(uint8_t(left) | uint8_t(right));
}

inline BoundExpressionFlags operator&(BoundExpressionFlags left, BoundExpressionFlags right)
{
    return BoundExpressionFlags(uint8_t(left) & uint8_t(right));
}

class BoundFunctionCall;
class BoundFunction;

class BoundExpression : public BoundNode
{
public:
    BoundExpression(Module* module_, const Span& span_, BoundNodeType boundNodeType_, TypeSymbol* type_);
    virtual BoundExpression* Clone() = 0;
    virtual bool IsComplete() const { return true; }
    virtual bool IsLvalueExpression() const { return false; }
    virtual bool HasValue() const { return false; }
    virtual std::string TypeString() const { return "expression"; }
    virtual std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const { return std::unique_ptr<Value>(); }
    virtual bool ContainsExceptionCapture() const { return GetFlag(BoundExpressionFlags::exceptionCapture); }
    const TypeSymbol* GetType() const { return type; }
    TypeSymbol* GetType() { return type; }
    bool GetFlag(BoundExpressionFlags flag) const { return (flags & flag) != BoundExpressionFlags::none;  }
    void SetFlag(BoundExpressionFlags flag) { flags = flags | flag; }
    void AddTemporaryDestructorCall(std::unique_ptr<BoundFunctionCall>&& destructorCall);
    void MoveTemporaryDestructorCallsTo(BoundExpression& expression);
    void DestroyTemporaries(Emitter& emitter);
private:
    TypeSymbol* type;
    BoundExpressionFlags flags;
    std::vector<std::unique_ptr<BoundFunctionCall>> temporaryDestructorCalls;
};

class BoundParameter : public BoundExpression
{
public:
    BoundParameter(Module* module_, const Span& span_, ParameterSymbol* parameterSymbol_);
    BoundExpression* Clone() override;
    ParameterSymbol* GetParameterSymbol() { return parameterSymbol; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    std::string TypeString() const override { return "parameter"; }
private:
    ParameterSymbol* parameterSymbol;
};

class BoundLocalVariable : public BoundExpression
{
public:
    BoundLocalVariable(Module* module_, const Span& span_, LocalVariableSymbol* localVariableSymbol_);
    BoundExpression* Clone() override;
    LocalVariableSymbol* GetLocalVariableSymbol() { return localVariableSymbol; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    std::string TypeString() const override { return "local variable"; }
private:
    LocalVariableSymbol* localVariableSymbol;
};

class BoundMemberVariable : public BoundExpression
{
public:
    BoundMemberVariable(Module* module_, const Span& span_, MemberVariableSymbol* memberVariableSymbol_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    void SetClassPtr(std::unique_ptr<BoundExpression>&& classPtr_);
    void SetStaticInitNeeded() { staticInitNeeded = true; }
    std::string TypeString() const override { return "member variable"; }
    MemberVariableSymbol* GetMemberVariableSymbol() { return memberVariableSymbol; }
private:
    MemberVariableSymbol* memberVariableSymbol;
    std::unique_ptr<BoundExpression> classPtr;
    bool staticInitNeeded;
};

class BoundConstant : public BoundExpression
{
public:
    BoundConstant(Module* module_, const Span& span_, ConstantSymbol* constantSymbol_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    std::string TypeString() const override { return "constant"; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override { return std::unique_ptr<Value>(constantSymbol->GetValue()->Clone()); }
    ConstantSymbol* GetConstantSymbol() { return constantSymbol; }
private:
    ConstantSymbol* constantSymbol;
};

class BoundEnumConstant : public BoundExpression
{
public:
    BoundEnumConstant(Module* module_, const Span& span_, EnumConstantSymbol* enumConstantSymbol_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    std::string TypeString() const override { return "enumeration constant"; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override { return std::unique_ptr<Value>(enumConstantSymbol->GetValue()->Clone()); }
    EnumConstantSymbol* GetEnumConstantSymbol() { return enumConstantSymbol; }
private:
    EnumConstantSymbol* enumConstantSymbol;
};

class BoundLiteral : public BoundExpression
{
public:
    BoundLiteral(Module* module_, std::unique_ptr<Value>&& value_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "literal"; }
    bool HasValue() const override { return true; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override;
    Value* GetValue() { return value.get(); }
private:
    std::unique_ptr<Value> value;
};

class BoundTemporary : public BoundExpression
{
public:
    BoundTemporary(Module* module_, std::unique_ptr<BoundExpression>&& rvalueExpr_, std::unique_ptr<BoundLocalVariable>&& backingStore_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    std::string TypeString() const override { return "temporary"; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override;
    bool ContainsExceptionCapture() const override;
    BoundExpression* RvalueExpr() { return rvalueExpr.get(); }
    BoundLocalVariable* BackingStore() { return backingStore.get(); }
private:
    std::unique_ptr<BoundExpression> rvalueExpr;
    std::unique_ptr<BoundLocalVariable> backingStore;
};

class BoundSizeOfExpression : public BoundExpression
{
public:
    BoundSizeOfExpression(Module* module_, const Span& span_, TypeSymbol* type_, TypeSymbol* pointerType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "sizeof"; }
    TypeSymbol* PointerType() { return pointerType; }
private:
    TypeSymbol* pointerType;
};

class BoundAddressOfExpression : public BoundExpression
{
public:
    BoundAddressOfExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "address of expression"; }
    std::unique_ptr<BoundExpression>& Subject() { return subject; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override;
    bool ContainsExceptionCapture() const override;
private:
    std::unique_ptr<BoundExpression> subject;
};

class BoundDereferenceExpression : public BoundExpression
{
public:
    BoundDereferenceExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "deference expression"; }
    bool IsLvalueExpression() const override { return true; }
    std::unique_ptr<BoundExpression>& Subject() { return subject; }
    bool ContainsExceptionCapture() const override;
private:
    std::unique_ptr<BoundExpression> subject;
};

class BoundReferenceToPointerExpression : public BoundExpression
{
public:
    BoundReferenceToPointerExpression(Module* module_, std::unique_ptr<BoundExpression>&& subject_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "reference to pointer expression"; }
    bool ContainsExceptionCapture() const override;
    std::unique_ptr<BoundExpression>& Subject() { return subject; }
private:
    std::unique_ptr<BoundExpression> subject;
};

class BoundFunctionCall : public BoundExpression
{
public:
    BoundFunctionCall(Module* module_, const Span& span_, FunctionSymbol* functionSymbol_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override;
    std::string TypeString() const override { return "function call"; }
    bool IsLvalueExpression() const override;
    const FunctionSymbol* GetFunctionSymbol() const { return functionSymbol; }
    FunctionSymbol* GetFunctionSymbol() { return functionSymbol; }
    void AddArgument(std::unique_ptr<BoundExpression>&& argument);
    void SetArguments(std::vector<std::unique_ptr<BoundExpression>>&& arguments_);
    const std::vector<std::unique_ptr<BoundExpression>>& Arguments() const { return arguments; }
    void AddTemporary(std::unique_ptr<BoundLocalVariable>&& temporary);
    bool ContainsExceptionCapture() const override;
private:
    FunctionSymbol* functionSymbol;
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    std::vector<std::unique_ptr<BoundLocalVariable>> temporaries;
};

class BoundDelegateCall : public BoundExpression
{
public:
    BoundDelegateCall(Module* module_, const Span& span_, DelegateTypeSymbol* delegateType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override;
    std::string TypeString() const override { return "delegate call"; }
    bool IsLvalueExpression() const override;
    DelegateTypeSymbol* GetDelegateSymbol() { return delegateTypeSymbol; }
    void AddArgument(std::unique_ptr<BoundExpression>&& argument);
    const std::vector<std::unique_ptr<BoundExpression>>& Arguments() const { return arguments; }
    bool ContainsExceptionCapture() const override;
private:
    DelegateTypeSymbol* delegateTypeSymbol;
    std::vector<std::unique_ptr<BoundExpression>> arguments;
};

class BoundClassDelegateCall : public BoundExpression
{
public:
    BoundClassDelegateCall(Module* module_, const Span& span_, ClassDelegateTypeSymbol* classDelegateType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override;
    std::string TypeString() const override { return "class delegate call"; }
    bool IsLvalueExpression() const override;
    ClassDelegateTypeSymbol* GetClassDelegateSymbol() { return classDelegateTypeSymbol; }
    void AddArgument(std::unique_ptr<BoundExpression>&& argument);
    const std::vector<std::unique_ptr<BoundExpression>>& Arguments() const { return arguments; }
    bool ContainsExceptionCapture() const override;
private:
    ClassDelegateTypeSymbol* classDelegateTypeSymbol;
    std::vector<std::unique_ptr<BoundExpression>> arguments;
};

class BoundConstructExpression : public BoundExpression
{
public:
    BoundConstructExpression(Module* module_, std::unique_ptr<BoundExpression>&& constructorCall_, TypeSymbol* resultType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    std::string TypeString() const override { return "construct expression"; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* ConstructorCall() { return constructorCall.get(); }
private:
    std::unique_ptr<BoundExpression> constructorCall;
};

class BoundConstructAndReturnTemporaryExpression : public BoundExpression
{
public:
    BoundConstructAndReturnTemporaryExpression(Module* module_, std::unique_ptr<BoundExpression>&& constructorCall_, std::unique_ptr<BoundExpression>&& boundTemporary_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    std::string TypeString() const override { return "construct and return temporary expression"; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* ConstructorCall() { return constructorCall.get(); }
    BoundExpression* BoundTemporary() { return boundTemporary.get(); }
private:
    std::unique_ptr<BoundExpression> constructorCall;
    std::unique_ptr<BoundExpression> boundTemporary;
};

class BoundClassOrClassDelegateConversionResult : public BoundExpression
{
public:
    BoundClassOrClassDelegateConversionResult(Module* module_, std::unique_ptr<BoundExpression>&& conversionResult_, std::unique_ptr<BoundFunctionCall>&& conversionFunctionCall_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override { return true; }
    std::string TypeString() const override { return "class conversion result"; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* ConversionResult() { return conversionResult.get(); }
    BoundFunctionCall* ConversionFunctionCall() { return conversionFunctionCall.get(); }
private:
    std::unique_ptr<BoundExpression> conversionResult;
    std::unique_ptr<BoundFunctionCall> conversionFunctionCall;
};

class BoundConversion : public BoundExpression
{
public:
    BoundConversion(Module* module_, std::unique_ptr<BoundExpression>&& sourceExpr_, FunctionSymbol* conversionFun_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool IsLvalueExpression() const override;
    std::string TypeString() const override { return "conversion"; }
    BoundExpression* SourceExpr() { return sourceExpr.get(); }
    FunctionSymbol* ConversionFun() { return conversionFun; }
    std::unique_ptr<Value> ToValue(BoundCompileUnit& boundCompileUnit) const override;
    bool ContainsExceptionCapture() const override;
    void AddTemporary(std::unique_ptr<BoundLocalVariable>&& temporary);
private:
    std::unique_ptr<BoundExpression> sourceExpr;
    FunctionSymbol* conversionFun;
    std::vector<std::unique_ptr<BoundLocalVariable>> temporaries;
};

class BoundIsExpression : public BoundExpression
{
public:
    BoundIsExpression(Module* module_, std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, TypeSymbol* boolType_,
        std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    std::string TypeString() const override { return "is expression"; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* Expr() { return expr.get(); }
    ClassTypeSymbol* RightClassType() { return rightClassType; }
private:
    std::unique_ptr<BoundExpression> expr;
    ClassTypeSymbol* rightClassType;
    std::unique_ptr<BoundLocalVariable> leftClassIdVar;
    std::unique_ptr<BoundLocalVariable> rightClassIdVar;
};

class BoundAsExpression : public BoundExpression
{
public:
    BoundAsExpression(Module* module_, std::unique_ptr<BoundExpression>&& expr_, ClassTypeSymbol* rightClassType_, std::unique_ptr<BoundLocalVariable>&& variable_,
        std::unique_ptr<BoundLocalVariable>&& leftClassIdVar_, std::unique_ptr<BoundLocalVariable>&& rightClassIdVar_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    std::string TypeString() const override { return "as expression"; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* Expr() { return expr.get(); }
    BoundLocalVariable* Variable() { return variable.get(); }
    ClassTypeSymbol* RightClassType() { return rightClassType; }
private:
    std::unique_ptr<BoundExpression> expr;
    ClassTypeSymbol* rightClassType;
    std::unique_ptr<BoundLocalVariable> variable;
    std::unique_ptr<BoundLocalVariable> leftClassIdVar;
    std::unique_ptr<BoundLocalVariable> rightClassIdVar;
};

class BoundTypeNameExpression : public BoundExpression
{
public:
    BoundTypeNameExpression(Module* module_, std::unique_ptr<BoundExpression>&& classPtr_, TypeSymbol* constCharPtrType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* ClassPtr() { return classPtr.get(); }
private:
    std::unique_ptr<BoundExpression> classPtr;
};

class BoundBitCast : public BoundExpression
{
public:
    BoundBitCast(Module* module_, std::unique_ptr<BoundExpression>&& expr_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    bool ContainsExceptionCapture() const override;
    BoundExpression* Expr() { return expr.get(); }
private:
    std::unique_ptr<BoundExpression> expr;
};

class BoundFunctionPtr : public BoundExpression
{
public:
    BoundFunctionPtr(Module* module_, const Span& span_, FunctionSymbol* function_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    FunctionSymbol* Function() { return function; }
private:
    FunctionSymbol* function;
};

class BoundDisjunction : public BoundExpression
{
public:
    BoundDisjunction(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    BoundExpression* Left() { return left.get(); }
    BoundExpression* Right() { return right.get(); }
    void SetTemporary(BoundLocalVariable* temporary_);
    bool ContainsExceptionCapture() const override;
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    std::unique_ptr<BoundLocalVariable> temporary;
};

class BoundConjunction : public BoundExpression
{
public:
    BoundConjunction(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& left_, std::unique_ptr<BoundExpression>&& right_, TypeSymbol* boolType_);
    BoundExpression* Clone() override;
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    bool HasValue() const override { return true; }
    BoundExpression* Left() { return left.get(); }
    BoundExpression* Right() { return right.get(); }
    void SetTemporary(BoundLocalVariable* temporary_);
    bool ContainsExceptionCapture() const override;
private:
    std::unique_ptr<BoundExpression> left;
    std::unique_ptr<BoundExpression> right;
    std::unique_ptr<BoundLocalVariable> temporary;
};

class BoundTypeExpression : public BoundExpression
{
public:
    BoundTypeExpression(Module* module_, const Span& span_, TypeSymbol* type_);
    BoundExpression* Clone() override;
    bool IsComplete() const override { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "type expression"; }
};

class BoundNamespaceExpression : public BoundExpression
{
public:
    BoundNamespaceExpression(Module* module_, const Span& span_, NamespaceSymbol* ns_);
    BoundExpression* Clone() override;
    bool IsComplete() const override { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "namespace expression"; }
    NamespaceSymbol* Ns() { return ns; }
private:
    NamespaceSymbol* ns;
    std::unique_ptr<TypeSymbol> nsType;
};

class BoundFunctionGroupExpression : public BoundExpression
{
public:
    BoundFunctionGroupExpression(Module* module_, const Span& span_, FunctionGroupSymbol* functionGroupSymbol_);
    BoundExpression* Clone() override;
    bool IsComplete() const override { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "function group expression"; }
    const FunctionGroupSymbol* FunctionGroup() const { return functionGroupSymbol; }
    FunctionGroupSymbol* FunctionGroup() { return functionGroupSymbol; }
    void SetClassPtr(std::unique_ptr<BoundExpression>&& classPtr_);
    BoundExpression* ClassPtr() { return classPtr.get(); }
    BoundExpression* ReleaseClassPtr() { return classPtr.release(); }
    bool IsScopeQualified() const { return scopeQualified; }
    void SetScopeQualified() { scopeQualified = true; }
    ContainerScope* QualifiedScope() const { return qualifiedScope; }
    void SetQualifiedScope(ContainerScope* qualifiedScope_) { qualifiedScope = qualifiedScope_; }
    void SetTemplateArgumentTypes(const std::vector<TypeSymbol*>& templateArgumentTypes_);
    const std::vector<TypeSymbol*>& TemplateArgumentTypes() const { return templateArgumentTypes; }
private:
    FunctionGroupSymbol* functionGroupSymbol;
    std::unique_ptr<TypeSymbol> functionGroupType;
    std::unique_ptr<BoundExpression> classPtr;
    bool scopeQualified;
    ContainerScope* qualifiedScope;
    std::vector<TypeSymbol*> templateArgumentTypes;
};

class BoundMemberExpression : public BoundExpression
{
public:
    BoundMemberExpression(Module* module_, const Span& span_, std::unique_ptr<BoundExpression>&& classPtr_, std::unique_ptr<BoundExpression>&& member_);
    BoundExpression* Clone() override;
    bool IsComplete() const override { return false; }
    void Load(Emitter& emitter, OperationFlags flags) override;
    void Store(Emitter& emitter, OperationFlags flags) override;
    void Accept(BoundNodeVisitor& visitor) override;
    std::string TypeString() const override { return "member expression"; }
    BoundExpression* ClassPtr() { return classPtr.get(); }
    void ResetClassPtr() { classPtr.reset(); }
    BoundExpression* ReleaseClassPtr() { return classPtr.release(); }
    BoundExpression* Member() { return member.get(); }
private:
    std::unique_ptr<BoundExpression> classPtr;
    std::unique_ptr<BoundExpression> member;
    std::unique_ptr<TypeSymbol> memberExpressionType;
};

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_BOUND_EXPRESSION_INCLUDED
