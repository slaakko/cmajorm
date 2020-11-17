// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/cmdoclib/SourceCodePrinter.hpp>
#include <cmajor/cmdoclib/Input.hpp>
#include <sngxml/dom/CharacterData.hpp>
#include <sngcm/cmparser/SourceTokenParser.hpp>
#include <sngcm/cmparser/LexerFileTokenParser.hpp>
#include <sngcm/cmparser/ParserFileTokenParser.hpp>
#include <cmajor/symbols/SymbolTable.hpp>
#include <cmajor/symbols/GlobalFlags.hpp>
#include <sngcm/ast/TypeExpr.hpp>
#include <sngcm/ast/Visitor.hpp>
#include <sngcm/ast/BasicType.hpp>
#include <sngcm/ast/Literal.hpp>
#include <sngcm/ast/Expression.hpp>
#include <sngcm/ast/SourceToken.hpp>
#include <soulng/util/Path.hpp>
#include <soulng/util/Unicode.hpp>
#include <soulng/util/CodeFormatter.hpp>
#include <soulng/util/TextUtils.hpp>
#include <boost/filesystem.hpp>
#include <fstream>

namespace cmajor { namespace cmdoclib {

using namespace cmajor::symbols;
using namespace sngcm::ast;
using namespace soulng::util;
using namespace soulng::unicode;

class OperatorMap
{
public:
    std::u32string GetOperatorId(const std::u32string& groupId) const;
    static void Init();
    static void Done();
    static OperatorMap& Instance();
private:
    OperatorMap();
    static std::unique_ptr<OperatorMap> instance;
    std::unordered_map<std::u32string, std::u32string> operatorMap;
};

OperatorMap::OperatorMap()
{
    operatorMap[U"operator<<"] = U"<<";
    operatorMap[U"operator>>"] = U">>";
    operatorMap[U"operator=="] = U"==";
    operatorMap[U"operator="] = U"=";
    operatorMap[U"operator<"] = U"<";
    operatorMap[U"operator->"] = U"->";
    operatorMap[U"operator++"] = U"++";
    operatorMap[U"operator--"] = U"--";
    operatorMap[U"operator+"] = U"+";
    operatorMap[U"operator-"] = U"-";
    operatorMap[U"operator*"] = U"*";
    operatorMap[U"operator/"] = U"/";
    operatorMap[U"operator%"] = U"%";
    operatorMap[U"operator&"] = U"&";
    operatorMap[U"operator|"] = U"|";
    operatorMap[U"operator^"] = U"^";
    operatorMap[U"operator!"] = U"!";
    operatorMap[U"operator~"] = U"~";
    operatorMap[U"operator[]"] = U"[]";
    operatorMap[U"operator()"] = U"()";
}

std::unique_ptr<OperatorMap> OperatorMap::instance;

void OperatorMap::Init()
{
    instance.reset(new OperatorMap());
}

void OperatorMap::Done()
{
    instance.reset();
}

OperatorMap& OperatorMap::Instance()
{
    return *instance;
}

std::u32string OperatorMap::GetOperatorId(const std::u32string& groupId) const
{
    auto it = operatorMap.find(groupId);
    if (it != operatorMap.cend())
    {
        std::u32string op = it->second;
        return op;
    }
    else
    {
        return std::u32string();
    }
}

std::u32string GetOperatorId(const std::u32string& groupId)
{
    return OperatorMap::Instance().GetOperatorId(groupId);
}

void WriteToElement(sngxml::dom::Element* element, const std::u32string& text)
{
    for (char32_t c : text)
    {
        if (c == ' ')
        {
            element->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::EntityReference(U"nbsp")));
        }
        else
        {
            element->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(std::u32string(1, c))));
        }
    }
}

class SourceCodePrinter : public sngcm::ast::Visitor, public sngcm::ast::SourceTokenFormatter
{
public:
    SourceCodePrinter(const std::string& htmlFilePath_, const std::u32string& title_, const std::string& styleFilePath_, const std::vector<std::u32string>& lines_, 
        SymbolTable& symbolTable_, Module& module_, int sourceFileIndex_);

    void WriteDocument();
    void MoveTo(const Span& span);
    void OpenLine();
    void CloseLine();
    void UseInputLine();
    void WriteRestOfInput();
    std::u32string MakeSymbolRef(Symbol* symbol);
    std::u32string MakeSymbolRef(Symbol* symbol, Module* module);
    void WriteSpace(int n);
    void WriteLineNumber(const std::u32string& lineNumberText);
    void WriteLink(const std::u32string& linkText, const std::u32string& href, bool type);
    void WriteFunctionGroupId(const std::u32string& groupId);
    
    void Keyword(const std::u32string& token) override;
    void Identifier(const std::u32string& token) override;
    void Number(const std::u32string& token) override;
    void Char(const std::u32string& token) override;
    void String(const std::u32string& token) override;
    void Spaces(const std::u32string& token) override;
    void Comment(const std::u32string& token) override;
    void Other(const std::u32string& token) override;

    void WriteSpecifiers(const Span& specifierSpan, Specifiers specifiers);
    void WriteAttributes(Attributes* attributes);

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
    void Visit(ArrayLiteralNode& arrayLiteralNode) override;
    void Visit(StructuredLiteralNode& structuredLiteralNode) override;
    void Visit(UuidLiteralNode& uuidLiteralNode) override;

    void Visit(CompileUnitNode& compileUnitNode) override;
    void Visit(NamespaceNode& namespaceNode) override;
    void Visit(AliasNode& aliasNode) override;
    void Visit(NamespaceImportNode& namespaceImportNode) override;
    void Visit(IdentifierNode& identifierNode) override;
    void Visit(TemplateIdNode& templateIdNode) override;
    void Visit(FunctionNode& functionNode) override;
    void Visit(ClassNode& classNode) override;
    void Visit(ThisInitializerNode& thisInitializerNode) override;
    void Visit(BaseInitializerNode& baseInitializerNode) override;
    void Visit(MemberInitializerNode& memberInitializerNode) override;
    void Visit(StaticConstructorNode& staticConstructorNode) override;
    void Visit(ConstructorNode& constructorNode) override;
    void Visit(DestructorNode& destructorNode) override;
    void Visit(MemberFunctionNode& memberFunctionNode) override;
    void Visit(ConversionFunctionNode& conversionFunctionNode) override;
    void Visit(MemberVariableNode& memberVariableNode) override;
    void Visit(InterfaceNode& interfaceNode) override;
    void Visit(DelegateNode& delegateNode) override;
    void Visit(ClassDelegateNode& classDelegateNode) override;

    void Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode) override;
    void Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode) override;
    void Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode) override;
    void Visit(WhereConstraintNode& whereConstraintNode) override;
    void Visit(PredicateConstraintNode& predicateConstraintNode) override;
    void Visit(IsConstraintNode& isConstraintNode) override;
    void Visit(MultiParamConstraintNode& multiParamConstraintNode) override;
    void Visit(TypeNameConstraintNode& typeNameConstraintNode) override;
    void Visit(ConstructorConstraintNode& constructorConstraintNode) override;
    void Visit(DestructorConstraintNode& destructorConstraintNode) override;
    void Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode) override;
    void Visit(FunctionConstraintNode& functionConstraintNode) override;
    void Visit(AxiomStatementNode& axiomStatementNode) override;
    void Visit(AxiomNode& axiomNode) override;
    void Visit(ConceptIdNode& conceptIdNode) override;
    void Visit(ConceptNode& conceptNode) override;

    void Visit(SameConstraintNode& sameConstraintNode) override;
    void Visit(DerivedConstraintNode& derivedConstraintNode) override;
    void Visit(ConvertibleConstraintNode& convertibleConstraintNode) override;
    void Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode) override;
    void Visit(CommonConstraintNode& commonConstraintNode) override;
    void Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode) override;

    void Visit(LabelNode& labelNode) override;
    void Visit(LabeledStatementNode& labeledStatementNode) override;
    void Visit(CompoundStatementNode& compoundStatementNode) override;
    void Visit(ReturnStatementNode& returnStatementNode) override;
    void Visit(IfStatementNode& ifStatementNode) override;
    void Visit(WhileStatementNode& whileStatementNode) override;
    void Visit(DoStatementNode& doStatementNode) override;
    void Visit(ForStatementNode& forStatementNode) override;
    void Visit(BreakStatementNode& breakStatementNode) override;
    void Visit(ContinueStatementNode& continueStatementNode) override;
    void Visit(GotoStatementNode& gotoStatementNode) override;
    void Visit(ConstructionStatementNode& constructionStatementNode) override;
    void Visit(DeleteStatementNode& deleteStatementNode) override;
    void Visit(DestroyStatementNode& destroyStatementNode) override;
    void Visit(AssignmentStatementNode& assignmentStatementNode) override;
    void Visit(ExpressionStatementNode& expressionStatementNode) override;
    void Visit(EmptyStatementNode& emptyStatementNode) override;
    void Visit(RangeForStatementNode& rangeForStatementNode) override;
    void Visit(SwitchStatementNode& switchStatementNode) override;
    void Visit(CaseStatementNode& caseStatementNode) override;
    void Visit(DefaultStatementNode& defaultStatementNode) override;
    void Visit(GotoCaseStatementNode& gotoCaseStatementNode) override;
    void Visit(GotoDefaultStatementNode& gotoDefaultStatementNode) override;
    void Visit(ThrowStatementNode& throwStatementNode) override;
    void Visit(TryStatementNode& tryStatementNode) override;
    void Visit(CatchNode& catchNode) override;
    void Visit(AssertStatementNode& assertStatementNode) override;
    void Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode) override;
    void Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode) override;
    void Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode) override;
    void Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode) override;
    void Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode) override;
    void Visit(ParenthesizedConditionalCompilationExpressionNode& parenthesizedConditionalCompilationExpressionNode) override;

    void Visit(TypedefNode& typedefNode) override;
    void Visit(ConstantNode& constantNode) override;
    void Visit(EnumTypeNode& enumTypeNode) override;
    void Visit(EnumConstantNode& enumConstantNode) override;
    void Visit(ParameterNode& parameterNode) override;
    void Visit(TemplateParameterNode& templateParameterNode) override;

    void Visit(ConstNode& constNode) override;
    void Visit(LValueRefNode& lvalueRefNode) override;
    void Visit(RValueRefNode& rvalueRefNode) override;
    void Visit(PointerNode& pointerNode) override;
    void Visit(ArrayNode& arrayNode) override;

    void Visit(DotNode& dotNode) override;
    void Visit(ArrowNode& arrowNode) override;
    void Visit(EquivalenceNode& equivalenceNode) override;
    void Visit(ImplicationNode& implicationNode) override;
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
    void Visit(CastNode& castNode) override;
    void Visit(ConstructNode& constructNode) override;
    void Visit(NewNode& newNode) override;
    void Visit(ThisNode& thisNode) override;
    void Visit(BaseNode& baseNode) override;
    void Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode) override;
private:
    Input* input;
    std::string htmlFilePath;
    std::u32string title;
    std::string styleFilePath;
    std::unique_ptr<sngxml::dom::Document> htmlDoc;
    std::unique_ptr<sngxml::dom::Element> htmlElement;
    std::unique_ptr<sngxml::dom::Element> bodyElement;
    std::unique_ptr<sngxml::dom::Element> lineElement;
    std::vector<std::u32string> lines;
    const std::vector<int>* lineStarts;
    int numDigits;
    int currentSourceLineNumber;
    int currentPos;
    SymbolTable& symbolTable;
    Module& module;
    int sourceFileIndex;
    FunctionSymbol* invokeFunctionSymbol;
    bool inBlockComment;
};

SourceCodePrinter::SourceCodePrinter(const std::string& htmlFilePath_, const std::u32string& title_, const std::string& styleFilePath_, const std::vector<std::u32string>& lines_, 
    SymbolTable& symbolTable_, Module& module_, int sourceFileIndex_) :
    input(GetInputPtr()), htmlFilePath(htmlFilePath_), title(title_), styleFilePath(styleFilePath_),
    htmlDoc(new sngxml::dom::Document()), htmlElement(new sngxml::dom::Element(U"html")), bodyElement(new sngxml::dom::Element(U"body")), lines(lines_), lineStarts(nullptr), numDigits(Log10(lines.size())), 
    currentSourceLineNumber(1), currentPos(0), symbolTable(symbolTable_), module(module_), sourceFileIndex(sourceFileIndex_), invokeFunctionSymbol(nullptr), inBlockComment(false)
{
    std::unique_ptr<sngxml::dom::Element> headElement(new sngxml::dom::Element(U"head"));
    std::unique_ptr<sngxml::dom::Element> metaElement(new sngxml::dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(metaElement.release()));
    std::unique_ptr<sngxml::dom::Element> titleElement(new sngxml::dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(titleElement.release()));
    std::unique_ptr<sngxml::dom::Element> linkElement(new sngxml::dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(headElement.release()));
}

void SourceCodePrinter::WriteDocument()
{
    std::ofstream htmlFile(htmlFilePath);
    CodeFormatter formatter(htmlFile);
    formatter.SetIndentSize(1);
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bodyElement.release()));
    htmlDoc->AppendChild(std::unique_ptr<sngxml::dom::Node>(htmlElement.release()));
    htmlDoc->Write(formatter);
}

void SourceCodePrinter::MoveTo(const Span& span)
{
    int lineNumber = span.line;
    while (lineNumber > currentSourceLineNumber)
    {
        if (!lineElement)
        {
            UseInputLine();
        }
        else
        {
            CloseLine();
        }
    }
    if (!lineElement)
    {
        OpenLine();
    }
    soulng::lexer::Lexer* lexer = (*module.GetLexers())[sourceFileIndex];
    Span s = span;
    if (s.Valid())
    {
        lexer->ConvertExternal(s);
        if (currentPos < s.start)
        {
            int length = s.start - currentPos;
            WriteSpace(length);
        }
    }
}

void SourceCodePrinter::OpenLine()
{
    lineElement.reset(new sngxml::dom::Element(U"span"));
    lineElement->SetAttribute(U"class", U"code");
    lineElement->SetAttribute(U"xml:space", U"preserve");
    lineElement->SetAttribute(U"id", ToUtf32(std::to_string(currentSourceLineNumber)));
    std::u32string lineNumberText = FormatNumber(currentSourceLineNumber, numDigits);
    WriteLineNumber(lineNumberText);
    WriteSpace(1);
    if (lineStarts->size() < currentSourceLineNumber)
    {
        throw std::runtime_error("invalid line starts vector");
    }
    int lineStart = (*lineStarts)[currentSourceLineNumber - 1];
    currentPos = lineStart;
}

void SourceCodePrinter::CloseLine()
{
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lineElement.release()));
    bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"br")));
    ++currentSourceLineNumber;
}

void SourceCodePrinter::UseInputLine()
{
    const std::u32string& line = lines[currentSourceLineNumber - 1];
    OpenLine();
    sngcm::parser::ParseSourceLine(line, this, inBlockComment);
    CloseLine();
}

void SourceCodePrinter::WriteRestOfInput()
{
    int n = lines.size();
    while (currentSourceLineNumber <= n)
    {
        UseInputLine();
    }
}

std::u32string SourceCodePrinter::MakeSymbolRef(Symbol* symbol)
{
    return MakeSymbolRef(symbol, symbol->GetModule());
}

std::u32string SourceCodePrinter::MakeSymbolRef(Symbol* symbol, Module* module)
{
    if (!symbol) return std::u32string();
    if (symbol->IsParentSymbol())
    {
        if (symbol->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
        {
            ClassTemplateSpecializationSymbol* specialization = static_cast<ClassTemplateSpecializationSymbol*>(symbol);
            return MakeSymbolRef(specialization->GetClassTemplate());
        }
        else
        {
            std::u32string file = symbol->Id();
            std::string contentFilePath = "../doc";
            if (file.empty() || file == U"ns_")
            {
                file = U"index";
                contentFilePath = "..";
            }
            if (module)
            {
                std::string libraryPrefix;
                if (module != symbolTable.GetModule())
                {
                    auto it = input->libraryPrefixMap.find(module->Name());
                    if (it != input->libraryPrefixMap.cend())
                    {
                        libraryPrefix = it->second;
                        if (!libraryPrefix.empty())
                        {
                            libraryPrefix = Path::Combine("..", libraryPrefix);
                        }
                    }
                    else
                    {
                        //throw std::runtime_error("module '" + ToUtf8(module->Name()) + "' not found in source code printer");
                        return std::u32string();
                    }
                    if (file == U"index")
                    {
                        contentFilePath = Path::Combine(Path::Combine("../..", libraryPrefix), ToUtf8(module->Name()));
                    }
                    else
                    {
                        contentFilePath = Path::Combine(Path::Combine("../..", libraryPrefix), ToUtf8(module->Name()) + "/doc");
                    }
                }
            }
            return ToUtf32(Path::Combine(contentFilePath, ToUtf8(file) + ".html"));
        }
    }
    else if (symbol->GetSymbolType() == SymbolType::functionGroupSymbol || symbol->GetSymbolType() == SymbolType::classGroupTypeSymbol ||
        !symbol->Parent() || symbol->Parent()->GetSymbolType() == SymbolType::declarationBlock || symbol->Parent()->IsFunctionSymbol())
    {
        return std::u32string();
    }
    else
    {
        return MakeSymbolRef(symbol->Parent(), module) + U"#" + symbol->Id();
    }
}

void SourceCodePrinter::WriteSpace(int n)
{
    for (int i = 0; i < n; ++i)
    {
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::EntityReference(U"nbsp")));
    }
    currentPos += n;
}

void SourceCodePrinter::WriteLineNumber(const std::u32string& lineNumberText)
{
    std::unique_ptr<sngxml::dom::Element> lineNumberElement(new sngxml::dom::Element(U"span"));
    lineNumberElement->SetAttribute(U"class", U"lineNumber");
    WriteToElement(lineNumberElement.get(), lineNumberText);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lineNumberElement.release()));
    currentPos += lineNumberText.length();
}

void SourceCodePrinter::WriteLink(const std::u32string& linkText, const std::u32string& href, bool type)
{
    if (href.empty())
    {
        std::unique_ptr<sngxml::dom::Element> identifierElement(new sngxml::dom::Element(U"span"));
        if (type)
        {
            identifierElement->SetAttribute(U"class", U"type");
        }
        else
        {
            identifierElement->SetAttribute(U"class", U"identifier");
        }
        WriteToElement(identifierElement.get(), linkText);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(identifierElement.release()));
        currentPos += linkText.length();
    }
    else
    {
        std::unique_ptr<sngxml::dom::Element> linkElement(new sngxml::dom::Element(U"a"));
        linkElement->SetAttribute(U"href", href);
        WriteToElement(linkElement.get(), linkText);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkElement.release()));
        currentPos += linkText.length();
    }
}

void SourceCodePrinter::WriteFunctionGroupId(const std::u32string& groupId)
{
    std::u32string operatorId = GetOperatorId(groupId);
    if (operatorId.empty())
    {
        Identifier(groupId);
    }
    else
    {
        Keyword(U"operator");
        Other(operatorId);
    }
}

/*
void SourceCodePrinter::WriteType(TypeSymbol* type)
{
    if (type->IsBasicTypeSymbol())
    {
        Keyword(type->CodeName());
    }
    else if (type->GetSymbolType() == SymbolType::derivedTypeSymbol)
    {
        DerivedTypeSymbol* derivedTypeSymbol = static_cast<DerivedTypeSymbol*>(type);
        const std::vector<Derivation>& derivations = derivedTypeSymbol->DerivationRec().derivations;
        if (HasFrontConstDerivation(derivations))
        {
            Keyword(U"const");
            WriteSpace(1);
        }
        WriteType(derivedTypeSymbol->BaseType());
        for (Derivation derivation : derivations)
        {
            switch (derivation)
            {
                case Derivation::lvalueRefDerivation:
                {
                    Other(U"&");
                    break;
                }
                case Derivation::rvalueRefDerivation:
                {
                    Other(U"&&");
                    break;
                }
                case Derivation::pointerDerivation:
                {
                    Other(U"*");
                    break;
                }
            }
        }
    }
    else if (type->GetSymbolType() == SymbolType::classTemplateSpecializationSymbol)
    {
        ClassTemplateSpecializationSymbol* classTemplateSpecializationSymbol = static_cast<ClassTemplateSpecializationSymbol*>(type);
        TypeSymbol* primaryType = classTemplateSpecializationSymbol->GetClassTemplate();
        WriteType(primaryType);
        Other(U"<");
        int n = classTemplateSpecializationSymbol->TemplateArgumentTypes().size();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            TypeSymbol* templateArgumentSymbol = classTemplateSpecializationSymbol->TemplateArgumentTypes()[i];
            WriteType(templateArgumentSymbol);
        }
        Other(U">");
    }
    else
    {
        WriteLink(type->CodeName(), MakeSymbolRef(type), true);
    }
}
*/

void SourceCodePrinter::Keyword(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"span"));
    keywordElement->SetAttribute(U"class", U"kw");
    WriteToElement(keywordElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::Identifier(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> identifierElement(new sngxml::dom::Element(U"span"));
    identifierElement->SetAttribute(U"class", U"identifier");
    WriteToElement(identifierElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(identifierElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::Number(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> numberElement(new sngxml::dom::Element(U"span"));
    numberElement->SetAttribute(U"class", U"number");
    WriteToElement(numberElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(numberElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::Char(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> charElement(new sngxml::dom::Element(U"span"));
    charElement->SetAttribute(U"class", U"char");
    WriteToElement(charElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(charElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::String(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> stringElement(new sngxml::dom::Element(U"span"));
    stringElement->SetAttribute(U"class", U"string");
    WriteToElement(stringElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(stringElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::Spaces(const std::u32string& token)
{
    WriteSpace(token.length());
}

void SourceCodePrinter::Comment(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> commentElement(new sngxml::dom::Element(U"span"));
    commentElement->SetAttribute(U"class", U"comment");
    WriteToElement(commentElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(commentElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::Other(const std::u32string& token)
{
    std::unique_ptr<sngxml::dom::Element> otherElement(new sngxml::dom::Element(U"span"));
    otherElement->SetAttribute(U"class", U"other");
    WriteToElement(otherElement.get(), token);
    lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(otherElement.release()));
    currentPos += token.length();
}

void SourceCodePrinter::WriteSpecifiers(const Span& specifierSpan, Specifiers specifiers)
{
    if (specifiers == Specifiers::none) return;
    MoveTo(specifierSpan);
    bool space = false;
    if ((specifiers & Specifiers::public_) != Specifiers::none)
    {
        Keyword(U"public");
        space = true;
    }
    else if ((specifiers & Specifiers::protected_) != Specifiers::none)
    {
        Keyword(U"protected");
        space = true;
    }
    else if ((specifiers & Specifiers::private_) != Specifiers::none)
    {
        Keyword(U"private");
        space = true;
    }
    else if ((specifiers & Specifiers::internal_) != Specifiers::none)
    {
        Keyword(U"internal");
        space = true;
    }
    if ((specifiers & Specifiers::static_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"static");
        space = true;
    }
    if ((specifiers & Specifiers::virtual_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"virtual");
        space = true;
    }
    else if ((specifiers & Specifiers::override_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"override");
        space = true;
    }
    else if ((specifiers & Specifiers::abstract_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"abstract");
        space = true;
    }
    if ((specifiers & Specifiers::inline_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"inline");
        space = true;
    }
    if ((specifiers & Specifiers::explicit_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"explicit");
        space = true;
    }
    if ((specifiers & Specifiers::external_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"extern");
        space = true;
    }
    if ((specifiers & Specifiers::suppress_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"suppress");
        space = true;
    }
    if ((specifiers & Specifiers::default_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"default");
        space = true;
    }
    if ((specifiers & Specifiers::constexpr_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"constexpr");
        space = true;
    }
    if ((specifiers & Specifiers::cdecl_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"cdecl");
        space = true;
    }
    if ((specifiers & Specifiers::nothrow_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"nothrow");
        space = true;
    }
    if ((specifiers & Specifiers::throw_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"throw");
        space = true;
    }
    if ((specifiers & Specifiers::new_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"new");
        space = true;
    }
    if ((specifiers & Specifiers::unit_test_) != Specifiers::none)
    {
        if (space)
        {
            WriteSpace(1);
        }
        Keyword(U"unit_test");
        space = true;
    }
}

void SourceCodePrinter::WriteAttributes(Attributes* attributes)
{
    Other(U"[");
    int n = attributes->GetAttributes().size();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        Attribute* attribute = attributes->GetAttributes()[i].get();
        MoveTo(attribute->GetSpan());
        Identifier(attribute->Name());
        if (!attribute->Value().empty())
        {
            Other(U"=");
            String(U"\"" + StringStr(attribute->Value()) + U"\"");
        }
    }
    Other(U"]");
}

void SourceCodePrinter::Visit(BoolNode& boolNode)
{
    MoveTo(boolNode.GetSpan());
    Keyword(U"bool");
}

void SourceCodePrinter::Visit(SByteNode& sbyteNode)
{
    MoveTo(sbyteNode.GetSpan());
    Keyword(U"sbyte");
}

void SourceCodePrinter::Visit(ByteNode& byteNode)
{
    MoveTo(byteNode.GetSpan());
    Keyword(U"byte");
}

void SourceCodePrinter::Visit(ShortNode& shortNode)
{
    MoveTo(shortNode.GetSpan());
    Keyword(U"short");
}

void SourceCodePrinter::Visit(UShortNode& ushortNode)
{
    MoveTo(ushortNode.GetSpan());
    Keyword(U"ushort");
}

void SourceCodePrinter::Visit(IntNode& intNode)
{
    MoveTo(intNode.GetSpan());
    Keyword(U"int");
}

void SourceCodePrinter::Visit(UIntNode& uintNode)
{
    MoveTo(uintNode.GetSpan());
    Keyword(U"uint");
}

void SourceCodePrinter::Visit(LongNode& longNode)
{
    MoveTo(longNode.GetSpan());
    Keyword(U"long");
}

void SourceCodePrinter::Visit(ULongNode& ulongNode)
{
    MoveTo(ulongNode.GetSpan());
    Keyword(U"ulong");
}

void SourceCodePrinter::Visit(FloatNode& floatNode)
{
    MoveTo(floatNode.GetSpan());
    Keyword(U"float");
}

void SourceCodePrinter::Visit(DoubleNode& doubleNode)
{
    MoveTo(doubleNode.GetSpan());
    Keyword(U"double");
}

void SourceCodePrinter::Visit(CharNode& charNode)
{
    MoveTo(charNode.GetSpan());
    Keyword(U"char");
}

void SourceCodePrinter::Visit(WCharNode& wcharNode)
{
    MoveTo(wcharNode.GetSpan());
    Keyword(U"wchar");
}

void SourceCodePrinter::Visit(UCharNode& ucharNode)
{
    MoveTo(ucharNode.GetSpan());
    Keyword(U"uchar");
}

void SourceCodePrinter::Visit(VoidNode& voidNode)
{
    MoveTo(voidNode.GetSpan());
    Keyword(U"void");
}

void SourceCodePrinter::Visit(BooleanLiteralNode& booleanLiteralNode)
{
    MoveTo(booleanLiteralNode.GetSpan());
    Keyword(booleanLiteralNode.Text()); 
}

void SourceCodePrinter::Visit(SByteLiteralNode& sbyteLiteralNode)
{
    MoveTo(sbyteLiteralNode.GetSpan());
    Number(sbyteLiteralNode.Text());
}

void SourceCodePrinter::Visit(ByteLiteralNode& byteLiteralNode)
{
    MoveTo(byteLiteralNode.GetSpan());
    Number(byteLiteralNode.Text());
}

void SourceCodePrinter::Visit(ShortLiteralNode& shortLiteralNode)
{
    MoveTo(shortLiteralNode.GetSpan());
    Number(shortLiteralNode.Text());
}

void SourceCodePrinter::Visit(UShortLiteralNode& ushortLiteralNode)
{
    MoveTo(ushortLiteralNode.GetSpan());
    Number(ushortLiteralNode.Text());
}

void SourceCodePrinter::Visit(IntLiteralNode& intLiteralNode)
{
    MoveTo(intLiteralNode.GetSpan());
    Number(intLiteralNode.Text());
}

void SourceCodePrinter::Visit(UIntLiteralNode& uintLiteralNode)
{
    MoveTo(uintLiteralNode.GetSpan());
    Number(uintLiteralNode.Text());
}

void SourceCodePrinter::Visit(LongLiteralNode& longLiteralNode)
{
    MoveTo(longLiteralNode.GetSpan());
    Number(longLiteralNode.Text());
}

void SourceCodePrinter::Visit(ULongLiteralNode& ulongLiteralNode)
{
    MoveTo(ulongLiteralNode.GetSpan());
    Number(ulongLiteralNode.Text());
}

void SourceCodePrinter::Visit(FloatLiteralNode& floatLiteralNode)
{
    MoveTo(floatLiteralNode.GetSpan());
    Number(floatLiteralNode.Text());
}

void SourceCodePrinter::Visit(DoubleLiteralNode& doubleLiteralNode)
{
    MoveTo(doubleLiteralNode.GetSpan());
    Number(doubleLiteralNode.Text());
}

void SourceCodePrinter::Visit(CharLiteralNode& charLiteralNode)
{
    MoveTo(charLiteralNode.GetSpan());
    Char(charLiteralNode.Text());
}

void SourceCodePrinter::Visit(WCharLiteralNode& wcharLiteralNode)
{
    MoveTo(wcharLiteralNode.GetSpan());
    Char(wcharLiteralNode.Text());
}

void SourceCodePrinter::Visit(UCharLiteralNode& ucharLiteralNode)
{
    MoveTo(ucharLiteralNode.GetSpan());
    Char(ucharLiteralNode.Text());
}

void SourceCodePrinter::Visit(StringLiteralNode& stringLiteralNode)
{
    MoveTo(stringLiteralNode.GetSpan());
    String(stringLiteralNode.Text());
}

void SourceCodePrinter::Visit(WStringLiteralNode& wstringLiteralNode)
{
    MoveTo(wstringLiteralNode.GetSpan());
    String(wstringLiteralNode.Text());
}

void SourceCodePrinter::Visit(UStringLiteralNode& ustringLiteralNode)
{
    MoveTo(ustringLiteralNode.GetSpan());
    String(ustringLiteralNode.Text());
}

void SourceCodePrinter::Visit(NullLiteralNode& nullLiteralNode)
{
    MoveTo(nullLiteralNode.GetSpan());
    Keyword(nullLiteralNode.Text());
}

void SourceCodePrinter::Visit(ArrayLiteralNode& arrayLiteralNode)
{
    MoveTo(arrayLiteralNode.GetSpan());
    Other(arrayLiteralNode.Text());
}

void SourceCodePrinter::Visit(StructuredLiteralNode& structuredLiteralNode)
{
    MoveTo(structuredLiteralNode.GetSpan());
    Other(structuredLiteralNode.Text());
}

void SourceCodePrinter::Visit(UuidLiteralNode& uuidLiteralNode)
{
    MoveTo(uuidLiteralNode.GetSpan());
    Other(uuidLiteralNode.Text());
}

void SourceCodePrinter::Visit(CompileUnitNode& compileUnitNode)
{
    lineStarts = &compileUnitNode.LineStarts();
    compileUnitNode.GlobalNs()->Accept(*this); 
    WriteRestOfInput();
}

void SourceCodePrinter::Visit(NamespaceNode& namespaceNode)
{
    int n = namespaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* node = namespaceNode.Members()[i];
        node->Accept(*this);
    }
}

void SourceCodePrinter::Visit(AliasNode& aliasNode)
{
    MoveTo(aliasNode.GetSpan());
    Keyword(U"using");
    WriteSpace(1);
    aliasNode.Id()->Accept(*this);
    Other(U" = ");
    aliasNode.Qid()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(NamespaceImportNode& namespaceImportNode)
{
    MoveTo(namespaceImportNode.GetSpan());
    Keyword(U"using");
    WriteSpace(1);
    namespaceImportNode.Ns()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(IdentifierNode& identifierNode)
{
    MoveTo(identifierNode.GetSpan());
    FunctionSymbol* invokedFunctionSymbol = symbolTable.GetInvoke(&identifierNode);
    if (invokedFunctionSymbol)
    {
        WriteLink(identifierNode.Str(), MakeSymbolRef(invokedFunctionSymbol), false);
    }
    else
    {
        Symbol* symbol = symbolTable.GetMappedSymbol(&identifierNode);
        if (symbol)
        {
            WriteLink(identifierNode.Str(), MakeSymbolRef(symbol), symbol->IsTypeSymbol());
        }
        else
        {
            Identifier(identifierNode.Str());
        }
    }
}

void SourceCodePrinter::Visit(TemplateIdNode& templateIdNode)
{
    MoveTo(templateIdNode.GetSpan());
    templateIdNode.Primary()->Accept(*this);
    Other(U"<");
    int n = templateIdNode.TemplateArguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        Node* arg = templateIdNode.TemplateArguments()[i];
        arg->Accept(*this);
    }
    Other(U">");
}

void SourceCodePrinter::Visit(FunctionNode& functionNode)
{
    MoveTo(functionNode.GetSpan());
    if (functionNode.GetAttributes())
    {
        WriteAttributes(functionNode.GetAttributes());
    }
    WriteSpecifiers(functionNode.SpecifierSpan(), functionNode.GetSpecifiers());
    if (functionNode.ReturnTypeExpr())
    {
        functionNode.ReturnTypeExpr()->Accept(*this);
    }
    MoveTo(functionNode.GroupIdSpan());
    Symbol* symbol = symbolTable.GetMappedSymbol(&functionNode);
    if (symbol)
    {
        WriteLink(functionNode.GroupId(), MakeSymbolRef(symbol), false);
    }
    else
    {
        WriteFunctionGroupId(functionNode.GroupId());
    }
    int nt = functionNode.TemplateParameters().Count();
    if (nt > 0)
    {
        Other(U"<");
        for (int i = 0; i < nt; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            functionNode.TemplateParameters()[i]->Accept(*this);
        }
        Other(U">");
    }
    Other(U"(");
    int n = functionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        functionNode.Parameters()[i]->Accept(*this);
    }
    Other(U")");
    if (functionNode.WhereConstraint())
    {
        functionNode.WhereConstraint()->Accept(*this);
    }
    if (functionNode.Body())
    {
        functionNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(ClassNode& classNode)
{
    MoveTo(classNode.GetSpan());
    if (classNode.GetAttributes())
    {
        WriteAttributes(classNode.GetAttributes());
    }
    WriteSpecifiers(classNode.SpecifierSpan(), classNode.GetSpecifiers());
    MoveTo(classNode.ClassSpan());
    Keyword(U"class");
    classNode.Id()->Accept(*this);
    int nt = classNode.TemplateParameters().Count();
    if (nt > 0)
    {
        Other(U"<");
        for (int i = 0; i < nt; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            classNode.TemplateParameters()[i]->Accept(*this);
        }
        Other(U">");
    }
    int nb = classNode.BaseClassOrInterfaces().Count();
    if (nb > 0)
    {
        Other(U" : ");
        for (int i = 0; i < nb; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            Node* baseClassOrInterface = classNode.BaseClassOrInterfaces()[i];
            baseClassOrInterface->Accept(*this);
        }
    }
    if (classNode.WhereConstraint())
    {
        classNode.WhereConstraint()->Accept(*this);
    }
    MoveTo(classNode.BeginBraceSpan());
    Other(U"{");
    int n = classNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* node = classNode.Members()[i];
        node->Accept(*this);
    }
    MoveTo(classNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(ThisInitializerNode& thisInitializerNode)
{
    MoveTo(thisInitializerNode.GetSpan());
    Keyword(U"this");
    Other(U"(");
    int n = thisInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        Node* arg = thisInitializerNode.Arguments()[i];
        arg->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(BaseInitializerNode& baseInitializerNode)
{
    MoveTo(baseInitializerNode.GetSpan());
    Keyword(U"base");
    Other(U"(");
    int n = baseInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        Node* arg = baseInitializerNode.Arguments()[i];
        arg->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(MemberInitializerNode& memberInitializerNode)
{
    MoveTo(memberInitializerNode.GetSpan());
    memberInitializerNode.MemberId()->Accept(*this);
    Other(U"(");
    int n = memberInitializerNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        Node* arg = memberInitializerNode.Arguments()[i];
        arg->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(StaticConstructorNode& staticConstructorNode)
{
    MoveTo(staticConstructorNode.GetSpan());
    if (staticConstructorNode.GetAttributes())
    {
        WriteAttributes(staticConstructorNode.GetAttributes());
    }
    WriteSpecifiers(staticConstructorNode.SpecifierSpan(), staticConstructorNode.GetSpecifiers());
    staticConstructorNode.ClassId()->Accept(*this);
    Other(U"()");
    int n = staticConstructorNode.Initializers().Count();
    if (n > 0)
    {
        Other(U" : ");
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            Node* initializer = staticConstructorNode.Initializers()[i];
            initializer->Accept(*this);
        }
    }
    if (staticConstructorNode.WhereConstraint())
    {
        staticConstructorNode.WhereConstraint()->Accept(*this);
    }
    if (staticConstructorNode.Body())
    {
        staticConstructorNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(ConstructorNode& constructorNode)
{
    MoveTo(constructorNode.GetSpan());
    if (constructorNode.GetAttributes())
    {
        WriteAttributes(constructorNode.GetAttributes());
    }
    WriteSpecifiers(constructorNode.SpecifierSpan(), constructorNode.GetSpecifiers());
    constructorNode.ClassId()->Accept(*this);
    Other(U"(");
    int n = constructorNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = constructorNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U")");
    int ni = constructorNode.Initializers().Count();
    if (ni > 0)
    {
        Other(U" : ");
        for (int i = 0; i < ni; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            Node* initializer = constructorNode.Initializers()[i];
            initializer->Accept(*this);
        }
    }
    if (constructorNode.WhereConstraint())
    {
        constructorNode.WhereConstraint()->Accept(*this);
    }
    if (constructorNode.Body())
    {
        constructorNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(DestructorNode& destructorNode)
{
    MoveTo(destructorNode.GetSpan());
    if (destructorNode.GetAttributes())
    {
        WriteAttributes(destructorNode.GetAttributes());
    }
    WriteSpecifiers(destructorNode.SpecifierSpan(), destructorNode.GetSpecifiers());
    if (destructorNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Other(U"~");
    destructorNode.ClassId()->Accept(*this);
    Other(U"(");
    Other(U")");
    if (destructorNode.WhereConstraint())
    {
        destructorNode.WhereConstraint()->Accept(*this);
    }
    if (destructorNode.Body())
    {
        destructorNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(MemberFunctionNode& memberFunctionNode)
{
    MoveTo(memberFunctionNode.GetSpan());
    if (memberFunctionNode.GetAttributes())
    {
        WriteAttributes(memberFunctionNode.GetAttributes());
    }
    WriteSpecifiers(memberFunctionNode.SpecifierSpan(), memberFunctionNode.GetSpecifiers());
    memberFunctionNode.ReturnTypeExpr()->Accept(*this);
    MoveTo(memberFunctionNode.GroupIdSpan());
    Symbol* symbol = symbolTable.GetMappedSymbol(&memberFunctionNode);
    if (symbol)
    {
        WriteLink(memberFunctionNode.GroupId(), MakeSymbolRef(symbol), false);
    }
    else
    {
        WriteFunctionGroupId(memberFunctionNode.GroupId());
    }
    Other(U"(");
    int n = memberFunctionNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = memberFunctionNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U")");
    if ((memberFunctionNode.GetSpecifiers() & Specifiers::const_) != Specifiers::none)
    {
        WriteSpace(1);
        Keyword(U"const");
    }
    if (memberFunctionNode.WhereConstraint())
    {
        memberFunctionNode.WhereConstraint()->Accept(*this);
    }
    if (memberFunctionNode.Body())
    {
        memberFunctionNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(ConversionFunctionNode& conversionFunctionNode)
{
    MoveTo(conversionFunctionNode.GetSpan());
    if (conversionFunctionNode.GetAttributes())
    {
        WriteAttributes(conversionFunctionNode.GetAttributes());
    }
    WriteSpecifiers(conversionFunctionNode.SpecifierSpan(), conversionFunctionNode.GetSpecifiers());
    if (conversionFunctionNode.GetSpecifiers() != Specifiers::none && conversionFunctionNode.GetSpecifiers() != Specifiers::const_)
    {
        WriteSpace(1);
    }
    Keyword(U"operator");
    conversionFunctionNode.ReturnTypeExpr()->Accept(*this);
    Other(U"(");
    Other(U")");
    if ((conversionFunctionNode.GetSpecifiers() & Specifiers::const_) != Specifiers::none)
    {
        WriteSpace(1);
        Keyword(U"const");
    }
    if (conversionFunctionNode.WhereConstraint())
    {
        conversionFunctionNode.WhereConstraint()->Accept(*this);
    }
    if (conversionFunctionNode.Body())
    {
        conversionFunctionNode.Body()->Accept(*this);
    }
    else
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(MemberVariableNode& memberVariableNode)
{
    MoveTo(memberVariableNode.GetSpan());
    if (memberVariableNode.GetAttributes())
    {
        WriteAttributes(memberVariableNode.GetAttributes());
    }
    WriteSpecifiers(memberVariableNode.SpecifierSpan(), memberVariableNode.GetSpecifiers());
    memberVariableNode.TypeExpr()->Accept(*this);
    memberVariableNode.Id()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(InterfaceNode& interfaceNode)
{
    MoveTo(interfaceNode.GetSpan());
    if (interfaceNode.GetAttributes())
    {
        WriteAttributes(interfaceNode.GetAttributes());
    }
    WriteSpecifiers(interfaceNode.SpecifierSpan(), interfaceNode.GetSpecifiers());
    if (interfaceNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"interface");
    interfaceNode.Id()->Accept(*this);
    MoveTo(interfaceNode.BeginBraceSpan());
    Other(U"{");
    int n = interfaceNode.Members().Count();
    for (int i = 0; i < n; ++i)
    {
        Node* member = interfaceNode.Members()[i];
        member->Accept(*this);
    }
    MoveTo(interfaceNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(DelegateNode& delegateNode)
{
    MoveTo(delegateNode.GetSpan());
    WriteSpecifiers(delegateNode.GetSpan(), delegateNode.GetSpecifiers());
    if (delegateNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"delegate");
    delegateNode.ReturnTypeExpr()->Accept(*this);
    delegateNode.Id()->Accept(*this);
    Other(U"(");
    int n = delegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = delegateNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U");");
}

void SourceCodePrinter::Visit(ClassDelegateNode& classDelegateNode)
{
    MoveTo(classDelegateNode.GetSpan());
    WriteSpecifiers(classDelegateNode.GetSpan(), classDelegateNode.GetSpecifiers());
    if (classDelegateNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"class");
    WriteSpace(1);
    Keyword(U"delegate");
    classDelegateNode.ReturnTypeExpr()->Accept(*this);
    classDelegateNode.Id()->Accept(*this);
    Other(U"(");
    int n = classDelegateNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = classDelegateNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U");");
}

void SourceCodePrinter::Visit(ParenthesizedConstraintNode& parenthesizedConstraintNode)
{
    MoveTo(parenthesizedConstraintNode.GetSpan());
    Other(U"(");
    parenthesizedConstraintNode.Constraint()->Accept(*this);
    Other(U")");
}

void SourceCodePrinter::Visit(DisjunctiveConstraintNode& disjunctiveConstraintNode)
{
    MoveTo(disjunctiveConstraintNode.GetSpan());
    disjunctiveConstraintNode.Left()->Accept(*this);
    Keyword(U" or ");
    disjunctiveConstraintNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ConjunctiveConstraintNode& conjunctiveConstraintNode)
{
    MoveTo(conjunctiveConstraintNode.GetSpan());
    conjunctiveConstraintNode.Left()->Accept(*this);
    Keyword(U" and ");
    conjunctiveConstraintNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(WhereConstraintNode& whereConstraintNode)
{
    MoveTo(whereConstraintNode.GetSpan());
    Keyword(U"where");
    whereConstraintNode.Constraint()->Accept(*this);
    if (whereConstraintNode.Semicolon())
    {
        Other(U";");
    }
}

void SourceCodePrinter::Visit(PredicateConstraintNode& predicateConstraintNode)
{
    MoveTo(predicateConstraintNode.GetSpan());
    predicateConstraintNode.InvokeExpr()->Accept(*this);
}

void SourceCodePrinter::Visit(IsConstraintNode& isConstraintNode)
{
    MoveTo(isConstraintNode.GetSpan());
    isConstraintNode.TypeExpr()->Accept(*this);
    Keyword(U" is ");
    isConstraintNode.ConceptOrTypeName()->Accept(*this);
}

void SourceCodePrinter::Visit(MultiParamConstraintNode& multiParamConstraintNode)
{
    MoveTo(multiParamConstraintNode.GetSpan());
    multiParamConstraintNode.ConceptId()->Accept(*this);
    Other(U"<");
    int n = multiParamConstraintNode.TypeExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        multiParamConstraintNode.TypeExprs()[i]->Accept(*this);
    }
    Other(U">");
}

void SourceCodePrinter::Visit(TypeNameConstraintNode& typeNameConstraintNode)
{
    MoveTo(typeNameConstraintNode.GetSpan());
    Keyword(U"typename");
    typeNameConstraintNode.TypeId()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(ConstructorConstraintNode& constructorConstraintNode)
{
    MoveTo(constructorConstraintNode.GetSpan());
    constructorConstraintNode.TypeParamId()->Accept(*this);
    Other(U"(");
    int n = constructorConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = constructorConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U");");
}

void SourceCodePrinter::Visit(DestructorConstraintNode& destructorConstraintNode)
{
    MoveTo(destructorConstraintNode.GetSpan());
    Other(U"~");
    destructorConstraintNode.TypeParamId()->Accept(*this);
    Other(U"(");
    Other(U");");
}

void SourceCodePrinter::Visit(MemberFunctionConstraintNode& memberFunctionConstraintNode)
{
    MoveTo(memberFunctionConstraintNode.GetSpan());
    memberFunctionConstraintNode.ReturnTypeExpr()->Accept(*this);
    memberFunctionConstraintNode.TypeParamId()->Accept(*this);
    Other(U".");
    WriteFunctionGroupId(memberFunctionConstraintNode.GroupId());
    Other(U"(");
    int n = memberFunctionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = memberFunctionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U");");
}
 
void SourceCodePrinter::Visit(FunctionConstraintNode& functionConstraintNode)
{
    MoveTo(functionConstraintNode.GetSpan());
    functionConstraintNode.ReturnTypeExpr()->Accept(*this);
    WriteSpace(1);
    WriteFunctionGroupId(functionConstraintNode.GroupId());
    Other(U"(");
    int n = functionConstraintNode.Parameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        ParameterNode* parameter = functionConstraintNode.Parameters()[i];
        parameter->Accept(*this);
    }
    Other(U");");
}

void SourceCodePrinter::Visit(AxiomStatementNode& axiomStatementNode)
{
    MoveTo(axiomStatementNode.GetSpan());
    axiomStatementNode.Expression()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(AxiomNode& axiomNode)
{
    MoveTo(axiomNode.GetSpan());
    Keyword(U"axiom");
    if (axiomNode.Id())
    {
        axiomNode.Id()->Accept(*this);
    }
    int n = axiomNode.Parameters().Count();
    if (n > 0)
    {
        Other(U"(");
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            ParameterNode* parameter = axiomNode.Parameters()[i];
            parameter->Accept(*this);
        }
        Other(U")");
    }
    MoveTo(axiomNode.BeginBraceSpan());
    Other(U"{");
    int ns = axiomNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        AxiomStatementNode* axiomStatement = axiomNode.Statements()[i];
        axiomStatement->Accept(*this);
    }
    MoveTo(axiomNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(ConceptIdNode& conceptIdNode)
{
    MoveTo(conceptIdNode.GetSpan());
    conceptIdNode.Id()->Accept(*this);
    Other(U"<");
    int n = conceptIdNode.TypeParameters().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        conceptIdNode.TypeParameters()[i]->Accept(*this);
    }
    Other(U">");
}

void SourceCodePrinter::Visit(ConceptNode& conceptNode)
{
    MoveTo(conceptNode.GetSpan());
    WriteSpecifiers(conceptNode.GetSpan(), conceptNode.GetSpecifiers());
    if (conceptNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"concept");
    conceptNode.Id()->Accept(*this);
    Other(U"<");
    int nt = conceptNode.TypeParameters().Count();
    for (int i = 0; i < nt; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        conceptNode.TypeParameters()[i]->Accept(*this);
    }
    Other(U">");
    if (conceptNode.Refinement())
    {
        Other(U" : ");
        conceptNode.Refinement()->Accept(*this);
    }
    int nc = conceptNode.Constraints().Count();
    for (int i = 0; i < nc; ++i)
    {
        if (conceptNode.Constraints()[i]->IsHeaderConstraint())
        {
            conceptNode.Constraints()[i]->Accept(*this);
        }
    }
    MoveTo(conceptNode.BeginBraceSpan());
    Other(U"{");
    for (int i = 0; i < nc; ++i)
    {
        if (!conceptNode.Constraints()[i]->IsHeaderConstraint())
        {
            conceptNode.Constraints()[i]->Accept(*this);
        }
    }
    int na = conceptNode.Axioms().Count();
    for (int i = 0; i < na; ++i)
    {
        conceptNode.Axioms()[i]->Accept(*this);
    }
    MoveTo(conceptNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(SameConstraintNode& sameConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(DerivedConstraintNode& derivedConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(ConvertibleConstraintNode& convertibleConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(ExplicitlyConvertibleConstraintNode& explicitlyConvertibleConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(CommonConstraintNode& commonConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(NonreferenceTypeConstraintNode& nonreferenceTypeConstraintNode)
{
    // todo
}

void SourceCodePrinter::Visit(LabelNode& labelNode)
{
    MoveTo(labelNode.GetSpan());
    Identifier(labelNode.Label());
    Other(U": ");
}

void SourceCodePrinter::Visit(LabeledStatementNode& labeledStatementNode)
{
    labeledStatementNode.Label()->Accept(*this);
    labeledStatementNode.Stmt()->Accept(*this);
}

void SourceCodePrinter::Visit(CompoundStatementNode& compoundStatementNode)
{
    MoveTo(compoundStatementNode.GetSpan());
    MoveTo(compoundStatementNode.BeginBraceSpan());
    Other(U"{");
    int n = compoundStatementNode.Statements().Count();
    for (int i = 0; i < n; ++i)
    {
        StatementNode* statement = compoundStatementNode.Statements()[i];
        statement->Accept(*this);
    }
    MoveTo(compoundStatementNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(ReturnStatementNode& returnStatementNode)
{
    MoveTo(returnStatementNode.GetSpan());
    Keyword(U"return");
    if (returnStatementNode.Expression())
    {
        returnStatementNode.Expression()->Accept(*this);
    }
    Other(U";");
}

void SourceCodePrinter::Visit(IfStatementNode& ifStatementNode)
{
    MoveTo(ifStatementNode.GetSpan());
    Keyword(U"if");
    MoveTo(ifStatementNode.LeftParenSpan());

    Other(U"(");
    ifStatementNode.Condition()->Accept(*this);
    MoveTo(ifStatementNode.RightParenSpan());
    Other(U")");
    ifStatementNode.ThenS()->Accept(*this);
    if (ifStatementNode.ElseS())
    {
        MoveTo(ifStatementNode.ElseSpan());
        Keyword(U"else");
        ifStatementNode.ElseS()->Accept(*this);
    }
}

void SourceCodePrinter::Visit(WhileStatementNode& whileStatementNode)
{
    MoveTo(whileStatementNode.GetSpan());
    Keyword(U"while");
    MoveTo(whileStatementNode.LeftParenSpan());
    Other(U"(");
    whileStatementNode.Condition()->Accept(*this);
    MoveTo(whileStatementNode.RightParenSpan());
    Other(U")");
    whileStatementNode.Statement()->Accept(*this);
}

void SourceCodePrinter::Visit(DoStatementNode& doStatementNode)
{
    MoveTo(doStatementNode.GetSpan());
    Keyword(U"do");
    doStatementNode.Statement()->Accept(*this);
    MoveTo(doStatementNode.WhileSpan());
    Keyword(U"while");
    MoveTo(doStatementNode.LeftParenSpan());
    Other(U"(");
    doStatementNode.Condition()->Accept(*this);
    MoveTo(doStatementNode.RightParenSpan());
    Other(U");");
}

void SourceCodePrinter::Visit(ForStatementNode& forStatementNode)
{
    MoveTo(forStatementNode.GetSpan());
    Keyword(U"for");
    MoveTo(forStatementNode.LeftParenSpan());
    Other(U"(");
    forStatementNode.InitS()->Accept(*this);
    if (forStatementNode.Condition())
    {
        forStatementNode.Condition()->Accept(*this);
    }
    Other(U";");
    forStatementNode.LoopS()->Accept(*this);
    MoveTo(forStatementNode.RightParenSpan());
    Other(U")");
    forStatementNode.ActionS()->Accept(*this);
}

void SourceCodePrinter::Visit(BreakStatementNode& breakStatementNode)
{
    MoveTo(breakStatementNode.GetSpan());
    Keyword(U"break");
    Other(U";");
}

void SourceCodePrinter::Visit(ContinueStatementNode& continueStatementNode)
{
    MoveTo(continueStatementNode.GetSpan());
    Keyword(U"continue");
    Other(U";");
}

void SourceCodePrinter::Visit(GotoStatementNode& gotoStatementNode)
{
    MoveTo(gotoStatementNode.GetSpan());
    Keyword(U"goto");
    WriteSpace(1);
    Identifier(gotoStatementNode.Target());
    Other(U";");
}

void SourceCodePrinter::Visit(ConstructionStatementNode& constructionStatementNode)
{
    MoveTo(constructionStatementNode.GetSpan());
    constructionStatementNode.TypeExpr()->Accept(*this);
    constructionStatementNode.Id()->Accept(*this);
    if (constructionStatementNode.Assignment())
    {
        Other(U" = ");
        constructionStatementNode.Arguments()[0]->Accept(*this);
    }
    else if (!constructionStatementNode.Empty())
    {
        Other(U"(");
        int n = constructionStatementNode.Arguments().Count();
        for (int i = 0; i < n; ++i)
        {
            if (i > 0)
            {
                Other(U", ");
            }
            constructionStatementNode.Arguments()[i]->Accept(*this);
        }
        Other(U")");
    }
    Other(U";");
}

void SourceCodePrinter::Visit(DeleteStatementNode& deleteStatementNode)
{
    MoveTo(deleteStatementNode.GetSpan());
    Keyword(U"delete");
    deleteStatementNode.Expression()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(DestroyStatementNode& destroyStatementNode)
{
    MoveTo(destroyStatementNode.GetSpan());
    Keyword(U"destroy");
    destroyStatementNode.Expression()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(AssignmentStatementNode& assignmentStatementNode)
{
    MoveTo(assignmentStatementNode.GetSpan());
    assignmentStatementNode.TargetExpr()->Accept(*this);
    Other(U" = ");
    assignmentStatementNode.SourceExpr()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(ExpressionStatementNode& expressionStatementNode)
{
    MoveTo(expressionStatementNode.GetSpan());
    expressionStatementNode.Expression()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(EmptyStatementNode& emptyStatementNode)
{
    MoveTo(emptyStatementNode.GetSpan());
    Other(U";");
}

void SourceCodePrinter::Visit(RangeForStatementNode& rangeForStatementNode)
{
    MoveTo(rangeForStatementNode.GetSpan());
    Keyword(U"for");
    MoveTo(rangeForStatementNode.LeftParenSpan());
    Other(U"(");
    rangeForStatementNode.TypeExpr()->Accept(*this);
    rangeForStatementNode.Id()->Accept(*this);
    MoveTo(rangeForStatementNode.ColonSpan());
    Other(U":");
    rangeForStatementNode.Container()->Accept(*this);
    MoveTo(rangeForStatementNode.RightParenSpan());
    Other(U")");
    rangeForStatementNode.Action()->Accept(*this);
}

void SourceCodePrinter::Visit(SwitchStatementNode& switchStatementNode)
{
    MoveTo(switchStatementNode.GetSpan());
    Keyword(U"switch");
    MoveTo(switchStatementNode.LeftParenSpan());
    Other(U"(");
    switchStatementNode.Condition()->Accept(*this);
    MoveTo(switchStatementNode.RightParenSpan());
    Other(U")");
    MoveTo(switchStatementNode.BeginBraceSpan());
    Other(U"{");
    int nc = switchStatementNode.Cases().Count();
    for (int i = 0; i < nc; ++i)
    {
        CaseStatementNode* caseStatement = switchStatementNode.Cases()[i];
        caseStatement->Accept(*this);
    }
    if (switchStatementNode.Default())
    {
        switchStatementNode.Default()->Accept(*this);
    }
    MoveTo(switchStatementNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(CaseStatementNode& caseStatementNode)
{
    MoveTo(caseStatementNode.GetSpan());
    int n = caseStatementNode.CaseExprs().Count();
    for (int i = 0; i < n; ++i)
    {
        MoveTo(caseStatementNode.CaseSpans()[i]);
        Keyword(U"case");
        caseStatementNode.CaseExprs()[i]->Accept(*this);
        Other(U":");
    }
    int ns = caseStatementNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        caseStatementNode.Statements()[i]->Accept(*this);
    }
}

void SourceCodePrinter::Visit(DefaultStatementNode& defaultStatementNode)
{
    MoveTo(defaultStatementNode.GetSpan());
    Keyword(U"default");
    Other(U":");
    int ns = defaultStatementNode.Statements().Count();
    for (int i = 0; i < ns; ++i)
    {
        defaultStatementNode.Statements()[i]->Accept(*this);
    }
}

void SourceCodePrinter::Visit(GotoCaseStatementNode& gotoCaseStatementNode)
{
    MoveTo(gotoCaseStatementNode.GetSpan());
    Keyword(U"goto");
    WriteSpace(1);
    Keyword(U"case");
    gotoCaseStatementNode.CaseExpr()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(GotoDefaultStatementNode& gotoDefaultStatementNode)
{
    MoveTo(gotoDefaultStatementNode.GetSpan());
    Keyword(U"goto");
    WriteSpace(1);
    Keyword(U"default");
    Other(U";");
}

void SourceCodePrinter::Visit(ThrowStatementNode& throwStatementNode)
{
    MoveTo(throwStatementNode.GetSpan());
    Keyword(U"throw");
    if (throwStatementNode.Expression())
    {
        throwStatementNode.Expression()->Accept(*this);
    }
    Other(U";");
}

void SourceCodePrinter::Visit(TryStatementNode& tryStatementNode)
{
    MoveTo(tryStatementNode.GetSpan());
    Keyword(U"try");
    tryStatementNode.TryBlock()->Accept(*this);
    int n = tryStatementNode.Catches().Count();
    for (int i = 0; i < n; ++i)
    {
        tryStatementNode.Catches()[i]->Accept(*this);
    }
}

void SourceCodePrinter::Visit(CatchNode& catchNode)
{
    MoveTo(catchNode.GetSpan());
    Keyword(U"catch");
    MoveTo(catchNode.LeftParenSpan());
    Other(U"(");
    catchNode.TypeExpr()->Accept(*this);
    if (catchNode.Id())
    {
        catchNode.Id()->Accept(*this);
    }
    MoveTo(catchNode.RightParenSpan());
    Other(U")");
    catchNode.CatchBlock()->Accept(*this);
}

void SourceCodePrinter::Visit(AssertStatementNode& assertStatementNode)
{
    MoveTo(assertStatementNode.GetSpan());
    Keyword(U"#assert");
    assertStatementNode.AssertExpr()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(ConditionalCompilationDisjunctionNode& conditionalCompilationDisjunctionNode)
{
    MoveTo(conditionalCompilationDisjunctionNode.GetSpan());
    conditionalCompilationDisjunctionNode.Left()->Accept(*this);
    Other(U" || ");
    conditionalCompilationDisjunctionNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ConditionalCompilationConjunctionNode& conditionalCompilationConjunctionNode)
{
    MoveTo(conditionalCompilationConjunctionNode.GetSpan());
    conditionalCompilationConjunctionNode.Left()->Accept(*this);
    Other(U" && ");
    conditionalCompilationConjunctionNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ConditionalCompilationNotNode& conditionalCompilationNotNode)
{
    MoveTo(conditionalCompilationNotNode.GetSpan());
    Other(U"!");
    conditionalCompilationNotNode.Expr()->Accept(*this);
}

void SourceCodePrinter::Visit(ConditionalCompilationPrimaryNode& conditionalCompilationPrimaryNode)
{
    MoveTo(conditionalCompilationPrimaryNode.GetSpan());
    Identifier(conditionalCompilationPrimaryNode.Symbol());
}

void SourceCodePrinter::Visit(ParenthesizedConditionalCompilationExpressionNode& parenthesizedConditionalCompilationExpressionNode)
{
    MoveTo(parenthesizedConditionalCompilationExpressionNode.GetSpan());
    Other(U"(");
    parenthesizedConditionalCompilationExpressionNode.Expr()->Accept(*this);
    Other(U")");
}

void SourceCodePrinter::Visit(ConditionalCompilationStatementNode& conditionalCompilationStatementNode)
{
    MoveTo(conditionalCompilationStatementNode.IfPart()->KeywordSpan());
    Keyword(U"#if");
    MoveTo(conditionalCompilationStatementNode.IfPart()->LeftParenSpan());
    Other(U"(");
    conditionalCompilationStatementNode.IfPart()->Expr()->Accept(*this);
    MoveTo(conditionalCompilationStatementNode.IfPart()->RightParenSpan());
    Other(U")");
    int ni = conditionalCompilationStatementNode.IfPart()->Statements().Count();
    for (int i = 0; i < ni; ++i)
    {
        conditionalCompilationStatementNode.IfPart()->Statements()[i]->Accept(*this);
    }
    int nep = conditionalCompilationStatementNode.ElifParts().Count();
    for (int i = 0; i < nep; ++i)
    {
        ConditionalCompilationPartNode* elifPart = conditionalCompilationStatementNode.ElifParts()[i];
        MoveTo(elifPart->KeywordSpan());
        Keyword(U"#elif");
        MoveTo(elifPart->LeftParenSpan());
        Other(U"(");
        elifPart->Expr()->Accept(*this);
        MoveTo(elifPart->RightParenSpan());
        Other(U")");
        int ns = elifPart->Statements().Count();
        for (int i = 0; i < ns; ++i)
        {
            elifPart->Statements()[i]->Accept(*this);
        }
    }
    if (conditionalCompilationStatementNode.ElsePart())
    {
        MoveTo(conditionalCompilationStatementNode.ElsePart()->KeywordSpan());
        Keyword(U"#else");
        int ns = conditionalCompilationStatementNode.ElsePart()->Statements().Count();
        for (int i = 0; i < ns; ++i)
        {
            conditionalCompilationStatementNode.ElsePart()->Statements()[i]->Accept(*this);
        }
    }
    MoveTo(conditionalCompilationStatementNode.EndIfSpan());
    Keyword(U"#endif");
}

void SourceCodePrinter::Visit(TypedefNode& typedefNode)
{
    MoveTo(typedefNode.GetSpan());
    WriteSpecifiers(typedefNode.GetSpan(), typedefNode.GetSpecifiers());
    if (typedefNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"typedef");
    typedefNode.TypeExpr()->Accept(*this);
    typedefNode.Id()->Accept(*this);
    Other(U";");
}

void SourceCodePrinter::Visit(ConstantNode& constantNode)
{
    MoveTo(constantNode.GetSpan());
    WriteSpecifiers(constantNode.GetSpan(), constantNode.GetSpecifiers());
    if (constantNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"const");
    constantNode.TypeExpr()->Accept(*this);
    constantNode.Id()->Accept(*this);
    if (constantNode.Value())
    {
        Other(U" = ");
        constantNode.Value()->Accept(*this);
    }
    Other(U";");
}

void SourceCodePrinter::Visit(EnumTypeNode& enumTypeNode)
{
    MoveTo(enumTypeNode.GetSpan());
    WriteSpecifiers(enumTypeNode.GetSpan(), enumTypeNode.GetSpecifiers());
    if (enumTypeNode.GetSpecifiers() != Specifiers::none)
    {
        WriteSpace(1);
    }
    Keyword(U"enum");
    enumTypeNode.Id()->Accept(*this);
    if (enumTypeNode.GetUnderlyingType())
    {
        Other(U" : ");
        enumTypeNode.GetUnderlyingType()->Accept(*this);
    }
    MoveTo(enumTypeNode.BeginBraceSpan());
    Other(U"{");
    int n = enumTypeNode.Constants().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        EnumConstantNode* constant = enumTypeNode.Constants()[i];
        constant->Accept(*this);
    }
    MoveTo(enumTypeNode.EndBraceSpan());
    Other(U"}");
}

void SourceCodePrinter::Visit(EnumConstantNode& enumConstantNode)
{
    MoveTo(enumConstantNode.GetSpan());
    enumConstantNode.Id()->Accept(*this);
    if (enumConstantNode.HasValue())
    {
        Other(U" = ");
        enumConstantNode.GetValue()->Accept(*this);
    }
}

void SourceCodePrinter::Visit(ParameterNode& parameterNode)
{
    MoveTo(parameterNode.GetSpan());
    parameterNode.TypeExpr()->Accept(*this);
    if (parameterNode.Id() && !parameterNode.ArtificialId())
    {
        parameterNode.Id()->Accept(*this);
    }
}

void SourceCodePrinter::Visit(TemplateParameterNode& templateParameterNode)
{
    MoveTo(templateParameterNode.GetSpan());
    templateParameterNode.Id()->Accept(*this);
    if (templateParameterNode.DefaultTemplateArgument())
    {
        Other(U" = ");
        templateParameterNode.DefaultTemplateArgument()->Accept(*this);
    }
}

void SourceCodePrinter::Visit(ConstNode& constNode)
{
    MoveTo(constNode.GetSpan());
    Keyword(U"const");
    WriteSpace(1);
    constNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(LValueRefNode& lvalueRefNode)
{
    MoveTo(lvalueRefNode.GetSpan());
    lvalueRefNode.Subject()->Accept(*this);
    Other(U"&");
}

void SourceCodePrinter::Visit(RValueRefNode& rvalueRefNode)
{
    MoveTo(rvalueRefNode.GetSpan());
    rvalueRefNode.Subject()->Accept(*this);
    Other(U"&&");
}

void SourceCodePrinter::Visit(PointerNode& pointerNode)
{
    MoveTo(pointerNode.GetSpan());
    pointerNode.Subject()->Accept(*this);
    Other(U"*");
}

void SourceCodePrinter::Visit(ArrayNode& arrayNode)
{
    MoveTo(arrayNode.GetSpan());
    arrayNode.Subject()->Accept(*this);
    Other(U"[");
    if (arrayNode.Size())
    {
        arrayNode.Size()->Accept(*this);
    }
    Other(U"]");
}

void SourceCodePrinter::Visit(DotNode& dotNode)
{
    MoveTo(dotNode.GetSpan());
    dotNode.Subject()->Accept(*this);
    Other(U".");
    dotNode.MemberId()->Accept(*this);
}

void SourceCodePrinter::Visit(ArrowNode& arrowNode)
{
    MoveTo(arrowNode.GetSpan());
    arrowNode.Subject()->Accept(*this);
    Other(U"->");
    arrowNode.MemberId()->Accept(*this);
}

void SourceCodePrinter::Visit(EquivalenceNode& equivalenceNode)
{
    MoveTo(equivalenceNode.GetSpan());
    equivalenceNode.Left()->Accept(*this);
    Other(U" <=> ");
    equivalenceNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ImplicationNode& implicationNode)
{
    MoveTo(implicationNode.GetSpan());
    implicationNode.Left()->Accept(*this);
    Other(U" => ");
    implicationNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(DisjunctionNode& disjunctionNode)
{
    MoveTo(disjunctionNode.GetSpan());
    disjunctionNode.Left()->Accept(*this);
    Other(U" || ");
    disjunctionNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ConjunctionNode& conjunctionNode)
{
    MoveTo(conjunctionNode.GetSpan());
    conjunctionNode.Left()->Accept(*this);
    Other(U" && ");
    conjunctionNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(BitOrNode& bitOrNode)
{
    MoveTo(bitOrNode.GetSpan());
    bitOrNode.Left()->Accept(*this);
    Other(U" | ");
    bitOrNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(BitXorNode& bitXorNode)
{
    MoveTo(bitXorNode.GetSpan());
    bitXorNode.Left()->Accept(*this);
    Other(U" ^ ");
    bitXorNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(BitAndNode& bitAndNode)
{
    MoveTo(bitAndNode.GetSpan());
    bitAndNode.Left()->Accept(*this);
    Other(U" & ");
    bitAndNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(EqualNode& equalNode)
{
    MoveTo(equalNode.GetSpan());
    equalNode.Left()->Accept(*this);
    Other(U" == ");
    equalNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(NotEqualNode& notEqualNode)
{
    MoveTo(notEqualNode.GetSpan());
    notEqualNode.Left()->Accept(*this);
    Other(U" != ");
    notEqualNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(LessNode& lessNode)
{
    MoveTo(lessNode.GetSpan());
    lessNode.Left()->Accept(*this);
    Other(U" < ");
    lessNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(GreaterNode& greaterNode)
{
    MoveTo(greaterNode.GetSpan());
    greaterNode.Left()->Accept(*this);
    Other(U" > ");
    greaterNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(LessOrEqualNode& lessOrEqualNode)
{
    MoveTo(lessOrEqualNode.GetSpan());
    lessOrEqualNode.Left()->Accept(*this);
    Other(U" <= ");
    lessOrEqualNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(GreaterOrEqualNode& greaterOrEqualNode)
{
    MoveTo(greaterOrEqualNode.GetSpan());
    greaterOrEqualNode.Left()->Accept(*this);
    Other(U" >= ");
    greaterOrEqualNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ShiftLeftNode& shiftLeftNode)
{
    MoveTo(shiftLeftNode.GetSpan());
    shiftLeftNode.Left()->Accept(*this);
    Other(U" << ");
    shiftLeftNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(ShiftRightNode& shiftRightNode)
{
    MoveTo(shiftRightNode.GetSpan());
    shiftRightNode.Left()->Accept(*this);
    Other(U" >> ");
    shiftRightNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(AddNode& addNode)
{
    MoveTo(addNode.GetSpan());
    addNode.Left()->Accept(*this);
    Other(U" + ");
    addNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(SubNode& subNode)
{
    MoveTo(subNode.GetSpan());
    subNode.Left()->Accept(*this);
    Other(U" - ");
    subNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(MulNode& mulNode)
{
    MoveTo(mulNode.GetSpan());
    mulNode.Left()->Accept(*this);
    Other(U" * ");
    mulNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(DivNode& divNode)
{
    MoveTo(divNode.GetSpan());
    divNode.Left()->Accept(*this);
    Other(U" / ");
    divNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(RemNode& remNode)
{
    MoveTo(remNode.GetSpan());
    remNode.Left()->Accept(*this);
    Other(U" % ");
    remNode.Right()->Accept(*this);
}

void SourceCodePrinter::Visit(NotNode& notNode)
{
    MoveTo(notNode.GetSpan());
    Other(U"!");
    notNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(UnaryPlusNode& unaryPlusNode)
{
    MoveTo(unaryPlusNode.GetSpan());
    Other(U"+");
    unaryPlusNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(UnaryMinusNode& unaryMinusNode)
{
    MoveTo(unaryMinusNode.GetSpan());
    Other(U"-");
    unaryMinusNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(PrefixIncrementNode& prefixIncrementNode)
{
    MoveTo(prefixIncrementNode.GetSpan());
    Other(U"++");
    prefixIncrementNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(PrefixDecrementNode& prefixDecrementNode)
{
    MoveTo(prefixDecrementNode.GetSpan());
    Other(U"--");
    prefixDecrementNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(DerefNode& derefNode)
{
    MoveTo(derefNode.GetSpan());
    Other(U"*");
    derefNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(AddrOfNode& addrOfNode)
{
    MoveTo(addrOfNode.GetSpan());
    Other(U"&");
    addrOfNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(ComplementNode& complementNode)
{
    MoveTo(complementNode.GetSpan());
    Other(U"~");
    complementNode.Subject()->Accept(*this);
}

void SourceCodePrinter::Visit(IsNode& isNode)
{
    MoveTo(isNode.GetSpan());
    isNode.Expr()->Accept(*this);
    Keyword(U" is ");
    isNode.TargetTypeExpr()->Accept(*this);
}

void SourceCodePrinter::Visit(AsNode& asNode)
{
    MoveTo(asNode.GetSpan());
    asNode.Expr()->Accept(*this);
    Keyword(U" as ");
    asNode.TargetTypeExpr()->Accept(*this);
}

void SourceCodePrinter::Visit(IndexingNode& indexingNode)
{
    MoveTo(indexingNode.GetSpan());
    indexingNode.Subject()->Accept(*this);
    Other(U"[");
    indexingNode.Index()->Accept(*this);
    Other(U"]");
}

void SourceCodePrinter::Visit(InvokeNode& invokeNode)
{
    MoveTo(invokeNode.GetSpan());
    invokeNode.Subject()->Accept(*this);
    Other(U"(");
    int n = invokeNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        invokeNode.Arguments()[i]->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(PostfixIncrementNode& postfixIncrementNode)
{
    MoveTo(postfixIncrementNode.GetSpan());
    postfixIncrementNode.Subject()->Accept(*this);
    Other(U"++");
}

void SourceCodePrinter::Visit(PostfixDecrementNode& postfixDecrementNode)
{
    MoveTo(postfixDecrementNode.GetSpan());
    postfixDecrementNode.Subject()->Accept(*this);
    Other(U"--");
}

void SourceCodePrinter::Visit(SizeOfNode& sizeOfNode)
{
    MoveTo(sizeOfNode.GetSpan());
    Keyword(U"sizeof");
    Other(U"(");
    sizeOfNode.Expression()->Accept(*this);
    Other(U")");
    
}

void SourceCodePrinter::Visit(TypeNameNode& typeNameNode)
{
    MoveTo(typeNameNode.GetSpan());
    Keyword(U"typename");
    Other(U"(");
    typeNameNode.Expression()->Accept(*this);
    Other(U")");
}

void SourceCodePrinter::Visit(CastNode& castNode)
{
    MoveTo(castNode.GetSpan());
    Keyword(U"cast");
    Other(U"<");
    castNode.TargetTypeExpr()->Accept(*this);
    Other(U">");
    Other(U"(");
    castNode.SourceExpr()->Accept(*this);
    Other(U")");
}

void SourceCodePrinter::Visit(ConstructNode& constructNode)
{
    MoveTo(constructNode.GetSpan());
    Keyword(U"construct");
    Other(U"<");
    constructNode.TypeExpr()->Accept(*this);
    Other(U">");
    Other(U"(");
    int n = constructNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        constructNode.Arguments()[i]->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(NewNode& newNode)
{
    MoveTo(newNode.GetSpan());
    Keyword(U"new");
    WriteSpace(1);
    newNode.TypeExpr()->Accept(*this);
    Other(U"(");
    int n = newNode.Arguments().Count();
    for (int i = 0; i < n; ++i)
    {
        if (i > 0)
        {
            Other(U", ");
        }
        newNode.Arguments()[i]->Accept(*this);
    }
    Other(U")");
}

void SourceCodePrinter::Visit(ThisNode& thisNode)
{
    MoveTo(thisNode.GetSpan());
    Keyword(U"this");
}

void SourceCodePrinter::Visit(BaseNode& baseNode)
{
    MoveTo(baseNode.GetSpan());
    Keyword(U"base");
}

void SourceCodePrinter::Visit(ParenthesizedExpressionNode& parenthesizedExpressionNode)
{
    MoveTo(parenthesizedExpressionNode.GetSpan());
    Other(U"(");
    parenthesizedExpressionNode.Subject()->Accept(*this);
    Other(U")");
}

bool HtmlSourceFilePathsUpToDate(sngcm::ast::Project* project)
{
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    std::string projectDir = GetFullPath(Path::Combine(contentDir, ToUtf8(project->Name())));
    std::string fileDir = GetFullPath(Path::Combine(projectDir, "file"));
    for (const std::string& sourceFilePath : project->SourceFilePaths())
    {
        std::string fileName = Path::GetFileName(sourceFilePath);
        std::string htmlFileName = Path::ChangeExtension(fileName, ".cm.html");
        std::string htmlFilePath = Path::Combine(fileDir, htmlFileName);
        if (!boost::filesystem::exists(htmlFilePath) || boost::filesystem::last_write_time(htmlFilePath) < boost::filesystem::last_write_time(sourceFilePath))
        {
            return false;
        }
    }
    return true;
}

void GenerateSourceCode(sngcm::ast::Project* project, cmajor::binder::BoundCompileUnit* boundCompileUnit, std::unordered_map<int, File>& fileMap)
{
    std::string fileName = Path::GetFileName(boundCompileUnit->GetCompileUnitNode()->FilePath());
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    boost::filesystem::create_directories(contentDir);
    std::string projectDir = GetFullPath(Path::Combine(contentDir, ToUtf8(project->Name())));
    boost::filesystem::create_directories(projectDir);
    std::string fileDir = GetFullPath(Path::Combine(projectDir, "file"));
    boost::filesystem::create_directories(fileDir);
    std::string htmlFileName = Path::ChangeExtension(fileName, ".cm.html");
    std::string htmlFilePath = Path::Combine(fileDir, htmlFileName);
    std::u32string title = project->Name();
    title.append(1, '/').append(ToUtf32(fileName));
    File file;
    file.name = title;
    file.htmlFilePath = Path::Combine(Path::Combine(Path::Combine("../..", ToUtf8(project->Name())), "file"), htmlFileName);
    fileMap[boundCompileUnit->GetCompileUnitNode()->GetSpan().fileIndex] = file;
    std::vector<std::u32string> lines;
    std::ifstream inputFile(boundCompileUnit->GetCompileUnitNode()->FilePath());
    std::string line;
    while (std::getline(inputFile, line))
    {
        lines.push_back(ToUtf32(line));
    }
    std::string styleFilePath = "../../../style/style.css";
    SourceCodePrinter printer(htmlFilePath, title, styleFilePath, lines, boundCompileUnit->GetSymbolTable(), boundCompileUnit->GetModule(),
        boundCompileUnit->GetCompileUnitNode()->GetSpan().fileIndex);
    boundCompileUnit->GetCompileUnitNode()->Accept(printer);
    printer.WriteDocument();
}

struct BasicSourcePrinter : public sngcm::ast::SourceTokenFormatter
{
    void Keyword(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> keywordElement(new sngxml::dom::Element(U"span"));
        keywordElement->SetAttribute(U"class", U"kw");
        WriteToElement(keywordElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(keywordElement.release()));
    }
    void Identifier(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> identifierElement(new sngxml::dom::Element(U"span"));
        identifierElement->SetAttribute(U"class", U"identifier");
        WriteToElement(identifierElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(identifierElement.release()));
    }
    void Number(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> numberElement(new sngxml::dom::Element(U"span"));
        numberElement->SetAttribute(U"class", U"number");
        WriteToElement(numberElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(numberElement.release()));
    }
    void Char(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> charElement(new sngxml::dom::Element(U"span"));
        charElement->SetAttribute(U"class", U"char");
        WriteToElement(charElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(charElement.release()));
    }
    void String(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> stringElement(new sngxml::dom::Element(U"span"));
        stringElement->SetAttribute(U"class", U"string");
        WriteToElement(stringElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(stringElement.release()));
    }
    void Spaces(const std::u32string& token) override
    {
        WriteSpace(token.length());
    }
    void Comment(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> commentElement(new sngxml::dom::Element(U"span"));
        commentElement->SetAttribute(U"class", U"comment");
        WriteToElement(commentElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(commentElement.release()));
    }
    void Other(const std::u32string& token) override
    {
        std::unique_ptr<sngxml::dom::Element> otherElement(new sngxml::dom::Element(U"span"));
        otherElement->SetAttribute(U"class", U"other");
        WriteToElement(otherElement.get(), token);
        lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(otherElement.release()));
    }
    void WriteSpace(int n)
    {
        for (int i = 0; i < n; ++i)
        {
            lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::EntityReference(U"nbsp")));
        }
    }
    std::unique_ptr<sngxml::dom::Element> lineElement;
};

struct LexerLineParser
{
    static void Parse(const std::u32string& line, sngcm::ast::SourceTokenFormatter* formatter, bool& inBlockComment)
    {
        sngcm::parser::ParseLexerFileLine(line, formatter, inBlockComment);
    }
};

struct ParserLineParser
{
    static void Parse(const std::u32string& line, sngcm::ast::SourceTokenFormatter* formatter, bool& inBlockComment)
    {
        sngcm::parser::ParseParserFileLine(line, formatter, inBlockComment);
    }
};

template<class LineParser>
struct SourcePrinter : public BasicSourcePrinter
{
    SourcePrinter(sngxml::dom::Element* bodyElement_) : inBlockComment(false), bodyElement(bodyElement_)
    {
    }
    void PrintLines(const std::vector<std::u32string>& lines)
    {
        int n = lines.size();
        int numDigits = Log10(n);
        for (int i = 0; i < n; ++i)
        {
            lineElement.reset(new sngxml::dom::Element(U"span"));
            lineElement->SetAttribute(U"class", U"code");
            lineElement->SetAttribute(U"xml:space", U"preserve");
            lineElement->SetAttribute(U"id", ToUtf32(std::to_string(i + 1)));
            std::u32string lineNumberText = FormatNumber(i + 1, numDigits);
            std::unique_ptr<sngxml::dom::Element> lineNumberElement(new sngxml::dom::Element(U"span"));
            lineNumberElement->SetAttribute(U"class", U"lineNumber");
            WriteToElement(lineNumberElement.get(), lineNumberText);
            lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(lineNumberElement.release()));
            lineElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::EntityReference(U"nbsp")));
            LineParser::Parse(lines[i], this, inBlockComment);
            bodyElement->AppendChild(std::move(lineElement));
            bodyElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Element(U"br")));
        }
    }
    bool inBlockComment;
    sngxml::dom::Element* bodyElement;
};

template<class LineParser>
void GenerateHtmlSource(sngcm::ast::Project* project, const std::string& filePath, int fileIndex, std::unordered_map<int, File>& fileMap)
{
    std::string fileName = Path::GetFileName(filePath);
    Input* input = GetInputPtr();
    std::string targetDir = input->targetDirPath;
    boost::filesystem::create_directories(targetDir);
    std::string contentDir = GetFullPath(Path::Combine(targetDir, "content"));
    boost::filesystem::create_directories(contentDir);
    std::string projectDir = GetFullPath(Path::Combine(contentDir, ToUtf8(project->Name())));
    boost::filesystem::create_directories(projectDir);
    std::string fileDir = GetFullPath(Path::Combine(projectDir, "file"));
    boost::filesystem::create_directories(fileDir);
    std::string extension = Path::GetExtension(filePath);
    std::string htmlFileName = Path::ChangeExtension(fileName, extension + ".html");
    std::string htmlFilePath = Path::Combine(fileDir, htmlFileName);
    std::u32string title = project->Name();
    title.append(1, '/').append(ToUtf32(fileName));
    File file;
    file.name = title;
    file.htmlFilePath = Path::Combine(Path::Combine(Path::Combine("../..", ToUtf8(project->Name())), "file"), htmlFileName);
    fileMap[fileIndex] = file;
    std::vector<std::u32string> lines;
    std::ifstream inputFile(filePath);
    std::string line;
    while (std::getline(inputFile, line))
    {
        lines.push_back(ToUtf32(line));
    }
    std::string styleFilePath = "../../../style/style.css";

    std::unique_ptr<sngxml::dom::Document> htmlDoc(new sngxml::dom::Document());
    std::unique_ptr<sngxml::dom::Element> htmlElement(new sngxml::dom::Element(U"html"));
    std::unique_ptr<sngxml::dom::Element> headElement(new sngxml::dom::Element(U"head"));
    std::unique_ptr<sngxml::dom::Element> metaElement(new sngxml::dom::Element(U"meta"));
    metaElement->SetAttribute(U"charset", U"utf-8");
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(metaElement.release()));
    std::unique_ptr<sngxml::dom::Element> titleElement(new sngxml::dom::Element(U"title"));
    titleElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(new sngxml::dom::Text(title)));
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(titleElement.release()));
    std::unique_ptr<sngxml::dom::Element> linkElement(new sngxml::dom::Element(U"link"));
    linkElement->SetAttribute(U"rel", U"stylesheet");
    linkElement->SetAttribute(U"type", U"text/css");
    std::u32string relativeStyleFilePath = ToUtf32(styleFilePath);
    linkElement->SetAttribute(U"href", relativeStyleFilePath);
    headElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(linkElement.release()));
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(headElement.release()));
    std::unique_ptr<sngxml::dom::Element> bodyElement(new sngxml::dom::Element(U"body"));
    SourcePrinter<LineParser> printer(bodyElement.get());
    printer.PrintLines(lines);
    std::ofstream htmlFile(htmlFilePath);
    CodeFormatter formatter(htmlFile);
    formatter.SetIndentSize(1);
    htmlElement->AppendChild(std::unique_ptr<sngxml::dom::Node>(bodyElement.release()));
    htmlDoc->AppendChild(std::unique_ptr<sngxml::dom::Node>(htmlElement.release()));
    htmlDoc->Write(formatter);
}

CMDOCLIB_API void GenerateLexerAndParserHtmlSources(sngcm::ast::Project* project, int maxFileIndex, std::unordered_map<int, File>& fileMap)
{
    for (const std::string& textFilePath : project->TextFilePaths())
    {
        std::string extension = Path::GetExtension(textFilePath);
        if (extension == ".lexer")
        {
            ++maxFileIndex;
            GenerateHtmlSource<LexerLineParser>(project, textFilePath, maxFileIndex, fileMap);
        }
        else if (extension == ".parser")
        {
            ++maxFileIndex;
            GenerateHtmlSource<ParserLineParser>(project, textFilePath, maxFileIndex, fileMap);
        }
    }
}

void SourceCodePrinterInit()
{
    OperatorMap::Init();
}

void SourceCodePrinterDone()
{
    OperatorMap::Done();
}

} } // namespace cmajor::cmdoclib
