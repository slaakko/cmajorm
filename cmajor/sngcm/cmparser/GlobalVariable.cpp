#include "GlobalVariable.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmparser/Specifier.hpp>
#include <sngcm/cmparser/Identifier.hpp>
#include <sngcm/cmparser/TypeExpr.hpp>
#include <sngcm/cmparser/Expression.hpp>
#include <sngcm/cmlexer/CmajorLexer.hpp>
#include <sngcm/cmlexer/CmajorTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/GlobalVariable.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorTokens;

soulng::parser::Match GlobalVariableParser::GlobalVariable(CmajorLexer& lexer, boost::uuids::uuid* moduleId, ParsingContext* ctx, sngcm::ast::CompileUnitNode* cu)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("GlobalVariable"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> type;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<Node> initializer;
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
                        soulng::parser::Match match = SpecifierParser::Specifiers(lexer);
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
                        soulng::parser::Match match = TypeExprParser::TypeExpr(lexer, moduleId, ctx);
                        type.reset(static_cast<sngcm::ast::Node*>(match.value));
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
                    soulng::parser::Match match = IdentifierParser::Identifier(lexer, moduleId);
                    id.reset(static_cast<IdentifierNode*>(match.value));
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch8 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == ASSIGN)
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
                                soulng::parser::Match match = ExpressionParser::Expression(lexer, moduleId, ctx);
                                initializer.reset(static_cast<Node*>(match.value));
                                *parentMatch11 = match;
                            }
                            *parentMatch10 = match;
                        }
                        *parentMatch9 = match;
                    }
                    if (match.hit)
                    {
                        *parentMatch8 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch7 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(false);
                if (*lexer == SEMICOLON)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    s.end = span.end;
                    GlobalVariableNode * value = new GlobalVariableNode(s, *moduleId, specifiers->value, type.release(), id.release(), cu);
                    value->SetInitializer(initializer.release());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("GlobalVariable"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, value);
                    }
                }
                *parentMatch13 = match;
            }
            *parentMatch12 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("GlobalVariable"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("GlobalVariable"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
