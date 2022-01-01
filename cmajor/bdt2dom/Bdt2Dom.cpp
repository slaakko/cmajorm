// =================================
// Copyright (c) 2022 Seppo Laakko
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
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <stdexcept>

namespace cmajor { namespace bdt2dom {

using namespace cmajor::binder;
using namespace soulng::unicode;
using namespace soulng::util;

class Bdt2DomVisitor : public BoundNodeVisitor
{
public:
    Bdt2DomVisitor();
    std::unique_ptr<sngxml::dom::Element> GetBoundCompileUnitElement() { return std::move(boundCompileUnitElement); }
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
    void Visit(BoundInitializationStatement& boundInitializationStatement) override;
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
    sngxml::dom::Element* currentElement;
    std::unique_ptr<sngxml::dom::Element> boundCompileUnitElement;
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
    boundCompileUnitElement.reset(new sngxml::dom::Element(U"BoundCompileUnit"));
    std::u32string value = ToUtf32(Path::GetFileName(boundCompileUnit.GetCompileUnitNode()->FilePath()));
    sngxml::dom::Element* prevElement = currentElement;
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
    std::unique_ptr<sngxml::dom::Element> namespaceElement(new sngxml::dom::Element(U"BoundNamespace"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = namespaceElement.get();
    std::u32string value = boundNamespace.GetNamespaceNode().Id()->Str();
    currentElement->SetAttribute(U"info", value);
    const std::vector<std::unique_ptr<BoundNode>>& members = boundNamespace.Members();
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(namespaceElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClass& boundClass)
{
    return;
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> classElement(new sngxml::dom::Element(U"BoundClass"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = classElement.get();
    std::u32string value = boundClass.GetClassTypeSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    const std::vector<std::unique_ptr<BoundNode>>& members = boundClass.Members();
    for (const std::unique_ptr<BoundNode>& member : members)
    {
        member->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEnumTypeDefinition& boundEnumTypeDefinition)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> enumElement(new sngxml::dom::Element(U"BoundEnumTypeDefinition"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = enumElement.get();
    std::u32string value = boundEnumTypeDefinition.GetEnumTypeSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunction& boundFunction)
{
    if (boundFunction.GetFunctionSymbol()->IsTemplateSpecialization()) return;
    if (boundFunction.GetFunctionSymbol()->GroupName() == U"@constructor") return;
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> functionElement(new sngxml::dom::Element(U"BoundFunction"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = functionElement.get();
    std::u32string value = boundFunction.GetFunctionSymbol()->Name();
    currentElement->SetAttribute(U"info", value);
    boundFunction.Body()->Accept(*this); 
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSequenceStatement& boundSequenceStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> sequenceElement(new sngxml::dom::Element(U"BoundSequenceStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = sequenceElement.get();
    boundSequenceStatement.First()->Accept(*this);
    boundSequenceStatement.Second()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(sequenceElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCompoundStatement& boundCompoundStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> compoundElement(new sngxml::dom::Element(U"BoundCompoundStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = compoundElement.get();
    for (const std::unique_ptr<BoundStatement>& statement : boundCompoundStatement.Statements())
    {
        statement->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(compoundElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundReturnStatement& boundReturnStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> returnElement(new sngxml::dom::Element(U"BoundReturnStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = returnElement.get();
    if (boundReturnStatement.ReturnFunctionCall())
    {
        boundReturnStatement.ReturnFunctionCall()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(returnElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundIfStatement& boundIfStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> ifElement(new sngxml::dom::Element(U"BoundIfStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ifElement.get();
    boundIfStatement.Condition()->Accept(*this);
    boundIfStatement.ThenS()->Accept(*this);
    if (boundIfStatement.ElseS())
    {
        boundIfStatement.ElseS()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ifElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundWhileStatement& boundWhileStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> whileElement(new sngxml::dom::Element(U"BoundWhileStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = whileElement.get();
    boundWhileStatement.Condition()->Accept(*this);
    boundWhileStatement.Statement()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(whileElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDoStatement& boundDoStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> doElement(new sngxml::dom::Element(U"BoundDoStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = doElement.get();
    boundDoStatement.Statement()->Accept(*this);
    boundDoStatement.Condition()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(doElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundForStatement& boundForStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> forElement(new sngxml::dom::Element(U"BoundForStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = forElement.get();
    boundForStatement.InitS()->Accept(*this);
    boundForStatement.Condition()->Accept(*this);
    boundForStatement.LoopS()->Accept(*this);
    boundForStatement.ActionS()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(forElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSwitchStatement& boundSwitchStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> switchElement(new sngxml::dom::Element(U"BoundSwitchStatement"));
    sngxml::dom::Element* prevElement = currentElement;
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
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(switchElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCaseStatement& boundCaseStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> caseElement(new sngxml::dom::Element(U"BoundCaseStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = caseElement.get();
    if (boundCaseStatement.CompoundStatement())
    {
        boundCaseStatement.CompoundStatement()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(caseElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDefaultStatement& boundDefaultStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> defaultElement(new sngxml::dom::Element(U"BoundDefaultStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = defaultElement.get();
    if (boundDefaultStatement.CompoundStatement())
    {
        boundDefaultStatement.CompoundStatement()->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(defaultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoCaseStatement& boundGotoCaseStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> gotoCaseElement(new sngxml::dom::Element(U"BoundGotoCaseStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = gotoCaseElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(gotoCaseElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoDefaultStatement& boundGotoDefaultStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> gotoDefaultElement(new sngxml::dom::Element(U"BoundGotoDefaultStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = gotoDefaultElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(gotoDefaultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundBreakStatement& boundBreakStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> breakElement(new sngxml::dom::Element(U"BoundBreakStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = breakElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(breakElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundContinueStatement& boundContinueStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> continueElement(new sngxml::dom::Element(U"BoundContinueStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = continueElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(continueElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundGotoStatement& boundGotoStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> gotoElement(new sngxml::dom::Element(U"BoundGotoStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = gotoElement.get();
    std::u32string value = boundGotoStatement.Target();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(gotoElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructionStatement& boundConstructionStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructionElement(new sngxml::dom::Element(U"BoundConstructionStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructionElement.get();
    boundConstructionStatement.ConstructorCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAssignmentStatement& boundAssignmentStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> assignmentElement(new sngxml::dom::Element(U"BoundAssignmentStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = assignmentElement.get();
    boundAssignmentStatement.AssignmentCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(assignmentElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundExpressionStatement& boundExpressionStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> expressionElement(new sngxml::dom::Element(U"BoundExpressionStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = expressionElement.get();
    boundExpressionStatement.Expression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(expressionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundInitializationStatement& boundInitializationStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> expressionElement(new sngxml::dom::Element(U"BoundInitializationStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = expressionElement.get();
    boundInitializationStatement.InitializationExpression()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(expressionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEmptyStatement& boundEmptyStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> emptyElement(new sngxml::dom::Element(U"BoundEmptyStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = emptyElement.get();
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(emptyElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSetVmtPtrStatement& boundSetVmtPtrStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> setVmtPtrElement(new sngxml::dom::Element(U"BoundSetVmtPtrStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = setVmtPtrElement.get();
    boundSetVmtPtrStatement.ClassPtr()->Accept(*this);
    std::u32string value = boundSetVmtPtrStatement.ClassType()->Name();
    currentElement->SetAttribute(U"info", value);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(setVmtPtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundThrowStatement& boundThrowStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> throwElement(new sngxml::dom::Element(U"BoundThrowStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = throwElement.get();
    boundThrowStatement.ThrowCallExpr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(throwElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundRethrowStatement& boundRethrowStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> rethrowElement(new sngxml::dom::Element(U"BoundRethrowStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = rethrowElement.get();
    boundRethrowStatement.ReleaseCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rethrowElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTryStatement& boundTryStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> tryElement(new sngxml::dom::Element(U"BoundTryStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = tryElement.get();
    boundTryStatement.TryBlock()->Accept(*this);
    for (const std::unique_ptr<BoundCatchStatement>& catchStatement : boundTryStatement.Catches())
    {
        catchStatement->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(tryElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundCatchStatement& boundCatchStatement)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> catchElement(new sngxml::dom::Element(U"BoundCatchStatement"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = catchElement.get();
    boundCatchStatement.CatchBlock()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(catchElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundParameter& boundParameter)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> parameterElement(new sngxml::dom::Element(U"BoundParameter"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = parameterElement.get();
    std::unique_ptr<sngxml::dom::Element> parameterSymbolElement = boundParameter.GetParameterSymbol()->ToDomElement(typeMap); 
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parameterSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(parameterElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundLocalVariable& boundLocalVariable)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> localVariableElement(new sngxml::dom::Element(U"BoundLocalVariable"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = localVariableElement.get();
    std::unique_ptr<sngxml::dom::Element> localVariableSymbolElement = boundLocalVariable.GetLocalVariableSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(localVariableSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(localVariableElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundMemberVariable& boundMemberVariable)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> memberVariableElement(new sngxml::dom::Element(U"BoundMemberVariable"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = memberVariableElement.get();
    std::unique_ptr<sngxml::dom::Element> memberVariableSymbolElement = boundMemberVariable.GetMemberVariableSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariableSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(memberVariableElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstant& boundConstant)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constantElement(new sngxml::dom::Element(U"BoundConstant"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constantElement.get();
    std::unique_ptr<sngxml::dom::Element> constantSymbolElement = boundConstant.GetConstantSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constantSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constantElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundEnumConstant& boundEnumConstant)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> enumConstantElement(new sngxml::dom::Element(U"BoundEnumConstant"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = enumConstantElement.get();
    std::u32string value = boundEnumConstant.GetEnumConstantSymbol()->Name();
    std::unique_ptr<sngxml::dom::Element> enumConstantSymbolElement = boundEnumConstant.GetEnumConstantSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantSymbolElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(enumConstantElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundLiteral& boundLiteral)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> literalElement(new sngxml::dom::Element(U"BoundLiteral"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = literalElement.get();
    if (boundLiteral.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundLiteral.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    std::unique_ptr<sngxml::dom::Element> valueElement(boundLiteral.GetValue()->ToDomElement());
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(valueElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(literalElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTemporary& boundTemporary)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> temporaryElement(new sngxml::dom::Element(U"BoundTemporary"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = temporaryElement.get();
    if (boundTemporary.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTemporary.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundTemporary.RvalueExpr()->Accept(*this);
    boundTemporary.BackingStore()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(temporaryElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundSizeOfExpression& boundSizeOfExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> sizeOfElement(new sngxml::dom::Element(U"BoundSizeOfExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = sizeOfElement.get();
    if (boundSizeOfExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundSizeOfExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(sizeOfElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAddressOfExpression& boundAddressOfExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> addrOfElement(new sngxml::dom::Element(U"BoundAddressOfExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = addrOfElement.get();
    if (boundAddressOfExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundAddressOfExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundAddressOfExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(addrOfElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDereferenceExpression& boundDereferenceExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> derefElement(new sngxml::dom::Element(U"BoundDereferenceExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = derefElement.get();
    if (boundDereferenceExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDereferenceExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundDereferenceExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(derefElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundReferenceToPointerExpression& boundReferenceToPointerExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> ref2PtrElement(new sngxml::dom::Element(U"BoundReferenceToPointerExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = ref2PtrElement.get();
    if (boundReferenceToPointerExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundReferenceToPointerExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundReferenceToPointerExpression.Subject()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(ref2PtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunctionCall& boundFunctionCall)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> callElement(new sngxml::dom::Element(U"BoundFunctionCall"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    FunctionSymbol* functionSymbol = boundFunctionCall.GetFunctionSymbol();
    if (functionSymbol)
    {
        const char* className = functionSymbol->ClassName();
        std::unique_ptr<sngxml::dom::Element> functionSymbolElement(new sngxml::dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", functionSymbol->Info());
        if (functionSymbol->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(functionSymbol->ClassArity())));
        }
        if (functionSymbol->ReturnType())
        {
            std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"returnType"));
            int typeId = typeMap.GetOrInsertType(boundFunctionCall.GetType());
            typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
            functionSymbolElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
        }
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionSymbolElement.release()));
    }
    for (const std::unique_ptr<BoundExpression>& argument : boundFunctionCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDelegateCall& boundDelegateCall)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> callElement(new sngxml::dom::Element(U"BoundDelegateCall"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    if (boundDelegateCall.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDelegateCall.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    std::unique_ptr<sngxml::dom::Element> delegateSymbolElement = boundDelegateCall.GetDelegateSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(delegateSymbolElement.release()));
    for (const std::unique_ptr<BoundExpression>& argument : boundDelegateCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClassDelegateCall& boundClassDelegateCall)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> callElement(new sngxml::dom::Element(U"BoundClassDelegateCall"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = callElement.get();
    if (boundClassDelegateCall.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundClassDelegateCall.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    std::unique_ptr<sngxml::dom::Element> classDelegateSymbolElement = boundClassDelegateCall.GetClassDelegateSymbol()->ToDomElement(typeMap);
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(classDelegateSymbolElement.release()));
    for (const std::unique_ptr<BoundExpression>& argument : boundClassDelegateCall.Arguments())
    {
        argument->Accept(*this);
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(callElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConversion& boundConversion)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conversionElement(new sngxml::dom::Element(U"BoundConversion"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conversionElement.get();
    if (boundConversion.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConversion.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundConversion.SourceExpr()->Accept(*this);
    FunctionSymbol* conversionFun = boundConversion.ConversionFun();
    if (conversionFun)
    {
        const char* className = conversionFun->ClassName();
        std::unique_ptr<sngxml::dom::Element> functionSymbolElement(new sngxml::dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", conversionFun->Info());
        if (conversionFun->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(conversionFun->ClassArity())));
        }
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionSymbolElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conversionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructExpression& boundConstructExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructElement(new sngxml::dom::Element(U"BoundConstructExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    if (boundConstructExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConstructExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundConstructExpression.ConstructorCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConstructAndReturnTemporaryExpression& boundConstructAndReturnTemporaryExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> constructElement(new sngxml::dom::Element(U"BoundConstructAndReturnTemporaryExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = constructElement.get();
    if (boundConstructAndReturnTemporaryExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConstructAndReturnTemporaryExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundConstructAndReturnTemporaryExpression.ConstructorCall()->Accept(*this);
    boundConstructAndReturnTemporaryExpression.BoundTemporary()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(constructElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundClassOrClassDelegateConversionResult& boundClassOrClassDelegateConversionResult)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conversionResultElement(new sngxml::dom::Element(U"BoundClassOrClassDelegateConversionResult"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conversionResultElement.get();
    if (boundClassOrClassDelegateConversionResult.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundClassOrClassDelegateConversionResult.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundClassOrClassDelegateConversionResult.ConversionResult()->Accept(*this);
    boundClassOrClassDelegateConversionResult.ConversionFunctionCall()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conversionResultElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundIsExpression& boundIsExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> isElement(new sngxml::dom::Element(U"BoundIsExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = isElement.get();
    boundIsExpression.Expr()->Accept(*this);
    if (boundIsExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundIsExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    std::unique_ptr<sngxml::dom::Element> rightTypeElement(new sngxml::dom::Element(U"type"));
    int rightTypeId = typeMap.GetOrInsertType(boundIsExpression.RightClassType());
    rightTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(rightTypeId)));
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rightTypeElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(isElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundAsExpression& boundAsExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> asElement(new sngxml::dom::Element(U"BoundAsExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = asElement.get();
    boundAsExpression.Expr()->Accept(*this);
    boundAsExpression.Variable()->Accept(*this);
    if (boundAsExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundAsExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    std::unique_ptr<sngxml::dom::Element> rightTypeElement(new sngxml::dom::Element(U"type"));
    int rightTypeId = typeMap.GetOrInsertType(boundAsExpression.RightClassType());
    rightTypeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(rightTypeId)));
    currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(rightTypeElement.release()));
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(asElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTypeNameExpression& boundTypeNameExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typeNameElement(new sngxml::dom::Element(U"BoundTypeNameExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    if (boundTypeNameExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTypeNameExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundTypeNameExpression.ClassPtr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundTypeIdExpression& boundTypeIdExpression)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> typeNameElement(new sngxml::dom::Element(U"BoundTypeIdExpression"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = typeNameElement.get();
    if (boundTypeIdExpression.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundTypeIdExpression.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundTypeIdExpression.ClassPtr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeNameElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundBitCast& boundBitCast)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> bitCastElement(new sngxml::dom::Element(U"BoundBitCast"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = bitCastElement.get();
    if (boundBitCast.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundBitCast.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundBitCast.Expr()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bitCastElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundFunctionPtr& boundFunctionPtr)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> functionPtrElement(new sngxml::dom::Element(U"BoundFunctionPtr"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = functionPtrElement.get();
    if (boundFunctionPtr.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundFunctionPtr.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    FunctionSymbol* functionSymbol = boundFunctionPtr.Function();
    if (functionSymbol)
    {
        const char* className = functionSymbol->ClassName();
        std::unique_ptr<sngxml::dom::Element> functionSymbolElement(new sngxml::dom::Element(ToUtf32(className)));
        functionSymbolElement->SetAttribute(U"info", functionSymbol->Info());
        if (functionSymbol->ClassArity() != 0)
        {
            functionSymbolElement->SetAttribute(U"classArity", ToUtf32(std::to_string(functionSymbol->ClassArity())));
        }
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionSymbolElement.release()));
    }
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(functionPtrElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundDisjunction& boundDisjunction)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> disjunctionElement(new sngxml::dom::Element(U"BoundDisjunction"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = disjunctionElement.get();
    if (boundDisjunction.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundDisjunction.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundDisjunction.Left()->Accept(*this);
    boundDisjunction.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(disjunctionElement.release()));
    currentElement = prevElement;
}

void Bdt2DomVisitor::Visit(BoundConjunction& boundConjunction)
{
    CheckCurrentElement();
    std::unique_ptr<sngxml::dom::Element> conjunctionElement(new sngxml::dom::Element(U"BoundConjunction"));
    sngxml::dom::Element* prevElement = currentElement;
    currentElement = conjunctionElement.get();
    if (boundConjunction.GetType())
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(new sngxml::dom::Element(U"type"));
        int typeId = typeMap.GetOrInsertType(boundConjunction.GetType());
        typeElement->SetAttribute(U"ref", U"type_" + ToUtf32(std::to_string(typeId)));
        currentElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundConjunction.Left()->Accept(*this);
    boundConjunction.Right()->Accept(*this);
    prevElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(conjunctionElement.release()));
    currentElement = prevElement;
}

std::unique_ptr<sngxml::dom::Document> GenerateBdtDocument(cmajor::binder::BoundNode* node)
{
    std::unique_ptr<sngxml::dom::Document> doc(new sngxml::dom::Document());
    Bdt2DomVisitor visitor;
    node->Accept(visitor);
    std::unique_ptr<sngxml::dom::Element> boundTreeElement(new sngxml::dom::Element(U"boundTree"));
    std::unique_ptr<sngxml::dom::Element> boundCompileUnitElement = visitor.GetBoundCompileUnitElement();
    boundTreeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(boundCompileUnitElement.release()));
    std::unique_ptr<sngxml::dom::Element> typesElement(new sngxml::dom::Element(U"types"));
    std::vector<std::unique_ptr<sngxml::dom::Element>> typeElements = visitor.GetTypeMap().TypeElements();
    int n = typeElements.size();
    for (int i = 0; i < n; ++i)
    {
        std::unique_ptr<sngxml::dom::Element> typeElement(std::move(typeElements[i]));
        typesElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typeElement.release()));
    }
    boundTreeElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(typesElement.release()));
    doc->AppendChild(std::unique_ptr<sngxml::dom::Node>(boundTreeElement.release()));
    return doc;
}

} } // namespace cmajor::bdt2dom
