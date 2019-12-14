#include "Primitive.hpp"
#include <cmajor/parsing/Action.hpp>
#include <cmajor/parsing/Rule.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>
#include <cmajor/parsing/Primitive.hpp>
#include <cmajor/parsing/Composite.hpp>
#include <cmajor/parsing/Nonterminal.hpp>
#include <cmajor/parsing/Exception.hpp>
#include <cmajor/parsing/StdLib.hpp>
#include <cmajor/parsing/XmlLog.hpp>
#include <soulng/util/Unicode.hpp>
#include <cmajor/syntax/Element.hpp>

namespace cmajor { namespace syntax {

using namespace cmajor::parsing;
using namespace soulng::util;
using namespace cmajor::unicode;

Primitive* Primitive::Create()
{
    return Create(new cmajor::parsing::ParsingDomain());
}

Primitive* Primitive::Create(cmajor::parsing::ParsingDomain* parsingDomain)
{
    RegisterParsingDomain(parsingDomain);
    Primitive* grammar(new Primitive(parsingDomain));
    parsingDomain->AddGrammar(grammar);
    grammar->CreateRules();
    grammar->Link();
    return grammar;
}

Primitive::Primitive(cmajor::parsing::ParsingDomain* parsingDomain_): cmajor::parsing::Grammar(ToUtf32("Primitive"), parsingDomain_->GetNamespaceScope(ToUtf32("cmajor.syntax")), parsingDomain_)
{
    SetOwner(0);
}

cmajor::parsing::Parser* Primitive::Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName)
{
    cmajor::parsing::Scanner scanner(start, end, fileName, fileIndex, SkipRule());
    std::unique_ptr<cmajor::parsing::XmlLog> xmlLog;
    if (Log())
    {
        xmlLog.reset(new cmajor::parsing::XmlLog(*Log(), MaxLogLineLength()));
        scanner.SetLog(xmlLog.get());
        xmlLog->WriteBeginRule("parse");
    }
    cmajor::parsing::ObjectStack stack;
    std::unique_ptr<cmajor::parsing::ParsingData> parsingData(new cmajor::parsing::ParsingData(GetParsingDomain()->GetNumRules()));
    scanner.SetParsingData(parsingData.get());
    cmajor::parsing::Match match = cmajor::parsing::Grammar::Parse(scanner, stack, parsingData.get());
    cmajor::parsing::Span stop = scanner.GetSpan();
    if (Log())
    {
        xmlLog->WriteEndRule("parse");
    }
    if (!match.Hit() || stop.Start() != int(end - start))
    {
        if (StartRule())
        {
            throw cmajor::parsing::ExpectationFailure(StartRule()->Info(), fileName, stop, start, end);
        }
        else
        {
            throw cmajor::parsing::ParsingException("grammar '" + ToUtf8(Name()) + "' has no start rule", fileName, scanner.GetSpan(), start, end);
        }
    }
    std::unique_ptr<cmajor::parsing::Object> value = std::move(stack.top());
    cmajor::parsing::Parser* result = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(value.get());
    stack.pop();
    return result;
}

class Primitive::PrimitiveRule : public cmajor::parsing::Rule
{
public:
    PrimitiveRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A2Action));
        cmajor::parsing::ActionParser* a3ActionParser = GetAction(ToUtf32("A3"));
        a3ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A3Action));
        cmajor::parsing::ActionParser* a4ActionParser = GetAction(ToUtf32("A4"));
        a4ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A4Action));
        cmajor::parsing::ActionParser* a5ActionParser = GetAction(ToUtf32("A5"));
        a5ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A5Action));
        cmajor::parsing::ActionParser* a6ActionParser = GetAction(ToUtf32("A6"));
        a6ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A6Action));
        cmajor::parsing::ActionParser* a7ActionParser = GetAction(ToUtf32("A7"));
        a7ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A7Action));
        cmajor::parsing::ActionParser* a8ActionParser = GetAction(ToUtf32("A8"));
        a8ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A8Action));
        cmajor::parsing::ActionParser* a9ActionParser = GetAction(ToUtf32("A9"));
        a9ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A9Action));
        cmajor::parsing::ActionParser* a10ActionParser = GetAction(ToUtf32("A10"));
        a10ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A10Action));
        cmajor::parsing::ActionParser* a11ActionParser = GetAction(ToUtf32("A11"));
        a11ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A11Action));
        cmajor::parsing::ActionParser* a12ActionParser = GetAction(ToUtf32("A12"));
        a12ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A12Action));
        cmajor::parsing::ActionParser* a13ActionParser = GetAction(ToUtf32("A13"));
        a13ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A13Action));
        cmajor::parsing::ActionParser* a14ActionParser = GetAction(ToUtf32("A14"));
        a14ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A14Action));
        cmajor::parsing::ActionParser* a15ActionParser = GetAction(ToUtf32("A15"));
        a15ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A15Action));
        cmajor::parsing::ActionParser* a16ActionParser = GetAction(ToUtf32("A16"));
        a16ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A16Action));
        cmajor::parsing::ActionParser* a17ActionParser = GetAction(ToUtf32("A17"));
        a17ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A17Action));
        cmajor::parsing::ActionParser* a18ActionParser = GetAction(ToUtf32("A18"));
        a18ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A18Action));
        cmajor::parsing::ActionParser* a19ActionParser = GetAction(ToUtf32("A19"));
        a19ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A19Action));
        cmajor::parsing::ActionParser* a20ActionParser = GetAction(ToUtf32("A20"));
        a20ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A20Action));
        cmajor::parsing::ActionParser* a21ActionParser = GetAction(ToUtf32("A21"));
        a21ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A21Action));
        cmajor::parsing::ActionParser* a22ActionParser = GetAction(ToUtf32("A22"));
        a22ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A22Action));
        cmajor::parsing::ActionParser* a23ActionParser = GetAction(ToUtf32("A23"));
        a23ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A23Action));
        cmajor::parsing::ActionParser* a24ActionParser = GetAction(ToUtf32("A24"));
        a24ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A24Action));
        cmajor::parsing::ActionParser* a25ActionParser = GetAction(ToUtf32("A25"));
        a25ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A25Action));
        cmajor::parsing::ActionParser* a26ActionParser = GetAction(ToUtf32("A26"));
        a26ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A26Action));
        cmajor::parsing::ActionParser* a27ActionParser = GetAction(ToUtf32("A27"));
        a27ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A27Action));
        cmajor::parsing::ActionParser* a28ActionParser = GetAction(ToUtf32("A28"));
        a28ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A28Action));
        cmajor::parsing::ActionParser* a29ActionParser = GetAction(ToUtf32("A29"));
        a29ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A29Action));
        cmajor::parsing::ActionParser* a30ActionParser = GetAction(ToUtf32("A30"));
        a30ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A30Action));
        cmajor::parsing::ActionParser* a31ActionParser = GetAction(ToUtf32("A31"));
        a31ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A31Action));
        cmajor::parsing::ActionParser* a32ActionParser = GetAction(ToUtf32("A32"));
        a32ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A32Action));
        cmajor::parsing::ActionParser* a33ActionParser = GetAction(ToUtf32("A33"));
        a33ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A33Action));
        cmajor::parsing::ActionParser* a34ActionParser = GetAction(ToUtf32("A34"));
        a34ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A34Action));
        cmajor::parsing::ActionParser* a35ActionParser = GetAction(ToUtf32("A35"));
        a35ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A35Action));
        cmajor::parsing::ActionParser* a36ActionParser = GetAction(ToUtf32("A36"));
        a36ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A36Action));
        cmajor::parsing::ActionParser* a37ActionParser = GetAction(ToUtf32("A37"));
        a37ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A37Action));
        cmajor::parsing::ActionParser* a38ActionParser = GetAction(ToUtf32("A38"));
        a38ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A38Action));
        cmajor::parsing::ActionParser* a39ActionParser = GetAction(ToUtf32("A39"));
        a39ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A39Action));
        cmajor::parsing::ActionParser* a40ActionParser = GetAction(ToUtf32("A40"));
        a40ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A40Action));
        cmajor::parsing::ActionParser* a41ActionParser = GetAction(ToUtf32("A41"));
        a41ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A41Action));
        cmajor::parsing::ActionParser* a42ActionParser = GetAction(ToUtf32("A42"));
        a42ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A42Action));
        cmajor::parsing::ActionParser* a43ActionParser = GetAction(ToUtf32("A43"));
        a43ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A43Action));
        cmajor::parsing::ActionParser* a44ActionParser = GetAction(ToUtf32("A44"));
        a44ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A44Action));
        cmajor::parsing::ActionParser* a45ActionParser = GetAction(ToUtf32("A45"));
        a45ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A45Action));
        cmajor::parsing::ActionParser* a46ActionParser = GetAction(ToUtf32("A46"));
        a46ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A46Action));
        cmajor::parsing::ActionParser* a47ActionParser = GetAction(ToUtf32("A47"));
        a47ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A47Action));
        cmajor::parsing::ActionParser* a48ActionParser = GetAction(ToUtf32("A48"));
        a48ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A48Action));
        cmajor::parsing::ActionParser* a49ActionParser = GetAction(ToUtf32("A49"));
        a49ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A49Action));
        cmajor::parsing::ActionParser* a50ActionParser = GetAction(ToUtf32("A50"));
        a50ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A50Action));
        cmajor::parsing::ActionParser* a51ActionParser = GetAction(ToUtf32("A51"));
        a51ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A51Action));
        cmajor::parsing::ActionParser* a52ActionParser = GetAction(ToUtf32("A52"));
        a52ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A52Action));
        cmajor::parsing::ActionParser* a53ActionParser = GetAction(ToUtf32("A53"));
        a53ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrimitiveRule>(this, &PrimitiveRule::A53Action));
        cmajor::parsing::NonterminalParser* charNonterminalParser = GetNonterminal(ToUtf32("Char"));
        charNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostChar));
        cmajor::parsing::NonterminalParser* stringNonterminalParser = GetNonterminal(ToUtf32("String"));
        stringNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostString));
        cmajor::parsing::NonterminalParser* charSetNonterminalParser = GetNonterminal(ToUtf32("CharSet"));
        charSetNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCharSet));
        cmajor::parsing::NonterminalParser* keywordNonterminalParser = GetNonterminal(ToUtf32("Keyword"));
        keywordNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeyword));
        cmajor::parsing::NonterminalParser* keywordListNonterminalParser = GetNonterminal(ToUtf32("KeywordList"));
        keywordListNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostKeywordList));
        cmajor::parsing::NonterminalParser* rangeNonterminalParser = GetNonterminal(ToUtf32("Range"));
        rangeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostRange));
        cmajor::parsing::NonterminalParser* emptyNonterminalParser = GetNonterminal(ToUtf32("Empty"));
        emptyNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEmpty));
        cmajor::parsing::NonterminalParser* spaceNonterminalParser = GetNonterminal(ToUtf32("Space"));
        spaceNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpace));
        cmajor::parsing::NonterminalParser* anyCharNonterminalParser = GetNonterminal(ToUtf32("AnyChar"));
        anyCharNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAnyChar));
        cmajor::parsing::NonterminalParser* letterNonterminalParser = GetNonterminal(ToUtf32("Letter"));
        letterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetter));
        cmajor::parsing::NonterminalParser* digitNonterminalParser = GetNonterminal(ToUtf32("Digit"));
        digitNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDigit));
        cmajor::parsing::NonterminalParser* hexDigitNonterminalParser = GetNonterminal(ToUtf32("HexDigit"));
        hexDigitNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostHexDigit));
        cmajor::parsing::NonterminalParser* upperLetterNonterminalParser = GetNonterminal(ToUtf32("UpperLetter"));
        upperLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUpperLetter));
        cmajor::parsing::NonterminalParser* lowerLetterNonterminalParser = GetNonterminal(ToUtf32("LowerLetter"));
        lowerLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLowerLetter));
        cmajor::parsing::NonterminalParser* titleLetterNonterminalParser = GetNonterminal(ToUtf32("TitleLetter"));
        titleLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostTitleLetter));
        cmajor::parsing::NonterminalParser* modifierLetterNonterminalParser = GetNonterminal(ToUtf32("ModifierLetter"));
        modifierLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierLetter));
        cmajor::parsing::NonterminalParser* otherLetterNonterminalParser = GetNonterminal(ToUtf32("OtherLetter"));
        otherLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherLetter));
        cmajor::parsing::NonterminalParser* casedLetterNonterminalParser = GetNonterminal(ToUtf32("CasedLetter"));
        casedLetterNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCasedLetter));
        cmajor::parsing::NonterminalParser* markNonterminalParser = GetNonterminal(ToUtf32("Mark"));
        markNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMark));
        cmajor::parsing::NonterminalParser* nonspacingMarkNonterminalParser = GetNonterminal(ToUtf32("NonspacingMark"));
        nonspacingMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNonspacingMark));
        cmajor::parsing::NonterminalParser* enclosingMarkNonterminalParser = GetNonterminal(ToUtf32("EnclosingMark"));
        enclosingMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostEnclosingMark));
        cmajor::parsing::NonterminalParser* spacingMarkNonterminalParser = GetNonterminal(ToUtf32("SpacingMark"));
        spacingMarkNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpacingMark));
        cmajor::parsing::NonterminalParser* numberNonterminalParser = GetNonterminal(ToUtf32("Number"));
        numberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostNumber));
        cmajor::parsing::NonterminalParser* decimalNumberNonterminalParser = GetNonterminal(ToUtf32("DecimalNumber"));
        decimalNumberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDecimalNumber));
        cmajor::parsing::NonterminalParser* letterNumberNonterminalParser = GetNonterminal(ToUtf32("LetterNumber"));
        letterNumberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLetterNumber));
        cmajor::parsing::NonterminalParser* otherNumberNonterminalParser = GetNonterminal(ToUtf32("OtherNumber"));
        otherNumberNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherNumber));
        cmajor::parsing::NonterminalParser* punctuationNonterminalParser = GetNonterminal(ToUtf32("Punctuation"));
        punctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPunctuation));
        cmajor::parsing::NonterminalParser* connectorPunctuationNonterminalParser = GetNonterminal(ToUtf32("ConnectorPunctuation"));
        connectorPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostConnectorPunctuation));
        cmajor::parsing::NonterminalParser* dashPunctuationNonterminalParser = GetNonterminal(ToUtf32("DashPunctuation"));
        dashPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostDashPunctuation));
        cmajor::parsing::NonterminalParser* openPunctuationNonterminalParser = GetNonterminal(ToUtf32("OpenPunctuation"));
        openPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOpenPunctuation));
        cmajor::parsing::NonterminalParser* closePunctuationNonterminalParser = GetNonterminal(ToUtf32("ClosePunctuation"));
        closePunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostClosePunctuation));
        cmajor::parsing::NonterminalParser* initialPunctuationNonterminalParser = GetNonterminal(ToUtf32("InitialPunctuation"));
        initialPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostInitialPunctuation));
        cmajor::parsing::NonterminalParser* finalPunctuationNonterminalParser = GetNonterminal(ToUtf32("FinalPunctuation"));
        finalPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFinalPunctuation));
        cmajor::parsing::NonterminalParser* otherPunctuationNonterminalParser = GetNonterminal(ToUtf32("OtherPunctuation"));
        otherPunctuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherPunctuation));
        cmajor::parsing::NonterminalParser* symbolNonterminalParser = GetNonterminal(ToUtf32("Symbol"));
        symbolNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSymbol));
        cmajor::parsing::NonterminalParser* mathSymbolNonterminalParser = GetNonterminal(ToUtf32("MathSymbol"));
        mathSymbolNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostMathSymbol));
        cmajor::parsing::NonterminalParser* currencySymbolNonterminalParser = GetNonterminal(ToUtf32("CurrencySymbol"));
        currencySymbolNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostCurrencySymbol));
        cmajor::parsing::NonterminalParser* modifierSymbolNonterminalParser = GetNonterminal(ToUtf32("ModifierSymbol"));
        modifierSymbolNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostModifierSymbol));
        cmajor::parsing::NonterminalParser* otherSymbolNonterminalParser = GetNonterminal(ToUtf32("OtherSymbol"));
        otherSymbolNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOtherSymbol));
        cmajor::parsing::NonterminalParser* separatorNonterminalParser = GetNonterminal(ToUtf32("Separator"));
        separatorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSeparator));
        cmajor::parsing::NonterminalParser* spaceSeparatorNonterminalParser = GetNonterminal(ToUtf32("SpaceSeparator"));
        spaceSeparatorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSpaceSeparator));
        cmajor::parsing::NonterminalParser* lineSeparatorNonterminalParser = GetNonterminal(ToUtf32("LineSeparator"));
        lineSeparatorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostLineSeparator));
        cmajor::parsing::NonterminalParser* paragraphSeparatorNonterminalParser = GetNonterminal(ToUtf32("ParagraphSeparator"));
        paragraphSeparatorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostParagraphSeparator));
        cmajor::parsing::NonterminalParser* otherNonterminalParser = GetNonterminal(ToUtf32("Other"));
        otherNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostOther));
        cmajor::parsing::NonterminalParser* controlNonterminalParser = GetNonterminal(ToUtf32("Control"));
        controlNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostControl));
        cmajor::parsing::NonterminalParser* formatNonterminalParser = GetNonterminal(ToUtf32("Format"));
        formatNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostFormat));
        cmajor::parsing::NonterminalParser* surrogateNonterminalParser = GetNonterminal(ToUtf32("Surrogate"));
        surrogateNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostSurrogate));
        cmajor::parsing::NonterminalParser* privateUseNonterminalParser = GetNonterminal(ToUtf32("PrivateUse"));
        privateUseNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostPrivateUse));
        cmajor::parsing::NonterminalParser* unassignedNonterminalParser = GetNonterminal(ToUtf32("Unassigned"));
        unassignedNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostUnassigned));
        cmajor::parsing::NonterminalParser* graphicNonterminalParser = GetNonterminal(ToUtf32("Graphic"));
        graphicNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostGraphic));
        cmajor::parsing::NonterminalParser* baseCharNonterminalParser = GetNonterminal(ToUtf32("BaseChar"));
        baseCharNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostBaseChar));
        cmajor::parsing::NonterminalParser* alphabeticNonterminalParser = GetNonterminal(ToUtf32("Alphabetic"));
        alphabeticNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostAlphabetic));
        cmajor::parsing::NonterminalParser* idStartNonterminalParser = GetNonterminal(ToUtf32("IdStart"));
        idStartNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdStart));
        cmajor::parsing::NonterminalParser* idContNonterminalParser = GetNonterminal(ToUtf32("IdCont"));
        idContNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<PrimitiveRule>(this, &PrimitiveRule::PostIdCont));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromChar;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromString;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCharSet;
    }
    void A3Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeyword;
    }
    void A4Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordList;
    }
    void A5Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromRange;
    }
    void A6Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEmpty;
    }
    void A7Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpace;
    }
    void A8Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAnyChar;
    }
    void A9Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetter;
    }
    void A10Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDigit;
    }
    void A11Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromHexDigit;
    }
    void A12Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUpperLetter;
    }
    void A13Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLowerLetter;
    }
    void A14Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromTitleLetter;
    }
    void A15Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierLetter;
    }
    void A16Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherLetter;
    }
    void A17Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCasedLetter;
    }
    void A18Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMark;
    }
    void A19Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNonspacingMark;
    }
    void A20Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromEnclosingMark;
    }
    void A21Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpacingMark;
    }
    void A22Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromNumber;
    }
    void A23Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDecimalNumber;
    }
    void A24Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLetterNumber;
    }
    void A25Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherNumber;
    }
    void A26Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPunctuation;
    }
    void A27Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromConnectorPunctuation;
    }
    void A28Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromDashPunctuation;
    }
    void A29Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOpenPunctuation;
    }
    void A30Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromClosePunctuation;
    }
    void A31Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromInitialPunctuation;
    }
    void A32Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFinalPunctuation;
    }
    void A33Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherPunctuation;
    }
    void A34Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSymbol;
    }
    void A35Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromMathSymbol;
    }
    void A36Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromCurrencySymbol;
    }
    void A37Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromModifierSymbol;
    }
    void A38Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOtherSymbol;
    }
    void A39Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSeparator;
    }
    void A40Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSpaceSeparator;
    }
    void A41Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromLineSeparator;
    }
    void A42Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromParagraphSeparator;
    }
    void A43Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromOther;
    }
    void A44Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromControl;
    }
    void A45Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromFormat;
    }
    void A46Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromSurrogate;
    }
    void A47Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromPrivateUse;
    }
    void A48Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromUnassigned;
    }
    void A49Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromGraphic;
    }
    void A50Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromBaseChar;
    }
    void A51Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromAlphabetic;
    }
    void A52Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdStart;
    }
    void A53Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromIdCont;
    }
    void PostChar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromChar_value = std::move(stack.top());
            context->fromChar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromChar_value.get());
            stack.pop();
        }
    }
    void PostString(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromString_value = std::move(stack.top());
            context->fromString = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromString_value.get());
            stack.pop();
        }
    }
    void PostCharSet(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharSet_value = std::move(stack.top());
            context->fromCharSet = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromCharSet_value.get());
            stack.pop();
        }
    }
    void PostKeyword(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromKeyword_value = std::move(stack.top());
            context->fromKeyword = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromKeyword_value.get());
            stack.pop();
        }
    }
    void PostKeywordList(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromKeywordList_value = std::move(stack.top());
            context->fromKeywordList = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromKeywordList_value.get());
            stack.pop();
        }
    }
    void PostRange(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromRange_value = std::move(stack.top());
            context->fromRange = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromRange_value.get());
            stack.pop();
        }
    }
    void PostEmpty(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEmpty_value = std::move(stack.top());
            context->fromEmpty = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromEmpty_value.get());
            stack.pop();
        }
    }
    void PostSpace(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpace_value = std::move(stack.top());
            context->fromSpace = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSpace_value.get());
            stack.pop();
        }
    }
    void PostAnyChar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAnyChar_value = std::move(stack.top());
            context->fromAnyChar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromAnyChar_value.get());
            stack.pop();
        }
    }
    void PostLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLetter_value = std::move(stack.top());
            context->fromLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromLetter_value.get());
            stack.pop();
        }
    }
    void PostDigit(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDigit_value = std::move(stack.top());
            context->fromDigit = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromDigit_value.get());
            stack.pop();
        }
    }
    void PostHexDigit(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromHexDigit_value = std::move(stack.top());
            context->fromHexDigit = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromHexDigit_value.get());
            stack.pop();
        }
    }
    void PostUpperLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUpperLetter_value = std::move(stack.top());
            context->fromUpperLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromUpperLetter_value.get());
            stack.pop();
        }
    }
    void PostLowerLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLowerLetter_value = std::move(stack.top());
            context->fromLowerLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromLowerLetter_value.get());
            stack.pop();
        }
    }
    void PostTitleLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromTitleLetter_value = std::move(stack.top());
            context->fromTitleLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromTitleLetter_value.get());
            stack.pop();
        }
    }
    void PostModifierLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromModifierLetter_value = std::move(stack.top());
            context->fromModifierLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromModifierLetter_value.get());
            stack.pop();
        }
    }
    void PostOtherLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOtherLetter_value = std::move(stack.top());
            context->fromOtherLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOtherLetter_value.get());
            stack.pop();
        }
    }
    void PostCasedLetter(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCasedLetter_value = std::move(stack.top());
            context->fromCasedLetter = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromCasedLetter_value.get());
            stack.pop();
        }
    }
    void PostMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMark_value = std::move(stack.top());
            context->fromMark = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromMark_value.get());
            stack.pop();
        }
    }
    void PostNonspacingMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNonspacingMark_value = std::move(stack.top());
            context->fromNonspacingMark = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromNonspacingMark_value.get());
            stack.pop();
        }
    }
    void PostEnclosingMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromEnclosingMark_value = std::move(stack.top());
            context->fromEnclosingMark = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromEnclosingMark_value.get());
            stack.pop();
        }
    }
    void PostSpacingMark(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpacingMark_value = std::move(stack.top());
            context->fromSpacingMark = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSpacingMark_value.get());
            stack.pop();
        }
    }
    void PostNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromNumber_value = std::move(stack.top());
            context->fromNumber = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromNumber_value.get());
            stack.pop();
        }
    }
    void PostDecimalNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDecimalNumber_value = std::move(stack.top());
            context->fromDecimalNumber = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromDecimalNumber_value.get());
            stack.pop();
        }
    }
    void PostLetterNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLetterNumber_value = std::move(stack.top());
            context->fromLetterNumber = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromLetterNumber_value.get());
            stack.pop();
        }
    }
    void PostOtherNumber(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOtherNumber_value = std::move(stack.top());
            context->fromOtherNumber = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOtherNumber_value.get());
            stack.pop();
        }
    }
    void PostPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPunctuation_value = std::move(stack.top());
            context->fromPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromPunctuation_value.get());
            stack.pop();
        }
    }
    void PostConnectorPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromConnectorPunctuation_value = std::move(stack.top());
            context->fromConnectorPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromConnectorPunctuation_value.get());
            stack.pop();
        }
    }
    void PostDashPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromDashPunctuation_value = std::move(stack.top());
            context->fromDashPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromDashPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOpenPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOpenPunctuation_value = std::move(stack.top());
            context->fromOpenPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOpenPunctuation_value.get());
            stack.pop();
        }
    }
    void PostClosePunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromClosePunctuation_value = std::move(stack.top());
            context->fromClosePunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromClosePunctuation_value.get());
            stack.pop();
        }
    }
    void PostInitialPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromInitialPunctuation_value = std::move(stack.top());
            context->fromInitialPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromInitialPunctuation_value.get());
            stack.pop();
        }
    }
    void PostFinalPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFinalPunctuation_value = std::move(stack.top());
            context->fromFinalPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromFinalPunctuation_value.get());
            stack.pop();
        }
    }
    void PostOtherPunctuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOtherPunctuation_value = std::move(stack.top());
            context->fromOtherPunctuation = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOtherPunctuation_value.get());
            stack.pop();
        }
    }
    void PostSymbol(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSymbol_value = std::move(stack.top());
            context->fromSymbol = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSymbol_value.get());
            stack.pop();
        }
    }
    void PostMathSymbol(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromMathSymbol_value = std::move(stack.top());
            context->fromMathSymbol = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromMathSymbol_value.get());
            stack.pop();
        }
    }
    void PostCurrencySymbol(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCurrencySymbol_value = std::move(stack.top());
            context->fromCurrencySymbol = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromCurrencySymbol_value.get());
            stack.pop();
        }
    }
    void PostModifierSymbol(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromModifierSymbol_value = std::move(stack.top());
            context->fromModifierSymbol = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromModifierSymbol_value.get());
            stack.pop();
        }
    }
    void PostOtherSymbol(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOtherSymbol_value = std::move(stack.top());
            context->fromOtherSymbol = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOtherSymbol_value.get());
            stack.pop();
        }
    }
    void PostSeparator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSeparator_value = std::move(stack.top());
            context->fromSeparator = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSeparator_value.get());
            stack.pop();
        }
    }
    void PostSpaceSeparator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSpaceSeparator_value = std::move(stack.top());
            context->fromSpaceSeparator = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSpaceSeparator_value.get());
            stack.pop();
        }
    }
    void PostLineSeparator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromLineSeparator_value = std::move(stack.top());
            context->fromLineSeparator = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromLineSeparator_value.get());
            stack.pop();
        }
    }
    void PostParagraphSeparator(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromParagraphSeparator_value = std::move(stack.top());
            context->fromParagraphSeparator = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromParagraphSeparator_value.get());
            stack.pop();
        }
    }
    void PostOther(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromOther_value = std::move(stack.top());
            context->fromOther = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromOther_value.get());
            stack.pop();
        }
    }
    void PostControl(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromControl_value = std::move(stack.top());
            context->fromControl = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromControl_value.get());
            stack.pop();
        }
    }
    void PostFormat(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromFormat_value = std::move(stack.top());
            context->fromFormat = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromFormat_value.get());
            stack.pop();
        }
    }
    void PostSurrogate(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromSurrogate_value = std::move(stack.top());
            context->fromSurrogate = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromSurrogate_value.get());
            stack.pop();
        }
    }
    void PostPrivateUse(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromPrivateUse_value = std::move(stack.top());
            context->fromPrivateUse = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromPrivateUse_value.get());
            stack.pop();
        }
    }
    void PostUnassigned(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromUnassigned_value = std::move(stack.top());
            context->fromUnassigned = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromUnassigned_value.get());
            stack.pop();
        }
    }
    void PostGraphic(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromGraphic_value = std::move(stack.top());
            context->fromGraphic = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromGraphic_value.get());
            stack.pop();
        }
    }
    void PostBaseChar(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromBaseChar_value = std::move(stack.top());
            context->fromBaseChar = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromBaseChar_value.get());
            stack.pop();
        }
    }
    void PostAlphabetic(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromAlphabetic_value = std::move(stack.top());
            context->fromAlphabetic = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromAlphabetic_value.get());
            stack.pop();
        }
    }
    void PostIdStart(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdStart_value = std::move(stack.top());
            context->fromIdStart = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromIdStart_value.get());
            stack.pop();
        }
    }
    void PostIdCont(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromIdCont_value = std::move(stack.top());
            context->fromIdCont = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromIdCont_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromChar(), fromString(), fromCharSet(), fromKeyword(), fromKeywordList(), fromRange(), fromEmpty(), fromSpace(), fromAnyChar(), fromLetter(), fromDigit(), fromHexDigit(), fromUpperLetter(), fromLowerLetter(), fromTitleLetter(), fromModifierLetter(), fromOtherLetter(), fromCasedLetter(), fromMark(), fromNonspacingMark(), fromEnclosingMark(), fromSpacingMark(), fromNumber(), fromDecimalNumber(), fromLetterNumber(), fromOtherNumber(), fromPunctuation(), fromConnectorPunctuation(), fromDashPunctuation(), fromOpenPunctuation(), fromClosePunctuation(), fromInitialPunctuation(), fromFinalPunctuation(), fromOtherPunctuation(), fromSymbol(), fromMathSymbol(), fromCurrencySymbol(), fromModifierSymbol(), fromOtherSymbol(), fromSeparator(), fromSpaceSeparator(), fromLineSeparator(), fromParagraphSeparator(), fromOther(), fromControl(), fromFormat(), fromSurrogate(), fromPrivateUse(), fromUnassigned(), fromGraphic(), fromBaseChar(), fromAlphabetic(), fromIdStart(), fromIdCont() {}
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromChar;
        cmajor::parsing::Parser* fromString;
        cmajor::parsing::Parser* fromCharSet;
        cmajor::parsing::Parser* fromKeyword;
        cmajor::parsing::Parser* fromKeywordList;
        cmajor::parsing::Parser* fromRange;
        cmajor::parsing::Parser* fromEmpty;
        cmajor::parsing::Parser* fromSpace;
        cmajor::parsing::Parser* fromAnyChar;
        cmajor::parsing::Parser* fromLetter;
        cmajor::parsing::Parser* fromDigit;
        cmajor::parsing::Parser* fromHexDigit;
        cmajor::parsing::Parser* fromUpperLetter;
        cmajor::parsing::Parser* fromLowerLetter;
        cmajor::parsing::Parser* fromTitleLetter;
        cmajor::parsing::Parser* fromModifierLetter;
        cmajor::parsing::Parser* fromOtherLetter;
        cmajor::parsing::Parser* fromCasedLetter;
        cmajor::parsing::Parser* fromMark;
        cmajor::parsing::Parser* fromNonspacingMark;
        cmajor::parsing::Parser* fromEnclosingMark;
        cmajor::parsing::Parser* fromSpacingMark;
        cmajor::parsing::Parser* fromNumber;
        cmajor::parsing::Parser* fromDecimalNumber;
        cmajor::parsing::Parser* fromLetterNumber;
        cmajor::parsing::Parser* fromOtherNumber;
        cmajor::parsing::Parser* fromPunctuation;
        cmajor::parsing::Parser* fromConnectorPunctuation;
        cmajor::parsing::Parser* fromDashPunctuation;
        cmajor::parsing::Parser* fromOpenPunctuation;
        cmajor::parsing::Parser* fromClosePunctuation;
        cmajor::parsing::Parser* fromInitialPunctuation;
        cmajor::parsing::Parser* fromFinalPunctuation;
        cmajor::parsing::Parser* fromOtherPunctuation;
        cmajor::parsing::Parser* fromSymbol;
        cmajor::parsing::Parser* fromMathSymbol;
        cmajor::parsing::Parser* fromCurrencySymbol;
        cmajor::parsing::Parser* fromModifierSymbol;
        cmajor::parsing::Parser* fromOtherSymbol;
        cmajor::parsing::Parser* fromSeparator;
        cmajor::parsing::Parser* fromSpaceSeparator;
        cmajor::parsing::Parser* fromLineSeparator;
        cmajor::parsing::Parser* fromParagraphSeparator;
        cmajor::parsing::Parser* fromOther;
        cmajor::parsing::Parser* fromControl;
        cmajor::parsing::Parser* fromFormat;
        cmajor::parsing::Parser* fromSurrogate;
        cmajor::parsing::Parser* fromPrivateUse;
        cmajor::parsing::Parser* fromUnassigned;
        cmajor::parsing::Parser* fromGraphic;
        cmajor::parsing::Parser* fromBaseChar;
        cmajor::parsing::Parser* fromAlphabetic;
        cmajor::parsing::Parser* fromIdStart;
        cmajor::parsing::Parser* fromIdCont;
    };
};

class Primitive::CharRule : public cmajor::parsing::Rule
{
public:
    CharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharRule>(this, &CharRule::A0Action));
        cmajor::parsing::NonterminalParser* chrNonterminalParser = GetNonterminal(ToUtf32("chr"));
        chrNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharRule>(this, &CharRule::Postchr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharParser(context->fromchr);
    }
    void Postchr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromchr_value = std::move(stack.top());
            context->fromchr = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromchr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromchr() {}
        cmajor::parsing::Parser* value;
        char32_t fromchr;
    };
};

class Primitive::StringRule : public cmajor::parsing::Rule
{
public:
    StringRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<StringRule>(this, &StringRule::A0Action));
        cmajor::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<StringRule>(this, &StringRule::Poststr));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new StringParser(context->fromstr);
    }
    void Poststr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromstr() {}
        cmajor::parsing::Parser* value;
        std::u32string fromstr;
    };
};

class Primitive::CharSetRule : public cmajor::parsing::Rule
{
public:
    CharSetRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("bool"), ToUtf32("inverse")));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::u32string"), ToUtf32("s")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A1Action));
        cmajor::parsing::ActionParser* a2ActionParser = GetAction(ToUtf32("A2"));
        a2ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetRule>(this, &CharSetRule::A2Action));
        cmajor::parsing::NonterminalParser* charSetRangeNonterminalParser = GetNonterminal(ToUtf32("CharSetRange"));
        charSetRangeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharSetRule>(this, &CharSetRule::PostCharSetRange));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CharSetParser(context->s, context->inverse);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->inverse = true;
    }
    void A2Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->s.append(context->fromCharSetRange);
    }
    void PostCharSetRange(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromCharSetRange_value = std::move(stack.top());
            context->fromCharSetRange = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromCharSetRange_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), inverse(), s(), fromCharSetRange() {}
        cmajor::parsing::Parser* value;
        bool inverse;
        std::u32string s;
        std::u32string fromCharSetRange;
    };
};

class Primitive::CharSetRangeRule : public cmajor::parsing::Rule
{
public:
    CharSetRangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("std::u32string"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::u32string>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetRangeRule>(this, &CharSetRangeRule::A1Action));
        cmajor::parsing::NonterminalParser* firstNonterminalParser = GetNonterminal(ToUtf32("first"));
        firstNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postfirst));
        cmajor::parsing::NonterminalParser* lastNonterminalParser = GetNonterminal(ToUtf32("last"));
        lastNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharSetRangeRule>(this, &CharSetRangeRule::Postlast));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
        context->value.append(1, '-');
        context->value.append(1, context->fromlast);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value.append(1, context->fromfirst);
    }
    void Postfirst(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromfirst_value = std::move(stack.top());
            context->fromfirst = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromfirst_value.get());
            stack.pop();
        }
    }
    void Postlast(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromlast_value = std::move(stack.top());
            context->fromlast = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromlast_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromfirst(), fromlast() {}
        std::u32string value;
        char32_t fromfirst;
        char32_t fromlast;
    };
};

class Primitive::CharSetCharRule : public cmajor::parsing::Rule
{
public:
    CharSetCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("char32_t"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<char32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CharSetCharRule>(this, &CharSetCharRule::A1Action));
        cmajor::parsing::NonterminalParser* escapeNonterminalParser = GetNonterminal(ToUtf32("escape"));
        escapeNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CharSetCharRule>(this, &CharSetCharRule::Postescape));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = *matchBegin;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromescape;
    }
    void Postescape(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromescape_value = std::move(stack.top());
            context->fromescape = *static_cast<cmajor::parsing::ValueObject<char32_t>*>(fromescape_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromescape() {}
        char32_t value;
        char32_t fromescape;
    };
};

class Primitive::KeywordRule : public cmajor::parsing::Rule
{
public:
    KeywordRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<KeywordRule>(this, &KeywordRule::A0Action));
        cmajor::parsing::NonterminalParser* keywordBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordBody"));
        keywordBodyNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<KeywordRule>(this, &KeywordRule::PostKeywordBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordBody;
    }
    void PostKeywordBody(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromKeywordBody_value = std::move(stack.top());
            context->fromKeywordBody = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromKeywordBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromKeywordBody() {}
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromKeywordBody;
    };
};

class Primitive::KeywordBodyRule : public cmajor::parsing::Rule
{
public:
    KeywordBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<KeywordBodyRule>(this, &KeywordBodyRule::A1Action));
        cmajor::parsing::NonterminalParser* strNonterminalParser = GetNonterminal(ToUtf32("str"));
        strNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Poststr));
        cmajor::parsing::NonterminalParser* continuationNonterminalParser = GetNonterminal(ToUtf32("continuation"));
        continuationNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<KeywordBodyRule>(this, &KeywordBodyRule::Postcontinuation));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr, context->fromcontinuation);
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordParser(context->fromstr);
    }
    void Poststr(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromstr_value = std::move(stack.top());
            context->fromstr = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromstr_value.get());
            stack.pop();
        }
    }
    void Postcontinuation(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromcontinuation_value = std::move(stack.top());
            context->fromcontinuation = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromcontinuation_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromstr(), fromcontinuation() {}
        cmajor::parsing::Parser* value;
        std::u32string fromstr;
        std::u32string fromcontinuation;
    };
};

class Primitive::KeywordListRule : public cmajor::parsing::Rule
{
public:
    KeywordListRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<KeywordListRule>(this, &KeywordListRule::A0Action));
        cmajor::parsing::NonterminalParser* keywordListBodyNonterminalParser = GetNonterminal(ToUtf32("KeywordListBody"));
        keywordListBodyNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<KeywordListRule>(this, &KeywordListRule::PostKeywordListBody));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromKeywordListBody;
    }
    void PostKeywordListBody(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromKeywordListBody_value = std::move(stack.top());
            context->fromKeywordListBody = *static_cast<cmajor::parsing::ValueObject<cmajor::parsing::Parser*>*>(fromKeywordListBody_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromKeywordListBody() {}
        cmajor::parsing::Parser* value;
        cmajor::parsing::Parser* fromKeywordListBody;
    };
};

class Primitive::KeywordListBodyRule : public cmajor::parsing::Rule
{
public:
    KeywordListBodyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
        AddLocalVariable(AttrOrVariable(ToUtf32("std::vector<std::u32string>"), ToUtf32("keywords")));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<KeywordListBodyRule>(this, &KeywordListBodyRule::A0Action));
        cmajor::parsing::NonterminalParser* selectorNonterminalParser = GetNonterminal(ToUtf32("selector"));
        selectorNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<KeywordListBodyRule>(this, &KeywordListBodyRule::Postselector));
        cmajor::parsing::NonterminalParser* stringArrayNonterminalParser = GetNonterminal(ToUtf32("StringArray"));
        stringArrayNonterminalParser->SetPreCall(new cmajor::parsing::MemberPreCall<KeywordListBodyRule>(this, &KeywordListBodyRule::PreStringArray));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new KeywordListParser(context->fromselector, context->keywords);
    }
    void Postselector(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromselector_value = std::move(stack.top());
            context->fromselector = *static_cast<cmajor::parsing::ValueObject<std::u32string>*>(fromselector_value.get());
            stack.pop();
        }
    }
    void PreStringArray(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<std::vector<std::u32string>*>(&context->keywords)));
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), keywords(), fromselector() {}
        cmajor::parsing::Parser* value;
        std::vector<std::u32string> keywords;
        std::u32string fromselector;
    };
};

class Primitive::RangeRule : public cmajor::parsing::Rule
{
public:
    RangeRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<RangeRule>(this, &RangeRule::A0Action));
        cmajor::parsing::NonterminalParser* sNonterminalParser = GetNonterminal(ToUtf32("s"));
        sNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Posts));
        cmajor::parsing::NonterminalParser* eNonterminalParser = GetNonterminal(ToUtf32("e"));
        eNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<RangeRule>(this, &RangeRule::Poste));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new RangeParser(context->froms, context->frome);
    }
    void Posts(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> froms_value = std::move(stack.top());
            context->froms = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(froms_value.get());
            stack.pop();
        }
    }
    void Poste(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> frome_value = std::move(stack.top());
            context->frome = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(frome_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), froms(), frome() {}
        cmajor::parsing::Parser* value;
        uint32_t froms;
        uint32_t frome;
    };
};

class Primitive::CodePointRule : public cmajor::parsing::Rule
{
public:
    CodePointRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("uint32_t"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<uint32_t>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A0Action));
        cmajor::parsing::ActionParser* a1ActionParser = GetAction(ToUtf32("A1"));
        a1ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CodePointRule>(this, &CodePointRule::A1Action));
        cmajor::parsing::NonterminalParser* xNonterminalParser = GetNonterminal(ToUtf32("x"));
        xNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postx));
        cmajor::parsing::NonterminalParser* dNonterminalParser = GetNonterminal(ToUtf32("d"));
        dNonterminalParser->SetPostCall(new cmajor::parsing::MemberPostCall<CodePointRule>(this, &CodePointRule::Postd));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromx;
    }
    void A1Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = context->fromd;
    }
    void Postx(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromx_value = std::move(stack.top());
            context->fromx = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromx_value.get());
            stack.pop();
        }
    }
    void Postd(cmajor::parsing::ObjectStack& stack, ParsingData* parsingData, bool matched)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            std::unique_ptr<cmajor::parsing::Object> fromd_value = std::move(stack.top());
            context->fromd = *static_cast<cmajor::parsing::ValueObject<uint32_t>*>(fromd_value.get());
            stack.pop();
        }
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value(), fromx(), fromd() {}
        uint32_t value;
        uint32_t fromx;
        uint32_t fromd;
    };
};

class Primitive::EmptyRule : public cmajor::parsing::Rule
{
public:
    EmptyRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EmptyRule>(this, &EmptyRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EmptyParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SpaceRule : public cmajor::parsing::Rule
{
public:
    SpaceRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpaceRule>(this, &SpaceRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::AnyCharRule : public cmajor::parsing::Rule
{
public:
    AnyCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AnyCharRule>(this, &AnyCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AnyCharParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::LetterRule : public cmajor::parsing::Rule
{
public:
    LetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LetterRule>(this, &LetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::DigitRule : public cmajor::parsing::Rule
{
public:
    DigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DigitRule>(this, &DigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DigitParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::HexDigitRule : public cmajor::parsing::Rule
{
public:
    HexDigitRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<HexDigitRule>(this, &HexDigitRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new HexDigitParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::UpperLetterRule : public cmajor::parsing::Rule
{
public:
    UpperLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UpperLetterRule>(this, &UpperLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UpperLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::LowerLetterRule : public cmajor::parsing::Rule
{
public:
    LowerLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LowerLetterRule>(this, &LowerLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LowerLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::TitleLetterRule : public cmajor::parsing::Rule
{
public:
    TitleLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<TitleLetterRule>(this, &TitleLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new TitleLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ModifierLetterRule : public cmajor::parsing::Rule
{
public:
    ModifierLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ModifierLetterRule>(this, &ModifierLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OtherLetterRule : public cmajor::parsing::Rule
{
public:
    OtherLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherLetterRule>(this, &OtherLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::CasedLetterRule : public cmajor::parsing::Rule
{
public:
    CasedLetterRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CasedLetterRule>(this, &CasedLetterRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CasedLetterParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::MarkRule : public cmajor::parsing::Rule
{
public:
    MarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MarkRule>(this, &MarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MarkParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::NonspacingMarkRule : public cmajor::parsing::Rule
{
public:
    NonspacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NonspacingMarkRule>(this, &NonspacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NonspacingMarkParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SpacingMarkRule : public cmajor::parsing::Rule
{
public:
    SpacingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpacingMarkRule>(this, &SpacingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpacingMarkParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::EnclosingMarkRule : public cmajor::parsing::Rule
{
public:
    EnclosingMarkRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<EnclosingMarkRule>(this, &EnclosingMarkRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new EnclosingMarkParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::NumberRule : public cmajor::parsing::Rule
{
public:
    NumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<NumberRule>(this, &NumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new NumberParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::DecimalNumberRule : public cmajor::parsing::Rule
{
public:
    DecimalNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DecimalNumberRule>(this, &DecimalNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DecimalNumberParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::LetterNumberRule : public cmajor::parsing::Rule
{
public:
    LetterNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LetterNumberRule>(this, &LetterNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LetterNumberParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OtherNumberRule : public cmajor::parsing::Rule
{
public:
    OtherNumberRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherNumberRule>(this, &OtherNumberRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherNumberParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::PunctuationRule : public cmajor::parsing::Rule
{
public:
    PunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PunctuationRule>(this, &PunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ConnectorPunctuationRule : public cmajor::parsing::Rule
{
public:
    ConnectorPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ConnectorPunctuationRule>(this, &ConnectorPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ConnectorPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::DashPunctuationRule : public cmajor::parsing::Rule
{
public:
    DashPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<DashPunctuationRule>(this, &DashPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new DashPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OpenPunctuationRule : public cmajor::parsing::Rule
{
public:
    OpenPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OpenPunctuationRule>(this, &OpenPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OpenPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ClosePunctuationRule : public cmajor::parsing::Rule
{
public:
    ClosePunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ClosePunctuationRule>(this, &ClosePunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ClosePunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::InitialPunctuationRule : public cmajor::parsing::Rule
{
public:
    InitialPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<InitialPunctuationRule>(this, &InitialPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new InitialPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::FinalPunctuationRule : public cmajor::parsing::Rule
{
public:
    FinalPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FinalPunctuationRule>(this, &FinalPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FinalPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OtherPunctuationRule : public cmajor::parsing::Rule
{
public:
    OtherPunctuationRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherPunctuationRule>(this, &OtherPunctuationRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherPunctuationParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SymbolRule : public cmajor::parsing::Rule
{
public:
    SymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SymbolRule>(this, &SymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SymbolParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::MathSymbolRule : public cmajor::parsing::Rule
{
public:
    MathSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<MathSymbolRule>(this, &MathSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new MathSymbolParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::CurrencySymbolRule : public cmajor::parsing::Rule
{
public:
    CurrencySymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<CurrencySymbolRule>(this, &CurrencySymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new CurrencySymbolParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ModifierSymbolRule : public cmajor::parsing::Rule
{
public:
    ModifierSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ModifierSymbolRule>(this, &ModifierSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ModifierSymbolParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OtherSymbolRule : public cmajor::parsing::Rule
{
public:
    OtherSymbolRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherSymbolRule>(this, &OtherSymbolRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherSymbolParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SeparatorRule : public cmajor::parsing::Rule
{
public:
    SeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SeparatorRule>(this, &SeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SeparatorParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SpaceSeparatorRule : public cmajor::parsing::Rule
{
public:
    SpaceSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SpaceSeparatorRule>(this, &SpaceSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SpaceSeparatorParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::LineSeparatorRule : public cmajor::parsing::Rule
{
public:
    LineSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<LineSeparatorRule>(this, &LineSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new LineSeparatorParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ParagraphSeparatorRule : public cmajor::parsing::Rule
{
public:
    ParagraphSeparatorRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ParagraphSeparatorRule>(this, &ParagraphSeparatorRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ParagraphSeparatorParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::OtherRule : public cmajor::parsing::Rule
{
public:
    OtherRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<OtherRule>(this, &OtherRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new OtherParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::ControlRule : public cmajor::parsing::Rule
{
public:
    ControlRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<ControlRule>(this, &ControlRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new ControlParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::FormatRule : public cmajor::parsing::Rule
{
public:
    FormatRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<FormatRule>(this, &FormatRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new FormatParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::SurrogateRule : public cmajor::parsing::Rule
{
public:
    SurrogateRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<SurrogateRule>(this, &SurrogateRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new SurrogateParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::PrivateUseRule : public cmajor::parsing::Rule
{
public:
    PrivateUseRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<PrivateUseRule>(this, &PrivateUseRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new PrivateUseParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::UnassignedRule : public cmajor::parsing::Rule
{
public:
    UnassignedRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<UnassignedRule>(this, &UnassignedRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new UnassignedParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::GraphicRule : public cmajor::parsing::Rule
{
public:
    GraphicRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<GraphicRule>(this, &GraphicRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new GraphicParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::BaseCharRule : public cmajor::parsing::Rule
{
public:
    BaseCharRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<BaseCharRule>(this, &BaseCharRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new BaseCharParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::AlphabeticRule : public cmajor::parsing::Rule
{
public:
    AlphabeticRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<AlphabeticRule>(this, &AlphabeticRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new AlphabeticParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::IdStartRule : public cmajor::parsing::Rule
{
public:
    IdStartRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdStartRule>(this, &IdStartRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdStartParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

class Primitive::IdContRule : public cmajor::parsing::Rule
{
public:
    IdContRule(const std::u32string& name_, Scope* enclosingScope_, int id_, Parser* definition_):
        cmajor::parsing::Rule(name_, enclosingScope_, id_, definition_)
    {
        SetValueTypeName(ToUtf32("cmajor::parsing::Parser*"));
    }
    void Enter(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData) override
    {
        parsingData->PushContext(Id(), new Context());
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
    }
    void Leave(cmajor::parsing::ObjectStack& stack, cmajor::parsing::ParsingData* parsingData, bool matched) override
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        if (matched)
        {
            stack.push(std::unique_ptr<cmajor::parsing::Object>(new cmajor::parsing::ValueObject<cmajor::parsing::Parser*>(context->value)));
        }
        parsingData->PopContext(Id());
    }
    void Link() override
    {
        cmajor::parsing::ActionParser* a0ActionParser = GetAction(ToUtf32("A0"));
        a0ActionParser->SetAction(new cmajor::parsing::MemberParsingAction<IdContRule>(this, &IdContRule::A0Action));
    }
    void A0Action(const char32_t* matchBegin, const char32_t* matchEnd, const Span& span, const std::string& fileName, ParsingData* parsingData, bool& pass)
    {
        Context* context = static_cast<Context*>(parsingData->GetContext(Id()));
        context->value = new IdContParser;
    }
private:
    struct Context : cmajor::parsing::Context
    {
        Context(): value() {}
        cmajor::parsing::Parser* value;
    };
};

void Primitive::GetReferencedGrammars()
{
    cmajor::parsing::ParsingDomain* pd = GetParsingDomain();
    cmajor::parsing::Grammar* grammar0 = pd->GetGrammar(ToUtf32("cmajor.parsing.stdlib"));
    if (!grammar0)
    {
        grammar0 = cmajor::parsing::stdlib::Create(pd);
    }
    AddGrammarReference(grammar0);
    cmajor::parsing::Grammar* grammar1 = pd->GetGrammar(ToUtf32("cmajor.syntax.Element"));
    if (!grammar1)
    {
        grammar1 = cmajor::syntax::Element::Create(pd);
    }
    AddGrammarReference(grammar1);
}

void Primitive::CreateRules()
{
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("hexuint"), this, ToUtf32("cmajor.parsing.stdlib.hexuint")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("char"), this, ToUtf32("cmajor.parsing.stdlib.char")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("string"), this, ToUtf32("cmajor.parsing.stdlib.string")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("StringArray"), this, ToUtf32("Element.StringArray")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("escape"), this, ToUtf32("cmajor.parsing.stdlib.escape")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("int"), this, ToUtf32("cmajor.parsing.stdlib.int")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("QualifiedId"), this, ToUtf32("Element.QualifiedId")));
    AddRuleLink(new cmajor::parsing::RuleLink(ToUtf32("uint"), this, ToUtf32("cmajor.parsing.stdlib.uint")));
    AddRule(new PrimitiveRule(ToUtf32("Primitive"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::AlternativeParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::AlternativeParser(
                        new cmajor::parsing::AlternativeParser(
                            new cmajor::parsing::AlternativeParser(
                                new cmajor::parsing::AlternativeParser(
                                    new cmajor::parsing::AlternativeParser(
                                        new cmajor::parsing::AlternativeParser(
                                            new cmajor::parsing::AlternativeParser(
                                                new cmajor::parsing::AlternativeParser(
                                                    new cmajor::parsing::AlternativeParser(
                                                        new cmajor::parsing::AlternativeParser(
                                                            new cmajor::parsing::AlternativeParser(
                                                                new cmajor::parsing::AlternativeParser(
                                                                    new cmajor::parsing::AlternativeParser(
                                                                        new cmajor::parsing::AlternativeParser(
                                                                            new cmajor::parsing::AlternativeParser(
                                                                                new cmajor::parsing::AlternativeParser(
                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                            new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                                new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                                    new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                                        new cmajor::parsing::AlternativeParser(
                                                                                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                                                                                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("Char"), ToUtf32("Char"), 0)),
                                                                                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                                                                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("String"), ToUtf32("String"), 0))),
                                                                                                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                                                                                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("CharSet"), ToUtf32("CharSet"), 0))),
                                                                                                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A3"),
                                                                                                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Keyword"), ToUtf32("Keyword"), 0))),
                                                                                                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A4"),
                                                                                                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("KeywordList"), ToUtf32("KeywordList"), 0))),
                                                                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A5"),
                                                                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("Range"), ToUtf32("Range"), 0))),
                                                                                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A6"),
                                                                                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Empty"), ToUtf32("Empty"), 0))),
                                                                                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A7"),
                                                                                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Space"), ToUtf32("Space"), 0))),
                                                                                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A8"),
                                                                                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("AnyChar"), ToUtf32("AnyChar"), 0))),
                                                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A9"),
                                                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("Letter"), ToUtf32("Letter"), 0))),
                                                                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A10"),
                                                                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Digit"), ToUtf32("Digit"), 0))),
                                                                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A11"),
                                                                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("HexDigit"), ToUtf32("HexDigit"), 0))),
                                                                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A12"),
                                                                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("UpperLetter"), ToUtf32("UpperLetter"), 0))),
                                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A13"),
                                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("LowerLetter"), ToUtf32("LowerLetter"), 0))),
                                                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A14"),
                                                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("TitleLetter"), ToUtf32("TitleLetter"), 0))),
                                                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A15"),
                                                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("ModifierLetter"), ToUtf32("ModifierLetter"), 0))),
                                                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A16"),
                                                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("OtherLetter"), ToUtf32("OtherLetter"), 0))),
                                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A17"),
                                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("CasedLetter"), ToUtf32("CasedLetter"), 0))),
                                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A18"),
                                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Mark"), ToUtf32("Mark"), 0))),
                                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A19"),
                                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("NonspacingMark"), ToUtf32("NonspacingMark"), 0))),
                                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A20"),
                                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("EnclosingMark"), ToUtf32("EnclosingMark"), 0))),
                                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A21"),
                                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("SpacingMark"), ToUtf32("SpacingMark"), 0))),
                                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A22"),
                                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Number"), ToUtf32("Number"), 0))),
                                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A23"),
                                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("DecimalNumber"), ToUtf32("DecimalNumber"), 0))),
                                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A24"),
                                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("LetterNumber"), ToUtf32("LetterNumber"), 0))),
                                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A25"),
                                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("OtherNumber"), ToUtf32("OtherNumber"), 0))),
                                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A26"),
                                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Punctuation"), ToUtf32("Punctuation"), 0))),
                                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A27"),
                                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("ConnectorPunctuation"), ToUtf32("ConnectorPunctuation"), 0))),
                                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A28"),
                                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("DashPunctuation"), ToUtf32("DashPunctuation"), 0))),
                                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A29"),
                                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("OpenPunctuation"), ToUtf32("OpenPunctuation"), 0))),
                                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A30"),
                                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("ClosePunctuation"), ToUtf32("ClosePunctuation"), 0))),
                                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A31"),
                                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("InitialPunctuation"), ToUtf32("InitialPunctuation"), 0))),
                                                                                                new cmajor::parsing::ActionParser(ToUtf32("A32"),
                                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("FinalPunctuation"), ToUtf32("FinalPunctuation"), 0))),
                                                                                            new cmajor::parsing::ActionParser(ToUtf32("A33"),
                                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("OtherPunctuation"), ToUtf32("OtherPunctuation"), 0))),
                                                                                        new cmajor::parsing::ActionParser(ToUtf32("A34"),
                                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("Symbol"), ToUtf32("Symbol"), 0))),
                                                                                    new cmajor::parsing::ActionParser(ToUtf32("A35"),
                                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("MathSymbol"), ToUtf32("MathSymbol"), 0))),
                                                                                new cmajor::parsing::ActionParser(ToUtf32("A36"),
                                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("CurrencySymbol"), ToUtf32("CurrencySymbol"), 0))),
                                                                            new cmajor::parsing::ActionParser(ToUtf32("A37"),
                                                                                new cmajor::parsing::NonterminalParser(ToUtf32("ModifierSymbol"), ToUtf32("ModifierSymbol"), 0))),
                                                                        new cmajor::parsing::ActionParser(ToUtf32("A38"),
                                                                            new cmajor::parsing::NonterminalParser(ToUtf32("OtherSymbol"), ToUtf32("OtherSymbol"), 0))),
                                                                    new cmajor::parsing::ActionParser(ToUtf32("A39"),
                                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Separator"), ToUtf32("Separator"), 0))),
                                                                new cmajor::parsing::ActionParser(ToUtf32("A40"),
                                                                    new cmajor::parsing::NonterminalParser(ToUtf32("SpaceSeparator"), ToUtf32("SpaceSeparator"), 0))),
                                                            new cmajor::parsing::ActionParser(ToUtf32("A41"),
                                                                new cmajor::parsing::NonterminalParser(ToUtf32("LineSeparator"), ToUtf32("LineSeparator"), 0))),
                                                        new cmajor::parsing::ActionParser(ToUtf32("A42"),
                                                            new cmajor::parsing::NonterminalParser(ToUtf32("ParagraphSeparator"), ToUtf32("ParagraphSeparator"), 0))),
                                                    new cmajor::parsing::ActionParser(ToUtf32("A43"),
                                                        new cmajor::parsing::NonterminalParser(ToUtf32("Other"), ToUtf32("Other"), 0))),
                                                new cmajor::parsing::ActionParser(ToUtf32("A44"),
                                                    new cmajor::parsing::NonterminalParser(ToUtf32("Control"), ToUtf32("Control"), 0))),
                                            new cmajor::parsing::ActionParser(ToUtf32("A45"),
                                                new cmajor::parsing::NonterminalParser(ToUtf32("Format"), ToUtf32("Format"), 0))),
                                        new cmajor::parsing::ActionParser(ToUtf32("A46"),
                                            new cmajor::parsing::NonterminalParser(ToUtf32("Surrogate"), ToUtf32("Surrogate"), 0))),
                                    new cmajor::parsing::ActionParser(ToUtf32("A47"),
                                        new cmajor::parsing::NonterminalParser(ToUtf32("PrivateUse"), ToUtf32("PrivateUse"), 0))),
                                new cmajor::parsing::ActionParser(ToUtf32("A48"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("Unassigned"), ToUtf32("Unassigned"), 0))),
                            new cmajor::parsing::ActionParser(ToUtf32("A49"),
                                new cmajor::parsing::NonterminalParser(ToUtf32("Graphic"), ToUtf32("Graphic"), 0))),
                        new cmajor::parsing::ActionParser(ToUtf32("A50"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("BaseChar"), ToUtf32("BaseChar"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A51"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("Alphabetic"), ToUtf32("Alphabetic"), 0))),
                new cmajor::parsing::ActionParser(ToUtf32("A52"),
                    new cmajor::parsing::NonterminalParser(ToUtf32("IdStart"), ToUtf32("IdStart"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A53"),
                new cmajor::parsing::NonterminalParser(ToUtf32("IdCont"), ToUtf32("IdCont"), 0)))));
    AddRule(new CharRule(ToUtf32("Char"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("chr"), ToUtf32("char"), 0))));
    AddRule(new StringRule(ToUtf32("String"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0))));
    AddRule(new CharSetRule(ToUtf32("CharSet"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::CharParser('['),
                            new cmajor::parsing::OptionalParser(
                                new cmajor::parsing::GroupingParser(
                                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                                        new cmajor::parsing::CharParser('^'))))),
                        new cmajor::parsing::KleeneStarParser(
                            new cmajor::parsing::GroupingParser(
                                new cmajor::parsing::ActionParser(ToUtf32("A2"),
                                    new cmajor::parsing::NonterminalParser(ToUtf32("CharSetRange"), ToUtf32("CharSetRange"), 0))))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(']')))))));
    AddRule(new CharSetRangeRule(ToUtf32("CharSetRange"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("first"), ToUtf32("CharSetChar"), 0),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser('-'),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::NonterminalParser(ToUtf32("last"), ToUtf32("CharSetChar"), 0))),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::EmptyParser()))))));
    AddRule(new CharSetCharRule(ToUtf32("CharSetChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::CharSetParser(ToUtf32("\\]"), true)),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("escape"), ToUtf32("escape"), 0)))));
    AddRule(new KeywordRule(ToUtf32("Keyword"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("keyword")),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('('))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("KeywordBody"), ToUtf32("KeywordBody"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new KeywordBodyRule(ToUtf32("KeywordBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::NonterminalParser(ToUtf32("str"), ToUtf32("string"), 0),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::AlternativeParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::CharParser(','),
                        new cmajor::parsing::ActionParser(ToUtf32("A0"),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::NonterminalParser(ToUtf32("continuation"), ToUtf32("QualifiedId"), 0)))),
                    new cmajor::parsing::ActionParser(ToUtf32("A1"),
                        new cmajor::parsing::EmptyParser()))))));
    AddRule(new KeywordListRule(ToUtf32("KeywordList"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::KeywordParser(ToUtf32("keyword_list")),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser('('))),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("KeywordListBody"), ToUtf32("KeywordListBody"), 0))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(')'))))));
    AddRule(new KeywordListBodyRule(ToUtf32("KeywordListBody"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::SequenceParser(
            new cmajor::parsing::SequenceParser(
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("selector"), ToUtf32("QualifiedId"), 0)),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::CharParser(','))),
            new cmajor::parsing::ActionParser(ToUtf32("A0"),
                new cmajor::parsing::ExpectationParser(
                    new cmajor::parsing::NonterminalParser(ToUtf32("StringArray"), ToUtf32("StringArray"), 1))))));
    AddRule(new RangeRule(ToUtf32("Range"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::GroupingParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::SequenceParser(
                        new cmajor::parsing::SequenceParser(
                            new cmajor::parsing::SequenceParser(
                                new cmajor::parsing::SequenceParser(
                                    new cmajor::parsing::KeywordParser(ToUtf32("range")),
                                    new cmajor::parsing::ExpectationParser(
                                        new cmajor::parsing::CharParser('('))),
                                new cmajor::parsing::ExpectationParser(
                                    new cmajor::parsing::NonterminalParser(ToUtf32("s"), ToUtf32("CodePoint"), 0))),
                            new cmajor::parsing::ExpectationParser(
                                new cmajor::parsing::CharParser(','))),
                        new cmajor::parsing::ExpectationParser(
                            new cmajor::parsing::NonterminalParser(ToUtf32("e"), ToUtf32("CodePoint"), 0))),
                    new cmajor::parsing::ExpectationParser(
                        new cmajor::parsing::CharParser(')')))))));
    AddRule(new CodePointRule(ToUtf32("CodePoint"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::AlternativeParser(
            new cmajor::parsing::TokenParser(
                new cmajor::parsing::SequenceParser(
                    new cmajor::parsing::CharParser('x'),
                    new cmajor::parsing::ActionParser(ToUtf32("A0"),
                        new cmajor::parsing::NonterminalParser(ToUtf32("x"), ToUtf32("hexuint"), 0)))),
            new cmajor::parsing::ActionParser(ToUtf32("A1"),
                new cmajor::parsing::NonterminalParser(ToUtf32("d"), ToUtf32("uint"), 0)))));
    AddRule(new EmptyRule(ToUtf32("Empty"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("empty")))));
    AddRule(new SpaceRule(ToUtf32("Space"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("space")))));
    AddRule(new AnyCharRule(ToUtf32("AnyChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("anychar")))));
    AddRule(new LetterRule(ToUtf32("Letter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("letter")))));
    AddRule(new DigitRule(ToUtf32("Digit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("digit")))));
    AddRule(new HexDigitRule(ToUtf32("HexDigit"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("hexdigit")))));
    AddRule(new UpperLetterRule(ToUtf32("UpperLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("upper_letter")))));
    AddRule(new LowerLetterRule(ToUtf32("LowerLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("lower_letter")))));
    AddRule(new TitleLetterRule(ToUtf32("TitleLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("title_letter")))));
    AddRule(new ModifierLetterRule(ToUtf32("ModifierLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("modifier_letter")))));
    AddRule(new OtherLetterRule(ToUtf32("OtherLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("other_letter")))));
    AddRule(new CasedLetterRule(ToUtf32("CasedLetter"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("cased_letter")))));
    AddRule(new MarkRule(ToUtf32("Mark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("mark")))));
    AddRule(new NonspacingMarkRule(ToUtf32("NonspacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("nonspacing_mark")))));
    AddRule(new SpacingMarkRule(ToUtf32("SpacingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("spacing_mark")))));
    AddRule(new EnclosingMarkRule(ToUtf32("EnclosingMark"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("enclosing_mark")))));
    AddRule(new NumberRule(ToUtf32("Number"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("number")))));
    AddRule(new DecimalNumberRule(ToUtf32("DecimalNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("decimal_number")))));
    AddRule(new LetterNumberRule(ToUtf32("LetterNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("letter_number")))));
    AddRule(new OtherNumberRule(ToUtf32("OtherNumber"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("other_number")))));
    AddRule(new PunctuationRule(ToUtf32("Punctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("punctuation")))));
    AddRule(new ConnectorPunctuationRule(ToUtf32("ConnectorPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("connector_punctuation")))));
    AddRule(new DashPunctuationRule(ToUtf32("DashPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("dash_punctuation")))));
    AddRule(new OpenPunctuationRule(ToUtf32("OpenPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("open_punctuation")))));
    AddRule(new ClosePunctuationRule(ToUtf32("ClosePunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("close_punctuation")))));
    AddRule(new InitialPunctuationRule(ToUtf32("InitialPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("initial_punctuation")))));
    AddRule(new FinalPunctuationRule(ToUtf32("FinalPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("final_punctuation")))));
    AddRule(new OtherPunctuationRule(ToUtf32("OtherPunctuation"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("other_punctuation")))));
    AddRule(new SymbolRule(ToUtf32("Symbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("symbol")))));
    AddRule(new MathSymbolRule(ToUtf32("MathSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("math_symbol")))));
    AddRule(new CurrencySymbolRule(ToUtf32("CurrencySymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("currency_symbol")))));
    AddRule(new ModifierSymbolRule(ToUtf32("ModifierSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("modifier_symbol")))));
    AddRule(new OtherSymbolRule(ToUtf32("OtherSymbol"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("other_symbol")))));
    AddRule(new SeparatorRule(ToUtf32("Separator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("separator")))));
    AddRule(new SpaceSeparatorRule(ToUtf32("SpaceSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("space_separator")))));
    AddRule(new LineSeparatorRule(ToUtf32("LineSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("line_separator")))));
    AddRule(new ParagraphSeparatorRule(ToUtf32("ParagraphSeparator"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("paragraph_separator")))));
    AddRule(new OtherRule(ToUtf32("Other"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("other")))));
    AddRule(new ControlRule(ToUtf32("Control"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("control")))));
    AddRule(new FormatRule(ToUtf32("Format"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("format")))));
    AddRule(new SurrogateRule(ToUtf32("Surrogate"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("surrogate")))));
    AddRule(new PrivateUseRule(ToUtf32("PrivateUse"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("private_use")))));
    AddRule(new UnassignedRule(ToUtf32("Unassigned"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("unassigned")))));
    AddRule(new GraphicRule(ToUtf32("Graphic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("graphic")))));
    AddRule(new BaseCharRule(ToUtf32("BaseChar"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("basechar")))));
    AddRule(new AlphabeticRule(ToUtf32("Alphabetic"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("alphabetic")))));
    AddRule(new IdStartRule(ToUtf32("IdStart"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("idstart")))));
    AddRule(new IdContRule(ToUtf32("IdCont"), GetScope(), GetParsingDomain()->GetNextRuleId(),
        new cmajor::parsing::ActionParser(ToUtf32("A0"),
            new cmajor::parsing::KeywordParser(ToUtf32("idcont")))));
}

} } // namespace cmajor.syntax
