#include "Interface.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmparser/Attribute.hpp>
#include <sngcm/cmparser/Specifier.hpp>
#include <sngcm/cmparser/Identifier.hpp>
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmparser/Parameter.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmparser/Interface.parser' using soulng parser generator spg version 3.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorTokens;

soulng::parser::Match InterfaceParser::Interface(CmajorLexer& lexer, ParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Interface"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<InterfaceNode> intf = std::unique_ptr<InterfaceNode>();
    Span s = Span();
    Span specifierSpan = Span();
    Span beginBraceSpan = Span();
    Span endBraceSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> id;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
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
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
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
                                            if (match.hit)
                                            {
                                                s = span;
                                            }
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
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
                                                soulng::parser::Match match = AttributeParser::Attributes(lexer);
                                                attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
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
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch12 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch13 = &match;
                                        {
                                            int64_t pos = lexer.GetPos();
                                            soulng::lexer::Span span = lexer.GetSpan();
                                            soulng::parser::Match match = SpecifierParser::Specifiers(lexer);
                                            specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                            if (match.hit)
                                            {
                                                specifierSpan = span;
                                            }
                                            *parentMatch13 = match;
                                        }
                                        *parentMatch12 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch14 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == INTERFACE)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch14 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
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
                                        soulng::parser::Match match = IdentifierParser::Identifier(lexer);
                                        id.reset(static_cast<IdentifierNode*>(match.value));
                                        if (match.hit)
                                        {
                                            *parentMatch17 = match;
                                        }
                                        else
                                        {
                                            lexer.ThrowExpectationFailure(span, U"identifier");
                                        }
                                    }
                                    if (match.hit)
                                    {
                                        s.end = span.end;
                                        intf.reset(new InterfaceNode(s, specifiers->value, id.release(), attrs.release()));
                                        intf->SetSpecifierSpan(specifierSpan);
                                    }
                                    *parentMatch16 = match;
                                }
                                *parentMatch15 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
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
                                    if (*lexer == LBRACE)
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
                                        lexer.ThrowExpectationFailure(span, ToUtf32(GetTokenInfo(LBRACE)));
                                    }
                                }
                                if (match.hit)
                                {
                                    beginBraceSpan = span;
                                }
                                *parentMatch19 = match;
                            }
                            *parentMatch18 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch21 = &match;
                    {
                        soulng::parser::Match match = InterfaceParser::InterfaceContent(lexer, ctx, intf.get());
                        *parentMatch21 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch22 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch23 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch24 = &match;
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
                                *parentMatch24 = match;
                            }
                            else
                            {
                                lexer.ThrowExpectationFailure(span, ToUtf32(GetTokenInfo(RBRACE)));
                            }
                        }
                        if (match.hit)
                        {
                            endBraceSpan = span;
                            intf->SetBeginBraceSpan(beginBraceSpan);
                            intf->SetEndBraceSpan(endBraceSpan);
                        }
                        *parentMatch23 = match;
                    }
                    *parentMatch22 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Interface"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, intf.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Interface"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Interface"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match InterfaceParser::InterfaceContent(CmajorLexer& lexer, ParsingContext* ctx, sngcm::ast::InterfaceNode* intf)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("InterfaceContent"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<Node> intfMemFun;
    soulng::parser::Match match(true);
    soulng::parser::Match* parentMatch0 = &match;
    {
        while (true)
        {
            int64_t save = lexer.GetPos();
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch1 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch2 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = InterfaceParser::InterfaceMemFun(lexer, ctx);
                        intfMemFun.reset(static_cast<Node*>(match.value));
                        if (match.hit)
                        {
                            intf->AddMember(intfMemFun.release());
                        }
                        *parentMatch2 = match;
                    }
                    *parentMatch1 = match;
                }
                if (match.hit)
                {
                    *parentMatch0 = match;
                }
                else
                {
                    lexer.SetPos(save);
                    break;
                }
            }
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InterfaceContent"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("InterfaceContent"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match InterfaceParser::InterfaceMemFun(CmajorLexer& lexer, ParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("InterfaceMemFun"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<MemberFunctionNode> memFun = std::unique_ptr<MemberFunctionNode>();
    Span s = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<sngcm::ast::Node> returnType;
    std::unique_ptr<soulng::parser::Value<std::u32string>> groupId;
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
                            int64_t pos = lexer.GetPos();
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match(true);
                            if (match.hit)
                            {
                                s = span;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch6 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                soulng::parser::Match match = AttributeParser::Attributes(lexer);
                                attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
                                if (match.hit)
                                {
                                    *parentMatch7 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch8 = &match;
                    {
                        soulng::parser::Match match = TypeExprParser::TypeExpr(lexer, ctx);
                        returnType.reset(static_cast<sngcm::ast::Node*>(match.value));
                        *parentMatch8 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch9 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch10 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match = InterfaceParser::InterfaceFunctionGroupId(lexer);
                            groupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            if (match.hit)
                            {
                                *parentMatch11 = match;
                            }
                            else
                            {
                                lexer.ThrowExpectationFailure(span, U"interface function group identifier");
                            }
                        }
                        if (match.hit)
                        {
                            s.end = span.end;
                            memFun.reset(new MemberFunctionNode(s, Specifiers(), returnType.release(), groupId->value, attrs.release()));
                        }
                        *parentMatch10 = match;
                    }
                    *parentMatch9 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch12 = &match;
            {
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch13 = &match;
                {
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = ParameterParser::ParameterList(lexer, ctx, memFun.get());
                    if (match.hit)
                    {
                        *parentMatch13 = match;
                    }
                    else
                    {
                        lexer.ThrowExpectationFailure(span, U"parameter list");
                    }
                }
                *parentMatch12 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch16 = &match;
                {
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == SEMICOLON)
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
                        lexer.ThrowExpectationFailure(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                    }
                }
                if (match.hit)
                {
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InterfaceMemFun"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, memFun.release());
                    }
                }
                *parentMatch15 = match;
            }
            *parentMatch14 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InterfaceMemFun"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("InterfaceMemFun"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match InterfaceParser::InterfaceFunctionGroupId(CmajorLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("InterfaceFunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::lexer::Span span = lexer.GetSpan();
        soulng::parser::Match match(false);
        if (*lexer == ID)
        {
            ++lexer;
            match.hit = true;
        }
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InterfaceFunctionGroupId"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(lexer.GetMatch(span)));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InterfaceFunctionGroupId"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("InterfaceFunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
