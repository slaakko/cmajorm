// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/ExpressionBinder.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundExpression.hpp>
#include <cmajor/binder/OverloadResolution.hpp>
#include <cmajor/binder/StatementBinder.hpp>
#include <cmajor/binder/TypeResolver.hpp>
#include <cmajor/binder/Evaluator.hpp>
#include <cmajor/binder/Access.hpp>
#include <cmajor/symbols/ClassTypeSymbol.hpp>
#include <cmajor/symbols/InterfaceTypeSymbol.hpp>
#include <cmajor/symbols/ConstantSymbol.hpp>
#include <cmajor/symbols/EnumSymbol.hpp>
#include <cmajor/symbols/TypedefSymbol.hpp>
#include <cmajor/symbols/TemplateSymbol.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <cmajor/symbols/DebugFlags.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/Log.hpp>
#include <soulng/util/Time.hpp>

namespace cmajor { namespace binder {

using soulng::lexer::Span;
using namespace soulng::unicode;

class ExpressionBinder : public sngcm::ast::Visitor
{
public:
    ExpressionBinder(const Span& span_, const boost::uuids::uuid& moduleId_, BoundCompileUnit& boundCompileUnit_, BoundFunction* boundFunction_, ContainerScope* containerScope_, 
        StatementBinder* statementBinder_, bool lvalue_);
    std::unique_ptr<BoundExpression> GetExpression() { return std::move(expression); }

    void Visit(BoolNode& boolNode) override;
    void Visit(SByteNode& sbyteNode) override;
    void Visit(ByteNode& byteNode) override;
    void Visit(ShortNode& shortNode) override;
    void Visit(UShortNode& ushortNode) override;
    void Visit(IntNode& intNode) override;
    void Visit(UIntNode& uintNode) override;
    void Visit(LongNode& longNode) override;
    void Visit(ULongNode& ulongNode) override;
    void Visit(FloatNode& floatNode) override;
    void Visit(DoubleNode& doubleNode) override;
    void Visit(CharNode& charNode) override;
    void Visit(WCharNode& wcharNode) override;
    void Visit(UCharNode& ucharNode) override;
    void Visit(VoidNode& voidNode) override;

    void Visit(BooleanLiteralNode& booleanLiteralNode) override;
    void Visit(SByteLiteralNode& sbyteLiteralNode) override;
    void Visit(ByteLiteralNode& byteLiteralNode) override;
    void Visit(ShortLiteralNode& shortLiteralNode) override;
    void Visit(UShortLiteralNode& ushortLiteralNode) override;
    void Visit(IntLiteralNode& intLiteralNode) override;
    void Visit(UIntLiteralNode& uintLiteralNode) override;
    void Visit(LongLiteralNode& longLiteralNode) override;
    void Visit(ULongLiteralNode& ulongLiteralNode) override;
    void Visit(FloatLiteralNode& floatLiteralNode) override;
    void Visit(DoubleLiteralNode& doubleLiteralNode) override;
    void Visit(CharLiteralNode& charLiteralNode) override;
    void Visit(WCharLiteralNode& wcharLiteralNode) override;
    void Visit(UCharLiteralNode& ucharLiteralNode) override;
    void Visit(StringLiteralNode& stringLiteralNode) override;
    void Visit(WStringLiteralNode& wstringLiteralNode) override;
    void Visit(UStringLiteralNode& ustringLiteralNode) override;
    void Visit(NullLiteralNode& nullLiteralNode) override;
    void Visit(UuidLiteralNode& uuidLiteralNode) override;

    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(ParameterNode& parameterNode) override;
    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void BindArrow(Node& node, const std::u32string& name);
    void Visit(DisjunctionNode& disjunctionNode) override;
    void Visit(ConjunctionNode& conjunctionNode) override;
    void Visit(BitOrNode& bitOrNode) override;
    void Visit(BitXorNode& bitXorNode) override;
    void Visit(BitAndNode& bitAndNode) override;
    void Visit(EqualNode& equalNode) override;
    void Visit(NotEqualNode& notEqualNode) override;
    void Visit(LessNode& lessNode) override;
    void Visit(GreaterNode& greaterNode) override;
    void Visit(LessOrEqualNode& lessOrEqualNode) override;
    void Visit(GreaterOrEqualNode& greaterOrEqualNode) override;
    void Visit(ShiftLeftNode& shiftLeftNode) override;
    void Visit(ShiftRightNode& shiftRightNode) override;
    void Visit(AddNode& addNode) override;
    void Visit(SubNode& subNode) override;
    void Visit(MulNode& mulNode) override;
    void Visit(DivNode& divNode) override;
    void Visit(RemNode& remNode) override;
    void Visit(NotNode& notNode) override;
    void Visit(UnaryPlusNode& unaryPlusNode) override;
    void Visit(UnaryMinusNode& unaryMinusNode) override;
    void Visit(PrefixIncrementNode& prefixIncrementNode) override;
    void Visit(PrefixDecrementNode& prefixDecrementNode) override;
    void Visit(DerefNode& derefNode) override;
    void Visit(AddrOfNode& addrOfNode) override;
    void Visit(ComplementNode& complementNode) override;
    void Visit(IsNode& isNode) override;
    void Visit(AsNode& asNode) override;
    void Visit(IndexingNode& indexingNode) override;
    void Visit(InvokeNode& invokeNode) override;
    void Visit(PostfixIncrementNode& postfixIncrementNode) override;
    void Visit(PostfixDecrementNode& postfixDecrementNode) override;
    void Visit(SizeOfNode& sizeOfNode) override;
    void Visit(TypeNameNode& typeNameNode) override;
    void Visit(TypeIdNode& typeNameNode) override;
    void Visit(CastNode& castNode) override;
    void Visit(ConstructNode& constructNode) override;
    void Visit(NewNode& newNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(BaseNode& baseNode) override;
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) override;
    void Visit(FunctionPtrNode& functionPtrNode) override;
    void BindUnaryOp(BoundExpression* operand, Node& node, const std::u32string& groupName);
private:
    Span span;
    boost::uuids::uuid moduleId;
    BoundCompileUnit& boundCompileUnit;
    SymbolTable& symbolTable;
    Module* module;
    BoundFunction* boundFunction;
    ContainerScope* containerScope;
    StatementBinder* statementBinder;
    std::unique_ptr<BoundExpression> expression;
    bool lvalue;
    bool inhibitCompile;
    void BindUnaryOp(UnaryNode& unaryNode, const std::u32string& groupName);
    void BindBinaryOp(BinaryNode& binaryNode, const std::u32string& groupName);
    void BindBinaryOp(BoundExpression* left, BoundExpression* right, Node& node, const std::u32string& groupName);
    void BindDerefExpr(Node& node);
    void BindSymbol(Symbol* symbol, IdentifierNode* idNode);
};

ExpressionBinder::ExpressionBinder(const Span& span_, const boost::uuids::uuid& moduleId_, BoundCompileUnit& boundCompileUnit_, BoundFunction* boundFunction_, ContainerScope* containerScope_, 
    StatementBinder* statementBinder_, bool lvalue_) :
    span(span_), moduleId(moduleId_), boundCompileUnit(boundCompileUnit_), symbolTable(boundCompileUnit.GetSymbolTable()), module(&boundCompileUnit.GetModule()),
    boundFunction(boundFunction_), containerScope(containerScope_), statementBinder(statementBinder_), lvalue(lvalue_), inhibitCompile(false)
{
}

void ExpressionBinder::BindUnaryOp(BoundExpression* operand, Node& node, const std::u32string& groupName)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::unique_ptr<BoundExpression>(operand));
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, operand->GetType()->BaseType()->ClassOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Exception> exception;
    std::unique_ptr<BoundFunctionCall> operatorFunCall = ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, node.GetSpan(), node.ModuleId(),
        OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
    if (!operatorFunCall)
    {
        if (arguments[0]->GetType()->PlainType(node.GetSpan(), node.ModuleId())->IsClassTypeSymbol())
        {
            if (arguments[0]->GetType()->IsReferenceType())
            {
                TypeSymbol* type = arguments[0]->GetType()->RemoveReference(node.GetSpan(), node.ModuleId())->AddPointer(node.GetSpan(), node.ModuleId());
                arguments[0].reset(new BoundReferenceToPointerExpression(std::move(arguments[0]), type));
            }
            else
            {
                TypeSymbol* type = arguments[0]->GetType()->PlainType(node.GetSpan(), node.ModuleId())->AddPointer(node.GetSpan(), node.ModuleId());
                arguments[0].reset(new BoundAddressOfExpression(std::move(arguments[0]), type));
            }
            operatorFunCall = std::move(ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, node.GetSpan(), node.ModuleId()));
        }
        else
        {
            throw *exception;
        }
    }
    CheckAccess(boundFunction->GetFunctionSymbol(), operatorFunCall->GetFunctionSymbol());
    LocalVariableSymbol* temporary = nullptr;
    if (operatorFunCall->GetFunctionSymbol()->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        TypeSymbol* type = operatorFunCall->GetFunctionSymbol()->ReturnType();
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsBound())
            {
                boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
            }
        }
        temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, node.GetSpan(), node.ModuleId());
        operatorFunCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(node.GetSpan(), node.ModuleId(), temporary)),
            type->AddPointer(node.GetSpan(), node.ModuleId()))));
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
            if (classType->Destructor())
            {
                std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                destructorCall->AddArgument(std::unique_ptr<BoundExpression>(operatorFunCall->Arguments().back()->Clone()));
                boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
            }
        }
    }
    expression.reset(operatorFunCall.release());
    if (temporary)
    {
        expression.reset(new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(node.GetSpan(), node.ModuleId(), temporary))));
        expression->SetFlag(BoundExpressionFlags::bindToRvalueReference);
    }
}

void ExpressionBinder::BindUnaryOp(UnaryNode& unaryNode, const std::u32string& groupName)
{
    unaryNode.Subject()->Accept(*this);
    BoundExpression* operand = expression.release();
    BindUnaryOp(operand, unaryNode, groupName);
}

void ExpressionBinder::BindBinaryOp(BinaryNode& binaryNode, const std::u32string& groupName)
{
    binaryNode.Left()->Accept(*this);
    BoundExpression* left = expression.release();
    binaryNode.Right()->Accept(*this);
    BoundExpression* right = expression.release();
    BindBinaryOp(left, right, binaryNode, groupName);
}

void ExpressionBinder::BindBinaryOp(BoundExpression* left, BoundExpression* right, Node& node, const std::u32string& groupName)
{
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    arguments.push_back(std::unique_ptr<BoundExpression>(left));
    arguments.push_back(std::unique_ptr<BoundExpression>(right));
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, left->GetType()->BaseType()->ClassOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, right->GetType()->BaseType()->ClassOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::unique_ptr<Exception> exception;
    std::unique_ptr<BoundFunctionCall> operatorFunCall = ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, node.GetSpan(), node.ModuleId(),
        OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
    if (!operatorFunCall)
    {
        if (arguments[0]->GetType()->PlainType(node.GetSpan(), node.ModuleId())->IsClassTypeSymbol())
        {
            if (arguments[0]->GetType()->IsReferenceType())
            {
                TypeSymbol* type = arguments[0]->GetType()->RemoveReference(node.GetSpan(), node.ModuleId())->AddPointer(node.GetSpan(), node.ModuleId());
                arguments[0].reset(new BoundReferenceToPointerExpression(std::move(arguments[0]), type));
            }
            else
            {
                TypeSymbol* type = arguments[0]->GetType()->PlainType(node.GetSpan(), node.ModuleId())->AddPointer(node.GetSpan(), node.ModuleId());
                arguments[0].reset(new BoundAddressOfExpression(std::move(arguments[0]), type));
            }
            operatorFunCall = std::move(ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, node.GetSpan(), node.ModuleId()));
        }
        else
        {
            throw *exception;
        }
    }
    CheckAccess(boundFunction->GetFunctionSymbol(), operatorFunCall->GetFunctionSymbol());
    LocalVariableSymbol* temporary = nullptr;
    if (operatorFunCall->GetFunctionSymbol()->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        TypeSymbol* type = operatorFunCall->GetFunctionSymbol()->ReturnType();
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsBound())
            {
                boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
            }
        }
        temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, node.GetSpan(), node.ModuleId());
        operatorFunCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(node.GetSpan(), node.ModuleId(), temporary)),
            type->AddPointer(node.GetSpan(), node.ModuleId()))));
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
            if (classType->Destructor())
            {
                std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                destructorCall->AddArgument(std::unique_ptr<BoundExpression>(operatorFunCall->Arguments().back()->Clone()));
                boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
            }
        }
    }
    expression.reset(operatorFunCall.release());
    if (temporary)
    {
        expression.reset(new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(node.GetSpan(), node.ModuleId(), temporary))));
        expression->SetFlag(BoundExpressionFlags::bindToRvalueReference);
    }
}

void ExpressionBinder::BindSymbol(Symbol* symbol, IdentifierNode* idNode)
{
    Span spn = span;
    boost::uuids::uuid mid = moduleId;
    if (idNode)
    {
        spn = idNode->GetSpan();
        mid = idNode->ModuleId();
    }
    switch (symbol->GetSymbolType())
    {
        case SymbolType::functionGroupSymbol:
        {
            FunctionGroupSymbol* functionGroupSymbol = static_cast<FunctionGroupSymbol*>(symbol);
            BoundFunctionGroupExpression* boundFunctionGroupExpression = new BoundFunctionGroupExpression(span, moduleId, functionGroupSymbol);
            ParameterSymbol* thisParam = boundFunction->GetFunctionSymbol()->GetThisParam();
            if (thisParam)
            {
                boundFunctionGroupExpression->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(spn, mid, thisParam)));
            }
            expression.reset(boundFunctionGroupExpression);
            break;
        }
        case SymbolType::classTypeSymbol: case SymbolType::classTemplateSpecializationSymbol:
        {
            ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), classTypeSymbol);
            expression.reset(new BoundTypeExpression(span, moduleId, classTypeSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, classTypeSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, classTypeSymbol);
            }
            break;
        }
        case SymbolType::classGroupTypeSymbol: 
        {
            ClassGroupTypeSymbol* classGroupTypeSymbol = static_cast<ClassGroupTypeSymbol*>(symbol);
            expression.reset(new BoundTypeExpression(span, moduleId, classGroupTypeSymbol));
            ClassTypeSymbol* classTypeSymbol = classGroupTypeSymbol->GetClass(0);
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc) && classTypeSymbol)
            {
                symbolTable.MapSymbol(idNode, classTypeSymbol);
            }
            if (idNode && classTypeSymbol)
            {
                MapIdentifierToSymbolDefinition(idNode, classTypeSymbol);
            }
            break;
        }
        case SymbolType::interfaceTypeSymbol:
        {
            InterfaceTypeSymbol* interfaceTypeSymbol = static_cast<InterfaceTypeSymbol*>(symbol); 
            expression.reset(new BoundTypeExpression(span, moduleId, interfaceTypeSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, interfaceTypeSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, interfaceTypeSymbol);
            }
            break;
        }
        case SymbolType::delegateTypeSymbol:
        {
            DelegateTypeSymbol* delegateTypeSymbol = static_cast<DelegateTypeSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), delegateTypeSymbol);
            expression.reset(new BoundTypeExpression(span, moduleId, delegateTypeSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, delegateTypeSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, delegateTypeSymbol);
            }
            break;
        }
        case SymbolType::classDelegateTypeSymbol:
        {
            ClassDelegateTypeSymbol* classDelegateTypeSymbol = static_cast<ClassDelegateTypeSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), classDelegateTypeSymbol);
            expression.reset(new BoundTypeExpression(span, moduleId, classDelegateTypeSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, classDelegateTypeSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, classDelegateTypeSymbol);
            }
            break;
        }
        case SymbolType::typedefSymbol:
        {
            TypedefSymbol* typedefSymbol = static_cast<TypedefSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), typedefSymbol);
            expression.reset(new BoundTypeExpression(span, moduleId, typedefSymbol->GetType()));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, typedefSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, typedefSymbol);
            }
            break;
        }
        case SymbolType::boundTemplateParameterSymbol:
        {
            BoundTemplateParameterSymbol* boundTemplateParameterSymbol = static_cast<BoundTemplateParameterSymbol*>(symbol);
            expression.reset(new BoundTypeExpression(span, moduleId, boundTemplateParameterSymbol->GetType()));
            break;
        }
        case SymbolType::parameterSymbol:
        {
            ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), parameterSymbol);
            expression.reset(new BoundParameter(spn, mid, parameterSymbol));
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, parameterSymbol);
            }
            break;
        }
        case SymbolType::localVariableSymbol:
        {
            LocalVariableSymbol* localVariableSymbol = static_cast<LocalVariableSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), localVariableSymbol);
            expression.reset(new BoundLocalVariable(spn, mid, localVariableSymbol));
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, localVariableSymbol);
            }
            break;
        }
        case SymbolType::memberVariableSymbol:
        {
            MemberVariableSymbol* memberVariableSymbol = static_cast<MemberVariableSymbol*>(symbol);
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, memberVariableSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, memberVariableSymbol);
            }
            FunctionSymbol* currentFuctionSymbol = boundFunction->GetFunctionSymbol();
            CheckAccess(currentFuctionSymbol, memberVariableSymbol);
            BoundMemberVariable* bmv = new BoundMemberVariable(spn, mid, memberVariableSymbol);
            bool accessFromOwnScope = false;
            ClassTypeSymbol* currentClass = currentFuctionSymbol->ContainingClassNoThrow();
            if (currentClass)
            {
                ClassTypeSymbol* cp = memberVariableSymbol->ContainingClassNoThrow();
                Assert(cp, "class type symbol expected");
                if (cp == currentClass)
                {
                    accessFromOwnScope = true;
                }
            }
            if (memberVariableSymbol->IsStatic())
            {
                if (!accessFromOwnScope)
                {
                    bmv->SetStaticInitNeeded();
                }
            }
            else
            {
                ParameterSymbol* thisParam = currentFuctionSymbol->GetThisParam();
                if (accessFromOwnScope && !currentFuctionSymbol->IsStatic())
                {
                    if (thisParam)
                    {
                        TypeSymbol* thisPointerType = thisParam->GetType()->BaseType()->AddPointer(span, moduleId);
                        if (thisParam->GetType()->IsConstType())
                        {
                            thisPointerType = thisPointerType->AddConst(span, moduleId);
                        }
                        bmv->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundParameter(spn, mid, thisParam)));
                    }
                    else
                    {
                        Assert(false, "this param expected");
                    }
                }
                else if (thisParam)
                {
                    ClassTypeSymbol* containingClass = memberVariableSymbol->ContainingClassNoThrow();
                    TypeSymbol* containingClassPointerType = containingClass->AddPointer(span, moduleId);
                    TypeSymbol* thisPointerType = thisParam->GetType()->BaseType()->AddPointer(span, moduleId);
                    if (thisParam->GetType()->IsConstType())
                    {
                        thisPointerType = thisPointerType->AddConst(span, moduleId);
                        containingClassPointerType->AddConst(span, moduleId);
                    }
                    ArgumentMatch argumentMatch;
                    FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(thisPointerType, containingClassPointerType, containerScope, boundFunction, span, moduleId, argumentMatch);
                    if (conversionFun)
                    {
                        bmv->SetClassPtr(std::unique_ptr<BoundExpression>(new BoundConversion(std::unique_ptr<BoundExpression>(new BoundParameter(
                            spn, mid, thisParam)), conversionFun)));
                    }
                }
            }
            expression.reset(bmv);
            break;
        }
        case SymbolType::constantSymbol:
        {
            ConstantSymbol* constantSymbol = static_cast<ConstantSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), constantSymbol);
            expression.reset(new BoundConstant(spn, mid, constantSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, constantSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, constantSymbol);
            }
            break;
        }
        case SymbolType::enumTypeSymbol:
        {
            EnumTypeSymbol* enumTypeSymbol = static_cast<EnumTypeSymbol*>(symbol);
            CheckAccess(boundFunction->GetFunctionSymbol(), enumTypeSymbol);
            expression.reset(new BoundTypeExpression(span, moduleId, enumTypeSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, enumTypeSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, enumTypeSymbol);
            }
            break;
        }
        case SymbolType::enumConstantSymbol:
        {
            EnumConstantSymbol* enumConstantSymbol = static_cast<EnumConstantSymbol*>(symbol);
            expression.reset(new BoundEnumConstant(spn, mid, enumConstantSymbol));
            if (idNode && GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, enumConstantSymbol);
            }
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, enumConstantSymbol);
            }
            break;
        }
        case SymbolType::namespaceSymbol:
        {
            NamespaceSymbol* ns = static_cast<NamespaceSymbol*>(symbol);
            expression.reset(new BoundNamespaceExpression(span, moduleId, ns));
            if (idNode)
            {
                MapIdentifierToSymbolDefinition(idNode, ns);
            }
            break;
        }
        case SymbolType::globalVariableGroupSymbol:
        {
            GlobalVariableSymbol* globalVariableSymbol = nullptr;
            GlobalVariableGroupSymbol* globalVariableGroup = static_cast<GlobalVariableGroupSymbol*>(symbol);
            std::vector<GlobalVariableSymbol*> globalVariables;
            globalVariableGroup->CollectGlobalVariables(boundCompileUnit.GetCompileUnitNode()->FilePath(), globalVariables);
            if (globalVariables.empty())
            {
                throw Exception("global variable group '" + ToUtf8(globalVariableGroup->Name()) + "' contains no relevant public or internal global variables", span, moduleId);
            }
            else if (globalVariables.size() == 1)
            {
                globalVariableSymbol = globalVariables.front();
                if (globalVariableSymbol->Access() == SymbolAccess::private_ && globalVariableSymbol->CompileUnitFilePath() != boundCompileUnit.GetCompileUnitNode()->FilePath())
                {
                    throw Exception("global variable group '" + ToUtf8(globalVariableGroup->Name()) +
                        "' does not contain a public or internal global variable with the given name but do contain a private global variable defined in the compile unit " +
                        globalVariableSymbol->CompileUnitFilePath(), span, moduleId, globalVariableSymbol->GetSpan(), globalVariableSymbol->SourceModuleId());
                }
                else
                {
                    expression.reset(new BoundGlobalVariable(spn, mid, globalVariableSymbol));
                }
            }
            else
            {
                std::string compileUnits;
                bool first = true;
                for (GlobalVariableSymbol* globalVariableSymbol : globalVariables)
                {
                    if (first)
                    {
                        first = false;
                    }
                    else
                    {
                        compileUnits.append(", ");
                    }
                    compileUnits.append(globalVariableSymbol->CompileUnitFilePath());
                }
                throw Exception("global variable group '" + ToUtf8(globalVariableGroup->Name()) +
                    "' does not contain a public or internal global variable with the given name but do contain private global variables defined in the following compile units: " + compileUnits, 
                    span, moduleId);
            }
            if (idNode)
            {
                if (globalVariableSymbol)
                {
                    MapIdentifierToSymbolDefinition(idNode, globalVariableSymbol);
                }
            }
            break;
        }
        default:
        {
            throw Exception("could not bind '" + ToUtf8(symbol->FullName()) + "'", span, moduleId, symbol->GetSpan(), symbol->SourceModuleId());
        }
    }
}

void ExpressionBinder::Visit(BoolNode& boolNode)
{
    expression.reset(new BoundTypeExpression(boolNode.GetSpan(), boolNode.ModuleId(), symbolTable.GetTypeByName(U"bool")));
}

void ExpressionBinder::Visit(SByteNode& sbyteNode)
{
    expression.reset(new BoundTypeExpression(sbyteNode.GetSpan(), sbyteNode.ModuleId(), symbolTable.GetTypeByName(U"sbyte")));
}

void ExpressionBinder::Visit(ByteNode& byteNode)
{
    expression.reset(new BoundTypeExpression(byteNode.GetSpan(), byteNode.ModuleId(), symbolTable.GetTypeByName(U"byte")));
}

void ExpressionBinder::Visit(ShortNode& shortNode)
{
    expression.reset(new BoundTypeExpression(shortNode.GetSpan(), shortNode.ModuleId(), symbolTable.GetTypeByName(U"short")));
}

void ExpressionBinder::Visit(UShortNode& ushortNode)
{
    expression.reset(new BoundTypeExpression(ushortNode.GetSpan(), ushortNode.ModuleId(), symbolTable.GetTypeByName(U"ushort")));
}

void ExpressionBinder::Visit(IntNode& intNode)
{
    expression.reset(new BoundTypeExpression(intNode.GetSpan(), intNode.ModuleId(), symbolTable.GetTypeByName(U"int")));
}

void ExpressionBinder::Visit(UIntNode& uintNode)
{
    expression.reset(new BoundTypeExpression(uintNode.GetSpan(), uintNode.ModuleId(), symbolTable.GetTypeByName(U"uint")));
}

void ExpressionBinder::Visit(LongNode& longNode)
{
    expression.reset(new BoundTypeExpression(longNode.GetSpan(), longNode.ModuleId(), symbolTable.GetTypeByName(U"long")));
}

void ExpressionBinder::Visit(ULongNode& ulongNode)
{
    expression.reset(new BoundTypeExpression(ulongNode.GetSpan(), ulongNode.ModuleId(), symbolTable.GetTypeByName(U"ulong")));
}

void ExpressionBinder::Visit(FloatNode& floatNode)
{
    expression.reset(new BoundTypeExpression(floatNode.GetSpan(), floatNode.ModuleId(), symbolTable.GetTypeByName(U"float")));
}

void ExpressionBinder::Visit(DoubleNode& doubleNode)
{
    expression.reset(new BoundTypeExpression(doubleNode.GetSpan(), doubleNode.ModuleId(), symbolTable.GetTypeByName(U"double")));
}

void ExpressionBinder::Visit(CharNode& charNode)
{
    expression.reset(new BoundTypeExpression(charNode.GetSpan(), charNode.ModuleId(), symbolTable.GetTypeByName(U"char")));
}

void ExpressionBinder::Visit(WCharNode& wcharNode)
{
    expression.reset(new BoundTypeExpression(wcharNode.GetSpan(), wcharNode.ModuleId(), symbolTable.GetTypeByName(U"wchar")));
}

void ExpressionBinder::Visit(UCharNode& ucharNode)
{
    expression.reset(new BoundTypeExpression(ucharNode.GetSpan(), ucharNode.ModuleId(), symbolTable.GetTypeByName(U"uchar")));
}

void ExpressionBinder::Visit(VoidNode& voidNode)
{
    expression.reset(new BoundTypeExpression(voidNode.GetSpan(), voidNode.ModuleId(), symbolTable.GetTypeByName(U"void")));
}

void ExpressionBinder::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new BoolValue(booleanLiteralNode.GetSpan(), booleanLiteralNode.ModuleId(), booleanLiteralNode.Value())), symbolTable.GetTypeByName(U"bool")));
}

void ExpressionBinder::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new SByteValue(sbyteLiteralNode.GetSpan(), sbyteLiteralNode.ModuleId(), sbyteLiteralNode.Value())), symbolTable.GetTypeByName(U"sbyte")));
}

void ExpressionBinder::Visit(ByteLiteralNode& byteLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new ByteValue(byteLiteralNode.GetSpan(), byteLiteralNode.ModuleId(), byteLiteralNode.Value())), symbolTable.GetTypeByName(U"byte")));
}

void ExpressionBinder::Visit(ShortLiteralNode& shortLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new ShortValue(shortLiteralNode.GetSpan(), shortLiteralNode.ModuleId(), shortLiteralNode.Value())), symbolTable.GetTypeByName(U"short")));
}

void ExpressionBinder::Visit(UShortLiteralNode& ushortLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new UShortValue(ushortLiteralNode.GetSpan(), ushortLiteralNode.ModuleId(), ushortLiteralNode.Value())), symbolTable.GetTypeByName(U"ushort")));
}

void ExpressionBinder::Visit(IntLiteralNode& intLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new IntValue(intLiteralNode.GetSpan(), intLiteralNode.ModuleId(), intLiteralNode.Value())), symbolTable.GetTypeByName(U"int")));
}

void ExpressionBinder::Visit(UIntLiteralNode& uintLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new UIntValue(uintLiteralNode.GetSpan(), uintLiteralNode.ModuleId(), uintLiteralNode.Value())), symbolTable.GetTypeByName(U"uint")));
}

void ExpressionBinder::Visit(LongLiteralNode& longLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new LongValue(longLiteralNode.GetSpan(), longLiteralNode.ModuleId(), longLiteralNode.Value())), symbolTable.GetTypeByName(U"long")));
}

void ExpressionBinder::Visit(ULongLiteralNode& ulongLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new ULongValue(ulongLiteralNode.GetSpan(), ulongLiteralNode.ModuleId(), ulongLiteralNode.Value())), symbolTable.GetTypeByName(U"ulong")));
}

void ExpressionBinder::Visit(FloatLiteralNode& floatLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new FloatValue(floatLiteralNode.GetSpan(), floatLiteralNode.ModuleId(), floatLiteralNode.Value())), symbolTable.GetTypeByName(U"float")));
}

void ExpressionBinder::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new DoubleValue(doubleLiteralNode.GetSpan(), doubleLiteralNode.ModuleId(), doubleLiteralNode.Value())), symbolTable.GetTypeByName(U"double")));
}

void ExpressionBinder::Visit(CharLiteralNode& charLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new CharValue(charLiteralNode.GetSpan(), charLiteralNode.ModuleId(), charLiteralNode.Value())), symbolTable.GetTypeByName(U"char")));
}

void ExpressionBinder::Visit(WCharLiteralNode& wcharLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new WCharValue(wcharLiteralNode.GetSpan(), wcharLiteralNode.ModuleId(), wcharLiteralNode.Value())), symbolTable.GetTypeByName(U"wchar")));
}

void ExpressionBinder::Visit(UCharLiteralNode& ucharLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new UCharValue(ucharLiteralNode.GetSpan(), ucharLiteralNode.ModuleId(), ucharLiteralNode.Value())), symbolTable.GetTypeByName(U"uchar")));
}

void ExpressionBinder::Visit(StringLiteralNode& stringLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new StringValue(stringLiteralNode.GetSpan(), stringLiteralNode.ModuleId(), boundCompileUnit.Install(stringLiteralNode.Value()), stringLiteralNode.Value())),
        symbolTable.GetTypeByName(U"char")->AddConst(stringLiteralNode.GetSpan(), stringLiteralNode.ModuleId())->AddPointer(stringLiteralNode.GetSpan(), stringLiteralNode.ModuleId())));
}

void ExpressionBinder::Visit(WStringLiteralNode& wstringLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new WStringValue(wstringLiteralNode.GetSpan(), wstringLiteralNode.ModuleId(), boundCompileUnit.Install(wstringLiteralNode.Value()), wstringLiteralNode.Value())),
        symbolTable.GetTypeByName(U"wchar")->AddConst(wstringLiteralNode.GetSpan(), wstringLiteralNode.ModuleId())->AddPointer(wstringLiteralNode.GetSpan(), wstringLiteralNode.ModuleId())));
}

void ExpressionBinder::Visit(UStringLiteralNode& ustringLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new UStringValue(ustringLiteralNode.GetSpan(), ustringLiteralNode.ModuleId(), boundCompileUnit.Install(ustringLiteralNode.Value()), ustringLiteralNode.Value())),
        symbolTable.GetTypeByName(U"uchar")->AddConst(ustringLiteralNode.GetSpan(), ustringLiteralNode.ModuleId())->AddPointer(ustringLiteralNode.GetSpan(), ustringLiteralNode.ModuleId())));
}

void ExpressionBinder::Visit(NullLiteralNode& nullLiteralNode) 
{
    TypeSymbol* nullPtrType = symbolTable.GetTypeByName(U"@nullptr_type");
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new NullValue(nullLiteralNode.GetSpan(), nullLiteralNode.ModuleId(), nullPtrType)), nullPtrType));
}

void ExpressionBinder::Visit(UuidLiteralNode& uuidLiteralNode)
{
    expression.reset(new BoundLiteral(std::unique_ptr<Value>(new UuidValue(uuidLiteralNode.GetSpan(), uuidLiteralNode.ModuleId(), boundCompileUnit.Install(uuidLiteralNode.GetUuid()))),
        symbolTable.GetTypeByName(U"void")->AddPointer(uuidLiteralNode.GetSpan(), uuidLiteralNode.ModuleId())));
}

void ExpressionBinder::Visit(IdentifierNode& identifierNode)
{
    boundCompileUnit.SetLatestIdentifier(&identifierNode);
    std::u32string name = identifierNode.Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
        {
            symbol = fileScope->Lookup(name);
            if (symbol)
            {
                break;
            }
        }
    }
    if (symbol)
    {
        BindSymbol(symbol, &identifierNode);
    }
    else
    {
        throw Exception("symbol '" + ToUtf8(name) + "' not found", identifierNode.GetSpan(), identifierNode.ModuleId());
    }
}

void ExpressionBinder::Visit(TemplateIdNode& templateIdNode)
{
    int arity = templateIdNode.TemplateArguments().Count();
    templateIdNode.Primary()->Accept(*this);
    if (expression->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
    {
        TypeSymbol* typeSymbol = expression->GetType();
        if (typeSymbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
        {
            ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(typeSymbol);
            typeSymbol = classGroup->GetClass(arity);
            if (templateIdNode.Primary()->GetNodeType() == sngcm::ast::NodeType::identifierNode)
            {
                MapIdentifierToSymbolDefinition(static_cast<sngcm::ast::IdentifierNode*>(templateIdNode.Primary()), typeSymbol);
            }
            expression.reset(new BoundTypeExpression(span, moduleId, typeSymbol));
        }
    }
    std::vector<TypeSymbol*> templateArgumentTypes;
    int n = arity;
    for (int i = 0; i < n; ++i)
    {
        Node* templateArgumentNode = templateIdNode.TemplateArguments()[i];
        TypeSymbol* type = ResolveType(templateArgumentNode, boundCompileUnit, containerScope);
        templateArgumentTypes.push_back(type);
    }
    if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
    {
        BoundFunctionGroupExpression* bfge = static_cast<BoundFunctionGroupExpression*>(expression.get());
        bfge->SetTemplateArgumentTypes(templateArgumentTypes);
    }
    else if (expression->GetBoundNodeType() == BoundNodeType::boundMemberExpression)
    {
        BoundMemberExpression* bme = static_cast<BoundMemberExpression*>(expression.get());
        if (bme->Member()->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
        {
            BoundFunctionGroupExpression* bfge = static_cast<BoundFunctionGroupExpression*>(bme->Member());
            bfge->SetTemplateArgumentTypes(templateArgumentTypes);
        }
        else if (bme->Member()->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
        {
            TypeSymbol* typeSymbol = bme->Member()->GetType();
            if (typeSymbol->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(typeSymbol);
                if (classTypeSymbol->IsClassTemplate())
                {
                    int m = classTypeSymbol->TemplateParameters().size();
                    if (n < m)
                    {
                        boundCompileUnit.GetClassTemplateRepository().ResolveDefaultTemplateArguments(templateArgumentTypes, classTypeSymbol, containerScope, templateIdNode.GetSpan(), templateIdNode.ModuleId());
                    }
                    ClassTemplateSpecializationSymbol* classTemplateSpecialization = symbolTable.MakeClassTemplateSpecialization(classTypeSymbol, templateArgumentTypes, 
                        templateIdNode.GetSpan(), templateIdNode.ModuleId());
                    if (!classTemplateSpecialization->IsBound())
                    {
                        boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(classTemplateSpecialization, containerScope, templateIdNode.GetSpan(), templateIdNode.ModuleId());
                    }
                    expression.reset(new BoundTypeExpression(span, moduleId, classTemplateSpecialization));
                }
            }
        }
        else
        {
            throw Exception("function group or class group expected", templateIdNode.GetSpan(), templateIdNode.ModuleId());
        }
    }
    else if (expression->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
    {
        TypeSymbol* typeSymbol = expression->GetType();
        if (typeSymbol->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classTypeSymbol = static_cast<ClassTypeSymbol*>(typeSymbol);
            if (classTypeSymbol->IsClassTemplate())
            {
                int m = classTypeSymbol->TemplateParameters().size();
                if (n < m)
                {
                    boundCompileUnit.GetClassTemplateRepository().ResolveDefaultTemplateArguments(templateArgumentTypes, classTypeSymbol, containerScope, templateIdNode.GetSpan(), templateIdNode.ModuleId());
                }
                ClassTemplateSpecializationSymbol* classTemplateSpecialization = symbolTable.MakeClassTemplateSpecialization(classTypeSymbol, templateArgumentTypes, templateIdNode.GetSpan(), templateIdNode.ModuleId());
                if (!classTemplateSpecialization->IsBound())
                {
                    boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(classTemplateSpecialization, containerScope, templateIdNode.GetSpan(), templateIdNode.ModuleId());
                }
                expression.reset(new BoundTypeExpression(span, moduleId, classTemplateSpecialization));
            }
        }
    }
    else
    {
        throw Exception("function group or class group expected", templateIdNode.GetSpan(), templateIdNode.ModuleId());
    }
}

void ExpressionBinder::Visit(ParameterNode& parameterNode)
{
    if (!parameterNode.Id())
    {
        throw Exception("parameter not named", parameterNode.GetSpan(), parameterNode.ModuleId());
    }
    std::u32string name = parameterNode.Id()->Str();
    Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_and_base_and_parent);
    if (!symbol)
    {
        for (const std::unique_ptr<FileScope>& fileScope : boundCompileUnit.FileScopes())
        {
            symbol = fileScope->Lookup(name);
            if (symbol)
            {
                break;
            }
        }
    }
    if (symbol)
    {
        if (symbol->GetSymbolType() == SymbolType::parameterSymbol)
        {
            ParameterSymbol* parameterSymbol = static_cast<ParameterSymbol*>(symbol);
            expression.reset(new BoundParameter(parameterNode.GetSpan(), parameterNode.ModuleId(), parameterSymbol));
        }
        else
        {
            throw Exception("symbol '" + ToUtf8(name) + "' does not denote a parameter", parameterNode.GetSpan(), parameterNode.ModuleId());
        }
    }
    else
    {
        throw Exception("parameter symbol '" + ToUtf8(name) + "' not found", parameterNode.GetSpan(), parameterNode.ModuleId());
    }
}

void ExpressionBinder::Visit(DotNode& dotNode)
{
    ContainerScope* prevContainerScope = containerScope;
    expression = BindExpression(dotNode.Subject(), boundCompileUnit, boundFunction, containerScope, statementBinder, false, true, true, false);
    IdentifierNode* idNode = boundCompileUnit.GetLatestIdentifier();
    boundCompileUnit.SetLatestIdentifier(dotNode.MemberId());
    if (expression->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
    {
        TypeSymbol* typeSymbol = expression->GetType();
        if (typeSymbol->GetSymbolType() == SymbolType::classGroupTypeSymbol)
        {
            ClassGroupTypeSymbol* classGroupTypeSymbol = static_cast<ClassGroupTypeSymbol*>(typeSymbol);
            typeSymbol = classGroupTypeSymbol->GetClass(0);
            if (GetGlobalFlag(GlobalFlags::cmdoc))
            {
                symbolTable.MapSymbol(idNode, typeSymbol);
            }
            MapIdentifierToSymbolDefinition(idNode, typeSymbol);
            if (!typeSymbol)
            {
                throw Exception("ordinary class not found from class group '" + ToUtf8(classGroupTypeSymbol->FullName()) + "'", span, moduleId, classGroupTypeSymbol->GetSpan(), classGroupTypeSymbol->SourceModuleId());
            }
            else
            {
                expression.reset(new BoundTypeExpression(span, moduleId, typeSymbol));
            }
        }
    }
    if (expression->GetBoundNodeType() == BoundNodeType::boundNamespaceExpression)
    {
        BoundNamespaceExpression* bns = static_cast<BoundNamespaceExpression*>(expression.get());
        containerScope = bns->Ns()->GetContainerScope();
        std::u32string name = dotNode.MemberId()->Str();
        Symbol* symbol = containerScope->Lookup(name, ScopeLookup::this_);
        if (symbol)
        {
            BindSymbol(symbol, dotNode.MemberId());
            if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
            {
                BoundFunctionGroupExpression* bfe = static_cast<BoundFunctionGroupExpression*>(expression.get());
                bfe->SetScopeQualified();
                bfe->SetQualifiedScope(containerScope);
            }
        }
        else
        {
            throw Exception("symbol '" + ToUtf8(name) + "' not found from namespace '" + ToUtf8(bns->Ns()->FullName()) + "'", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
        }
    }
    else
    {
        TypeSymbol* type = expression->GetType()->PlainType(dotNode.GetSpan(), dotNode.ModuleId());
        if (type->IsClassDelegateType())
        {
            ClassDelegateTypeSymbol* classDelegateType = static_cast<ClassDelegateTypeSymbol*>(type);
            type = classDelegateType->ObjectDelegatePairType();
        }
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->BaseType());
            ContainerScope* scope = classType->GetContainerScope();
            std::u32string name = dotNode.MemberId()->Str();
            Symbol* symbol = scope->Lookup(name, ScopeLookup::this_and_base);
            if (symbol)
            {
                std::unique_ptr<BoundExpression> classPtr;
                BoundExpression* plainClassPtr = expression.get();
                if (expression->GetType()->IsClassTypeSymbol())
                {
                    TypeSymbol* type = expression->GetType()->AddPointer(dotNode.GetSpan(), dotNode.ModuleId());
                    classPtr.reset(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(expression.release()), type));
                }
                else if (expression->GetType()->IsReferenceType())
                {
                    TypeSymbol* type = expression->GetType()->RemoveReference(dotNode.GetSpan(), dotNode.ModuleId())->AddPointer(dotNode.GetSpan(), dotNode.ModuleId());
                    classPtr.reset(new BoundReferenceToPointerExpression(std::unique_ptr<BoundExpression>(expression.release()), type));
                }
                else
                {
                    classPtr.reset(expression.release());
                }
                BindSymbol(symbol, dotNode.MemberId());
                if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                {
                    BoundFunctionGroupExpression* bfg = static_cast<BoundFunctionGroupExpression*>(expression.get());
                    if (!classPtr->GetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr))
                    {
                        Symbol* parent = symbol->Parent();
                        Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
                        ClassTypeSymbol* owner = static_cast<ClassTypeSymbol*>(parent);
                        if (classType->HasBaseClass(owner))
                        {
                            if (classPtr->GetType()->IsConstType())
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddConst(span, moduleId)->AddPointer(span, moduleId), owner->AddConst(span, moduleId)->AddPointer(span, moduleId), containerScope, boundFunction, dotNode.GetSpan(), dotNode.ModuleId(), argumentMatch)));
                            }
                            else
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddPointer(span, moduleId), owner->AddPointer(span, moduleId), containerScope, boundFunction, dotNode.GetSpan(), dotNode.ModuleId(), argumentMatch)));
                            }
                        }
                    }
                    if (plainClassPtr->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
                    {
                        BoundTypeExpression* bte = static_cast<BoundTypeExpression*>(plainClassPtr);
                        bfg->SetScopeQualified();
                        bfg->SetQualifiedScope(bte->GetType()->GetContainerScope());
                    }
                    BoundMemberExpression* bme = new BoundMemberExpression(dotNode.GetSpan(), dotNode.ModuleId(), std::unique_ptr<BoundExpression>(classPtr.release()), std::move(expression));
                    expression.reset(bme);
                }
                else if (expression->GetBoundNodeType() == BoundNodeType::boundMemberVariable)
                {
                    BoundMemberVariable* bmv = static_cast<BoundMemberVariable*>(expression.get());
                    if (!bmv->GetMemberVariableSymbol()->IsStatic())
                    {
                        Symbol* parent = symbol->Parent();
                        Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
                        ClassTypeSymbol* owner = static_cast<ClassTypeSymbol*>(parent);
                        if (classType->HasBaseClass(owner))
                        {
                            if (classPtr->GetType()->IsConstType())
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddConst(span, moduleId)->AddPointer(span, moduleId), owner->AddConst(span, moduleId)->AddPointer(span, moduleId), containerScope, boundFunction, dotNode.GetSpan(), dotNode.ModuleId(), argumentMatch)));
                            }
                            else
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddPointer(span, moduleId), owner->AddPointer(span, moduleId), containerScope, boundFunction, dotNode.GetSpan(), dotNode.ModuleId(), argumentMatch)));
                            }
                        }
                        bmv->SetClassPtr(std::unique_ptr<BoundExpression>(classPtr.release()));
                    }
                }
                else if (expression->GetBoundNodeType() != BoundNodeType::boundTypeExpression && expression->GetBoundNodeType() != BoundNodeType::boundConstant)
                {
                    throw Exception("symbol '" + ToUtf8(name) + "' does not denote a function group, member variable, or type", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
                }
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from class '" + ToUtf8(classType->FullName()) + "'", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
            }
        }
        else if (type->GetSymbolType() == SymbolType::interfaceTypeSymbol)
        {
            InterfaceTypeSymbol* interfaceType = static_cast<InterfaceTypeSymbol*>(type->BaseType());
            ContainerScope* scope = interfaceType->GetContainerScope();
            std::u32string name = dotNode.MemberId()->Str();
            Symbol* symbol = scope->Lookup(name, ScopeLookup::this_);
            if (symbol)
            {
                std::unique_ptr<BoundExpression> interfacePtr;
                interfacePtr.reset(expression.release());
                BindSymbol(symbol, dotNode.MemberId());
                if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                {
                    BoundFunctionGroupExpression* bfg = static_cast<BoundFunctionGroupExpression*>(expression.get());
                    BoundMemberExpression* bme = new BoundMemberExpression(dotNode.GetSpan(), dotNode.ModuleId(), std::unique_ptr<BoundExpression>(interfacePtr.release()), std::move(expression));
                    expression.reset(bme);
                }
                else 
                {
                    throw Exception("symbol '" + ToUtf8(name) + "' does not denote a function group", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
                }
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from interface '" + ToUtf8(interfaceType->FullName()) + "'", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
            }
        }
        else if (type->GetSymbolType() == SymbolType::enumTypeSymbol)
        {
            EnumTypeSymbol* enumType = static_cast<EnumTypeSymbol*>(type);
            ContainerScope* scope = enumType->GetContainerScope();
            std::u32string name = dotNode.MemberId()->Str();
            Symbol* symbol = scope->Lookup(name);
            if (symbol)
            {
                BindSymbol(symbol, dotNode.MemberId());
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from enumerated type '" + ToUtf8(enumType->FullName()) + "'", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
            }
        }
        else if (type->GetSymbolType() == SymbolType::arrayTypeSymbol)
        {
            ArrayTypeSymbol* arrayType = static_cast<ArrayTypeSymbol*>(type);
            ContainerScope* scope = arrayType->GetContainerScope();
            std::u32string name = dotNode.MemberId()->Str();
            Symbol* symbol = scope->Lookup(name);
            if (symbol)
            {
                std::unique_ptr<BoundExpression> receiverPtr = std::move(expression);
                BindSymbol(symbol, dotNode.MemberId());
                if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                {
                    BoundFunctionGroupExpression* bfe = static_cast<BoundFunctionGroupExpression*>(expression.get());
                    bfe->SetScopeQualified();
                    bfe->SetQualifiedScope(scope);
                    bfe->SetClassPtr(std::move(receiverPtr));
                }
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from array type '" + ToUtf8(arrayType->FullName()) + "'", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
            }
        }
        else if (type->IsCharacterPointerType() && expression->GetBoundNodeType() == BoundNodeType::boundLiteral)
        {
            TypeSymbol* stringFunctionContainer = symbolTable.GetTypeByName(U"@string_functions");
            ContainerScope* scope = stringFunctionContainer->GetContainerScope();
            std::u32string name = dotNode.MemberId()->Str();
            Symbol* symbol = scope->Lookup(name);
            if (symbol)
            {
                std::unique_ptr<BoundExpression> receiverPtr = std::move(expression);
                BindSymbol(symbol, dotNode.MemberId());
                if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                {
                    BoundFunctionGroupExpression* bfe = static_cast<BoundFunctionGroupExpression*>(expression.get());
                    bfe->SetScopeQualified();
                    bfe->SetQualifiedScope(scope);
                    bfe->SetClassPtr(std::move(receiverPtr));
                }
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from string functions", dotNode.MemberId()->GetSpan(), dotNode.MemberId()->ModuleId());
            }
        }
        else
        {
            throw Exception("expression must denote a namespace, class type, interface type, array type or an enumerated type type object", dotNode.GetSpan(), dotNode.ModuleId());
        }
    }
    containerScope = prevContainerScope;
}

void ExpressionBinder::BindArrow(Node& node, const std::u32string& name)
{
    if (expression->GetType()->IsPointerType())
    {
        std::unique_ptr<BoundExpression> classPtr(std::move(expression));
        if (classPtr->GetType()->BaseType()->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(classPtr->GetType()->BaseType());
            ContainerScope* scope = classType->GetContainerScope();
            Symbol* symbol = scope->Lookup(name, ScopeLookup::this_and_base);
            if (symbol)
            {
                BindSymbol(symbol, nullptr);
                if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
                {
                    BoundFunctionGroupExpression* bfg = static_cast<BoundFunctionGroupExpression*>(expression.get());
                    if (!classPtr->GetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr))
                    {
                        Symbol* parent = symbol->Parent();
                        Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
                        ClassTypeSymbol* owner = static_cast<ClassTypeSymbol*>(parent);
                        if (classType->HasBaseClass(owner))
                        {
                            if (classPtr->GetType()->IsConstType())
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddConst(span, moduleId)->AddPointer(span, moduleId), owner->AddConst(span, moduleId)->AddPointer(span, moduleId), containerScope, boundFunction, node.GetSpan(), node.ModuleId(), argumentMatch)));
                            }
                            else
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddPointer(span, moduleId), owner->AddPointer(span, moduleId), containerScope, boundFunction, node.GetSpan(), node.ModuleId(), argumentMatch)));
                            }
                        }
                    }
                    BoundMemberExpression* bme = new BoundMemberExpression(node.GetSpan(), node.ModuleId(), std::unique_ptr<BoundExpression>(classPtr.release()), std::move(expression));
                    expression.reset(bme);
                }
                else if (expression->GetBoundNodeType() == BoundNodeType::boundMemberVariable)
                {
                    BoundMemberVariable* bmv = static_cast<BoundMemberVariable*>(expression.get());
                    if (!bmv->GetMemberVariableSymbol()->IsStatic())
                    {
                        Symbol* parent = symbol->Parent();
                        Assert(parent->GetSymbolType() == SymbolType::classTypeSymbol || parent->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol, "class type expected");
                        ClassTypeSymbol* owner = static_cast<ClassTypeSymbol*>(parent);
                        if (classType->HasBaseClass(owner))
                        {
                            if (classPtr->GetType()->IsConstType())
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddConst(span, moduleId)->AddPointer(span, moduleId), owner->AddConst(span, moduleId)->AddPointer(span, moduleId), containerScope, boundFunction, node.GetSpan(), node.ModuleId(), argumentMatch)));
                            }
                            else
                            {
                                ArgumentMatch argumentMatch;
                                classPtr.reset(new BoundConversion(std::unique_ptr<BoundExpression>(classPtr.release()),
                                    boundCompileUnit.GetConversion(classType->AddPointer(span, moduleId), owner->AddPointer(span, moduleId), containerScope, boundFunction, node.GetSpan(), node.ModuleId(), argumentMatch)));
                            }
                        }
                        bmv->SetClassPtr(std::unique_ptr<BoundExpression>(classPtr.release()));
                    }
                    else
                    {
                        throw Exception("member variable '" + ToUtf8(bmv->GetMemberVariableSymbol()->FullName()) + +"' is static", node.GetSpan(), node.ModuleId());
                    }
                }
                else
                {
                    throw Exception("symbol '" + ToUtf8(name) + "' does not denote a function group or a member variable", node.GetSpan(), node.ModuleId());
                }
            }
            else
            {
                throw Exception("symbol '" + ToUtf8(name) + "' not found from class '" + ToUtf8(classType->FullName()) + "'", node.GetSpan(), node.ModuleId());
            }
        }
        else
        {
            throw Exception("type of arrow expression subject must be pointer to class type", node.GetSpan(), node.ModuleId());
        }
    }
    else if (expression->GetType()->IsClassTypeSymbol())
    {
        TypeSymbol* type = expression->GetType();
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsBound())
            {
                boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
            }
        }
        TypeSymbol* pointerType = type->AddPointer(node.GetSpan(), node.ModuleId());
        LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, node.GetSpan(), node.ModuleId());
        Assert(expression->GetBoundNodeType() == BoundNodeType::boundFunctionCall, "function call expected");
        BoundFunctionCall* boundFunctionCall = static_cast<BoundFunctionCall*>(expression.get());
        boundFunctionCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(node.GetSpan(), node.ModuleId(), temporary)), pointerType)));
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
            if (classType->Destructor())
            {
                std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                destructorCall->AddArgument(std::unique_ptr<BoundExpression>(boundFunctionCall->Arguments().back()->Clone()));
                boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
            }
        }
        expression.reset(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(
            new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary)))), pointerType));
        BindUnaryOp(expression.release(), node, U"operator->");
        BindArrow(node, name);
    }
    else
    {
        throw Exception("arrow operator member function must return a class type object or a pointer to a class type object", node.GetSpan(), node.ModuleId());
    }
}

void ExpressionBinder::Visit(ArrowNode& arrowNode) 
{
    arrowNode.Subject()->Accept(*this);
    bool argIsExplicitThisOrBasePtr = expression->GetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
    if (expression->GetType()->IsReferenceType() && expression->GetType()->PlainType(arrowNode.GetSpan(), arrowNode.ModuleId())->IsClassTypeSymbol())
    {
        TypeSymbol* type = expression->GetType()->RemoveReference(arrowNode.GetSpan(), arrowNode.ModuleId())->AddPointer(arrowNode.GetSpan(), arrowNode.ModuleId());
        expression.reset(new BoundReferenceToPointerExpression(std::move(expression), type));
    }
    else if (expression->GetType()->IsReferenceType())
    {
        TypeSymbol* type = expression->GetType()->RemoveReference(arrowNode.GetSpan(), arrowNode.ModuleId())->AddPointer(arrowNode.GetSpan(), arrowNode.ModuleId());
        expression.reset(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundDereferenceExpression(std::move(expression), type)), type->AddPointer(arrowNode.GetSpan(), arrowNode.ModuleId())));
    }
    else
    {
        TypeSymbol* type = expression->GetType()->AddPointer(arrowNode.GetSpan(), arrowNode.ModuleId());
        expression.reset(new BoundAddressOfExpression(std::move(expression), type));
    }
    BindUnaryOp(expression.release(), arrowNode, U"operator->");
    if (argIsExplicitThisOrBasePtr)
    {
        expression->SetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
    }
    BindArrow(arrowNode, arrowNode.MemberId()->Str());
    if (argIsExplicitThisOrBasePtr)
    {
        expression->SetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
    }
    boundCompileUnit.SetLatestIdentifier(arrowNode.MemberId());
}

void ExpressionBinder::Visit(DisjunctionNode& disjunctionNode) 
{
    std::unique_ptr<BoundExpression> left = BindExpression(disjunctionNode.Left(), boundCompileUnit, boundFunction, containerScope, statementBinder);
    std::unique_ptr<BoundExpression> right = BindExpression(disjunctionNode.Right(), boundCompileUnit, boundFunction, containerScope, statementBinder);
    BoundDisjunction* boundDisjunction = new BoundDisjunction(disjunctionNode.GetSpan(), disjunctionNode.ModuleId(), std::move(left), std::move(right), symbolTable.GetTypeByName(U"bool"));
    LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"bool"), disjunctionNode.GetSpan(), disjunctionNode.ModuleId());
    boundDisjunction->SetTemporary(new BoundLocalVariable(disjunctionNode.GetSpan(), disjunctionNode.ModuleId(), temporary));
    expression.reset(boundDisjunction);
}

void ExpressionBinder::Visit(ConjunctionNode& conjunctionNode) 
{
    std::unique_ptr<BoundExpression> left = BindExpression(conjunctionNode.Left(), boundCompileUnit, boundFunction, containerScope, statementBinder);
    std::unique_ptr<BoundExpression> right = BindExpression(conjunctionNode.Right(), boundCompileUnit, boundFunction, containerScope, statementBinder);
    BoundConjunction* boundConjunction = new BoundConjunction(conjunctionNode.GetSpan(), conjunctionNode.ModuleId(), std::move(left), std::move(right), symbolTable.GetTypeByName(U"bool"));
    LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"bool"), conjunctionNode.GetSpan(), conjunctionNode.ModuleId());
    boundConjunction->SetTemporary(new BoundLocalVariable(conjunctionNode.GetSpan(), conjunctionNode.ModuleId(), temporary));
    expression.reset(boundConjunction);
}

void ExpressionBinder::Visit(BitOrNode& bitOrNode) 
{
    BindBinaryOp(bitOrNode, U"operator|");
}

void ExpressionBinder::Visit(BitXorNode& bitXorNode) 
{
    BindBinaryOp(bitXorNode, U"operator^");
}

void ExpressionBinder::Visit(BitAndNode& bitAndNode) 
{
    BindBinaryOp(bitAndNode, U"operator&");
}

void ExpressionBinder::Visit(EqualNode& equalNode) 
{
    BindBinaryOp(equalNode, U"operator==");
}

void ExpressionBinder::Visit(NotEqualNode& notEqualNode) 
{
    BindBinaryOp(notEqualNode, U"operator==");
    BindUnaryOp(expression.release(), notEqualNode, U"operator!");
}

void ExpressionBinder::Visit(LessNode& lessNode) 
{
    BindBinaryOp(lessNode, U"operator<");
}

void ExpressionBinder::Visit(GreaterNode& greaterNode) 
{
    greaterNode.Left()->Accept(*this);
    BoundExpression* left = expression.release();
    greaterNode.Right()->Accept(*this);
    BoundExpression* right = expression.release();
    BindBinaryOp(right, left, greaterNode, U"operator<");
}

void ExpressionBinder::Visit(LessOrEqualNode& lessOrEqualNode) 
{
    lessOrEqualNode.Left()->Accept(*this);
    BoundExpression* left = expression.release();
    lessOrEqualNode.Right()->Accept(*this);
    BoundExpression* right = expression.release();
    BindBinaryOp(right, left, lessOrEqualNode, U"operator<");
    BindUnaryOp(expression.release(), lessOrEqualNode, U"operator!");
}

void ExpressionBinder::Visit(GreaterOrEqualNode& greaterOrEqualNode) 
{
    BindBinaryOp(greaterOrEqualNode, U"operator<");
    BindUnaryOp(expression.release(), greaterOrEqualNode, U"operator!");
}

void ExpressionBinder::Visit(ShiftLeftNode& shiftLeftNode) 
{
    BindBinaryOp(shiftLeftNode, U"operator<<");
}

void ExpressionBinder::Visit(ShiftRightNode& shiftRightNode) 
{
    BindBinaryOp(shiftRightNode, U"operator>>");
}

void ExpressionBinder::Visit(AddNode& addNode) 
{
    BindBinaryOp(addNode, U"operator+");
}

void ExpressionBinder::Visit(SubNode& subNode) 
{
    BindBinaryOp(subNode, U"operator-");
}

void ExpressionBinder::Visit(MulNode& mulNode) 
{
    BindBinaryOp(mulNode, U"operator*");
}

void ExpressionBinder::Visit(DivNode& divNode) 
{
    BindBinaryOp(divNode, U"operator/");
}

void ExpressionBinder::Visit(RemNode& remNode) 
{
    BindBinaryOp(remNode, U"operator%");
}

void ExpressionBinder::Visit(NotNode& notNode) 
{
    BindUnaryOp(notNode, U"operator!");
}

void ExpressionBinder::Visit(UnaryPlusNode& unaryPlusNode) 
{
    BindUnaryOp(unaryPlusNode, U"operator+");
}

void ExpressionBinder::Visit(UnaryMinusNode& unaryMinusNode) 
{
    BindUnaryOp(unaryMinusNode, U"operator-");
}

void ExpressionBinder::Visit(PrefixIncrementNode& prefixIncrementNode) 
{
    if (statementBinder->CompilingThrow())
    {
        throw Exception("prefix increment in throw expression no allowed", prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId());
    }
    prefixIncrementNode.Subject()->Accept(*this);
    if (expression->GetType()->PlainType(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId())->IsClassTypeSymbol())
    {
        BindUnaryOp(prefixIncrementNode, U"operator++");
    }
    else
    {
        if (!inhibitCompile)
        {
            if (expression->GetType()->IsUnsignedType())
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext),
                    new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext), 
                        new ByteLiteralNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), 1u)));
                statementBinder->CompileStatement(&assignmentStatement, false);
            }
            else
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext),
                    new AddNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), prefixIncrementNode.Subject()->Clone(cloneContext), 
                        new SByteLiteralNode(prefixIncrementNode.GetSpan(), prefixIncrementNode.ModuleId(), 1)));
                statementBinder->CompileStatement(&assignmentStatement, false);
            }
        }
        bool prevInhibitCompile = inhibitCompile;
        inhibitCompile = true;
        prefixIncrementNode.Subject()->Accept(*this);
        inhibitCompile = prevInhibitCompile;
    }
}

void ExpressionBinder::Visit(PrefixDecrementNode& prefixDecrementNode) 
{
    if (statementBinder->CompilingThrow())
    {
        throw Exception("prefix decrement in throw expression no allowed", prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId());
    }
    prefixDecrementNode.Subject()->Accept(*this);
    if (expression->GetType()->PlainType(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId())->IsClassTypeSymbol())
    {
        BindUnaryOp(prefixDecrementNode, U"operator--");
    }
    else
    {
        if (!inhibitCompile)
        {
            if (expression->GetType()->IsUnsignedType())
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext),
                    new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext), 
                        new ByteLiteralNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), 1u)));
                statementBinder->CompileStatement(&assignmentStatement, false);
            }
            else
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext),
                    new SubNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), prefixDecrementNode.Subject()->Clone(cloneContext), 
                        new SByteLiteralNode(prefixDecrementNode.GetSpan(), prefixDecrementNode.ModuleId(), 1)));
                statementBinder->CompileStatement(&assignmentStatement, false);
            }
        }
        bool prevInhibitCompile = inhibitCompile;
        inhibitCompile = true;
        prefixDecrementNode.Subject()->Accept(*this);
        inhibitCompile = prevInhibitCompile;
    }
}

void ExpressionBinder::BindDerefExpr(Node& node)
{
    if (expression->GetType()->IsPointerType())
    {
        TypeSymbol* type = expression->GetType()->RemovePointer(node.GetSpan(), node.ModuleId());
        expression.reset(new BoundDereferenceExpression(std::unique_ptr<BoundExpression>(expression.release()), type));
    }
    else 
    {
        TypeSymbol* plainSubjectType = expression->GetType()->PlainType(node.GetSpan(), node.ModuleId());
        if (plainSubjectType->IsClassTypeSymbol())
        {
            if (expression->GetType()->IsReferenceType())
            {
                TypeSymbol* type = expression->GetType()->RemoveReference(node.GetSpan(), node.ModuleId())->AddPointer(node.GetSpan(), node.ModuleId());
                expression.reset(new BoundReferenceToPointerExpression(std::move(expression), type));
            }
            else if (expression->GetType()->IsClassTypeSymbol())
            {
                TypeSymbol* type = expression->GetType()->AddPointer(node.GetSpan(), node.ModuleId());
                expression.reset(new BoundAddressOfExpression(std::move(expression), type));
            }
            BindUnaryOp(expression.release(), node, U"operator*");
        }
        else
        {
            throw Exception("dereference needs pointer or class type argument", node.GetSpan(), node.ModuleId());
        }
    }
}

void ExpressionBinder::Visit(DerefNode& derefNode) 
{
    derefNode.Subject()->Accept(*this);
    BindDerefExpr(derefNode);
}

void ExpressionBinder::Visit(AddrOfNode& addrOfNode) 
{
    addrOfNode.Subject()->Accept(*this);
    if (expression->IsLvalueExpression())
    {
        if (expression->GetType()->IsReferenceType())
        {
            TypeSymbol* type = expression->GetType()->RemoveReference(addrOfNode.GetSpan(), addrOfNode.ModuleId())->AddPointer(addrOfNode.GetSpan(), addrOfNode.ModuleId());
            expression.reset(new BoundReferenceToPointerExpression(std::unique_ptr<BoundExpression>(expression.release()), type));
        }
        else
        {
            TypeSymbol* type = expression->GetType()->AddPointer(addrOfNode.GetSpan(), addrOfNode.ModuleId());
            expression.reset(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(expression.release()), type));
        }
    }
    else
    {
        throw Exception("cannot take address of " + expression->TypeString(), addrOfNode.GetSpan(), addrOfNode.ModuleId());
    }
}

void ExpressionBinder::Visit(ComplementNode& complementNode) 
{
    BindUnaryOp(complementNode, U"operator~");
}

void ExpressionBinder::Visit(IsNode& isNode) 
{
    TypeSymbol* rightType = ResolveType(isNode.TargetTypeExpr(), boundCompileUnit, containerScope);
    if (rightType->IsPointerType())
    {
        TypeSymbol* rightBaseType = rightType->RemovePointer(span, moduleId);
        if (rightBaseType->IsClassTypeSymbol())
        {
            ClassTypeSymbol* rightClassType = static_cast<ClassTypeSymbol*>(rightBaseType);
            if (rightClassType->IsPolymorphic())
            {
                std::unique_ptr<BoundExpression> boundExpr = BindExpression(isNode.Expr(), boundCompileUnit, boundFunction, containerScope, statementBinder, false, false, false, false);
                TypeSymbol* leftType = boundExpr->GetType();
                if (leftType->IsPointerType())
                {
                    TypeSymbol* leftBaseType = leftType->RemovePointer(span, moduleId);
                    if (leftBaseType->IsClassTypeSymbol())
                    {
                        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftBaseType);
                        if (leftClassType->IsPolymorphic())
                        {
                            std::unique_ptr<BoundLocalVariable> leftClassIdVar(new BoundLocalVariable(isNode.GetSpan(), isNode.ModuleId(), 
                                boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"ulong"), isNode.GetSpan(), isNode.ModuleId())));
                            std::unique_ptr<BoundLocalVariable> rightClassIdVar(new BoundLocalVariable(isNode.GetSpan(), isNode.ModuleId(), 
                                boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"ulong"), isNode.GetSpan(), isNode.ModuleId())));
                            expression.reset(new BoundIsExpression(std::move(boundExpr), rightClassType, symbolTable.GetTypeByName(U"bool"),
                                std::move(leftClassIdVar), std::move(rightClassIdVar)));
                        }
                        else
                        {
                            throw Exception("left type in 'is' expression must be pointer to polymorphic class type", isNode.Expr()->GetSpan(), isNode.Expr()->ModuleId());
                        }
                    }
                    else
                    {
                        throw Exception("left type in 'is' expression must be pointer to polymorphic class type", isNode.Expr()->GetSpan(), isNode.Expr()->ModuleId());
                    }
                }
                else
                {
                    throw Exception("left type in 'is' expression must be pointer to polymorphic class type", isNode.Expr()->GetSpan(), isNode.Expr()->ModuleId());
                }
            }
            else
            {
                throw Exception("right type in 'is' expression must be pointer to polymorphic class type", isNode.TargetTypeExpr()->GetSpan(), isNode.TargetTypeExpr()->ModuleId());
            }
        }
        else
        {
            throw Exception("right type in 'is' expression must be be pointer to polymorphic class type", isNode.TargetTypeExpr()->GetSpan(), isNode.TargetTypeExpr()->ModuleId());
        }
    }
    else
    {
        throw Exception("right type in 'is' expression must be be pointer to polymorphic class type", isNode.TargetTypeExpr()->GetSpan(), isNode.TargetTypeExpr()->ModuleId());
    }
}

void ExpressionBinder::Visit(AsNode& asNode) 
{
    TypeSymbol* rightType = ResolveType(asNode.TargetTypeExpr(), boundCompileUnit, containerScope);
    if (rightType->IsPointerType())
    {
        TypeSymbol* rightBaseType = rightType->RemovePointer(span, moduleId);
        if (rightBaseType->IsClassTypeSymbol())
        {
            ClassTypeSymbol* rightClassType = static_cast<ClassTypeSymbol*>(rightBaseType);
            if (rightClassType->IsPolymorphic())
            {
                std::unique_ptr<BoundExpression> boundExpr = BindExpression(asNode.Expr(), boundCompileUnit, boundFunction, containerScope, statementBinder, false, false, false, false);
                TypeSymbol* leftType = boundExpr->GetType();
                if (leftType->IsPointerType())
                {
                    TypeSymbol* leftBaseType = leftType->RemovePointer(span, moduleId);
                    if (leftBaseType->IsClassTypeSymbol())
                    {
                        ClassTypeSymbol* leftClassType = static_cast<ClassTypeSymbol*>(leftBaseType);
                        if (leftClassType->IsPolymorphic())
                        {
                            std::unique_ptr<BoundLocalVariable> leftClassIdVar(new BoundLocalVariable(asNode.GetSpan(), asNode.ModuleId(), 
                                boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"ulong"), asNode.GetSpan(), asNode.ModuleId())));
                            std::unique_ptr<BoundLocalVariable> rightClassIdVar(new BoundLocalVariable(asNode.GetSpan(), asNode.ModuleId(),
                                boundFunction->GetFunctionSymbol()->CreateTemporary(symbolTable.GetTypeByName(U"ulong"), asNode.GetSpan(), asNode.ModuleId())));
                            expression.reset(new BoundAsExpression(std::move(boundExpr), rightClassType,
                                std::unique_ptr<BoundLocalVariable>(new BoundLocalVariable(asNode.GetSpan(), asNode.ModuleId(), boundFunction->GetFunctionSymbol()->CreateTemporary(
                                    rightClassType->AddPointer(asNode.GetSpan(), asNode.ModuleId()), asNode.GetSpan(), asNode.ModuleId()))),
                                std::move(leftClassIdVar), std::move(rightClassIdVar)));
                        }
                        else
                        {
                            throw Exception("left type in 'as' expression must be pointer to polymorphic class type", asNode.Expr()->GetSpan(), asNode.Expr()->ModuleId());
                        }
                    }
                    else
                    {
                        throw Exception("left type in 'as' expression must be pointer to polymorphic class type", asNode.Expr()->GetSpan(), asNode.Expr()->ModuleId());
                    }
                }
                else
                {
                    throw Exception("left type in 'as' expression must be pointer to polymorphic class type", asNode.Expr()->GetSpan(), asNode.Expr()->ModuleId());
                }
            }
            else
            {
                throw Exception("right type in 'as' expression must be pointer to polymorphic class type", asNode.TargetTypeExpr()->GetSpan(), asNode.TargetTypeExpr()->ModuleId());
            }
        }
        else
        {
            throw Exception("right type in 'as' expression must be be pointer to polymorphic class type", asNode.TargetTypeExpr()->GetSpan(), asNode.TargetTypeExpr()->ModuleId());
        }
    }
    else
    {
        throw Exception("right type in 'as' expression must be be pointer to polymorphic class type", asNode.TargetTypeExpr()->GetSpan(), asNode.TargetTypeExpr()->ModuleId());
    }
}

void ExpressionBinder::Visit(IndexingNode& indexingNode) 
{
    indexingNode.Subject()->Accept(*this);
    std::unique_ptr<BoundExpression> subject = std::move(expression);
    indexingNode.Index()->Accept(*this);
    std::unique_ptr<BoundExpression> index = std::move(expression);
    TypeSymbol* plainSubjectType = subject->GetType()->PlainType(indexingNode.GetSpan(), indexingNode.ModuleId());
    if (plainSubjectType->IsClassTypeSymbol())
    {
        BindBinaryOp(subject.release(), index.release(), indexingNode, U"operator[]");
    }
    else  if (plainSubjectType->IsPointerType())
    {
        BindBinaryOp(subject.release(), index.release(), indexingNode, U"operator+");
        BindDerefExpr(indexingNode);
    }
    else if (plainSubjectType->IsArrayType())
    {
        std::unique_ptr<Value> value = Evaluate(&indexingNode, static_cast<ArrayTypeSymbol*>(plainSubjectType)->ElementType(), containerScope, boundCompileUnit, true, boundFunction, 
            indexingNode.GetSpan(), indexingNode.ModuleId());
        if (value)
        {
            expression.reset(new BoundLiteral(std::move(value), value->GetType(&symbolTable)));
        }
        else
        {
            BindBinaryOp(subject.release(), index.release(), indexingNode, U"operator[]");
        }
    }
    else
    {
        throw Exception("subscript operator can be applied only to pointer, array or class type subject", indexingNode.GetSpan(), indexingNode.ModuleId());
    }
}

void ExpressionBinder::Visit(InvokeNode& invokeNode) 
{
    IdentifierNode* prevIdentifier = boundCompileUnit.GetLatestIdentifier();
    boundCompileUnit.SetLatestIdentifier(nullptr);
    invokeNode.Subject()->Accept(*this);
    IdentifierNode* invokeId = boundCompileUnit.GetLatestIdentifier();
    boundCompileUnit.SetLatestIdentifier(prevIdentifier);
    bool argIsExplicitThisOrBasePtr = expression->GetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    FunctionGroupSymbol* functionGroupSymbol = nullptr;
    std::vector<TypeSymbol*> templateArgumentTypes;
    std::u32string groupName;
    bool scopeQualified = false;
    LocalVariableSymbol* temporary = nullptr;
    if (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
    {
        BoundFunctionGroupExpression* bfge = static_cast<BoundFunctionGroupExpression*>(expression.get());
        functionGroupSymbol = bfge->FunctionGroup();
        templateArgumentTypes = bfge->TemplateArgumentTypes();
        groupName = functionGroupSymbol->Name();
        if (bfge->IsScopeQualified())
        {
            functionScopeLookups.clear();
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, bfge->QualifiedScope()));
            scopeQualified = true;
            if (bfge->ClassPtr())
            {
                arguments.push_back(std::unique_ptr<BoundExpression>(bfge->ReleaseClassPtr()));
            }
        }
    }
    else if (expression->GetBoundNodeType() == BoundNodeType::boundMemberExpression)
    {
        BoundMemberExpression* bme = static_cast<BoundMemberExpression*>(expression.get());
        if (bme->Member()->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression)
        {
            BoundFunctionGroupExpression* bfge = static_cast<BoundFunctionGroupExpression*>(bme->Member());
            functionGroupSymbol = bfge->FunctionGroup();
            templateArgumentTypes = bfge->TemplateArgumentTypes();
            groupName = functionGroupSymbol->Name();
            if (bfge->IsScopeQualified())
            {
                functionScopeLookups.clear();
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, bfge->QualifiedScope()));
                scopeQualified = true;
            }
            if (!scopeQualified)
            {
                functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base, bme->ClassPtr()->GetType()->BaseType()->ClassInterfaceOrNsScope()));
            }
            arguments.push_back(std::unique_ptr<BoundExpression>(bme->ReleaseClassPtr()));
            if (arguments.front()->GetType()->PlainType(span, moduleId)->GetSymbolType() == SymbolType::interfaceTypeSymbol)
            {
                if (arguments.front()->GetType()->IsReferenceType())
                {
                    TypeSymbol* type = arguments.front()->GetType()->RemoveReference(span, moduleId)->AddPointer(span, moduleId);
                    arguments[0].reset(new BoundReferenceToPointerExpression(std::move(arguments[0]), type));
                }
                else
                {
                    TypeSymbol* type = arguments.front()->GetType()->AddPointer(span, moduleId);
                    arguments[0].reset(new BoundAddressOfExpression(std::move(arguments[0]), type));
                }
            }
        }
        else
        {
            throw Exception("invoke cannot be applied to this type of expression", invokeNode.Subject()->GetSpan(), invokeNode.Subject()->ModuleId());
        }
    }
    else if (expression->GetBoundNodeType() == BoundNodeType::boundTypeExpression)
    {
        TypeSymbol* type = expression->GetType();
        if (type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
        {
            ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(type);
            ClassTypeSymbol* classTypeSymbol = classGroup->GetClass(0);
            if (!classTypeSymbol)
            {
                throw Exception("ordinary class not found from class group '" + ToUtf8(classGroup->FullName()) + "'", span, moduleId, classGroup->GetSpan(), classGroup->SourceModuleId());
            }
            expression.reset(new BoundTypeExpression(span, moduleId, classTypeSymbol));
            type = classTypeSymbol;
        }
        if (!scopeQualified)
        {
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base, type->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
        }
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsBound())
            {
                boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
            }
        }
        temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, invokeNode.GetSpan(), invokeNode.ModuleId());
        std::unique_ptr<BoundExpression> addrOfTemporary(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(invokeNode.GetSpan(), invokeNode.ModuleId(), temporary)), type->AddPointer(invokeNode.GetSpan(), invokeNode.ModuleId())));
        arguments.push_back(std::move(addrOfTemporary));
        groupName = U"@constructor";
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
            if (classType->Destructor())
            {
                std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                destructorCall->AddArgument(std::unique_ptr<BoundExpression>(arguments.back()->Clone()));
                boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
            }
        }
    }
    else if (expression->GetType()->PlainType(span, moduleId)->IsClassTypeSymbol())
    {
        TypeSymbol* type = expression->GetType();
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type->PlainType(span, moduleId));
        groupName = U"operator()";
        functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_, classType->GetContainerScope()));
        if (type->IsReferenceType())
        {
            expression.reset(new BoundReferenceToPointerExpression(std::move(expression), type->RemoveReference(span, moduleId)->AddPointer(span, moduleId)));
        }
        else
        {
            expression.reset(new BoundAddressOfExpression(std::move(expression), type->AddPointer(span, moduleId)));
        }
        arguments.push_back(std::unique_ptr<BoundExpression>(expression.release()));
    }
    else if (expression->GetType()->PlainType(span, moduleId)->GetSymbolType() == SymbolType::delegateTypeSymbol)
    {
        TypeSymbol* type = expression->GetType();
        if (type->IsReferenceType())
        {
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundDereferenceExpression(std::move(expression), type->RemoveReference(span, moduleId))));
        }
        else
        {
            arguments.push_back(std::move(expression));
        }
        DelegateTypeSymbol* delegateTypeSymbol = static_cast<DelegateTypeSymbol*>(type->BaseType());
        int n = invokeNode.Arguments().Count();
        if (n != delegateTypeSymbol->Arity())
        {
            throw Exception("wrong number of arguments for calling delegate type '" + ToUtf8(delegateTypeSymbol->FullName()) + "'", span, moduleId);
        }
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* delegateParameterType = delegateTypeSymbol->Parameters()[i]->GetType();
            Node* argument = invokeNode.Arguments()[i];
            argument->Accept(*this);
            TypeSymbol* argumentType = expression->GetType();
            if (!TypesEqual(argumentType, delegateParameterType))
            {
                if (TypesEqual(argumentType->PlainType(span, moduleId), delegateParameterType->PlainType(span, moduleId)))
                {
                    if (argumentType->IsReferenceType() && !delegateParameterType->IsReferenceType())
                    {
                        TypeSymbol* type = argumentType->RemoveReference(span, moduleId);
                        BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(expression), type);
                        expression.reset(dereferenceExpression);
                    }
                    else if (!argumentType->IsReferenceType() && (delegateParameterType->IsReferenceType() || delegateParameterType->IsClassTypeSymbol()))
                    {
                        TypeSymbol* type = argumentType->AddLvalueReference(span, moduleId);
                        BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(expression), type);
                        expression.reset(addressOfExpression);
                    }
                }
                else
                {
                    ArgumentMatch argumentMatch;
                    FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(argumentType, delegateParameterType, containerScope, boundFunction, span, moduleId, argumentMatch);
                    if (conversionFun)
                    {
                        BoundConversion* conversion = new BoundConversion(std::move(expression), conversionFun);
                        expression.reset(conversion);
                    }
                    else
                    {
                        throw Exception("cannot convert '" + ToUtf8(argumentType->FullName()) + "' type argument to '" + ToUtf8(delegateParameterType->FullName()) + "' type parameter",
                            argument->GetSpan(), argument->ModuleId(), span, moduleId);
                    }
                }
            }
            arguments.push_back(std::unique_ptr<BoundExpression>(expression.release()));
        }
        BoundDelegateCall* delegateCall = new BoundDelegateCall(span, moduleId, delegateTypeSymbol);
        for (std::unique_ptr<BoundExpression>& argument : arguments)
        {
            delegateCall->AddArgument(std::move(argument));
        }
        LocalVariableSymbol* temporary = nullptr;
        if (delegateTypeSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            TypeSymbol* type = delegateTypeSymbol->ReturnType();
            if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
                if (!specialization->IsBound())
                {
                    boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
                }
            }
            temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, invokeNode.GetSpan(), invokeNode.ModuleId());
            delegateCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary)),
                type->AddPointer(invokeNode.GetSpan(), invokeNode.ModuleId()))));
            if (type->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
                if (classType->Destructor())
                {
                    std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                    destructorCall->AddArgument(std::unique_ptr<BoundExpression>(delegateCall->Arguments().back()->Clone()));
                    boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
                }
            }
        }
        expression.reset(delegateCall);
        if (temporary)
        {
            expression.reset(new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary))));
            expression->SetFlag(BoundExpressionFlags::bindToRvalueReference);
        }
        return;
    }
    else if (expression->GetType()->PlainType(span, moduleId)->GetSymbolType() == SymbolType::classDelegateTypeSymbol)
    {
        TypeSymbol* type = expression->GetType();
        if (type->IsReferenceType())
        {
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundReferenceToPointerExpression(std::move(expression), type->RemoveReference(span, moduleId)->AddPointer(span, moduleId))));
        }
        else
        {
            arguments.push_back(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::move(expression), type->AddPointer(span, moduleId))));
        }
        ClassDelegateTypeSymbol* classDelegateTypeSymbol = static_cast<ClassDelegateTypeSymbol*>(type->BaseType());
        int n = invokeNode.Arguments().Count();
        if (n != classDelegateTypeSymbol->Arity())
        {
            throw Exception("wrong number of arguments for calling delegate type '" + ToUtf8(classDelegateTypeSymbol->FullName()) + "'", span, moduleId);
        }
        for (int i = 0; i < n; ++i)
        {
            TypeSymbol* classDelegateParameterType = classDelegateTypeSymbol->Parameters()[i]->GetType();
            Node* argument = invokeNode.Arguments()[i];
            argument->Accept(*this);
            TypeSymbol* argumentType = expression->GetType();
            if (!TypesEqual(argumentType, classDelegateParameterType))
            {
                if (TypesEqual(argumentType->PlainType(span, moduleId), classDelegateParameterType->PlainType(span, moduleId)))
                {
                    if (argumentType->IsReferenceType() && !classDelegateParameterType->IsReferenceType())
                    {
                        TypeSymbol* type = argumentType->RemoveReference(span, moduleId);
                        BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(expression), type);
                        expression.reset(dereferenceExpression);
                    }
                    else if (!argumentType->IsReferenceType() && (classDelegateParameterType->IsReferenceType() || classDelegateParameterType->IsClassTypeSymbol()))
                    {
                        TypeSymbol* type = argumentType->AddLvalueReference(span, moduleId);
                        BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(expression), type);
                        expression.reset(addressOfExpression);
                    }
                }
                else
                {
                    ArgumentMatch argumentMatch;
                    FunctionSymbol* conversionFun = boundCompileUnit.GetConversion(argumentType, classDelegateParameterType, containerScope, boundFunction, span, moduleId, argumentMatch);
                    if (conversionFun)
                    {
                        BoundConversion* conversion = new BoundConversion(std::move(expression), conversionFun);
                        expression.reset(conversion);
                    }
                    else
                    {
                        throw Exception("cannot convert '" + ToUtf8(argumentType->FullName()) + "' type argument to '" + ToUtf8(classDelegateParameterType->FullName()) + "' type parameter",
                            argument->GetSpan(), argument->ModuleId(), span, moduleId);
                    }
                }
            }
            arguments.push_back(std::unique_ptr<BoundExpression>(expression.release()));
        }
        BoundClassDelegateCall* classDelegateCall = new BoundClassDelegateCall(span, moduleId, classDelegateTypeSymbol);
        for (std::unique_ptr<BoundExpression>& argument : arguments)
        {
            classDelegateCall->AddArgument(std::move(argument));
        }
        LocalVariableSymbol* temporary = nullptr;
        if (classDelegateTypeSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
        {
            TypeSymbol* type = classDelegateTypeSymbol->ReturnType();
            if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
            {
                ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
                if (!specialization->IsBound())
                {
                    boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
                }
            }
            temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, invokeNode.GetSpan(), invokeNode.ModuleId());
            classDelegateCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary)),
                type->AddPointer(invokeNode.GetSpan(), invokeNode.ModuleId()))));
            if (type->IsClassTypeSymbol())
            {
                ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
                if (classType->Destructor())
                {
                    std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                    destructorCall->AddArgument(std::unique_ptr<BoundExpression>(classDelegateCall->Arguments().back()->Clone()));
                    boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
                }
            }
        }
        expression.reset(classDelegateCall);
        if (temporary)
        {
            expression.reset(new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary))));
            expression->SetFlag(BoundExpressionFlags::bindToRvalueReference);
        }
        return;
    }
    else
    {
        throw Exception("invoke cannot be applied to this type of expression", invokeNode.Subject()->GetSpan(), invokeNode.Subject()->ModuleId());
    }
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* argument = invokeNode.Arguments()[i];
        argument->Accept(*this);
        if (expression->GetType()->GetSymbolType() != SymbolType::functionGroupTypeSymbol && !scopeQualified)
        {
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, expression->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
        }
        arguments.push_back(std::unique_ptr<BoundExpression>(expression.release()));
    }
    if (!scopeQualified)
    {
        functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    }
    std::unique_ptr<Exception> exception;
    std::unique_ptr<Exception> thisEx;
    std::unique_ptr<Exception> nsEx;
    if (!arguments.empty() && arguments[0]->GetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr))
    {
        argIsExplicitThisOrBasePtr = true;
    }
    std::unique_ptr<BoundFunctionCall> functionCall = ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, 
        invokeNode.GetSpan(), invokeNode.ModuleId(), OverloadResolutionFlags::dontThrow, templateArgumentTypes, exception);
    if (!functionCall)
    {
        ParameterSymbol* thisParam = boundFunction->GetFunctionSymbol()->GetThisParam();
        bool thisParamInserted = false;
        if (thisParam)
        {
            BoundParameter* boundThisParam = new BoundParameter(invokeNode.GetSpan(), invokeNode.ModuleId(), thisParam);
            arguments.insert(arguments.begin(), std::unique_ptr<BoundExpression>(boundThisParam));
            thisParamInserted = true;
            functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base, thisParam->GetType()->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
            functionCall = std::move(ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, invokeNode.GetSpan(), invokeNode.ModuleId(),
                OverloadResolutionFlags::dontThrow, templateArgumentTypes, thisEx));
        }
        if (!functionCall)
        {
            if (thisParamInserted)
            {
                arguments.erase(arguments.begin());
            }
            if (!arguments.empty())
            {
                arguments.erase(arguments.begin());
            }
            functionCall = std::move(ResolveOverload(groupName, containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, invokeNode.GetSpan(), invokeNode.ModuleId(),
                OverloadResolutionFlags::dontThrow, templateArgumentTypes, nsEx));
        }
    }
    if (!functionCall)
    {
        if (CastOverloadException* castException = dynamic_cast<CastOverloadException*>(exception.get()))
        {
            throw *exception;
        }
        if (CastOverloadException* castException = dynamic_cast<CastOverloadException*>(thisEx.get()))
        {
            throw *thisEx;
        }
        if (CastOverloadException* castException = dynamic_cast<CastOverloadException*>(nsEx.get()))
        {
            throw *nsEx;
        }
        if (CannotBindConstToNonconstOverloadException* bindException = dynamic_cast<CannotBindConstToNonconstOverloadException*>(exception.get()))
        {
            throw *exception;
        }
        if (CannotBindConstToNonconstOverloadException* bindException = dynamic_cast<CannotBindConstToNonconstOverloadException*>(thisEx.get()))
        {
            throw *thisEx;
        }
        if (CannotBindConstToNonconstOverloadException* bindException = dynamic_cast<CannotBindConstToNonconstOverloadException*>(nsEx.get()))
        {
            throw *nsEx;
        }
        if (CannotAssignToConstOverloadException* assignmentException = dynamic_cast<CannotAssignToConstOverloadException*>(exception.get()))
        {
            throw *exception;
        }
        if (CannotAssignToConstOverloadException* assignmentException = dynamic_cast<CannotAssignToConstOverloadException*>(thisEx.get()))
        {
            throw *thisEx;
        }
        if (CannotAssignToConstOverloadException* assignmentException = dynamic_cast<CannotAssignToConstOverloadException*>(nsEx.get()))
        {
            throw *nsEx;
        }
        Exception* ex = exception.get();
        if (dynamic_cast<NoViableFunctionException*>(ex) && thisEx)
        {
            ex = thisEx.get();
        }
        if (dynamic_cast<NoViableFunctionException*>(ex) && nsEx)
        {
            ex = nsEx.get();
        }
        if (ex)
        {
            throw *ex;
        }
        else
        {
            throw Exception("overload resolution failed: overload not found", invokeNode.GetSpan(), invokeNode.ModuleId());
        }
    }
    CheckAccess(boundFunction->GetFunctionSymbol(), functionCall->GetFunctionSymbol());
    FunctionSymbol* functionSymbol = functionCall->GetFunctionSymbol();
    if (functionSymbol->GetSymbolType() == SymbolType::memberFunctionSymbol && !functionSymbol->IsStatic() && functionSymbol->IsVirtualAbstractOrOverride())
    {
        if (argIsExplicitThisOrBasePtr)
        {
            if (functionSymbol->IsAbstract())
            {
                throw Exception("cannot call abstract member function", span, moduleId, functionSymbol->GetSpan(), functionSymbol->SourceModuleId());
            }
        }
        else
        {
            functionCall->SetFlag(BoundExpressionFlags::virtualCall);
        }
    }
    if (functionSymbol->ReturnsClassInterfaceOrClassDelegateByValue())
    {
        TypeSymbol* type = functionSymbol->ReturnType();
        if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(type);
            if (!specialization->IsBound())
            {
                boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, invokeNode.GetSpan(), invokeNode.ModuleId());
            }
        }
        temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(type, invokeNode.GetSpan(), invokeNode.ModuleId());
        functionCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(invokeNode.GetSpan(), invokeNode.ModuleId(), temporary)),
            type->AddPointer(invokeNode.GetSpan(), invokeNode.ModuleId()))));
        if (type->IsClassTypeSymbol())
        {
            ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(type);
            if (classType->Destructor())
            {
                std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                destructorCall->AddArgument(std::unique_ptr<BoundExpression>(functionCall->Arguments().back()->Clone()));
                boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
            }
        }
    }
    expression.reset(functionCall.release());
    if (temporary)
    {
        expression.reset(new BoundConstructAndReturnTemporaryExpression(std::move(expression), std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary))));
        expression->SetFlag(BoundExpressionFlags::bindToRvalueReference);
    }
    if (functionSymbol->IsConstExpr())
    {
        TypeSymbol* returnType = functionSymbol->ReturnType();
        if (returnType && !returnType->IsVoidType())
        {
            std::unique_ptr<Value> value = Evaluate(&invokeNode, returnType, containerScope, boundCompileUnit, true, boundFunction, span, moduleId);
            if (value)
            {
                TypeSymbol* type = value->GetType(&symbolTable);
                BoundLiteral* literal = new BoundLiteral(std::move(value), type);
                expression.reset(literal);
            }
        }
    }
    else if (functionSymbol->IsCompileTimePrimitiveFunction())
    {
        TypeSymbol* returnType = functionSymbol->ReturnType();
        if (returnType && !returnType->IsVoidType())
        {
            std::unique_ptr<Value> value = Evaluate(&invokeNode, returnType, containerScope, boundCompileUnit, true, boundFunction, span, moduleId);
            if (value)
            {
                TypeSymbol* type = value->GetType(&symbolTable);
                BoundLiteral* literal = new BoundLiteral(std::move(value), type);
                expression.reset(literal);
            }
        }
    }
    if (functionSymbol->FullName() == U"System.CaptureCurrentException()")
    {
        if (!statementBinder->InsideCatch())
        {
            throw Exception("System.CaptureCurrentException() can only be called from inside a catch block", span, moduleId);
        }
        else
        {
            expression->SetFlag(BoundExpressionFlags::exceptionCapture);
        }
    }
    if (GetGlobalFlag(GlobalFlags::cmdoc) && functionSymbol->HasSource())
    {
        symbolTable.MapInvoke(invokeId, functionSymbol);
    }
    if (functionSymbol->HasSource())
    {
        MapIdentifierToSymbolDefinition(invokeId, functionSymbol);
    }
}

void ExpressionBinder::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    if (statementBinder->CompilingThrow())
    {
        throw Exception("postfix increment in throw expression no allowed", postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId());
    }
    bool prevInhibitCompile = inhibitCompile;
    inhibitCompile = true;
    postfixIncrementNode.Subject()->Accept(*this);
    inhibitCompile = prevInhibitCompile;
    if (!inhibitCompile)
    {
        if (expression->GetType()->PlainType(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId())->IsClassTypeSymbol())
        {
            CloneContext cloneContext;
            ExpressionStatementNode prefixIncrementExpression(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), new PrefixIncrementNode(postfixIncrementNode.GetSpan(), 
                postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext)));
            statementBinder->CompileStatement(&prefixIncrementExpression, true);
        }
        else
        {
            if (expression->GetType()->IsUnsignedType())
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext),
                    new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext), 
                        new ByteLiteralNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), 1u)));
                statementBinder->CompileStatement(&assignmentStatement, true);
            }
            else
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext),
                    new AddNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), postfixIncrementNode.Subject()->Clone(cloneContext), 
                        new SByteLiteralNode(postfixIncrementNode.GetSpan(), postfixIncrementNode.ModuleId(), 1)));
                statementBinder->CompileStatement(&assignmentStatement, true);
            }
        }
    }
    postfixIncrementNode.Subject()->Accept(*this);
}

void ExpressionBinder::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    if (statementBinder->CompilingThrow())
    {
        throw Exception("postfix decrement in throw expression no allowed", postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId());
    }
    bool prevInhibitCompile = inhibitCompile;
    inhibitCompile = true;
    postfixDecrementNode.Subject()->Accept(*this);
    inhibitCompile = prevInhibitCompile;
    if (!inhibitCompile)
    {
        if (expression->GetType()->PlainType(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId())->IsClassTypeSymbol())
        {
            CloneContext cloneContext;
            ExpressionStatementNode prefixDecrementExpression(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), new PrefixDecrementNode(postfixDecrementNode.GetSpan(), 
                postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext)));
            statementBinder->CompileStatement(&prefixDecrementExpression, true);
        }
        else
        {
            if (expression->GetType()->IsUnsignedType())
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext),
                    new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext), 
                        new ByteLiteralNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), 1u)));
                statementBinder->CompileStatement(&assignmentStatement, true);
            }
            else
            {
                CloneContext cloneContext;
                AssignmentStatementNode assignmentStatement(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext),
                    new SubNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), postfixDecrementNode.Subject()->Clone(cloneContext),
                        new SByteLiteralNode(postfixDecrementNode.GetSpan(), postfixDecrementNode.ModuleId(), 1)));
                statementBinder->CompileStatement(&assignmentStatement, true);
            }
        }
    }
    postfixDecrementNode.Subject()->Accept(*this);
}

void ExpressionBinder::Visit(SizeOfNode& sizeOfNode) 
{
    sizeOfNode.Expression()->Accept(*this);
    if (expression->GetBoundNodeType() == BoundNodeType::boundTypeExpression && expression->GetType()->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(expression->GetType());
        ClassTypeSymbol* classTypeSymbol = classGroup->GetClass(0);
        if (classTypeSymbol)
        {
            expression.reset(new BoundTypeExpression(span, moduleId, classTypeSymbol));
        }
        else
        {
            throw Exception("ordinary class not found from class group '" + ToUtf8(classGroup->FullName()) + "'", span, moduleId, classGroup->GetSpan(), classGroup->SourceModuleId());
        }
    }
    expression.reset(new BoundSizeOfExpression(sizeOfNode.GetSpan(), sizeOfNode.ModuleId(), symbolTable.GetTypeByName(U"long"), expression->GetType()->AddPointer(sizeOfNode.GetSpan(), sizeOfNode.ModuleId())));
}

void ExpressionBinder::Visit(TypeNameNode& typeNameNode) 
{
    bool staticTypeName = typeNameNode.Static();
    std::unique_ptr<BoundExpression> expr = BindExpression(typeNameNode.Expression(), boundCompileUnit, boundFunction, containerScope, statementBinder, false, false, true, false);
    TypeSymbol* type = expr->GetType();
    if (type->GetSymbolType() == SymbolType::classGroupTypeSymbol)
    {
        ClassGroupTypeSymbol* classGroup = static_cast<ClassGroupTypeSymbol*>(type);
        ClassTypeSymbol* classTypeSymbol = classGroup->GetClass(0);
        if (!classTypeSymbol)
        {
            throw Exception("ordinary class not found from class group '" + ToUtf8(classGroup->FullName()) + "'", span, moduleId, classGroup->GetSpan(), classGroup->SourceModuleId());
        }
        expr.reset(new BoundTypeExpression(span, moduleId, classTypeSymbol));
        type = classTypeSymbol;
        staticTypeName = true;
    }
    if (expr->GetType()->PlainType(typeNameNode.GetSpan(), typeNameNode.ModuleId())->IsClassTypeSymbol())
    {
        ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(expr->GetType()->BaseType());
        if (!staticTypeName && classType->IsPolymorphic())
        {
            if (expr->GetBoundNodeType() == BoundNodeType::boundDereferenceExpression)
            {
                BoundDereferenceExpression* derefExpr = static_cast<BoundDereferenceExpression*>(expr.get());
                expr.reset(derefExpr->Subject().release());
            }
            else
            {
                TypeSymbol* ptrType = expr->GetType()->AddPointer(typeNameNode.GetSpan(), typeNameNode.ModuleId());
                expr.reset(new BoundAddressOfExpression(std::move(expr), ptrType));
            }
            expression.reset(new BoundTypeNameExpression(std::move(expr), symbolTable.GetTypeByName(U"char")->AddConst(typeNameNode.GetSpan(), typeNameNode.ModuleId())->AddPointer(
                typeNameNode.GetSpan(), typeNameNode.ModuleId())));
        }
        else
        {
            expression.reset(new BoundLiteral(std::unique_ptr<Value>(new StringValue(typeNameNode.GetSpan(), typeNameNode.ModuleId(), boundCompileUnit.Install(ToUtf8(classType->FullName())),
                ToUtf8(classType->FullName()))), symbolTable.GetTypeByName(U"char")->AddConst(typeNameNode.GetSpan(), typeNameNode.ModuleId())->AddPointer(typeNameNode.GetSpan(), typeNameNode.ModuleId())));
        }
    }
    else
    {
        expression.reset(new BoundLiteral(std::unique_ptr<Value>(new StringValue(typeNameNode.GetSpan(), typeNameNode.ModuleId(), boundCompileUnit.Install(ToUtf8(expr->GetType()->FullName())),
            ToUtf8(expr->GetType()->FullName()))), symbolTable.GetTypeByName(U"char")->AddConst(typeNameNode.GetSpan(), typeNameNode.ModuleId())->AddPointer(typeNameNode.GetSpan(), typeNameNode.ModuleId())));
    }
}

void ExpressionBinder::Visit(TypeIdNode& typeIdNode)
{
    std::unique_ptr<BoundExpression> expr = BindExpression(typeIdNode.Expression(), boundCompileUnit, boundFunction, containerScope, statementBinder, false, false, true, false);
    if (expr->GetType()->IsPointerType())
    {
        TypeSymbol* exprBaseType = expr->GetType()->RemovePointer(span, moduleId);
        if (exprBaseType->IsClassTypeSymbol())
        {
            ClassTypeSymbol* exprClassType = static_cast<ClassTypeSymbol*>(exprBaseType);
            if (exprClassType->IsPolymorphic())
            {
                expression.reset(new BoundTypeIdExpression(std::move(expr), symbolTable.GetTypeByName(U"ulong")));
            }
            else
            {
                throw Exception("typeid can be applied to a pointer to a polymorphic class type expression",
                    typeIdNode.GetSpan(), typeIdNode.ModuleId(), boundFunction->GetFunctionSymbol()->GetSpan(), boundFunction->GetFunctionSymbol()->SourceModuleId());
            }
        }
        else
        {
            throw Exception("typeid can be applied to a pointer to a polymorphic class type expression",
                typeIdNode.GetSpan(), typeIdNode.ModuleId(), boundFunction->GetFunctionSymbol()->GetSpan(), boundFunction->GetFunctionSymbol()->SourceModuleId());
        }
    }
    else
    {
        throw Exception("typeid can be applied to a pointer to a polymorphic class type expression", 
            typeIdNode.GetSpan(), typeIdNode.ModuleId(), boundFunction->GetFunctionSymbol()->GetSpan(), boundFunction->GetFunctionSymbol()->SourceModuleId());
    }
}

void ExpressionBinder::Visit(CastNode& castNode) 
{
    TypeSymbol* targetType = ResolveType(castNode.TargetTypeExpr(), boundCompileUnit, containerScope);
    castNode.SourceExpr()->Accept(*this);
    std::vector<std::unique_ptr<BoundExpression>> targetExprArgs;
    targetExprArgs.push_back(std::unique_ptr<BoundExpression>(new BoundTypeExpression(castNode.GetSpan(), castNode.ModuleId(), targetType)));
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, targetType->BaseType()->ClassInterfaceEnumDelegateOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    std::unique_ptr<BoundFunctionCall> castFunctionCall = ResolveOverload(U"@return", containerScope, functionScopeLookups, targetExprArgs, boundCompileUnit, boundFunction, castNode.GetSpan(),
        castNode.ModuleId());
    std::vector<std::unique_ptr<BoundExpression>> castArguments;
    castArguments.push_back(std::move(expression));
    FunctionMatch functionMatch(castFunctionCall->GetFunctionSymbol());
    bool conversionFound = FindConversions(boundCompileUnit, castFunctionCall->GetFunctionSymbol(), castArguments, functionMatch, ConversionType::explicit_, containerScope, boundFunction,
        castNode.GetSpan(), castNode.ModuleId());
    if (conversionFound)
    {
        Assert(!functionMatch.argumentMatches.empty(), "argument match expected");
        ArgumentMatch& argumentMatch = functionMatch.argumentMatches[0];
        if (argumentMatch.preReferenceConversionFlags != OperationFlags::none)
        {
            if (argumentMatch.preReferenceConversionFlags == OperationFlags::addr)
            {
                TypeSymbol* type = castArguments[0]->GetType()->AddLvalueReference(span, moduleId);
                BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(castArguments[0]), type);
                castArguments[0].reset(addressOfExpression);
            }
            else if (argumentMatch.preReferenceConversionFlags == OperationFlags::deref)
            {
                TypeSymbol* type = castArguments[0]->GetType()->RemoveReference(span, moduleId);
                BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(castArguments[0]), type);
                castArguments[0].reset(dereferenceExpression);
            }
        }
        FunctionSymbol* conversionFun = argumentMatch.conversionFun;
        if (conversionFun)
        {
            if (conversionFun->GetSymbolType() == SymbolType::constructorSymbol)
            {
                BoundFunctionCall* constructorCall = new BoundFunctionCall(span, moduleId, conversionFun);
                if (conversionFun->ConversionTargetType()->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
                {
                    ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(conversionFun->ConversionTargetType());
                    if (!specialization->IsBound())
                    {
                        boundCompileUnit.GetClassTemplateRepository().BindClassTemplateSpecialization(specialization, containerScope, span, moduleId);
                    }
                }
                LocalVariableSymbol* temporary = boundFunction->GetFunctionSymbol()->CreateTemporary(conversionFun->ConversionTargetType(), span, moduleId);
                constructorCall->AddArgument(std::unique_ptr<BoundExpression>(new BoundAddressOfExpression(std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary)),
                    conversionFun->ConversionTargetType()->AddPointer(span, moduleId))));
                TypeSymbol* conversionTargetType = conversionFun->ConversionTargetType();
                if (conversionTargetType->IsClassTypeSymbol())
                {
                    ClassTypeSymbol* classType = static_cast<ClassTypeSymbol*>(conversionTargetType);
                    if (classType->Destructor())
                    {
                        std::unique_ptr<BoundFunctionCall> destructorCall(new BoundFunctionCall(span, moduleId, classType->Destructor()));
                        destructorCall->AddArgument(std::unique_ptr<BoundExpression>(constructorCall->Arguments()[0]->Clone()));
                        boundFunction->AddTemporaryDestructorCall(std::move(destructorCall), boundFunction, containerScope, span, moduleId);
                    }
                }
                constructorCall->AddArgument(std::move(castArguments[0]));
                BoundConstructAndReturnTemporaryExpression* conversion = new BoundConstructAndReturnTemporaryExpression(std::unique_ptr<BoundExpression>(constructorCall),
                    std::unique_ptr<BoundExpression>(new BoundLocalVariable(span, moduleId, temporary)));
                castArguments[0].reset(conversion);
            }
            else
            {
                castArguments[0].reset(new BoundConversion(std::unique_ptr<BoundExpression>(castArguments[0].release()), conversionFun));
            }
        }
        if (argumentMatch.postReferenceConversionFlags != OperationFlags::none)
        {
            if (argumentMatch.postReferenceConversionFlags == OperationFlags::addr)
            {
                TypeSymbol* type = castArguments[0]->GetType()->AddLvalueReference(span, moduleId);
                BoundAddressOfExpression* addressOfExpression = new BoundAddressOfExpression(std::move(castArguments[0]), type);
                castArguments[0].reset(addressOfExpression);
            }
            else if (argumentMatch.postReferenceConversionFlags == OperationFlags::deref)
            {
                TypeSymbol* type = castArguments[0]->GetType()->RemoveReference(span, moduleId);
                BoundDereferenceExpression* dereferenceExpression = new BoundDereferenceExpression(std::move(castArguments[0]), type);
                castArguments[0].reset(dereferenceExpression);
            }
        }
        castFunctionCall->SetArguments(std::move(castArguments));
    }
    else
    {
        throw Exception("no explicit conversion from '" + ToUtf8(castArguments[0]->GetType()->FullName()) + "' to '" + ToUtf8(targetType->FullName()) + "' exists",
            castNode.GetSpan(), castNode.ModuleId(), boundFunction->GetFunctionSymbol()->GetSpan(), boundFunction->GetFunctionSymbol()->SourceModuleId());
    }
    CheckAccess(boundFunction->GetFunctionSymbol(), castFunctionCall->GetFunctionSymbol());
    expression.reset(castFunctionCall.release());
}

void ExpressionBinder::Visit(ConstructNode& constructNode) 
{
    TypeSymbol* resultType = nullptr;
    int n = constructNode.Arguments().Count();
    if (n == 0)
    {
        throw Exception("must supply at least one argument to construct expression", constructNode.GetSpan(), constructNode.ModuleId());
    }
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    for (int i = 0; i < n; ++i)
    {
        Node* argumentNode = constructNode.Arguments()[i];
        if (i == 0)
        {
            CloneContext cloneContext;
            CastNode castNode(constructNode.GetSpan(), constructNode.ModuleId(), new PointerNode(constructNode.GetSpan(), constructNode.ModuleId(), 
                constructNode.TypeExpr()->Clone(cloneContext)), argumentNode->Clone(cloneContext));
            castNode.Accept(*this);
            resultType = expression->GetType();
            if (!resultType->IsPointerType())
            {
                throw Exception("first argument of a construct expression must be of a pointer type", argumentNode->GetSpan(), argumentNode->ModuleId());
            }
            if (!resultType->RemovePointer(constructNode.GetSpan(), constructNode.ModuleId())->IsClassTypeSymbol())
            {
                expression->SetFlag(BoundExpressionFlags::deref);
            }
        }
        else
        {
            argumentNode->Accept(*this);
        }
        arguments.push_back(std::move(expression));
    }
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, 
        resultType->RemovePointer(constructNode.GetSpan(), constructNode.ModuleId())->ClassInterfaceEnumDelegateOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    expression = ResolveOverload(U"@constructor", containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, constructNode.GetSpan(), constructNode.ModuleId());
    expression.reset(new BoundConstructExpression(std::move(expression), resultType));
}

void ExpressionBinder::Visit(NewNode& newNode)
{
    CloneContext cloneContext;
    InvokeNode* invokeMemAlloc = nullptr;
    bool memDebug = boundCompileUnit.GetModule().IsSymbolDefined(U"MEM_DEBUG");
    if (GetBackEnd() == BackEnd::llvm || GetBackEnd() == BackEnd::cmcpp)
    {
        if (memDebug)
        {
            invokeMemAlloc = new InvokeNode(newNode.GetSpan(), newNode.ModuleId(), new IdentifierNode(newNode.GetSpan(), newNode.ModuleId(), U"RtMemAllocInfo"));
        }
        else
        {
            invokeMemAlloc = new InvokeNode(newNode.GetSpan(), newNode.ModuleId(), new IdentifierNode(newNode.GetSpan(), newNode.ModuleId(), U"RtMemAlloc"));
        }
    }
    else if (GetBackEnd() == BackEnd::cmsx)
    {
        invokeMemAlloc = new InvokeNode(newNode.GetSpan(), newNode.ModuleId(), new DotNode(newNode.GetSpan(), newNode.ModuleId(), 
            new IdentifierNode(newNode.GetSpan(), newNode.ModuleId(), U"System"), new IdentifierNode(newNode.GetSpan(), newNode.ModuleId(), U"MemAlloc")));
    }
    invokeMemAlloc->AddArgument(new SizeOfNode(newNode.GetSpan(), newNode.ModuleId(), newNode.TypeExpr()->Clone(cloneContext)));
    if (memDebug)
    {
        TypeNameNode* typeNameNode = new TypeNameNode(newNode.GetSpan(), newNode.ModuleId(), newNode.TypeExpr()->Clone(cloneContext));
        typeNameNode->SetStatic();
        invokeMemAlloc->AddArgument(typeNameNode);
    }
    CastNode castNode(newNode.GetSpan(), newNode.ModuleId(), new PointerNode(newNode.GetSpan(), newNode.ModuleId(), newNode.TypeExpr()->Clone(cloneContext)), invokeMemAlloc);
    castNode.Accept(*this);
    std::vector<std::unique_ptr<BoundExpression>> arguments;
    TypeSymbol* resultType = expression->GetType();
    if (!resultType->RemovePointer(newNode.GetSpan(), newNode.ModuleId())->IsClassTypeSymbol())
    {
        expression->SetFlag(BoundExpressionFlags::deref);
    }
    arguments.push_back(std::move(expression));
    int n = newNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        newNode.Arguments()[i]->Accept(*this);
        arguments.push_back(std::move(expression));
    }
    std::vector<FunctionScopeLookup> functionScopeLookups;
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, containerScope));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::this_and_base_and_parent, resultType->RemovePointer(newNode.GetSpan(), newNode.ModuleId())->ClassInterfaceEnumDelegateOrNsScope()));
    functionScopeLookups.push_back(FunctionScopeLookup(ScopeLookup::fileScopes, nullptr));
    expression = ResolveOverload(U"@constructor", containerScope, functionScopeLookups, arguments, boundCompileUnit, boundFunction, newNode.GetSpan(), newNode.ModuleId());
    expression.reset(new BoundConstructExpression(std::move(expression), resultType));
}

void ExpressionBinder::Visit(ThisNode& thisNode) 
{
    ParameterSymbol* thisParam = boundFunction->GetFunctionSymbol()->GetThisParam();
    if (thisParam)
    {
        expression.reset(new BoundParameter(thisNode.GetSpan(), thisNode.ModuleId(), thisParam));
        expression->SetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
    }
    else
    {
        throw Exception("'this' can only be used in member function context", thisNode.GetSpan(), thisNode.ModuleId());
    }
}

void ExpressionBinder::Visit(BaseNode& baseNode) 
{
    ParameterSymbol* thisParam = boundFunction->GetFunctionSymbol()->GetThisParam();
    if (thisParam)
    {
        TypeSymbol* thisType = thisParam->GetType()->BaseType();
        if (thisType->IsClassTypeSymbol())
        {
            ClassTypeSymbol* thisClassType = static_cast<ClassTypeSymbol*>(thisType);
            if (thisClassType->BaseClass())
            {
                TypeSymbol* basePointerType = thisClassType->BaseClass()->AddPointer(baseNode.GetSpan(), baseNode.ModuleId());
                if (thisParam->GetType()->IsConstType())
                {
                    basePointerType = basePointerType->AddConst(baseNode.GetSpan(), baseNode.ModuleId());
                }
                ArgumentMatch argumentMatch;
                FunctionSymbol* thisAsBaseConversionFunction = boundCompileUnit.GetConversion(thisParam->GetType(), basePointerType, containerScope, boundFunction, baseNode.GetSpan(), baseNode.ModuleId(), argumentMatch);
                if (thisAsBaseConversionFunction)
                {
                    expression.reset(new BoundConversion(std::unique_ptr<BoundExpression>(new BoundParameter(baseNode.GetSpan(), baseNode.ModuleId(), thisParam)), thisAsBaseConversionFunction));
                    expression->SetFlag(BoundExpressionFlags::argIsExplicitThisOrBasePtr);
                }
                else
                {
                    throw Exception("cannot convert from '" + ToUtf8(thisParam->GetType()->FullName()) + "' to '" + ToUtf8(basePointerType->FullName()) + "'", baseNode.GetSpan(), baseNode.ModuleId());
                }
            }
            else
            {
                throw Exception("class '" + ToUtf8(thisClassType->FullName()) + "' does not have a base class", baseNode.GetSpan(), baseNode.ModuleId());
            }
        }
        else
        {
            throw Exception("'base' can only be used in member function context", baseNode.GetSpan(), baseNode.ModuleId());
        }
    }
    else
    {
        throw Exception("'base' can only be used in member function context", baseNode.GetSpan(), baseNode.ModuleId());
    }
}

void ExpressionBinder::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    parenthesizedExpressionNode.Subject()->Accept(*this);
}

void ExpressionBinder::Visit(FunctionPtrNode& functionPtrNode)
{
    BoundExpression* boundExpression = static_cast<BoundFunctionPtr*>(functionPtrNode.GetBoundExpression());
    expression.reset(boundExpression->Clone());
}

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder)
{
    return BindExpression(node, boundCompileUnit, boundFunction, containerScope, statementBinder, false);
}

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, bool lvalue)
{
    return BindExpression(node, boundCompileUnit, boundFunction, containerScope, statementBinder, lvalue, false);
}

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, bool lvalue,
    bool acceptFunctionGroupOrMemberExpression)
{
    return BindExpression(node, boundCompileUnit, boundFunction, containerScope, statementBinder, lvalue, acceptFunctionGroupOrMemberExpression, false);
}

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, bool lvalue,
    bool acceptFunctionGroupOrMemberExpression, bool acceptIncomplete)
{
    return BindExpression(node, boundCompileUnit, boundFunction, containerScope, statementBinder, lvalue, acceptFunctionGroupOrMemberExpression, acceptIncomplete, true);
}

std::unique_ptr<BoundExpression> BindExpression(Node* node, BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder, bool lvalue,
    bool acceptFunctionGroupOrMemberExpression, bool acceptIncomplete, bool moveTemporaryDestructorCalls)
{
    ExpressionBinder expressionBinder(node->GetSpan(), node->ModuleId(), boundCompileUnit, boundFunction, containerScope, statementBinder, lvalue);
    node->Accept(expressionBinder);
    std::unique_ptr<BoundExpression> expression = expressionBinder.GetExpression();
    if (!expression)
    {
        throw Exception("could not bind expression", node->GetSpan(), node->ModuleId());
    }
    if (moveTemporaryDestructorCalls)
    {
        boundFunction->MoveTemporaryDestructorCallsTo(*expression);
    }
    if (acceptFunctionGroupOrMemberExpression && (expression->GetBoundNodeType() == BoundNodeType::boundFunctionGroupExpression || expression->GetBoundNodeType() == BoundNodeType::boundMemberExpression))
    {
        return expression;
    }
    if (!acceptIncomplete)
    {
        if (!expression->IsComplete())
        {
            throw Exception("incomplete expression", node->GetSpan(), node->ModuleId());
        }
    }
    if (lvalue && !expression->IsLvalueExpression())
    {
        throw Exception("not an lvalue expression", node->GetSpan(), node->ModuleId());
    }
    return expression;
}

std::unique_ptr<BoundExpression> BindUnaryOp(BoundExpression* operand, Node& node, const std::u32string& groupName,
    BoundCompileUnit& boundCompileUnit, BoundFunction* boundFunction, ContainerScope* containerScope, StatementBinder* statementBinder)
{
    ExpressionBinder expressionBinder(node.GetSpan(), node.ModuleId(), boundCompileUnit, boundFunction, containerScope, statementBinder, false);
    expressionBinder.BindUnaryOp(operand, node, groupName);
    std::unique_ptr<BoundExpression> expression = expressionBinder.GetExpression();
    if (!expression)
    {
        throw Exception("cound not bind expression", node.GetSpan(), node.ModuleId());
    }
    return expression;
}

} } // namespace cmajor::binder
