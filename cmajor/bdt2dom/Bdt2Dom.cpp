// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/bdt2dom/Bdt2Dom.hpp>
#include <cmajor/binder/BoundNodeVisitor.hpp>
#include <cmajor/binder/BoundCompileUnit.hpp>
#include <cmajor/binder/BoundNamespace.hpp>
#include <cmajor/binder/BoundClass.hpp>
#include <cmajor/binder/BoundEnum.hpp>
#include <cmajor/binder/BoundFunction.hpp>
#include <cmajor/binder/BoundStatement.hpp>
#include <cmajor/symbols/Symbol.hpp>
#include <cmajor/symbols/TypeMap.hpp>
#include <cmajor/util/Path.hpp>
#include <cmajor/util/Unicode.hpp>
#include <stdexcept>

namespace cmajor { namespace bdt2dom {

using namespace cmajor::binder;
using namespace cmajor::unicode;
using namespace cmajor::util;

class Bdt2DomVisitor : public BoundNodeVisitor
{
public:
    Bdt2DomVisitor();
    std::unique_ptr<dom::Element> GetBoundCompileUnitElement() { return std::move(boundCompileUnitElement); }
    TypeMap& GetTypeMap() { return typeMap; }
    void CheckCurrentElement();
    void Visit(BoundCompileUnit& boundCompileUnit) override;
    void Visit(BoundNamespace& boundNamespace) override;
    void Visit(BoundClass& boundClass) override;
    void Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition) override;
    void Visit(BoundFunction& boundFunction) override;
    void Visit(BoundSequenceStatement& boundSequenceStatement) override;
    void Visit(BoundCompoundStatement& boundCompoundStatement) override;
    void Visit(BoundReturnStatement& boundReturnStatement) override;
    void Visit(BoundIfStatement& boundIfStatement) override;
    void Visit(BoundWhileStatement& boundWhileStatement) override;
    void Visit(BoundDoStatement& boundDoStatement) override;
    void Visit(BoundForStatement& boundForStatement) override;
    void Visit(BoundSwitchStatement& boundSwitchStatement) override;
    void Visit(BoundCaseStatement& boundCaseStatement) override;
    void Visit(BoundDefaultStatement& boundDefaultStatement) override;
    void Visit(BoundGotoCaseStatement& boundGotoCaseStatement) override;
    void Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement) override;
    void Visit(BoundBreakStatement& boundBreakStatement) override;
    void Visit(BoundContinueStatement& boundContinueStatement) override;
    void Visit(BoundGotoStatement& boundGotoStatement) override;
    void Visit(BoundConstructionStatement& boundConstructionStatement) override;
    void Visit(BoundAssignmentStatement& boundAssignmentStatement) override;
    void Visit(BoundExpressionStatement& boundExpressionStatement) override;
    void Visit(BoundEmptyStatement& boundEmptyStatement) override;
    void Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement) override;
    void Visit(BoundThrowStatement& boundThrowStatement) override;
    void Visit(BoundRethrowStatement& boundRethrowStatement) override;
    void Visit(BoundTryStatement& boundTryStatement) override;
    void Visit(BoundCatchStatement& boundCatchStatement) override;
    void Visit(BoundParameter& boundParameter) override;
    void Visit(BoundLocalVariable& boundLocalVariable) override;
    void Visit(BoundMemberVariable& boundMemberVariable) override;
    void Visit(BoundConstant& boundConstant) override;
    void Visit(BoundEnumConstant& boundEnumConstant) override;
    void Visit(BoundLiteral& boundLiteral) override;
    void Visit(BoundTemporary& boundTemporary) override;
    void Visit(BoundSizeOfExpression& boundSizeOfExpression) override;
    void Visit(BoundAddressOfExpression& boundAddressOfExpression) override;
    void Visit(BoundDereferenceExpression& boundDereferenceExpression) override;
    void Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression) override;
    void Visit(BoundFunctionCall& boundFunctionCall) override;
    void Visit(BoundDelegateCall& boundDelegateCall) override;
    void Visit(BoundClassDelegateCall& boundClassDelegateCall) override;
    void Visit(BoundConversion& boundConversion) override;
    void Visit(BoundConstructExpression& boundConstructExpression) override;
    void Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression) override;
    void Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult) override;
    void Visit(BoundIsExpression& boundIsExpression) override;
    void Visit(BoundAsExpression& boundAsExpression) override;
    void Visit(BoundTypeNameExpression& boundTypeNameExpression) override;
    void Visit(BoundTypeIdExpression& boundTypeIdExpression) override;
    void Visit(BoundBitCast& boundBitCast) override;
    void Visit(BoundFunctionPtr& boundFunctionPtr) override;
    void Visit(BoundDisjunction& boundDisjunction) override;
    void Visit(BoundConjunction& boundConjunction) override;
private:
    dom::Element* currentElement;
    std::unique_ptr<dom::Element> boundCompileUnitElement;
    TypeMap typeMap;
};

Bdt2DomVisitor::Bdt2DomVisitor() : currentElement(nullptr)
{
}

void Bdt2DomVisitor::CheckCurrentElement()
{
    if (!currentElement)
    {
        throw std::runtime_error("current element not set");
    }
}

void Bdt2DomVisitor::Visit(BoundCompileUnit& boundCompileUnit)
{
    boundCompileUnitElement.reset(new dom::Element(U"BoundCompileUnit"));
    std::u32string value = ToUtf32(Path::GetFileName(boundCompileUnit.GetCompileUnitNode()->FilePath()));
    dom::Element* prevElement = currentElement;
    currentElement = boundCompileUnitElement.get();
    currentElement->SetAttribute(U"info", value);
    const std::vector<std::unique_ptr<BoundNode>>& boundNodes = boundCompileUnit.BoundNodes();
    for (const std::unique_ptr<BoundNode>& boundNode : boundNodes)
    {
        boundNode->Accept(*this);
    }
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundNamespace& boundNamespace)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> namespaceElement(new dom::Element(U"BoundNamespace"));
    dom::Element* prevElement = currentElement;
    currentElement = namespaceElement.get();
    std::u32string value = boundNamespace.GetNamespaceNode().Id()->Str();
    currentElement->SetAttribute(U"info", value);
    const std::vector<std::unique_ptr<BoundNode>>& members = boundNamespace.Members();
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(namespaceElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClass& boundClass)
{
    return;
    CheckCurrentElement();
    std::unique_ptr<dom::Element> classElement(new dom::Element(U"BoundClass"));
    dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    std::u32string value = boundClass.GetClassTypeSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    const std::vector<std::unique_ptr<BoundNode>>& members = boundClass.Members();
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> enumElement(new dom::Element(U"BoundEnumTypeDefinition"));
    dom::Element* prevElement = currentElement;
    currentElement = enumElement.get();
    std::u32string value = boundEnumTypeDefinition.GetEnumTypeSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsTemplateSpecialization()) return;
    if (boundFunction.GetFunctionSymbol()->GroupName() == U"@constructor") return;
    CheckCurrentElement();
    std::unique_ptr<dom::Element> functionElement(new dom::Element(U"BoundFunction"));
    dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    std::u32string value = boundFunction.GetFunctionSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    boundFunction.Body()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSequenceStatement& boundSequenceStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> sequenceElement(new dom::Element(U"BoundSequenceStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = sequenceElement.get();
    boundSequenceStatement.First()->Accept(*this);
    boundSequenceStatement.Second()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(sequenceElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> compoundElement(new dom::Element(U"BoundCompoundStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = compoundElement.get();
    for (const std::unique_ptr<BoundStatement>& statement : boundCompoundStatement.Statements())
    {
        statement->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(compoundElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundReturnStatement& boundReturnStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> returnElement(new dom::Element(U"BoundReturnStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = returnElement.get();
    if (boundReturnStatement.ReturnFunctionCall())
    {
        boundReturnStatement.ReturnFunctionCall()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(returnElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundIfStatement& boundIfStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> ifElement(new dom::Element(U"BoundIfStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = ifElement.get();
    boundIfStatement.Condition()->Accept(*this);
    boundIfStatement.ThenS()->Accept(*this);
    if (boundIfStatement.ElseS())
    {
        boundIfStatement.ElseS()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ifElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundWhileStatement& boundWhileStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> whileElement(new dom::Element(U"BoundWhileStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = whileElement.get();
    boundWhileStatement.Condition()->Accept(*this);
    boundWhileStatement.Statement()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(whileElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDoStatement& boundDoStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> doElement(new dom::Element(U"BoundDoStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = doElement.get();
    boundDoStatement.Statement()->Accept(*this);
    boundDoStatement.Condition()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(doElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundForStatement& boundForStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> forElement(new dom::Element(U"BoundForStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = forElement.get();
    boundForStatement.InitS()->Accept(*this);
    boundForStatement.Condition()->Accept(*this);
    boundForStatement.LoopS()->Accept(*this);
    boundForStatement.ActionS()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(forElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSwitchStatement& boundSwitchStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> switchElement(new dom::Element(U"BoundSwitchStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = switchElement.get();
    boundSwitchStatement.Condition()->Accept(*this);
    for (const std::unique_ptr<BoundCaseStatement>& caseStatement : boundSwitchStatement.CaseStatements())
    {
        caseStatement->Accept(*this);
    }
    if (boundSwitchStatement.DefaultStatement())
    {
        boundSwitchStatement.DefaultStatement()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(switchElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCaseStatement& boundCaseStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> caseElement(new dom::Element(U"BoundCaseStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = caseElement.get();
    if (boundCaseStatement.CompoundStatement())
    {
        boundCaseStatement.CompoundStatement()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(caseElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDefaultStatement& boundDefaultStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> defaultElement(new dom::Element(U"BoundDefaultStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = defaultElement.get();
    if (boundDefaultStatement.CompoundStatement())
    {
        boundDefaultStatement.CompoundStatement()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(defaultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> gotoCaseElement(new dom::Element(U"BoundGotoCaseStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = gotoCaseElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(gotoCaseElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> gotoDefaultElement(new dom::Element(U"BoundGotoDefaultStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = gotoDefaultElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(gotoDefaultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundBreakStatement& boundBreakStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> breakElement(new dom::Element(U"BoundBreakStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = breakElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(breakElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundContinueStatement& boundContinueStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> continueElement(new dom::Element(U"BoundContinueStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = continueElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(continueElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoStatement& boundGotoStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> gotoElement(new dom::Element(U"BoundGotoStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = gotoElement.get();
    std::u32string value = boundGotoStatement.Target();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(gotoElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructionStatement& boundConstructionStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructionElement(new dom::Element(U"BoundConstructionStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = constructionElement.get();
    boundConstructionStatement.ConstructorCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> assignmentElement(new dom::Element(U"BoundAssignmentStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = assignmentElement.get();
    boundAssignmentStatement.AssignmentCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(assignmentElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundExpressionStatement& boundExpressionStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> expressionElement(new dom::Element(U"BoundExpressionStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = expressionElement.get();
    boundExpressionStatement.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(expressionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> emptyElement(new dom::Element(U"BoundEmptyStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = emptyElement.get();
    prevElement->AppendChild(std::unique_ptr<dom::Node>(emptyElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> setVmtPtrElement(new dom::Element(U"BoundSetVmtPtrStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = setVmtPtrElement.get();
    boundSetVmtPtrStatement.ClassPtr()->Accept(*this);
    std::u32string value = boundSetVmtPtrStatement.ClassType()->Name();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(setVmtPtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundThrowStatement& boundThrowStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> throwElement(new dom::Element(U"BoundThrowStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = throwElement.get();
    boundThrowStatement.ThrowCallExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(throwElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> rethrowElement(new dom::Element(U"BoundRethrowStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = rethrowElement.get();
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(rethrowElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTryStatement& boundTryStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> tryElement(new dom::Element(U"BoundTryStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = tryElement.get();
    boundTryStatement.TryBlock()->Accept(*this);
    for (const std::unique_ptr<BoundCatchStatement>& catchStatement : boundTryStatement.Catches())
    {
        catchStatement->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(tryElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCatchStatement& boundCatchStatement)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> catchElement(new dom::Element(U"BoundCatchStatement"));
    dom::Element* prevElement = currentElement;
    currentElement = catchElement.get();
    boundCatchStatement.CatchBlock()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(catchElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundParameter& boundParameter)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> parameterElement(new dom::Element(U"BoundParameter"));
    dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    std::unique_ptr<dom::Element> parameterSymbolElement = boundParameter.GetParameterSymbol()->ToDomElement(typeMap); 
    currentElement->AppendChild(std::unique_ptr<dom::Node>(parameterSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundLocalVariable& boundLocalVariable)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> localVariableElement(new dom::Element(U"BoundLocalVariable"));
    dom::Element* prevElement = currentElement;
    currentElement = localVariableElement.get();
    std::unique_ptr<dom::Element> localVariableSymbolElement = boundLocalVariable.GetLocalVariableSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(localVariableSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(localVariableElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundMemberVariable& boundMemberVariable)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> memberVariableElement(new dom::Element(U"BoundMemberVariable"));
    dom::Element* prevElement = currentElement;
    currentElement = memberVariableElement.get();
    std::unique_ptr<dom::Element> memberVariableSymbolElement = boundMemberVariable.GetMemberVariableSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(memberVariableSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(memberVariableElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstant& boundConstant)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constantElement(new dom::Element(U"BoundConstant"));
    dom::Element* prevElement = currentElement;
    currentElement = constantElement.get();
    std::unique_ptr<dom::Element> constantSymbolElement = boundConstant.GetConstantSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(constantSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constantElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEnumConstant& boundEnumConstant)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> enumConstantElement(new dom::Element(U"BoundEnumConstant"));
    dom::Element* prevElement = currentElement;
    currentElement = enumConstantElement.get();
    std::u32string value = boundEnumConstant.GetEnumConstantSymbol()->Name();
    std::unique_ptr<dom::Element> enumConstantSymbolElement = boundEnumConstant.GetEnumConstantSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundLiteral& boundLiteral)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> literalElement(new dom::Element(U"BoundLiteral"));
    dom::Element* prevElement = currentElement;
    currentElement = literalElement.get();
    if (boundLiteral.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundLiteral.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    std::unique_ptr<dom::Element> valueElement(boundLiteral.GetValue()->ToDomElement());
    currentElement->AppendChild(std::unique_ptr<dom::Node>(valueElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(literalElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTemporary& boundTemporary)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> temporaryElement(new dom::Element(U"BoundTemporary"));
    dom::Element* prevElement = currentElement;
    currentElement = temporaryElement.get();
    if (boundTemporary.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTemporary.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundTemporary.RvalueExpr()->Accept(*this);
    boundTemporary.BackingStore()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(temporaryElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSizeOfExpression& boundSizeOfExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> sizeOfElement(new dom::Element(U"BoundSizeOfExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = sizeOfElement.get();
    if (boundSizeOfExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundSizeOfExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(sizeOfElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAddressOfExpression& boundAddressOfExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> addrOfElement(new dom::Element(U"BoundAddressOfExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = addrOfElement.get();
    if (boundAddressOfExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundAddressOfExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundAddressOfExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(addrOfElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDereferenceExpression& boundDereferenceExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> derefElement(new dom::Element(U"BoundDereferenceExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = derefElement.get();
    if (boundDereferenceExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDereferenceExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundDereferenceExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(derefElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> ref2PtrElement(new dom::Element(U"BoundReferenceToPointerExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = ref2PtrElement.get();
    if (boundReferenceToPointerExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundReferenceToPointerExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundReferenceToPointerExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(ref2PtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunctionCall& boundFunctionCall)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> callElement(new dom::Element(U"BoundFunctionCall"));
    dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    FunctionSymbol* functionSymbol = boundFunctionCall.GetFunctionSymbol();
    if (functionSymbol)
    {
        const char* className = functionSymbol->ClassName();
        std::unique_ptr<dom::Element> functionSymbolElement(new dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", functionSymbol->Info());
        if (functionSymbol->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(functionSymbol->ClassArity())));
        }
        if (functionSymbol->ReturnType())
        {
            std::unique_ptr<dom::Element> typeElement(new dom::Element(U"returnType"));
            int typeId = typeMap.GetOrInsertType(boundFunctionCall.GetType());
            typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
            functionSymbolElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
        }
        currentElement->AppendChild(std::unique_ptr<dom::Node>(functionSymbolElement.release()));
    }
    for (const std::unique_ptr<BoundExpression>& argument : boundFunctionCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDelegateCall& boundDelegateCall)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> callElement(new dom::Element(U"BoundDelegateCall"));
    dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    if (boundDelegateCall.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDelegateCall.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    std::unique_ptr<dom::Element> delegateSymbolElement = boundDelegateCall.GetDelegateSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(delegateSymbolElement.release()));
    for (const std::unique_ptr<BoundExpression>& argument : boundDelegateCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClassDelegateCall& boundClassDelegateCall)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> callElement(new dom::Element(U"BoundClassDelegateCall"));
    dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    if (boundClassDelegateCall.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundClassDelegateCall.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    std::unique_ptr<dom::Element> classDelegateSymbolElement = boundClassDelegateCall.GetClassDelegateSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<dom::Node>(classDelegateSymbolElement.release()));
    for (const std::unique_ptr<BoundExpression>& argument : boundClassDelegateCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConversion& boundConversion)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conversionElement(new dom::Element(U"BoundConversion"));
    dom::Element* prevElement = currentElement;
    currentElement = conversionElement.get();
    if (boundConversion.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConversion.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundConversion.SourceExpr()->Accept(*this);
    FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    if (conversionFun)
    {
        const char* className = conversionFun->ClassName();
        std::unique_ptr<dom::Element> functionSymbolElement(new dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", conversionFun->Info());
        if (conversionFun->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(conversionFun->ClassArity())));
        }
        currentElement->AppendChild(std::unique_ptr<dom::Node>(functionSymbolElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conversionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructExpression& boundConstructExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructElement(new dom::Element(U"BoundConstructExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    if (boundConstructExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConstructExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundConstructExpression.ConstructorCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> constructElement(new dom::Element(U"BoundConstructAndReturnTemporaryExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    if (boundConstructAndReturnTemporaryExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConstructAndReturnTemporaryExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundConstructAndReturnTemporaryExpression.ConstructorCall()->Accept(*this);
    boundConstructAndReturnTemporaryExpression.BoundTemporary()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conversionResultElement(new dom::Element(U"BoundClassOrClassDelegateConversionResult"));
    dom::Element* prevElement = currentElement;
    currentElement = conversionResultElement.get();
    if (boundClassOrClassDelegateConversionResult.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundClassOrClassDelegateConversionResult.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundClassOrClassDelegateConversionResult.ConversionResult()->Accept(*this);
    boundClassOrClassDelegateConversionResult.ConversionFunctionCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conversionResultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundIsExpression& boundIsExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> isElement(new dom::Element(U"BoundIsExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    boundIsExpression.Expr()->Accept(*this);
    if (boundIsExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundIsExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    std::unique_ptr<dom::Element> rightTypeElement(new dom::Element(U"type"));
    int rightTypeId = typeMap.GetOrInsertType(boundIsExpression.RightClassType());
    rightTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(rightTypeId)));
    currentElement->AppendChild(std::unique_ptr<dom::Node>(rightTypeElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAsExpression& boundAsExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> asElement(new dom::Element(U"BoundAsExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = asElement.get();
    boundAsExpression.Expr()->Accept(*this);
    boundAsExpression.Variable()->Accept(*this);
    if (boundAsExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundAsExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    std::unique_ptr<dom::Element> rightTypeElement(new dom::Element(U"type"));
    int rightTypeId = typeMap.GetOrInsertType(boundAsExpression.RightClassType());
    rightTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(rightTypeId)));
    currentElement->AppendChild(std::unique_ptr<dom::Node>(rightTypeElement.release()));
    prevElement->AppendChild(std::unique_ptr<dom::Node>(asElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTypeNameExpression& boundTypeNameExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> typeNameElement(new dom::Element(U"BoundTypeNameExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    if (boundTypeNameExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTypeNameExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundTypeNameExpression.ClassPtr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTypeIdExpression& boundTypeIdExpression)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> typeNameElement(new dom::Element(U"BoundTypeIdExpression"));
    dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    if (boundTypeIdExpression.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTypeIdExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundTypeIdExpression.ClassPtr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundBitCast& boundBitCast)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> bitCastElement(new dom::Element(U"BoundBitCast"));
    dom::Element* prevElement = currentElement;
    currentElement = bitCastElement.get();
    if (boundBitCast.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundBitCast.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundBitCast.Expr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(bitCastElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunctionPtr& boundFunctionPtr)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> functionPtrElement(new dom::Element(U"BoundFunctionPtr"));
    dom::Element* prevElement = currentElement;
    currentElement = functionPtrElement.get();
    if (boundFunctionPtr.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundFunctionPtr.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    FunctionSymbol* functionSymbol = boundFunctionPtr.Function();
    if (functionSymbol)
    {
        const char* className = functionSymbol->ClassName();
        std::unique_ptr<dom::Element> functionSymbolElement(new dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", functionSymbol->Info());
        if (functionSymbol->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(functionSymbol->ClassArity())));
        }
        currentElement->AppendChild(std::unique_ptr<dom::Node>(functionSymbolElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<dom::Node>(functionPtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDisjunction& boundDisjunction)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> disjunctionElement(new dom::Element(U"BoundDisjunction"));
    dom::Element* prevElement = currentElement;
    currentElement = disjunctionElement.get();
    if (boundDisjunction.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDisjunction.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundDisjunction.Left()->Accept(*this);
    boundDisjunction.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(disjunctionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConjunction& boundConjunction)
{
    CheckCurrentElement();
    std::unique_ptr<dom::Element> conjunctionElement(new dom::Element(U"BoundConjunction"));
    dom::Element* prevElement = currentElement;
    currentElement = conjunctionElement.get();
    if (boundConjunction.GetType())
    {
        std::unique_ptr<dom::Element> typeElement(new dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConjunction.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundConjunction.Left()->Accept(*this);
    boundConjunction.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<dom::Node>(conjunctionElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<dom::Document> GenerateBdtDocument(cmajor::binder::BoundNode* node) 
{
    std::unique_ptr<dom::Document> doc(new dom::Document());
    Bdt2DomVisitor visitor;
    node->Accept(visitor);
    std::unique_ptr<dom::Element> boundTreeElement(new dom::Element(U"boundTree"));
    std::unique_ptr<dom::Element> boundCompileUnitElement = visitor.GetBoundCompileUnitElement();
    boundTreeElement->AppendChild(std::unique_ptr<dom::Node>(boundCompileUnitElement.release()));
    std::unique_ptr<dom::Element> typesElement(new dom::Element(U"types"));
    std::vector<std::unique_ptr<dom::Element>> typeElements = visitor.GetTypeMap().TypeElements();
    int n = typeElements.size();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<dom::Element> typeElement(std::move(typeElements[i]));
        typesElement->AppendChild(std::unique_ptr<dom::Node>(typeElement.release()));
    }
    boundTreeElement->AppendChild(std::unique_ptr<dom::Node>(typesElement.release()));
    doc->AppendChild(std::unique_ptr<dom::Node>(boundTreeElement.release()));
    return doc;
}

} } // namespace cmajor::bdt2dom
