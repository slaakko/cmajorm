#include "Literal.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Literal.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowLiteralParser::Literal(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Literal"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 113);
    std::unique_ptr<LiteralNode> simpleLiteral;
    std::unique_ptr<LiteralNode> complexLiteral;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = NothrowLiteralParser::SimpleLiteral(lexer, moduleId);
            simpleLiteral.reset(static_cast<LiteralNode*>(match.value));
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Literal"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, simpleLiteral.release());
                }
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowLiteralParser::ComplexLiteral(lexer, moduleId, ctx);
                    complexLiteral.reset(static_cast<LiteralNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Literal"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, complexLiteral.release());
                        }
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Literal"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Literal"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowLiteralParser::SimpleLiteral(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("SimpleLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 114);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t save = lexer.GetPos();
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        int64_t save = lexer.GetPos();
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::SourcePos sourcePos = lexer.GetSourcePos(pos);
                        soulng::lexer::Span span = lexer.GetSpan();
                        switch (*lexer)
                        {
                            case TRUE:
                            {
                                ++lexer;
                                {
                                    LiteralNode * value = new BooleanLiteralNode(span, *moduleId, true);
                                    value->SetText(lexer.GetMatch(span));
                                    value->SetLexerFlags(lexer.Flags());
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, value);
                                    }
                                }
                                break;
                            }
                            case FALSE:
                            {
                                ++lexer;
                                {
                                    LiteralNode * value = new BooleanLiteralNode(span, *moduleId, false);
                                    value->SetText(lexer.GetMatch(span));
                                    value->SetLexerFlags(lexer.Flags());
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, value);
                                    }
                                }
                                break;
                            }
                        }
                        *parentMatch4 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch5 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    bool pass = true;
                                    soulng::parser::Match match(false);
                                    if (*lexer == FLOATINGLIT)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    if (match.hit)
                                    {
                                        if (!ParseFloatingLiteralNothrow(lexer.FileName(), lexer.GetToken(pos), lexer.floatingLit, lexer.floatingLitFloat)) pass = false;
                                        else
                                        {
                                            LiteralNode * value = CreateFloatingLiteralNode(span, *moduleId, lexer.floatingLit, lexer.floatingLitFloat);
                                            value->SetText(lexer.GetMatch(span));
                                            value->SetLexerFlags(lexer.Flags());
                                            {
                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                return soulng::parser::Match(true, value);
                                            }
                                        }
                                    }
                                    if (match.hit && !pass)
                                    {
                                        match = soulng::parser::Match(false);
                                    }
                                    *parentMatch6 = match;
                                }
                                *parentMatch5 = match;
                            }
                            *parentMatch4 = match;
                        }
                    }
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch8 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                bool pass = true;
                                soulng::parser::Match match(false);
                                if (*lexer == INTLIT)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                if (match.hit)
                                {
                                    if (!ParseIntegerLiteralNothrow(lexer.FileName(), lexer.GetToken(pos), lexer.intLit, lexer.intLitUnsigned)) pass = false;
                                    else
                                    {
                                        LiteralNode * value = CreateIntegerLiteralNode(span, *moduleId, lexer.intLit, lexer.intLitUnsigned);
                                        value->SetText(lexer.GetMatch(span));
                                        value->SetLexerFlags(lexer.Flags());
                                        {
                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                            return soulng::parser::Match(true, value);
                                        }
                                    }
                                }
                                if (match.hit && !pass)
                                {
                                    match = soulng::parser::Match(false);
                                }
                                *parentMatch8 = match;
                            }
                            *parentMatch7 = match;
                        }
                        *parentMatch3 = match;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::lexer::Span span = lexer.GetSpan();
                            bool pass = true;
                            soulng::parser::Match match(false);
                            if (*lexer == CHARLIT)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                if (!ParseCharacterLiteralNothrow(lexer.FileName(), lexer.GetToken(pos), lexer.chrLit, lexer.chrLitPrefix)) pass = false;
                                else
                                {
                                    LiteralNode * value = CreateCharacterLiteralNode(span, *moduleId, lexer.chrLit, lexer.chrLitPrefix);
                                    value->SetText(lexer.GetMatch(span));
                                    value->SetLexerFlags(lexer.Flags());
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, value);
                                    }
                                }
                            }
                            if (match.hit && !pass)
                            {
                                match = soulng::parser::Match(false);
                            }
                            *parentMatch10 = match;
                        }
                        *parentMatch9 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch11 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch12 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        bool pass = true;
                        soulng::parser::Match match(false);
                        if (*lexer == STRINGLIT)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        if (match.hit)
                        {
                            if (!ParseStringLiteralNothrow(lexer.FileName(), lexer.GetToken(pos), lexer.strLit, lexer.strLitPrefix)) pass = false;
                            else
                            {
                                LiteralNode * value = CreateStringLiteralNode(span, *moduleId, lexer.strLit, lexer.strLitPrefix);
                                value->SetText(lexer.GetMatch(span));
                                value->SetLexerFlags(lexer.Flags());
                                {
                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                    return soulng::parser::Match(true, value);
                                }
                            }
                        }
                        if (match.hit && !pass)
                        {
                            match = soulng::parser::Match(false);
                        }
                        *parentMatch12 = match;
                    }
                    *parentMatch11 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch13 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch14 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == NULLLIT)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        LiteralNode * value = new NullLiteralNode(span, *moduleId);
                        value->SetText(lexer.GetMatch(span));
                        value->SetLexerFlags(lexer.Flags());
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, value);
                        }
                    }
                    *parentMatch14 = match;
                }
                *parentMatch13 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SimpleLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("SimpleLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowLiteralParser::ComplexLiteral(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ComplexLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 115);
    std::unique_ptr<ArrayLiteralNode> arrayLiteral;
    std::unique_ptr<StructuredLiteralNode> structuredLiteral;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = NothrowLiteralParser::ArrayLiteral(lexer, moduleId, ctx);
            arrayLiteral.reset(static_cast<ArrayLiteralNode*>(match.value));
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ComplexLiteral"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, arrayLiteral.release());
                }
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowLiteralParser::StructuredLiteral(lexer, moduleId, ctx);
                    structuredLiteral.reset(static_cast<StructuredLiteralNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ComplexLiteral"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, structuredLiteral.release());
                        }
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ComplexLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ComplexLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowLiteralParser::ArrayLiteral(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ArrayLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 116);
    std::unique_ptr<ArrayLiteralNode> arrayLiteral = std::unique_ptr<ArrayLiteralNode>();
    std::unique_ptr<Node> expr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(false);
                if (*lexer == LBRACKET)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    arrayLiteral.reset(new ArrayLiteralNode(span, *moduleId));
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch6 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                expr.reset(static_cast<Node*>(match.value));
                                if (match.hit)
                                {
                                    arrayLiteral->AddValue(expr.release());
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch8 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch9 = &match;
                                {
                                    while (true)
                                    {
                                        int64_t save = lexer.GetPos();
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                if (*lexer == COMMA)
                                                {
                                                    ++lexer;
                                                    match.hit = true;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch11 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch12 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                                        expr.reset(static_cast<Node*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            arrayLiteral->AddValue(expr.release());
                                                        }
                                                        *parentMatch12 = match;
                                                    }
                                                    *parentMatch11 = match;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            if (match.hit)
                                            {
                                                *parentMatch9 = match;
                                            }
                                            else
                                            {
                                                lexer.SetPos(save);
                                                break;
                                            }
                                        }
                                    }
                                }
                                *parentMatch8 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch5 = match;
                    }
                    if (match.hit)
                    {
                        *parentMatch4 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch3 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch13 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch14 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch15 = &match;
                {
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == RBRACKET)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        lexer.ResetRecovered();
                        *parentMatch15 = match;
                    }
                    else
                    {
                        match.hit = true;
                        lexer.SetRecovered();
                        *parentMatch15 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACKET)));
                    }
                }
                if (match.hit)
                {
                    arrayLiteral->SetSpanEnd(span.end);
                    arrayLiteral->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArrayLiteral"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, arrayLiteral.release());
                    }
                }
                *parentMatch14 = match;
            }
            *parentMatch13 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArrayLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ArrayLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowLiteralParser::StructuredLiteral(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("StructuredLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 117);
    std::unique_ptr<StructuredLiteralNode> structuredLiteral = std::unique_ptr<StructuredLiteralNode>();
    std::unique_ptr<Node> expr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(false);
                if (*lexer == LBRACE)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    structuredLiteral.reset(new StructuredLiteralNode(span, *moduleId));
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch6 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                expr.reset(static_cast<Node*>(match.value));
                                if (match.hit)
                                {
                                    structuredLiteral->AddMember(expr.release());
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch8 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch9 = &match;
                                {
                                    while (true)
                                    {
                                        int64_t save = lexer.GetPos();
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                if (*lexer == COMMA)
                                                {
                                                    ++lexer;
                                                    match.hit = true;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch11 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch12 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                                        expr.reset(static_cast<Node*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            structuredLiteral->AddMember(expr.release());
                                                        }
                                                        *parentMatch12 = match;
                                                    }
                                                    *parentMatch11 = match;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            if (match.hit)
                                            {
                                                *parentMatch9 = match;
                                            }
                                            else
                                            {
                                                lexer.SetPos(save);
                                                break;
                                            }
                                        }
                                    }
                                }
                                *parentMatch8 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch5 = match;
                    }
                    if (match.hit)
                    {
                        *parentMatch4 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch3 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch13 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch14 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch15 = &match;
                {
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == RBRACE)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        lexer.ResetRecovered();
                        *parentMatch15 = match;
                    }
                    else
                    {
                        match.hit = true;
                        lexer.SetRecovered();
                        *parentMatch15 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACE)));
                    }
                }
                if (match.hit)
                {
                    structuredLiteral->SetSpanEnd(span.end);
                    structuredLiteral->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StructuredLiteral"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, structuredLiteral.release());
                    }
                }
                *parentMatch14 = match;
            }
            *parentMatch13 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StructuredLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("StructuredLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
