#include "Enumeration.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmparser/Specifier.hpp>
#include <sngcm/cmparser/Identifier.hpp>
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmparser/Expression.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/sngcm/cmparser/Enumeration.parser' using soulng parser generator spg version 3.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorTokens;

soulng::parser::Match EnumerationParser::EnumType(CmajorLexer& lexer, ParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EnumType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<EnumTypeNode> enumTypeNode = std::unique_ptr<EnumTypeNode>();
    Span s = Span();
    Span beginBraceSpan = Span();
    Span endBraceSpan = Span();
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> enumTypeId;
    std::unique_ptr<Node> underlyingType;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match = SpecifierParser::Specifiers(lexer);
                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                if (match.hit)
                                {
                                    s = span;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == ENUM)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch9 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch10 = &match;
                                {
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match = IdentifierParser::Identifier(lexer);
                                    enumTypeId.reset(static_cast<IdentifierNode*>(match.value));
                                    if (match.hit)
                                    {
                                        *parentMatch10 = match;
                                    }
                                    else
                                    {
                                        lexer.ThrowExpectationFailure(span, U"identifier");
                                    }
                                }
                                if (match.hit)
                                {
                                    s.end = span.end;
                                    enumTypeNode.reset(new EnumTypeNode(s, specifiers->value, enumTypeId.release()));
                                }
                                *parentMatch9 = match;
                            }
                            *parentMatch8 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch11 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch12 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch13 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch14 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = EnumerationParser::UnderlyingType(lexer, ctx);
                                    underlyingType.reset(static_cast<Node*>(match.value));
                                    if (match.hit)
                                    {
                                        enumTypeNode->SetUnderlyingType(underlyingType.release());
                                    }
                                    *parentMatch14 = match;
                                }
                                *parentMatch13 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch12 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch11 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch15 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch16 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch17 = &match;
                        {
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match(false);
                            if (*lexer == LBRACE)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                *parentMatch17 = match;
                            }
                            else
                            {
                                lexer.ThrowExpectationFailure(span, ToUtf32(GetTokenInfo(LBRACE)));
                            }
                        }
                        if (match.hit)
                        {
                            beginBraceSpan = span;
                        }
                        *parentMatch16 = match;
                    }
                    *parentMatch15 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch18 = &match;
            {
                soulng::parser::Match match = EnumerationParser::EnumConstants(lexer, ctx, enumTypeNode.get());
                *parentMatch18 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch19 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch20 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch21 = &match;
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
                        *parentMatch21 = match;
                    }
                    else
                    {
                        lexer.ThrowExpectationFailure(span, ToUtf32(GetTokenInfo(RBRACE)));
                    }
                }
                if (match.hit)
                {
                    endBraceSpan = span;
                    enumTypeNode->SetBeginBraceSpan(beginBraceSpan);
                    enumTypeNode->SetEndBraceSpan(endBraceSpan);
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumType"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, enumTypeNode.release());
                    }
                }
                *parentMatch20 = match;
            }
            *parentMatch19 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EnumType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match EnumerationParser::UnderlyingType(CmajorLexer& lexer, ParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("UnderlyingType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<sngcm::ast::Node> type;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        if (*lexer == COLON)
        {
            ++lexer;
            match.hit = true;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match = TypeExprParser::TypeExpr(lexer, ctx);
                type.reset(static_cast<sngcm::ast::Node*>(match.value));
                if (match.hit)
                {
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("UnderlyingType"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, type.release());
                    }
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("UnderlyingType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("UnderlyingType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match EnumerationParser::EnumConstants(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EnumConstants"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<EnumConstantNode> enumConstant;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match = EnumerationParser::EnumConstant(lexer, ctx, enumType);
                enumConstant.reset(static_cast<EnumConstantNode*>(match.value));
                if (match.hit)
                {
                    *parentMatch2 = match;
                }
                else
                {
                    lexer.ThrowExpectationFailure(span, U"enumeration constant");
                }
            }
            if (match.hit)
            {
                enumType->AddConstant(enumConstant.release());
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch3 = &match;
        {
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch4 = &match;
            {
                while (true)
                {
                    int64_t save = lexer.GetPos();
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == COMMA)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch8 = &match;
                                    {
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = EnumerationParser::EnumConstant(lexer, ctx, enumType);
                                        enumConstant.reset(static_cast<EnumConstantNode*>(match.value));
                                        if (match.hit)
                                        {
                                            *parentMatch8 = match;
                                        }
                                        else
                                        {
                                            lexer.ThrowExpectationFailure(span, U"enumeration constant");
                                        }
                                    }
                                    if (match.hit)
                                    {
                                        enumType->AddConstant(enumConstant.release());
                                    }
                                    *parentMatch7 = match;
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
                            break;
                        }
                    }
                }
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstants"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EnumConstants"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match EnumerationParser::EnumConstant(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EnumConstant"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<Node> expr = std::unique_ptr<Node>();
    std::unique_ptr<IdentifierNode> constantId;
    std::unique_ptr<Node> constantValue;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::lexer::Span span = lexer.GetSpan();
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match = IdentifierParser::Identifier(lexer);
                constantId.reset(static_cast<IdentifierNode*>(match.value));
                if (match.hit)
                {
                    *parentMatch2 = match;
                }
                else
                {
                    lexer.ThrowExpectationFailure(span, U"identifier");
                }
            }
            if (match.hit)
            {
                s = span;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch3 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch4 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch5 = &match;
                {
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch6 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == ASSIGN)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch6 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch8 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = ExpressionParser::Expression(lexer, ctx);
                                constantValue.reset(static_cast<Node*>(match.value));
                                if (match.hit)
                                {
                                    expr.reset(constantValue.release());
                                    expr->SetFullSpan();
                                    Span v = expr->GetSpan();
                                    s.end = expr->GetSpan().end;
                                    EnumConstantNode * value = new EnumConstantNode(s, constantId.release(), expr.release());
                                    value->SetHasValue();
                                    value->SetStrValue(lexer.GetMatch(v));
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstant"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, value);
                                    }
                                }
                                *parentMatch8 = match;
                            }
                            *parentMatch7 = match;
                        }
                        *parentMatch6 = match;
                    }
                    *parentMatch5 = match;
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
                                soulng::parser::Match match(true);
                                if (match.hit)
                                {
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstant"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, new EnumConstantNode(s, constantId.release(), MakeNextEnumConstantValue(s, enumType)));
                                    }
                                }
                                *parentMatch10 = match;
                            }
                            *parentMatch9 = match;
                        }
                        *parentMatch5 = match;
                    }
                }
                *parentMatch4 = match;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstant"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EnumConstant"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
