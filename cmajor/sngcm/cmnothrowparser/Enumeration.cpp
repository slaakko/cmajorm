#include "Enumeration.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Enumeration.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowEnumerationParser::EnumType(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
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
    soulng::lexer::RuleGuard ruleGuard(lexer, 70);
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
                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
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
                                soulng::parser::Match match = NothrowEnumerationParser::DefiningEnumTypeId(lexer, moduleId);
                                enumTypeId.reset(static_cast<IdentifierNode*>(match.value));
                                if (match.hit)
                                {
                                    s.end = span.end;
                                    enumTypeNode.reset(new EnumTypeNode(s, *moduleId, specifiers->value, enumTypeId.release()));
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
                    soulng::parser::Match* parentMatch10 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch12 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch13 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowEnumerationParser::UnderlyingType(lexer, moduleId, ctx);
                                    underlyingType.reset(static_cast<Node*>(match.value));
                                    if (match.hit)
                                    {
                                        enumTypeNode->SetUnderlyingType(underlyingType.release());
                                    }
                                    *parentMatch13 = match;
                                }
                                *parentMatch12 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch11 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch10 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch14 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch15 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch16 = &match;
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
                                *parentMatch16 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch16 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(LBRACE)));
                            }
                        }
                        if (match.hit)
                        {
                            beginBraceSpan = span;
                        }
                        *parentMatch15 = match;
                    }
                    *parentMatch14 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch17 = &match;
            {
                soulng::parser::Match match = NothrowEnumerationParser::EnumConstants(lexer, moduleId, ctx, enumTypeNode.get());
                *parentMatch17 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch18 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch19 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch20 = &match;
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
                        *parentMatch20 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch20 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACE)));
                    }
                }
                if (match.hit)
                {
                    endBraceSpan = span;
                    enumTypeNode->SetBeginBraceSpan(beginBraceSpan);
                    enumTypeNode->SetEndBraceSpan(endBraceSpan);
                    enumTypeNode->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumType"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, enumTypeNode.release());
                    }
                }
                *parentMatch19 = match;
            }
            *parentMatch18 = match;
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

soulng::parser::Match NothrowEnumerationParser::DefiningEnumTypeId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DefiningEnumTypeId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 71);
    std::unique_ptr<IdentifierNode> enumTypeId;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
        enumTypeId.reset(static_cast<IdentifierNode*>(match.value));
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DefiningEnumTypeId"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, enumTypeId.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DefiningEnumTypeId"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DefiningEnumTypeId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowEnumerationParser::UnderlyingType(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
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
    soulng::lexer::RuleGuard ruleGuard(lexer, 72);
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
                soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
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

soulng::parser::Match NothrowEnumerationParser::EnumConstants(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType)
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
    soulng::lexer::RuleGuard ruleGuard(lexer, 73);
    std::unique_ptr<EnumConstantNode> enumConstant;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = NothrowEnumerationParser::EnumConstant(lexer, moduleId, ctx, enumType);
            enumConstant.reset(static_cast<EnumConstantNode*>(match.value));
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
        soulng::parser::Match* parentMatch2 = &match;
        {
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch3 = &match;
            {
                while (true)
                {
                    int64_t save = lexer.GetPos();
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch4 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == COMMA)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            *parentMatch4 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch5 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowEnumerationParser::EnumConstant(lexer, moduleId, ctx, enumType);
                                    enumConstant.reset(static_cast<EnumConstantNode*>(match.value));
                                    if (match.hit)
                                    {
                                        enumType->AddConstant(enumConstant.release());
                                    }
                                    *parentMatch6 = match;
                                }
                                *parentMatch5 = match;
                            }
                            *parentMatch4 = match;
                        }
                        if (match.hit)
                        {
                            *parentMatch3 = match;
                        }
                        else
                        {
                            lexer.SetPos(save);
                            break;
                        }
                    }
                }
            }
            *parentMatch2 = match;
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

soulng::parser::Match NothrowEnumerationParser::EnumConstant(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::EnumTypeNode* enumType)
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
    soulng::lexer::RuleGuard ruleGuard(lexer, 74);
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
            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
            constantId.reset(static_cast<IdentifierNode*>(match.value));
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
        soulng::parser::Match* parentMatch2 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == ASSIGN)
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
                                soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                                constantValue.reset(static_cast<Node*>(match.value));
                                if (match.hit)
                                {
                                    expr.reset(constantValue.release());
                                    expr->SetFullSpan();
                                    Span v = expr->GetSpan();
                                    s.end = expr->GetSpan().end;
                                    EnumConstantNode * value = new EnumConstantNode(s, *moduleId, constantId.release(), expr.release());
                                    value->SetHasValue();
                                    value->SetStrValue(lexer.GetMatch(v));
                                    value->SetLexerFlags(lexer.Flags());
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstant"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, value);
                                    }
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch9 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match(true);
                                if (match.hit)
                                {
                                    EnumConstantNode * node = new EnumConstantNode(s, *moduleId, constantId.release(), MakeNextEnumConstantValue(s, *moduleId, enumType));
                                    node->SetLexerFlags(lexer.Flags());
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumConstant"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, node);
                                    }
                                }
                                *parentMatch9 = match;
                            }
                            *parentMatch8 = match;
                        }
                        *parentMatch4 = match;
                    }
                }
                *parentMatch3 = match;
            }
            *parentMatch2 = match;
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
