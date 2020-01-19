// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmupd/ConversionVisitor.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/CompileUnit.hpp>
#include <sngcm/ast/Namespace.hpp>
#include <sngcm/ast/Identifier.hpp>
#include <sngcm/ast/Template.hpp>
#include <sngcm/ast/Function.hpp>
#include <sngcm/ast/Attribute.hpp>
#include <sngcm/ast/Class.hpp>
#include <sngcm/ast/Interface.hpp>
#include <sngcm/ast/Delegate.hpp>
#include <sngcm/ast/Concept.hpp>
#include <sngcm/ast/Statement.hpp>
#include <sngcm/ast/Typedef.hpp>
#include <sngcm/ast/Constant.hpp>
#include <sngcm/ast/Enumeration.hpp>
#include <sngcm/ast/Parameter.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/GlobalVariable.hpp>
#include <soulng/util/Unicode.hpp>
#include <map>
#include <set>
#include <iostream>

std::map<std::u32string, std::u32string> idMap;
std::map<std::u32string, bool> removedIds;
std::map<std::u32string, std::u32string> groupIdMap;

using namespace soulng::unicode;

void InitMaps()
{
    idMap[U"System.Text.RegularExpressions"] = U"System.RegularExpressions";
    idMap[U"DomDocument"] = U"Document";
    idMap[U"DomAttr"] = U"Attr";
    idMap[U"DomCDataSection"] = U"CDataSection";
    idMap[U"DomCharacterData"] = U"CharacterData";
    idMap[U"DomComment"] = U"Comment";
    idMap[U"DomDocumentFragment"] = U"DocumentFragment";
    idMap[U"DomDocumentBuilder"] = U"DomDocumentHandler";
    idMap[U"DomElement"] = U"Element";
    idMap[U"DomEntityReference"] = U"EntityReference";
    idMap[U"DomNode"] = U"Node";
    idMap[U"DomParentNode"] = U"ParentNode";
    idMap[U"DomProcessingInstruction"] = U"ProcessingInstruction";
    idMap[U"DomText"] = U"Text";
    idMap[U"DomText"] = U"Text";
    idMap[U"ParseXmlContentToDomDocument"] = U"ParseDocument";
    idMap[U"ParseXmlFileToDomDocument"] = U"ReadDocument";

    removedIds[U"RegExFlags"] = false;
    removedIds[U"XPathDebug"] = false;
    removedIds[U"System.Text.Parsing"] = false;
    removedIds[U"System.Text.Parsing.CodeDom"] = false;

    groupIdMap[U"CDataSection"] = U"HandleCDataSection";
    groupIdMap[U"Comment"] = U"HandleComment";
    groupIdMap[U"PI"] = U"HandlePI";
    groupIdMap[U"Text"] = U"HandleText";
}



IdentifierNode* ChangeId(const std::u32string& str)
{
    auto it = idMap.find(str);
    if (it != idMap.cend())
    {
        return new IdentifierNode(Span(), it->second);
    }
    auto rit = removedIds.find(str);
    if (rit != removedIds.cend())
    {
        if (!rit->second)
        {
            rit->second = true;
            std::cout << "'" + ToUtf8(str) << "' not supported any more, you need to do manual changes" << std::endl;
        }
    }
    return nullptr;
}

std::u32string MapGroupId(const std::u32string& groupId, bool& changed)
{
    auto it = groupIdMap.find(groupId);
    if (it != groupIdMap.cend())
    {
        changed = true;
        return it->second;
    }
    return groupId;
}

ConversionVisitor::ConversionVisitor() : changed(false), baseClassId(false), derivesFromXmlContentHandler(false)
{
}

std::unique_ptr<CompileUnitNode> ConversionVisitor::GetConvertedCompileUnit()
{
    if (node)
    {
        if (node->GetNodeType() == NodeType::compileUnitNode)
        {
            return std::unique_ptr<CompileUnitNode>(static_cast<CompileUnitNode*>(node.release()));
        }
        else
        {
            throw std::runtime_error("not compile unit node");
        }
    }
    else
    {
        throw std::runtime_error("node null");
    }
}

void ConversionVisitor::Visit(BoolNode& boolNode)
{
    node.reset(boolNode.Clone(ctx));
}

void ConversionVisitor::Visit(SByteNode& sbyteNode)
{
    node.reset(sbyteNode.Clone(ctx));
}

void ConversionVisitor::Visit(ByteNode& byteNode)
{
    node.reset(byteNode.Clone(ctx));
}

void ConversionVisitor::Visit(ShortNode& shortNode)
{
    node.reset(shortNode.Clone(ctx));
}

void ConversionVisitor::Visit(UShortNode& ushortNode)
{
    node.reset(ushortNode.Clone(ctx));
}

void ConversionVisitor::Visit(IntNode& intNode)
{
    node.reset(intNode.Clone(ctx));
}

void ConversionVisitor::Visit(UIntNode& uintNode)
{
    node.reset(uintNode.Clone(ctx));
}

void ConversionVisitor::Visit(LongNode& longNode)
{
    node.reset(longNode.Clone(ctx));
}

void ConversionVisitor::Visit(ULongNode& ulongNode)
{
    node.reset(ulongNode.Clone(ctx));
}

void ConversionVisitor::Visit(FloatNode& floatNode)
{
    node.reset(floatNode.Clone(ctx));
}

void ConversionVisitor::Visit(DoubleNode& doubleNode)
{
    node.reset(doubleNode.Clone(ctx));
}

void ConversionVisitor::Visit(CharNode& charNode)
{
    node.reset(charNode.Clone(ctx));
}

void ConversionVisitor::Visit(WCharNode& wcharNode)
{
    node.reset(wcharNode.Clone(ctx));
}

void ConversionVisitor::Visit(UCharNode& ucharNode)
{
    node.reset(ucharNode.Clone(ctx));
}

void ConversionVisitor::Visit(VoidNode& voidNode)
{
    node.reset(voidNode.Clone(ctx));
}

void ConversionVisitor::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    node.reset(booleanLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    node.reset(sbyteLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(ByteLiteralNode& byteLiteralNode)
{
    node.reset(byteLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(ShortLiteralNode& shortLiteralNode)
{
    node.reset(shortLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(UShortLiteralNode& ushortLiteralNode)
{
    node.reset(ushortLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(IntLiteralNode& intLiteralNode)
{
    node.reset(intLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(UIntLiteralNode& uintLiteralNode)
{
    node.reset(uintLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(LongLiteralNode& longLiteralNode)
{
    node.reset(longLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(ULongLiteralNode& ulongLiteralNode)
{
    node.reset(ulongLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(FloatLiteralNode& floatLiteralNode)
{
    node.reset(floatLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    node.reset(doubleLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(CharLiteralNode& charLiteralNode)
{
    node.reset(charLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(WCharLiteralNode& wcharLiteralNode)
{
    node.reset(wcharLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(UCharLiteralNode& ucharLiteralNode)
{
    node.reset(ucharLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(StringLiteralNode& stringLiteralNode)
{
    node.reset(stringLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(WStringLiteralNode& wstringLiteralNode)
{
    node.reset(wstringLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(UStringLiteralNode& ustringLiteralNode)
{
    node.reset(ustringLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(NullLiteralNode& nullLiteralNode)
{
    node.reset(nullLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    std::unique_ptr<ArrayLiteralNode> clone(new ArrayLiteralNode(Span()));
    int n = arrayLiteralNode.Values().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* value = arrayLiteralNode.Values()[i];
        value->Accept(*this);
        clone->AddValue(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    std::unique_ptr<StructuredLiteralNode> clone(new StructuredLiteralNode(Span()));
    int n = structuredLiteralNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = structuredLiteralNode.Members()[i];
        member->Accept(*this);
        clone->AddMember(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(UuidLiteralNode& uuidLiteralNode)
{
    node.reset(uuidLiteralNode.Clone(ctx));
}

void ConversionVisitor::Visit(CompileUnitNode& compileUnitNode)
{
    std::unique_ptr<CompileUnitNode> clone(new CompileUnitNode(Span(), compileUnitNode.FilePath()));
    compileUnitNode.GlobalNs()->Accept(*this);
    clone->ResetGlobalNs(static_cast<NamespaceNode*>(node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(NamespaceNode& namespaceNode)
{
    namespaceNode.Id()->Accept(*this);
    IdentifierNode* id = static_cast<IdentifierNode*>(node.release());
    std::unique_ptr<NamespaceNode> clone(new NamespaceNode(Span(), id));
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        namespaceNode.Members()[i]->Accept(*this);
        clone->AddMember(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AliasNode& aliasNode)
{
    aliasNode.Id()->Accept(*this);
    IdentifierNode* id = static_cast<IdentifierNode*>(node.release());
    aliasNode.Qid()->Accept(*this);
    IdentifierNode* qid = static_cast<IdentifierNode*>(node.release());
    node.reset(new AliasNode(Span(), id, qid));
}

void ConversionVisitor::Visit(NamespaceImportNode& namespaceImportNode)
{
    namespaceImportNode.Ns()->Accept(*this);
    IdentifierNode* ns = static_cast<IdentifierNode*>(node.release());
    node.reset(new NamespaceImportNode(Span(), ns));
}

void ConversionVisitor::Visit(IdentifierNode& identifierNode)
{
    std::u32string str = identifierNode.Str();
    if (baseClassId)
    {
        if (str == U"XmlContentHandler" || str == U"System.Xml.XmlContentHandler")
        {
            derivesFromXmlContentHandler = true;
        }
    }
    IdentifierNode* newId = ChangeId(str);
    if (newId)
    {
        changed = true;
        node.reset(newId);
    }
    else
    {
        node.reset(new IdentifierNode(Span(), str));
    }
}

void ConversionVisitor::Visit(CursorIdNode& cursorIdNode)
{
    std::u32string str = cursorIdNode.Str();
    node.reset(new CursorIdNode(Span(), str));
}

void ConversionVisitor::Visit(TemplateIdNode& templateIdNode)
{
    templateIdNode.Primary()->Accept(*this);
    std::unique_ptr<TemplateIdNode> clone(new TemplateIdNode(Span(), node.release()));
    int n = templateIdNode.TemplateArguments().Count();
    for (int i = 0; i < n; ++i)
    {
        templateIdNode.TemplateArguments()[i]->Accept(*this);
        clone->AddTemplateArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(FunctionNode& functionNode)
{
    functionNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    std::unique_ptr<Attributes> attributes;
    if (functionNode.GetAttributes())
    {
        attributes.reset(functionNode.GetAttributes()->Clone());
    }
    std::unique_ptr<FunctionNode> clone(new FunctionNode(Span(), functionNode.GetSpecifiers(), returnTypeExpr.release(), functionNode.GroupId(), attributes.release()));
    int n = functionNode.TemplateParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        functionNode.TemplateParameters()[i]->Accept(*this);
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(node.release()));
    }
    int np = functionNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        functionNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    if (functionNode.WhereConstraint())
    {
        functionNode.WhereConstraint()->Accept(*this);
        std::unique_ptr<WhereConstraintNode> constraint(static_cast<WhereConstraintNode*>(node.release()));
        clone->SetConstraint(constraint.release());
    }
    if (functionNode.Body())
    {
        functionNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ClassNode& classNode)
{
    bool prevDerivesFromXmlContentHandler = derivesFromXmlContentHandler;
    classNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<Attributes> attributes;
    if (classNode.GetAttributes())
    {
        attributes.reset(classNode.GetAttributes()->Clone());
    }
    std::unique_ptr<ClassNode> clone(new ClassNode(Span(), classNode.GetSpecifiers(), id.release(), attributes.release()));
    int nt = classNode.TemplateParameters().Count();
    for (int i = 0; i < nt; ++i)
    {
        classNode.TemplateParameters()[i]->Accept(*this);
        clone->AddTemplateParameter(static_cast<TemplateParameterNode*>(node.release()));
    }
    int nb = classNode.BaseClassOrInterfaces().Count();
    for (int i = 0; i < nb; ++i)
    {
        bool prevBasClassId = baseClassId;
        baseClassId = true;
        classNode.BaseClassOrInterfaces()[i]->Accept(*this);
        baseClassId = prevBasClassId;
        clone->AddBaseClassOrInterface(node.release());
    }
    if (classNode.WhereConstraint())
    {
        classNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    int nm = classNode.Members().Count();
    for (int i = 0; i < nm; ++i)
    {
        classNode.Members()[i]->Accept(*this);
        clone->AddMember(node.release());
    }
    node.reset(clone.release());
    derivesFromXmlContentHandler = prevDerivesFromXmlContentHandler;
}

void ConversionVisitor::Visit(ThisInitializerNode& thisInitializerNode)
{
    std::unique_ptr<ThisInitializerNode> clone(new ThisInitializerNode(Span()));
    int n = thisInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        thisInitializerNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(BaseInitializerNode& baseInitializerNode)
{
    std::unique_ptr<BaseInitializerNode> clone(new BaseInitializerNode(Span()));
    int n = baseInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        baseInitializerNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MemberInitializerNode& memberInitializerNode)
{
    memberInitializerNode.MemberId()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<MemberInitializerNode> clone(new MemberInitializerNode(Span(), id.release()));
    int n = memberInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        memberInitializerNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(StaticConstructorNode& staticConstructorNode)
{
    std::unique_ptr<Attributes> attributes;
    if (staticConstructorNode.GetAttributes())
    {
        attributes.reset(staticConstructorNode.GetAttributes()->Clone());
    }
    std::unique_ptr<StaticConstructorNode> clone(new StaticConstructorNode(Span(), staticConstructorNode.GetSpecifiers(), attributes.release()));
    int ni = staticConstructorNode.Initializers().Count();
    for (int i = 0; i < ni; ++i)
    {
        staticConstructorNode.Initializers()[i]->Accept(*this);
        clone->AddInitializer(static_cast<InitializerNode*>(node.release()));
    }
    if (staticConstructorNode.WhereConstraint())
    {
        staticConstructorNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    if (staticConstructorNode.Body())
    {
        staticConstructorNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    staticConstructorNode.ClassId()->Accept(*this);
    clone->SetClassId(static_cast<IdentifierNode*>(node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConstructorNode& constructorNode)
{
    std::unique_ptr<Attributes> attributes;
    if (constructorNode.GetAttributes())
    {
        attributes.reset(constructorNode.GetAttributes()->Clone());
    }
    std::unique_ptr<ConstructorNode> clone(new ConstructorNode(Span(), constructorNode.GetSpecifiers(), attributes.release()));
    int ni = constructorNode.Initializers().Count();
    for (int i = 0; i < ni; ++i)
    {
        constructorNode.Initializers()[i]->Accept(*this);
        clone->AddInitializer(static_cast<InitializerNode*>(node.release()));
    }
    if (constructorNode.WhereConstraint())
    {
        constructorNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    int np = constructorNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        constructorNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    if (constructorNode.Body())
    {
        constructorNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    constructorNode.ClassId()->Accept(*this);
    clone->SetClassId(static_cast<IdentifierNode*>(node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DestructorNode& destructorNode)
{
    std::unique_ptr<Attributes> attributes;
    if (destructorNode.GetAttributes())
    {
        attributes.reset(destructorNode.GetAttributes()->Clone());
    }
    std::unique_ptr<DestructorNode> clone(new DestructorNode(Span(), destructorNode.GetSpecifiers(), attributes.release()));
    if (destructorNode.WhereConstraint())
    {
        destructorNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    if (destructorNode.Body())
    {
        destructorNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    destructorNode.ClassId()->Accept(*this);
    clone->SetClassId(static_cast<IdentifierNode*>(node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MemberFunctionNode& memberFunctionNode)
{
    std::unique_ptr<Attributes> attributes;
    if (memberFunctionNode.GetAttributes())
    {
        attributes.reset(memberFunctionNode.GetAttributes()->Clone());
    }
    memberFunctionNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    std::u32string groupId = memberFunctionNode.GroupId();
    if (derivesFromXmlContentHandler)
    {
        groupId = MapGroupId(groupId, changed);
    }
    std::unique_ptr<MemberFunctionNode> clone(new MemberFunctionNode(Span(), memberFunctionNode.GetSpecifiers(), returnTypeExpr.release(), groupId, attributes.release()));
    if (memberFunctionNode.WhereConstraint())
    {
        memberFunctionNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    int np = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        memberFunctionNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    if (memberFunctionNode.Body())
    {
        memberFunctionNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    if (memberFunctionNode.IsConst())
    {
        clone->SetConst();
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    std::unique_ptr<Attributes> attributes;
    if (conversionFunctionNode.GetAttributes())
    {
        attributes.reset(conversionFunctionNode.GetAttributes()->Clone());
    }
    conversionFunctionNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    std::unique_ptr<ConversionFunctionNode> clone(new ConversionFunctionNode(Span(), conversionFunctionNode.GetSpecifiers(), returnTypeExpr.release(), attributes.release()));
    if (conversionFunctionNode.WhereConstraint())
    {
        conversionFunctionNode.WhereConstraint()->Accept(*this);
        clone->SetConstraint(static_cast<WhereConstraintNode*>(node.release()));
    }
    int np = conversionFunctionNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        conversionFunctionNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    if (conversionFunctionNode.Body())
    {
        conversionFunctionNode.Body()->Accept(*this);
        clone->SetBody(static_cast<CompoundStatementNode*>(node.release()));
    }
    if (conversionFunctionNode.IsConst())
    {
        clone->SetConst();
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MemberVariableNode& memberVariableNode)
{
    std::unique_ptr<Attributes> attributes;
    if (memberVariableNode.GetAttributes())
    {
        attributes.reset(memberVariableNode.GetAttributes()->Clone());
    }
    memberVariableNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    memberVariableNode.Id()->Accept(*this);
    std::unique_ptr <IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<MemberVariableNode> clone(new MemberVariableNode(Span(), memberVariableNode.GetSpecifiers(), typeExpr.release(), id.release(), attributes.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(InterfaceNode& interfaceNode)
{
    std::unique_ptr<Attributes> attributes;
    if (interfaceNode.GetAttributes())
    {
        attributes.reset(interfaceNode.GetAttributes()->Clone());
    }
    interfaceNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<InterfaceNode> clone(new InterfaceNode(Span(), interfaceNode.GetSpecifiers(), id.release(), attributes.release()));
    int n = interfaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        interfaceNode.Members()[i]->Accept(*this);
        clone->AddMember(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DelegateNode& delegateNode)
{
    delegateNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    delegateNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<DelegateNode> clone(new DelegateNode(Span(), delegateNode.GetSpecifiers(), returnTypeExpr.release(), id.release()));
    int np = delegateNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        delegateNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ClassDelegateNode& classDelegateNode)
{
    classDelegateNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    classDelegateNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<ClassDelegateNode> clone(new ClassDelegateNode(Span(), classDelegateNode.GetSpecifiers(), returnTypeExpr.release(), id.release()));
    int np = classDelegateNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        classDelegateNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    std::unique_ptr<ConstraintNode> constraint(static_cast<ConstraintNode*>(node.release()));
    std::unique_ptr<ParenthesizedConstraintNode> clone(new ParenthesizedConstraintNode(Span(), constraint.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    disjunctiveConstraintNode.Left()->Accept(*this);
    std::unique_ptr<ConstraintNode> left(static_cast<ConstraintNode*>(node.release()));
    disjunctiveConstraintNode.Right()->Accept(*this);
    std::unique_ptr<ConstraintNode> right(static_cast<ConstraintNode*>(node.release()));
    std::unique_ptr<DisjunctiveConstraintNode> clone(new DisjunctiveConstraintNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    conjunctiveConstraintNode.Left()->Accept(*this);
    std::unique_ptr<ConstraintNode> left(static_cast<ConstraintNode*>(node.release()));
    conjunctiveConstraintNode.Right()->Accept(*this);
    std::unique_ptr<ConstraintNode> right(static_cast<ConstraintNode*>(node.release()));
    std::unique_ptr<ConjunctiveConstraintNode> clone(new ConjunctiveConstraintNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(WhereConstraintNode& whereConstraintNode)
{
    whereConstraintNode.Constraint()->Accept(*this);
    std::unique_ptr<ConstraintNode> constraint(static_cast<ConstraintNode*>(node.release()));
    std::unique_ptr<WhereConstraintNode> clone(new WhereConstraintNode(Span(), constraint.release()));
    if (whereConstraintNode.Semicolon())
    {
        clone->SetSemicolon();
    }
    if (whereConstraintNode.IsHeaderConstraint())
    {
        clone->SetHeaderConstraint();
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    predicateConstraintNode.InvokeExpr()->Accept(*this);
    std::unique_ptr<PredicateConstraintNode> clone(new PredicateConstraintNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(IsConstraintNode& isConstraintNode)
{
    isConstraintNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
    std::unique_ptr<Node> conceptOrTypeName(node.release());
    std::unique_ptr<IsConstraintNode> clone(new IsConstraintNode(Span(), typeExpr.release(), conceptOrTypeName.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    multiParamConstraintNode.ConceptId()->Accept(*this);
    std::unique_ptr<IdentifierNode> conceptId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<MultiParamConstraintNode> clone(new MultiParamConstraintNode(Span(), conceptId.release()));
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
        clone->AddTypeExpr(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    typeNameConstraintNode.TypeId()->Accept(*this);
    node.reset(new TypeNameConstraintNode(Span(), node.release()));
}

void ConversionVisitor::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    constructorConstraintNode.TypeParamId()->Accept(*this);
    std::unique_ptr<IdentifierNode> typeParamId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr< ConstructorConstraintNode> clone(new ConstructorConstraintNode(Span(), typeParamId.release()));
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        constructorConstraintNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    destructorConstraintNode.TypeParamId()->Accept(*this);
    std::unique_ptr<IdentifierNode> typeParamId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<DestructorConstraintNode> clone(new DestructorConstraintNode(Span(), typeParamId.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    memberFunctionConstraintNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    std::unique_ptr<IdentifierNode> typeParamId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<MemberFunctionConstraintNode> clone(new MemberFunctionConstraintNode(Span(), returnTypeExpr.release(), typeParamId.release(), memberFunctionConstraintNode.GroupId()));
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        memberFunctionConstraintNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(FunctionConstraintNode& functionConstraintNode)
{
    functionConstraintNode.ReturnTypeExpr()->Accept(*this);
    std::unique_ptr<Node> returnTypeExpr(node.release());
    std::unique_ptr<FunctionConstraintNode> clone(new FunctionConstraintNode(Span(), returnTypeExpr.release(), functionConstraintNode.GroupId()));
    int n = functionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        functionConstraintNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AxiomStatementNode& axiomStatementNode)
{
    std::unique_ptr<Node> expression;
    if (axiomStatementNode.Expression())
    {
        axiomStatementNode.Expression()->Accept(*this);
        expression.reset(node.release());
    }
    std::unique_ptr<AxiomStatementNode> clone(new AxiomStatementNode(Span(), expression.release(), axiomStatementNode.Text()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AxiomNode& axiomNode)
{
    axiomNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<AxiomNode> clone(new AxiomNode(Span(), id.release()));
    int np = axiomNode.Parameters().Count();
    for (int i = 0; i < np; ++i)
    {
        axiomNode.Parameters()[i]->Accept(*this);
        clone->AddParameter(static_cast<ParameterNode*>(node.release()));
    }
    int ns = axiomNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        axiomNode.Statements()[i]->Accept(*this);
        clone->AddStatement(static_cast<AxiomStatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConceptIdNode& conceptIdNode)
{
    conceptIdNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<ConceptIdNode> clone(new ConceptIdNode(Span(), id.release()));
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        conceptIdNode.TypeParameters()[i]->Accept(*this);
        clone->AddTypeParameter(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConceptNode& conceptNode)
{
    conceptNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<ConceptNode> clone(new ConceptNode(Span(), conceptNode.GetSpecifiers(), id.release()));
    int nt = conceptNode.TypeParameters().Count();
    for (int i = 0; i < nt; ++i)
    {
        conceptNode.TypeParameters()[i]->Accept(*this);
        clone->AddTypeParameter(static_cast<IdentifierNode*>(node.release()));
    }
    if (conceptNode.Refinement())
    {
        conceptNode.Refinement()->Accept(*this);
        clone->SetRefinement(static_cast<ConceptIdNode*>(node.release()));
    }
    int nc = conceptNode.Constraints().Count();
    for (int i = 0; i < nc; ++i)
    {
        conceptNode.Constraints()[i]->Accept(*this);
        clone->AddConstraint(static_cast<ConstraintNode*>(node.release()));
    }
    int na = conceptNode.Axioms().Count();
    for (int i = 0; i < na; ++i)
    {
        conceptNode.Axioms()[i]->Accept(*this);
        clone->AddAxiom(static_cast<AxiomNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(LabelNode& labelNode)
{
    node.reset(new LabelNode(Span(), labelNode.Label()));
}

void ConversionVisitor::Visit(LabeledStatementNode& labeledStatementNode)
{
    labeledStatementNode.Label()->Accept(*this);
    std::unique_ptr<LabelNode> labelNode(static_cast<LabelNode*>(node.release()));
    labeledStatementNode.Stmt()->Accept(*this);
    std::unique_ptr<StatementNode> stmt(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<LabeledStatementNode> clone(new LabeledStatementNode(Span(), stmt.release()));
    clone->SetLabelNode(labelNode.release());
    node.reset(clone.release());
}

void ConversionVisitor::Visit(CompoundStatementNode& compoundStatementNode)
{
    std::unique_ptr<CompoundStatementNode> clone(new CompoundStatementNode(Span()));
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        compoundStatementNode.Statements()[i]->Accept(*this);
        clone->AddStatement(static_cast<StatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ReturnStatementNode& returnStatementNode)
{
    std::unique_ptr<Node> exprNode;
    if (returnStatementNode.Expression())
    {
        returnStatementNode.Expression()->Accept(*this);
        exprNode.reset(node.release());
    }
    std::unique_ptr<ReturnStatementNode> clone(new ReturnStatementNode(Span(), exprNode.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(IfStatementNode& ifStatementNode)
{
    ifStatementNode.Condition()->Accept(*this);
    std::unique_ptr<Node> condition(node.release());
    ifStatementNode.ThenS()->Accept(*this);
    std::unique_ptr<StatementNode> thenS(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<StatementNode> elseS;
    if (ifStatementNode.ElseS())
    {
        ifStatementNode.ElseS()->Accept(*this);
        elseS.reset(static_cast<StatementNode*>(node.release()));
    }
    std::unique_ptr<IfStatementNode> clone(new IfStatementNode(Span(), condition.release(), thenS.release(), elseS.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(WhileStatementNode& whileStatementNode)
{
    whileStatementNode.Condition()->Accept(*this);
    std::unique_ptr<Node> condition(node.release());
    whileStatementNode.Statement()->Accept(*this);
    std::unique_ptr<StatementNode> statement(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<WhileStatementNode> clone(new WhileStatementNode(Span(), condition.release(), statement.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DoStatementNode& doStatementNode)
{
    doStatementNode.Condition()->Accept(*this);
    std::unique_ptr<Node> condition(node.release());
    doStatementNode.Statement()->Accept(*this);
    std::unique_ptr<StatementNode> statement(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<DoStatementNode> clone(new DoStatementNode(Span(), statement.release(), condition.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ForStatementNode& forStatementNode)
{
    std::unique_ptr<StatementNode> initS;
    if (forStatementNode.InitS())
    {
        forStatementNode.InitS()->Accept(*this);
        initS.reset(static_cast<StatementNode*>(node.release()));
    }
    std::unique_ptr<Node> condition;
    if (forStatementNode.Condition())
    {
        forStatementNode.Condition()->Accept(*this);
        condition.reset(node.release());
    }
    std::unique_ptr<StatementNode> loopS;
    if (forStatementNode.LoopS())
    {
        forStatementNode.LoopS()->Accept(*this);
        loopS.reset(static_cast<StatementNode*>(node.release()));
    }
    std::unique_ptr<StatementNode> actionS;
    forStatementNode.ActionS()->Accept(*this);
    actionS.reset(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<ForStatementNode> clone(new ForStatementNode(Span(), initS.release(), condition.release(), loopS.release(), actionS.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(BreakStatementNode& breakStatementNode)
{
    node.reset(breakStatementNode.Clone(ctx));
}

void ConversionVisitor::Visit(ContinueStatementNode& continueStatementNode)
{
    node.reset(continueStatementNode.Clone(ctx));
}

void ConversionVisitor::Visit(GotoStatementNode& gotoStatementNode)
{
    node.reset(gotoStatementNode.Clone(ctx));
}

void ConversionVisitor::Visit(ConstructionStatementNode& constructionStatementNode)
{
    std::unique_ptr<Node> typeExpr;
    constructionStatementNode.TypeExpr()->Accept(*this);
    typeExpr.reset(node.release());
    std::unique_ptr<IdentifierNode> id;
    constructionStatementNode.Id()->Accept(*this);
    id.reset(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<ConstructionStatementNode> clone(new ConstructionStatementNode(Span(), typeExpr.release(), id.release()));
    int n = constructionStatementNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        constructionStatementNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    if (constructionStatementNode.Assignment())
    {
        clone->SetAssignment();
    }
    if (constructionStatementNode.Empty())
    {
        clone->SetEmpty();
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DeleteStatementNode& deleteStatementNode)
{
    deleteStatementNode.Expression()->Accept(*this);
    std::unique_ptr<DeleteStatementNode> clone(new DeleteStatementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DestroyStatementNode& destroyStatementNode)
{
    destroyStatementNode.Expression()->Accept(*this);
    std::unique_ptr<DestroyStatementNode> clone(new DestroyStatementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    assignmentStatementNode.TargetExpr()->Accept(*this);
    std::unique_ptr<Node> targetExpr(node.release());
    assignmentStatementNode.SourceExpr()->Accept(*this);
    std::unique_ptr<Node> sourceExpr(node.release());
    std::unique_ptr<AssignmentStatementNode> clone(new AssignmentStatementNode(Span(), targetExpr.release(), sourceExpr.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ExpressionStatementNode& expressionStatementNode)
{
    expressionStatementNode.Expression()->Accept(*this);
    std::unique_ptr<ExpressionStatementNode> clone(new ExpressionStatementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(EmptyStatementNode& emptyStatementNode)
{
    node.reset(emptyStatementNode.Clone(ctx));
}

void ConversionVisitor::Visit(RangeForStatementNode& rangeForStatementNode)
{
    rangeForStatementNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    rangeForStatementNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    rangeForStatementNode.Container()->Accept(*this);
    std::unique_ptr<Node> container(node.release());
    rangeForStatementNode.Action()->Accept(*this);
    std::unique_ptr<StatementNode> action(static_cast<StatementNode*>(node.release()));
    std::unique_ptr<RangeForStatementNode> clone(new RangeForStatementNode(Span(), typeExpr.release(), id.release(), container.release(), action.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(SwitchStatementNode& switchStatementNode)
{
    switchStatementNode.Condition()->Accept(*this);
    std::unique_ptr<Node> condition(node.release());
    std::unique_ptr<SwitchStatementNode> clone(new SwitchStatementNode(Span(), condition.release()));
    int n = switchStatementNode.Cases().Count();
    for (int i = 0; i < n; ++i)
    {
        switchStatementNode.Cases()[i]->Accept(*this);
        clone->AddCase(static_cast<CaseStatementNode*>(node.release()));
    }
    if (switchStatementNode.Default())
    {
        switchStatementNode.Default()->Accept(*this);
        clone->SetDefault(static_cast<DefaultStatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(CaseStatementNode& caseStatementNode)
{
    std::unique_ptr<CaseStatementNode> clone(new CaseStatementNode(Span()));
    int n = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        caseStatementNode.CaseExprs()[i]->Accept(*this);
        clone->AddCaseExpr(node.release());
    }
    int ns = caseStatementNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        caseStatementNode.Statements()[i]->Accept(*this);
        clone->AddStatement(static_cast<StatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DefaultStatementNode& defaultStatementNode)
{
    std::unique_ptr<DefaultStatementNode> clone(new DefaultStatementNode(Span()));
    int ns = defaultStatementNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        defaultStatementNode.Statements()[i]->Accept(*this);
        clone->AddStatement(static_cast<StatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    gotoCaseStatementNode.CaseExpr()->Accept(*this);
    std::unique_ptr<GotoCaseStatementNode> clone(new GotoCaseStatementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    node.reset(gotoDefaultStatementNode.Clone(ctx));
}

void ConversionVisitor::Visit(ThrowStatementNode& throwStatementNode)
{
    std::unique_ptr<Node> exception;
    if (throwStatementNode.Expression())
    {
        throwStatementNode.Expression()->Accept(*this);
        exception.reset(node.release());
    }
    std::unique_ptr<ThrowStatementNode> clone(new ThrowStatementNode(Span(), exception.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TryStatementNode& tryStatementNode)
{
    tryStatementNode.TryBlock()->Accept(*this);
    std::unique_ptr<TryStatementNode> clone(new TryStatementNode(Span(), static_cast<CompoundStatementNode*>(node.release())));
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        tryStatementNode.Catches()[i]->Accept(*this);
        clone->AddCatch(static_cast<CatchNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(CatchNode& catchNode)
{
    catchNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    std::unique_ptr<IdentifierNode> id;
    if (catchNode.Id())
    {
        catchNode.Id()->Accept(*this);
        id.reset(static_cast<IdentifierNode*>(node.release()));
    }
    catchNode.CatchBlock()->Accept(*this);
    std::unique_ptr<CatchNode> clone(new CatchNode(Span(), typeExpr.release(), id.release(), static_cast<CompoundStatementNode*>(node.release())));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AssertStatementNode& assertStatementNode)
{
    assertStatementNode.AssertExpr()->Accept(*this);
    std::unique_ptr<AssertStatementNode> clone(new AssertStatementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationPartNode& conditionalCompilationPartNode)
{
    std::unique_ptr<ConditionalCompilationExpressionNode> expr;
    if (conditionalCompilationPartNode.Expr())
    {
        conditionalCompilationPartNode.Expr()->Accept(*this);
        expr.reset(static_cast<ConditionalCompilationExpressionNode*>(node.release()));
    }
    std::unique_ptr<ConditionalCompilationPartNode> clone(new ConditionalCompilationPartNode(Span(), expr.release()));
    int n = conditionalCompilationPartNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        conditionalCompilationPartNode.Statements()[i]->Accept(*this);
        clone->AddStatement(static_cast<StatementNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    std::unique_ptr<ConditionalCompilationExpressionNode> left(static_cast<ConditionalCompilationExpressionNode*>(node.release()));
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
    std::unique_ptr<ConditionalCompilationExpressionNode> right(static_cast<ConditionalCompilationExpressionNode*>(node.release()));
    std::unique_ptr<ConditionalCompilationDisjunctionNode> clone(new ConditionalCompilationDisjunctionNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    std::unique_ptr<ConditionalCompilationExpressionNode> left(static_cast<ConditionalCompilationExpressionNode*>(node.release()));
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
    std::unique_ptr<ConditionalCompilationExpressionNode> right(static_cast<ConditionalCompilationExpressionNode*>(node.release()));
    std::unique_ptr<ConditionalCompilationConjunctionNode> clone(new ConditionalCompilationConjunctionNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    conditionalCompilationNotNode.Expr()->Accept(*this);
    std::unique_ptr<ConditionalCompilationNotNode> clone(new ConditionalCompilationNotNode(Span(), static_cast<ConditionalCompilationExpressionNode*>(node.release())));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    node.reset(conditionalCompilationPrimaryNode.Clone(ctx));
}

void ConversionVisitor::Visit(ParenthesizedConditionalCompilationExpressionNode& parenthesizeCondCompExprNode)
{
    parenthesizeCondCompExprNode.Expr()->Accept(*this);
    std::unique_ptr<ParenthesizedConditionalCompilationExpressionNode> clone(new ParenthesizedConditionalCompilationExpressionNode(Span(),
        static_cast<ConditionalCompilationExpressionNode*>(node.release())));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    conditionalCompilationStatementNode.IfPart()->Accept(*this);
    std::unique_ptr<ConditionalCompilationStatementNode> clone(new ConditionalCompilationStatementNode(Span()));
    clone->SetIfPart(static_cast<ConditionalCompilationPartNode*>(node.release()));
    int n = conditionalCompilationStatementNode.ElifParts().Count();
    for (int i = 0; i < n; ++i)
    {
        conditionalCompilationStatementNode.ElifParts()[i]->Accept(*this);
        clone->AddElifPart(static_cast<ConditionalCompilationPartNode*>(node.release()));
    }
    if (conditionalCompilationStatementNode.ElsePart())
    {
        conditionalCompilationStatementNode.ElsePart()->Accept(*this);
        clone->SetElsePart(static_cast<ConditionalCompilationPartNode*>(node.release()));
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TypedefNode& typedefNode)
{
    typedefNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    typedefNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<TypedefNode> clone(new TypedefNode(Span(), typedefNode.GetSpecifiers(), typeExpr.release(), id.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConstantNode& constantNode)
{
    constantNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    constantNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    constantNode.Value()->Accept(*this);
    std::unique_ptr<ConstantNode> clone(new ConstantNode(Span(), constantNode.GetSpecifiers(), typeExpr.release(), id.release(), node.release()));
    clone->SetStrValue(constantNode.StrValue());
    node.reset(clone.release());
}

void ConversionVisitor::Visit(EnumTypeNode& enumTypeNode)
{
    enumTypeNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<Node> underlyingType;
    if (enumTypeNode.GetUnderlyingType())
    {
        enumTypeNode.GetUnderlyingType()->Accept(*this);
        underlyingType.reset(node.release());
    }
    std::unique_ptr<EnumTypeNode> clone(new EnumTypeNode(Span(), enumTypeNode.GetSpecifiers(), id.release()));
    int n = enumTypeNode.Constants().Count();
    for (int i = 0; i < n; ++i)
    {
        enumTypeNode.Constants()[i]->Accept(*this);
        clone->AddConstant(static_cast<EnumConstantNode*>(node.release()));
    }
    if (underlyingType)
    {
        clone->SetUnderlyingType(underlyingType.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(EnumConstantNode& enumConstantNode)
{
    enumConstantNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<Node> value;
    if (enumConstantNode.GetValue())
    {
        enumConstantNode.GetValue()->Accept(*this);
        value.reset(node.release());
    }
    std::unique_ptr<EnumConstantNode> clone(new EnumConstantNode(Span(), id.release(), value.release()));
    if (enumConstantNode.HasValue())
    {
        clone->SetHasValue();
    }
    clone->SetStrValue(enumConstantNode.StrValue());
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ParameterNode& parameterNode)
{
    parameterNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    std::unique_ptr<IdentifierNode> id;
    if (parameterNode.Id())
    {
        parameterNode.Id()->Accept(*this);
        id.reset(static_cast<IdentifierNode*>(node.release()));
    }
    std::unique_ptr<ParameterNode> clone(new ParameterNode(Span(), typeExpr.release(), id.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TemplateParameterNode& templateParameterNode)
{
    templateParameterNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<Node> defaultTemplateArg;
    if (templateParameterNode.DefaultTemplateArgument())
    {
        templateParameterNode.DefaultTemplateArgument()->Accept(*this);
        defaultTemplateArg.reset(node.release());
    }
    std::unique_ptr<TemplateParameterNode> clone(new TemplateParameterNode(Span(), id.release(), defaultTemplateArg.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConstNode& constNode)
{
    constNode.Subject()->Accept(*this);
    std::unique_ptr<ConstNode> clone(new ConstNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(LValueRefNode& lvalueRefNode)
{
    lvalueRefNode.Subject()->Accept(*this);
    std::unique_ptr<LValueRefNode> clone(new LValueRefNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(RValueRefNode& rvalueRefNode)
{
    rvalueRefNode.Subject()->Accept(*this);
    std::unique_ptr<RValueRefNode> clone(new RValueRefNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PointerNode& pointerNode)
{
    pointerNode.Subject()->Accept(*this);
    std::unique_ptr<PointerNode> clone(new PointerNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ArrayNode& arrayNode)
{
    std::unique_ptr<Node> size;
    if (arrayNode.Size())
    {
        arrayNode.Size()->Accept(*this);
        size.reset(node.release());
    }
    arrayNode.Subject()->Accept(*this);
    std::unique_ptr<ArrayNode> clone(new ArrayNode(Span(), node.release(), size.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DotNode& dotNode)
{
    dotNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    dotNode.MemberId()->Accept(*this);
    std::unique_ptr<IdentifierNode> memberId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<DotNode> clone(new DotNode(Span(), subject.release(), memberId.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ArrowNode& arrowNode)
{
    arrowNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    arrowNode.MemberId()->Accept(*this);
    std::unique_ptr<IdentifierNode> memberId(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<ArrowNode> clone(new ArrowNode(Span(), subject.release(), memberId.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(EquivalenceNode& equivalenceNode)
{
    equivalenceNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    equivalenceNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<EquivalenceNode> clone(new EquivalenceNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ImplicationNode& implicationNode)
{
    implicationNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    implicationNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<ImplicationNode> clone(new ImplicationNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DisjunctionNode& disjunctionNode)
{
    disjunctionNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    disjunctionNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<DisjunctionNode> clone(new DisjunctionNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConjunctionNode& conjunctionNode)
{
    conjunctionNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    conjunctionNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<ConjunctionNode> clone(new ConjunctionNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(BitOrNode& bitOrNode)
{
    bitOrNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    bitOrNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<BitOrNode> clone(new BitOrNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(BitXorNode& bitXorNode)
{
    bitXorNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    bitXorNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<BitXorNode> clone(new BitXorNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(BitAndNode& bitAndNode)
{
    bitAndNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    bitAndNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<BitAndNode> clone(new BitAndNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(EqualNode& equalNode)
{
    equalNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    equalNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<EqualNode> clone(new EqualNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(NotEqualNode& notEqualNode)
{
    notEqualNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    notEqualNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<NotEqualNode> clone(new NotEqualNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(LessNode& lessNode)
{
    lessNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    lessNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<LessNode> clone(new LessNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(GreaterNode& greaterNode)
{
    greaterNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    greaterNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<GreaterNode> clone(new GreaterNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(LessOrEqualNode& lessOrEqualNode)
{
    lessOrEqualNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    lessOrEqualNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<LessOrEqualNode> clone(new LessOrEqualNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(GreaterOrEqualNode& greaterOrEqualNode)
{
    greaterOrEqualNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    greaterOrEqualNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<GreaterOrEqualNode> clone(new GreaterOrEqualNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ShiftLeftNode& shiftLeftNode)
{
    shiftLeftNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    shiftLeftNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<ShiftLeftNode> clone(new ShiftLeftNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ShiftRightNode& shiftRightNode)
{
    shiftRightNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    shiftRightNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<ShiftRightNode> clone(new ShiftRightNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AddNode& addNode)
{
    addNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    addNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<AddNode> clone(new AddNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(SubNode& subNode)
{
    subNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    subNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<SubNode> clone(new SubNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(MulNode& mulNode)
{
    mulNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    mulNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<MulNode> clone(new MulNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DivNode& divNode)
{
    divNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    divNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<DivNode> clone(new DivNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(RemNode& remNode)
{
    remNode.Left()->Accept(*this);
    std::unique_ptr<Node> left(node.release());
    remNode.Right()->Accept(*this);
    std::unique_ptr<Node> right(node.release());
    std::unique_ptr<RemNode> clone(new RemNode(Span(), left.release(), right.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(NotNode& notNode)
{
    notNode.Subject()->Accept(*this);
    std::unique_ptr<NotNode> clone(new NotNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(UnaryPlusNode& unaryPlusNode)
{
    unaryPlusNode.Subject()->Accept(*this);
    std::unique_ptr<UnaryPlusNode> clone(new UnaryPlusNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(UnaryMinusNode& unaryMinusNode)
{
    unaryMinusNode.Subject()->Accept(*this);
    std::unique_ptr<UnaryMinusNode> clone(new UnaryMinusNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PrefixIncrementNode& prefixIncrementNode)
{
    prefixIncrementNode.Subject()->Accept(*this);
    std::unique_ptr<PrefixIncrementNode> clone(new PrefixIncrementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PrefixDecrementNode& prefixDecrementNode)
{
    prefixDecrementNode.Subject()->Accept(*this);
    std::unique_ptr<PrefixDecrementNode> clone(new PrefixDecrementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(DerefNode& derefNode)
{
    derefNode.Subject()->Accept(*this);
    std::unique_ptr<DerefNode> clone(new DerefNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AddrOfNode& addrOfNode)
{
    addrOfNode.Subject()->Accept(*this);
    std::unique_ptr<AddrOfNode> clone(new AddrOfNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ComplementNode& complementNode)
{
    complementNode.Subject()->Accept(*this);
    std::unique_ptr<ComplementNode> clone(new ComplementNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(IsNode& isNode)
{
    isNode.Expr()->Accept(*this);
    std::unique_ptr<Node> expr(node.release());
    isNode.TargetTypeExpr()->Accept(*this);
    std::unique_ptr<Node> targetTypeExpr(node.release());
    std::unique_ptr<IsNode> clone(new IsNode(Span(), expr.release(), targetTypeExpr.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(AsNode& asNode)
{
    asNode.Expr()->Accept(*this);
    std::unique_ptr<Node> expr(node.release());
    asNode.TargetTypeExpr()->Accept(*this);
    std::unique_ptr<Node> targetTypeExpr(node.release());
    std::unique_ptr<AsNode> clone(new AsNode(Span(), expr.release(), targetTypeExpr.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(IndexingNode& indexingNode)
{
    indexingNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    indexingNode.Index()->Accept(*this);
    std::unique_ptr<Node> index(node.release());
    std::unique_ptr<IndexingNode> clone(new IndexingNode(Span(), subject.release(), index.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(InvokeNode& invokeNode)
{
    invokeNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    std::unique_ptr<InvokeNode> clone(new InvokeNode(Span(), subject.release()));
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        invokeNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    postfixIncrementNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    std::unique_ptr<PostfixIncrementNode> clone(new PostfixIncrementNode(Span(), subject.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    postfixDecrementNode.Subject()->Accept(*this);
    std::unique_ptr<Node> subject(node.release());
    std::unique_ptr<PostfixDecrementNode> clone(new PostfixDecrementNode(Span(), subject.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(SizeOfNode& sizeOfNode)
{
    sizeOfNode.Expression()->Accept(*this);
    std::unique_ptr<SizeOfNode> clone(new SizeOfNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TypeNameNode& typeNameNode)
{
    typeNameNode.Expression()->Accept(*this);
    std::unique_ptr<TypeNameNode> clone(new TypeNameNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(TypeIdNode& typeIdNode)
{
    typeIdNode.Expression()->Accept(*this);
    std::unique_ptr<TypeIdNode> clone(new TypeIdNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(CastNode& castNode)
{
    castNode.TargetTypeExpr()->Accept(*this);
    std::unique_ptr<Node> targetTypeExpr(node.release());
    castNode.SourceExpr()->Accept(*this);
    std::unique_ptr<Node> sourceExpr(node.release());
    std::unique_ptr<CastNode> clone(new CastNode(Span(), targetTypeExpr.release(), sourceExpr.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ConstructNode& constructNode)
{
    constructNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    std::unique_ptr<ConstructNode> clone(new ConstructNode(Span(), typeExpr.release()));
    int n = constructNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        constructNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(NewNode& newNode)
{
    newNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    std::unique_ptr<NewNode> clone(new NewNode(Span(), typeExpr.release()));
    int n = newNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        newNode.Arguments()[i]->Accept(*this);
        clone->AddArgument(node.release());
    }
    node.reset(clone.release());
}

void ConversionVisitor::Visit(ThisNode& thisNode)
{
    node.reset(thisNode.Clone(ctx));
}

void ConversionVisitor::Visit(BaseNode& baseNode)
{
    node.reset(baseNode.Clone(ctx));
}

void ConversionVisitor::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    parenthesizedExpressionNode.Subject()->Accept(*this);
    std::unique_ptr<ParenthesizedExpressionNode> clone(new ParenthesizedExpressionNode(Span(), node.release()));
    node.reset(clone.release());
}

void ConversionVisitor::Visit(GlobalVariableNode& globalVariableNode)
{
    globalVariableNode.TypeExpr()->Accept(*this);
    std::unique_ptr<Node> typeExpr(node.release());
    globalVariableNode.Id()->Accept(*this);
    std::unique_ptr<IdentifierNode> id(static_cast<IdentifierNode*>(node.release()));
    std::unique_ptr<Node> initializer;
    if (globalVariableNode.Initializer())
    {
        globalVariableNode.Initializer()->Accept(*this);
        initializer.reset(node.release());
    }
    std::unique_ptr<GlobalVariableNode> clone(new GlobalVariableNode(Span(), globalVariableNode.GetSpecifiers(), typeExpr.release(), id.release(), nullptr));
    if (initializer)
    {
        clone->SetInitializer(initializer.release());
    }
    node.reset(clone.release());
}
