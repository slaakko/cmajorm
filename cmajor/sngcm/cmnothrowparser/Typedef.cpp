#include "Typedef.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Typedef.parser' using soulng parser generator spg version 3.10.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowTypedefParser::Typedef(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Typedef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> type;
    std::unique_ptr<IdentifierNode> id;
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
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                        specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                        if (match.hit)
                        {
                            s = span;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == TYPEDEF)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                {
                    soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                    type.reset(static_cast<sngcm::ast::Node*>(match.value));
                    *parentMatch6 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch7 = &match;
            {
                soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                id.reset(static_cast<IdentifierNode*>(match.value));
                *parentMatch7 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match(false);
                    if (*lexer == SEMICOLON)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        *parentMatch10 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch10 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                    }
                }
                if (match.hit)
                {
                    s.end = span.end;
                    TypedefNode * node = new TypedefNode(s, *moduleId, specifiers->value, type.release(), id.release());
                    node->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Typedef"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, node);
                    }
                }
                *parentMatch9 = match;
            }
            *parentMatch8 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Typedef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Typedef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
