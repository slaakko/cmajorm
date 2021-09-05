#include "Function.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>
#include <sngcm/cmnothrowparser/Attribute.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Concept.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/Template.hpp>
#include <sngcm/cmnothrowparser/Parameter.hpp>
#include <sngcm/cmnothrowparser/Statement.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Function.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowFunctionParser::Function(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Function"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 101);
    std::unique_ptr<FunctionNode> fun = std::unique_ptr<FunctionNode>();
    Span s = Span();
    Span specifierSpan = Span();
    std::unique_ptr<sngcm::ast::AttributesNode> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> returnType;
    std::unique_ptr<soulng::parser::Value<std::u32string>> functionGroupId;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch8 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(true);
                                        if (match.hit)
                                        {
                                            s = span;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch9 = &match;
                                    {
                                        soulng::parser::Match match(true);
                                        int64_t save = lexer.GetPos();
                                        soulng::parser::Match* parentMatch10 = &match;
                                        {
                                            soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                            attrs.reset(static_cast<sngcm::ast::AttributesNode*>(match.value));
                                            if (match.hit)
                                            {
                                                *parentMatch10 = match;
                                            }
                                            else
                                            {
                                                lexer.SetPos(save);
                                            }
                                        }
                                        *parentMatch9 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
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
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                        specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                        if (match.hit)
                                        {
                                            specifierSpan = span;
                                        }
                                        *parentMatch12 = match;
                                    }
                                    *parentMatch11 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch13 = &match;
                            {
                                soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                returnType.reset(static_cast<sngcm::ast::Node*>(match.value));
                                *parentMatch13 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
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
                                soulng::parser::Match match = NothrowFunctionParser::FunctionGroupId(lexer, moduleId, ctx);
                                functionGroupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                if (match.hit)
                                {
                                    s.end = span.end;
                                    fun.reset(new FunctionNode(s, *moduleId, specifiers->value, returnType.release(), functionGroupId->value, attrs.release()));
                                    fun->SetSpecifierSpan(specifierSpan);
                                    fun->SetGroupIdSpan(span);
                                }
                                *parentMatch15 = match;
                            }
                            *parentMatch14 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch16 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch17 = &match;
                        {
                            soulng::parser::Match match = NothrowTemplateParser::TemplateParameterList(lexer, moduleId, ctx, fun.get());
                            if (match.hit)
                            {
                                *parentMatch17 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch16 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch18 = &match;
                {
                    soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, fun.get());
                    *parentMatch18 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch19 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch20 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch21 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch22 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                            constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                            if (match.hit)
                            {
                                fun->SetConstraint(constraint.release());
                            }
                            *parentMatch22 = match;
                        }
                        *parentMatch21 = match;
                    }
                    if (match.hit)
                    {
                        *parentMatch20 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch19 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch23 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch24 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch25 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch26 = &match;
                    {
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch27 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                            body.reset(static_cast<CompoundStatementNode*>(match.value));
                            if (match.hit)
                            {
                                fun->SetBody(body.release());
                            }
                            *parentMatch27 = match;
                        }
                        *parentMatch26 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch28 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == SEMICOLON)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch28 = match;
                            }
                            *parentMatch26 = match;
                        }
                    }
                    *parentMatch25 = match;
                }
                if (match.hit)
                {
                    fun->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Function"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, fun.release());
                    }
                }
                *parentMatch24 = match;
            }
            *parentMatch23 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Function"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Function"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowFunctionParser::FunctionGroupId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("FunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 102);
    std::unique_ptr<soulng::parser::Value<std::u32string>> operatorFunctionGroupId;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
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
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("FunctionGroupId"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(lexer.GetMatch(span)));
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
                    soulng::parser::Match match = NothrowFunctionParser::OperatorFunctionGroupId(lexer, moduleId, ctx);
                    operatorFunctionGroupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("FunctionGroupId"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(operatorFunctionGroupId->value));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("FunctionGroupId"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("FunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowFunctionParser::OperatorFunctionGroupId(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 103);
    std::unique_ptr<sngcm::ast::Node> typeExpr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match(true);
            if (match.hit)
            {
                lexer.parsingOperatorFunctionGroupId = true;
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
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == OPERATOR)
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
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch8 = &match;
                                {
                                    int64_t save = lexer.GetPos();
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch9 = &match;
                                    {
                                        int64_t save = lexer.GetPos();
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch10 = &match;
                                        {
                                            int64_t save = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch11 = &match;
                                            {
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch12 = &match;
                                                {
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch13 = &match;
                                                    {
                                                        int64_t save = lexer.GetPos();
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch14 = &match;
                                                        {
                                                            int64_t save = lexer.GetPos();
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch15 = &match;
                                                            {
                                                                int64_t save = lexer.GetPos();
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch16 = &match;
                                                                {
                                                                    int64_t save = lexer.GetPos();
                                                                    soulng::parser::Match match(false);
                                                                    soulng::parser::Match* parentMatch17 = &match;
                                                                    {
                                                                        int64_t save = lexer.GetPos();
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch18 = &match;
                                                                        {
                                                                            int64_t save = lexer.GetPos();
                                                                            soulng::parser::Match match(false);
                                                                            soulng::parser::Match* parentMatch19 = &match;
                                                                            {
                                                                                int64_t save = lexer.GetPos();
                                                                                soulng::parser::Match match(false);
                                                                                soulng::parser::Match* parentMatch20 = &match;
                                                                                {
                                                                                    int64_t save = lexer.GetPos();
                                                                                    soulng::parser::Match match(false);
                                                                                    soulng::parser::Match* parentMatch21 = &match;
                                                                                    {
                                                                                        int64_t save = lexer.GetPos();
                                                                                        soulng::parser::Match match(false);
                                                                                        soulng::parser::Match* parentMatch22 = &match;
                                                                                        {
                                                                                            int64_t save = lexer.GetPos();
                                                                                            soulng::parser::Match match(false);
                                                                                            soulng::parser::Match* parentMatch23 = &match;
                                                                                            {
                                                                                                int64_t save = lexer.GetPos();
                                                                                                soulng::parser::Match match(false);
                                                                                                soulng::parser::Match* parentMatch24 = &match;
                                                                                                {
                                                                                                    int64_t save = lexer.GetPos();
                                                                                                    soulng::parser::Match match(false);
                                                                                                    soulng::parser::Match* parentMatch25 = &match;
                                                                                                    {
                                                                                                        int64_t save = lexer.GetPos();
                                                                                                        soulng::parser::Match match(false);
                                                                                                        soulng::parser::Match* parentMatch26 = &match;
                                                                                                        {
                                                                                                            int64_t save = lexer.GetPos();
                                                                                                            soulng::parser::Match match(false);
                                                                                                            soulng::parser::Match* parentMatch27 = &match;
                                                                                                            {
                                                                                                                int64_t pos = lexer.GetPos();
                                                                                                                soulng::parser::Match match(false);
                                                                                                                soulng::parser::Match* parentMatch28 = &match;
                                                                                                                {
                                                                                                                    soulng::parser::Match match(false);
                                                                                                                    soulng::parser::Match* parentMatch29 = &match;
                                                                                                                    int64_t save = lexer.GetPos();
                                                                                                                    {
                                                                                                                        soulng::parser::Match match(false);
                                                                                                                        soulng::parser::Match* parentMatch30 = &match;
                                                                                                                        {
                                                                                                                            soulng::parser::Match match(false);
                                                                                                                            soulng::parser::Match* parentMatch31 = &match;
                                                                                                                            {
                                                                                                                                soulng::parser::Match match(false);
                                                                                                                                if (*lexer == LANGLE)
                                                                                                                                {
                                                                                                                                    ++lexer;
                                                                                                                                    match.hit = true;
                                                                                                                                }
                                                                                                                                *parentMatch31 = match;
                                                                                                                            }
                                                                                                                            if (match.hit)
                                                                                                                            {
                                                                                                                                soulng::parser::Match match(false);
                                                                                                                                soulng::parser::Match* parentMatch32 = &match;
                                                                                                                                {
                                                                                                                                    soulng::parser::Match match(false);
                                                                                                                                    if (*lexer == LANGLE)
                                                                                                                                    {
                                                                                                                                        ++lexer;
                                                                                                                                        match.hit = true;
                                                                                                                                    }
                                                                                                                                    *parentMatch32 = match;
                                                                                                                                }
                                                                                                                                *parentMatch31 = match;
                                                                                                                            }
                                                                                                                            *parentMatch30 = match;
                                                                                                                        }
                                                                                                                        *parentMatch29 = match;
                                                                                                                    }
                                                                                                                    if (match.hit)
                                                                                                                    {
                                                                                                                        soulng::parser::Match match(false);
                                                                                                                        soulng::parser::Match* parentMatch33 = &match;
                                                                                                                        {
                                                                                                                            int64_t tmp = lexer.GetPos();
                                                                                                                            lexer.SetPos(save);
                                                                                                                            save = tmp;
                                                                                                                            soulng::parser::Match match(false);
                                                                                                                            soulng::parser::Match* parentMatch34 = &match;
                                                                                                                            {
                                                                                                                                soulng::parser::Match match(false);
                                                                                                                                soulng::parser::Match* parentMatch35 = &match;
                                                                                                                                {
                                                                                                                                    soulng::parser::Match match(false);
                                                                                                                                    soulng::parser::Match* parentMatch36 = &match;
                                                                                                                                    {
                                                                                                                                        soulng::parser::Match match(false);
                                                                                                                                        soulng::parser::Match* parentMatch37 = &match;
                                                                                                                                        {
                                                                                                                                            soulng::parser::Match match(false);
                                                                                                                                            if (*lexer == LANGLE)
                                                                                                                                            {
                                                                                                                                                ++lexer;
                                                                                                                                                match.hit = true;
                                                                                                                                            }
                                                                                                                                            *parentMatch37 = match;
                                                                                                                                        }
                                                                                                                                        if (match.hit)
                                                                                                                                        {
                                                                                                                                            soulng::parser::Match match(false);
                                                                                                                                            soulng::parser::Match* parentMatch38 = &match;
                                                                                                                                            {
                                                                                                                                                soulng::parser::Match match(false);
                                                                                                                                                if (*lexer == LANGLE)
                                                                                                                                                {
                                                                                                                                                    ++lexer;
                                                                                                                                                    match.hit = true;
                                                                                                                                                }
                                                                                                                                                *parentMatch38 = match;
                                                                                                                                            }
                                                                                                                                            *parentMatch37 = match;
                                                                                                                                        }
                                                                                                                                        *parentMatch36 = match;
                                                                                                                                    }
                                                                                                                                    if (match.hit)
                                                                                                                                    {
                                                                                                                                        soulng::parser::Match match(false);
                                                                                                                                        soulng::parser::Match* parentMatch39 = &match;
                                                                                                                                        {
                                                                                                                                            soulng::parser::Match match(false);
                                                                                                                                            soulng::parser::Match* parentMatch40 = &match;
                                                                                                                                            {
                                                                                                                                                soulng::parser::Match match(false);
                                                                                                                                                soulng::parser::Match* parentMatch41 = &match;
                                                                                                                                                {
                                                                                                                                                    soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                                                                                                                                    typeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                                                                                                                                                    *parentMatch41 = match;
                                                                                                                                                }
                                                                                                                                                if (match.hit)
                                                                                                                                                {
                                                                                                                                                    soulng::parser::Match match(false);
                                                                                                                                                    soulng::parser::Match* parentMatch42 = &match;
                                                                                                                                                    {
                                                                                                                                                        soulng::parser::Match match(true);
                                                                                                                                                        soulng::parser::Match* parentMatch43 = &match;
                                                                                                                                                        {
                                                                                                                                                            while (true)
                                                                                                                                                            {
                                                                                                                                                                int64_t save = lexer.GetPos();
                                                                                                                                                                {
                                                                                                                                                                    soulng::parser::Match match(false);
                                                                                                                                                                    soulng::parser::Match* parentMatch44 = &match;
                                                                                                                                                                    {
                                                                                                                                                                        soulng::parser::Match match(false);
                                                                                                                                                                        if (*lexer == COMMA)
                                                                                                                                                                        {
                                                                                                                                                                            ++lexer;
                                                                                                                                                                            match.hit = true;
                                                                                                                                                                        }
                                                                                                                                                                        *parentMatch44 = match;
                                                                                                                                                                    }
                                                                                                                                                                    if (match.hit)
                                                                                                                                                                    {
                                                                                                                                                                        soulng::parser::Match match(false);
                                                                                                                                                                        soulng::parser::Match* parentMatch45 = &match;
                                                                                                                                                                        {
                                                                                                                                                                            soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                                                                                                                                                            typeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                                                                                                                                                                            *parentMatch45 = match;
                                                                                                                                                                        }
                                                                                                                                                                        *parentMatch44 = match;
                                                                                                                                                                    }
                                                                                                                                                                    if (match.hit)
                                                                                                                                                                    {
                                                                                                                                                                        *parentMatch43 = match;
                                                                                                                                                                    }
                                                                                                                                                                    else
                                                                                                                                                                    {
                                                                                                                                                                        lexer.SetPos(save);
                                                                                                                                                                        break;
                                                                                                                                                                    }
                                                                                                                                                                }
                                                                                                                                                            }
                                                                                                                                                        }
                                                                                                                                                        *parentMatch42 = match;
                                                                                                                                                    }
                                                                                                                                                    *parentMatch41 = match;
                                                                                                                                                }
                                                                                                                                                *parentMatch40 = match;
                                                                                                                                            }
                                                                                                                                            *parentMatch39 = match;
                                                                                                                                        }
                                                                                                                                        *parentMatch36 = match;
                                                                                                                                    }
                                                                                                                                    *parentMatch35 = match;
                                                                                                                                }
                                                                                                                                if (match.hit)
                                                                                                                                {
                                                                                                                                    soulng::parser::Match match(false);
                                                                                                                                    soulng::parser::Match* parentMatch46 = &match;
                                                                                                                                    {
                                                                                                                                        soulng::parser::Match match(false);
                                                                                                                                        if (*lexer == RANGLE)
                                                                                                                                        {
                                                                                                                                            ++lexer;
                                                                                                                                            match.hit = true;
                                                                                                                                        }
                                                                                                                                        *parentMatch46 = match;
                                                                                                                                    }
                                                                                                                                    *parentMatch35 = match;
                                                                                                                                }
                                                                                                                                *parentMatch34 = match;
                                                                                                                            }
                                                                                                                            *parentMatch33 = match;
                                                                                                                        }
                                                                                                                        if (!match.hit)
                                                                                                                        {
                                                                                                                            lexer.SetPos(save);
                                                                                                                        }
                                                                                                                        *parentMatch29 = soulng::parser::Match(!match.hit, match.value);
                                                                                                                    }
                                                                                                                    *parentMatch28 = match;
                                                                                                                }
                                                                                                                if (match.hit)
                                                                                                                {
                                                                                                                    lexer.parsingOperatorFunctionGroupId = false;
                                                                                                                    {
                                                                                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator<<"));
                                                                                                                    }
                                                                                                                }
                                                                                                                *parentMatch27 = match;
                                                                                                            }
                                                                                                            *parentMatch26 = match;
                                                                                                            if (!match.hit)
                                                                                                            {
                                                                                                                soulng::parser::Match match(false);
                                                                                                                soulng::parser::Match* parentMatch47 = &match;
                                                                                                                lexer.SetPos(save);
                                                                                                                {
                                                                                                                    soulng::parser::Match match(false);
                                                                                                                    soulng::parser::Match* parentMatch48 = &match;
                                                                                                                    {
                                                                                                                        int64_t pos = lexer.GetPos();
                                                                                                                        soulng::parser::Match match(false);
                                                                                                                        if (*lexer == SHIFTRIGHT)
                                                                                                                        {
                                                                                                                            ++lexer;
                                                                                                                            match.hit = true;
                                                                                                                        }
                                                                                                                        if (match.hit)
                                                                                                                        {
                                                                                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                                                                                            {
                                                                                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator>>"));
                                                                                                                            }
                                                                                                                        }
                                                                                                                        *parentMatch48 = match;
                                                                                                                    }
                                                                                                                    *parentMatch47 = match;
                                                                                                                }
                                                                                                                *parentMatch26 = match;
                                                                                                            }
                                                                                                        }
                                                                                                        *parentMatch25 = match;
                                                                                                        if (!match.hit)
                                                                                                        {
                                                                                                            soulng::parser::Match match(false);
                                                                                                            soulng::parser::Match* parentMatch49 = &match;
                                                                                                            lexer.SetPos(save);
                                                                                                            {
                                                                                                                soulng::parser::Match match(false);
                                                                                                                soulng::parser::Match* parentMatch50 = &match;
                                                                                                                {
                                                                                                                    int64_t pos = lexer.GetPos();
                                                                                                                    soulng::parser::Match match(false);
                                                                                                                    if (*lexer == EQ)
                                                                                                                    {
                                                                                                                        ++lexer;
                                                                                                                        match.hit = true;
                                                                                                                    }
                                                                                                                    if (match.hit)
                                                                                                                    {
                                                                                                                        lexer.parsingOperatorFunctionGroupId = false;
                                                                                                                        {
                                                                                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator=="));
                                                                                                                        }
                                                                                                                    }
                                                                                                                    *parentMatch50 = match;
                                                                                                                }
                                                                                                                *parentMatch49 = match;
                                                                                                            }
                                                                                                            *parentMatch25 = match;
                                                                                                        }
                                                                                                    }
                                                                                                    *parentMatch24 = match;
                                                                                                    if (!match.hit)
                                                                                                    {
                                                                                                        soulng::parser::Match match(false);
                                                                                                        soulng::parser::Match* parentMatch51 = &match;
                                                                                                        lexer.SetPos(save);
                                                                                                        {
                                                                                                            soulng::parser::Match match(false);
                                                                                                            soulng::parser::Match* parentMatch52 = &match;
                                                                                                            {
                                                                                                                int64_t pos = lexer.GetPos();
                                                                                                                soulng::parser::Match match(false);
                                                                                                                if (*lexer == ASSIGN)
                                                                                                                {
                                                                                                                    ++lexer;
                                                                                                                    match.hit = true;
                                                                                                                }
                                                                                                                if (match.hit)
                                                                                                                {
                                                                                                                    lexer.parsingOperatorFunctionGroupId = false;
                                                                                                                    {
                                                                                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator="));
                                                                                                                    }
                                                                                                                }
                                                                                                                *parentMatch52 = match;
                                                                                                            }
                                                                                                            *parentMatch51 = match;
                                                                                                        }
                                                                                                        *parentMatch24 = match;
                                                                                                    }
                                                                                                }
                                                                                                *parentMatch23 = match;
                                                                                                if (!match.hit)
                                                                                                {
                                                                                                    soulng::parser::Match match(false);
                                                                                                    soulng::parser::Match* parentMatch53 = &match;
                                                                                                    lexer.SetPos(save);
                                                                                                    {
                                                                                                        soulng::parser::Match match(false);
                                                                                                        soulng::parser::Match* parentMatch54 = &match;
                                                                                                        {
                                                                                                            int64_t pos = lexer.GetPos();
                                                                                                            soulng::parser::Match match(false);
                                                                                                            if (*lexer == LANGLE)
                                                                                                            {
                                                                                                                ++lexer;
                                                                                                                match.hit = true;
                                                                                                            }
                                                                                                            if (match.hit)
                                                                                                            {
                                                                                                                lexer.parsingOperatorFunctionGroupId = false;
                                                                                                                {
                                                                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator<"));
                                                                                                                }
                                                                                                            }
                                                                                                            *parentMatch54 = match;
                                                                                                        }
                                                                                                        *parentMatch53 = match;
                                                                                                    }
                                                                                                    *parentMatch23 = match;
                                                                                                }
                                                                                            }
                                                                                            *parentMatch22 = match;
                                                                                            if (!match.hit)
                                                                                            {
                                                                                                soulng::parser::Match match(false);
                                                                                                soulng::parser::Match* parentMatch55 = &match;
                                                                                                lexer.SetPos(save);
                                                                                                {
                                                                                                    soulng::parser::Match match(false);
                                                                                                    soulng::parser::Match* parentMatch56 = &match;
                                                                                                    {
                                                                                                        int64_t pos = lexer.GetPos();
                                                                                                        soulng::parser::Match match(false);
                                                                                                        if (*lexer == ARROW)
                                                                                                        {
                                                                                                            ++lexer;
                                                                                                            match.hit = true;
                                                                                                        }
                                                                                                        if (match.hit)
                                                                                                        {
                                                                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                                                                            {
                                                                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator->"));
                                                                                                            }
                                                                                                        }
                                                                                                        *parentMatch56 = match;
                                                                                                    }
                                                                                                    *parentMatch55 = match;
                                                                                                }
                                                                                                *parentMatch22 = match;
                                                                                            }
                                                                                        }
                                                                                        *parentMatch21 = match;
                                                                                        if (!match.hit)
                                                                                        {
                                                                                            soulng::parser::Match match(false);
                                                                                            soulng::parser::Match* parentMatch57 = &match;
                                                                                            lexer.SetPos(save);
                                                                                            {
                                                                                                soulng::parser::Match match(false);
                                                                                                soulng::parser::Match* parentMatch58 = &match;
                                                                                                {
                                                                                                    int64_t pos = lexer.GetPos();
                                                                                                    soulng::parser::Match match(false);
                                                                                                    if (*lexer == PLUSPLUS)
                                                                                                    {
                                                                                                        ++lexer;
                                                                                                        match.hit = true;
                                                                                                    }
                                                                                                    if (match.hit)
                                                                                                    {
                                                                                                        lexer.parsingOperatorFunctionGroupId = false;
                                                                                                        {
                                                                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator++"));
                                                                                                        }
                                                                                                    }
                                                                                                    *parentMatch58 = match;
                                                                                                }
                                                                                                *parentMatch57 = match;
                                                                                            }
                                                                                            *parentMatch21 = match;
                                                                                        }
                                                                                    }
                                                                                    *parentMatch20 = match;
                                                                                    if (!match.hit)
                                                                                    {
                                                                                        soulng::parser::Match match(false);
                                                                                        soulng::parser::Match* parentMatch59 = &match;
                                                                                        lexer.SetPos(save);
                                                                                        {
                                                                                            soulng::parser::Match match(false);
                                                                                            soulng::parser::Match* parentMatch60 = &match;
                                                                                            {
                                                                                                int64_t pos = lexer.GetPos();
                                                                                                soulng::parser::Match match(false);
                                                                                                if (*lexer == MINUSMINUS)
                                                                                                {
                                                                                                    ++lexer;
                                                                                                    match.hit = true;
                                                                                                }
                                                                                                if (match.hit)
                                                                                                {
                                                                                                    lexer.parsingOperatorFunctionGroupId = false;
                                                                                                    {
                                                                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator--"));
                                                                                                    }
                                                                                                }
                                                                                                *parentMatch60 = match;
                                                                                            }
                                                                                            *parentMatch59 = match;
                                                                                        }
                                                                                        *parentMatch20 = match;
                                                                                    }
                                                                                }
                                                                                *parentMatch19 = match;
                                                                                if (!match.hit)
                                                                                {
                                                                                    soulng::parser::Match match(false);
                                                                                    soulng::parser::Match* parentMatch61 = &match;
                                                                                    lexer.SetPos(save);
                                                                                    {
                                                                                        soulng::parser::Match match(false);
                                                                                        soulng::parser::Match* parentMatch62 = &match;
                                                                                        {
                                                                                            int64_t pos = lexer.GetPos();
                                                                                            soulng::parser::Match match(false);
                                                                                            if (*lexer == PLUS)
                                                                                            {
                                                                                                ++lexer;
                                                                                                match.hit = true;
                                                                                            }
                                                                                            if (match.hit)
                                                                                            {
                                                                                                lexer.parsingOperatorFunctionGroupId = false;
                                                                                                {
                                                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator+"));
                                                                                                }
                                                                                            }
                                                                                            *parentMatch62 = match;
                                                                                        }
                                                                                        *parentMatch61 = match;
                                                                                    }
                                                                                    *parentMatch19 = match;
                                                                                }
                                                                            }
                                                                            *parentMatch18 = match;
                                                                            if (!match.hit)
                                                                            {
                                                                                soulng::parser::Match match(false);
                                                                                soulng::parser::Match* parentMatch63 = &match;
                                                                                lexer.SetPos(save);
                                                                                {
                                                                                    soulng::parser::Match match(false);
                                                                                    soulng::parser::Match* parentMatch64 = &match;
                                                                                    {
                                                                                        int64_t pos = lexer.GetPos();
                                                                                        soulng::parser::Match match(false);
                                                                                        if (*lexer == MINUS)
                                                                                        {
                                                                                            ++lexer;
                                                                                            match.hit = true;
                                                                                        }
                                                                                        if (match.hit)
                                                                                        {
                                                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                                                            {
                                                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator-"));
                                                                                            }
                                                                                        }
                                                                                        *parentMatch64 = match;
                                                                                    }
                                                                                    *parentMatch63 = match;
                                                                                }
                                                                                *parentMatch18 = match;
                                                                            }
                                                                        }
                                                                        *parentMatch17 = match;
                                                                        if (!match.hit)
                                                                        {
                                                                            soulng::parser::Match match(false);
                                                                            soulng::parser::Match* parentMatch65 = &match;
                                                                            lexer.SetPos(save);
                                                                            {
                                                                                soulng::parser::Match match(false);
                                                                                soulng::parser::Match* parentMatch66 = &match;
                                                                                {
                                                                                    int64_t pos = lexer.GetPos();
                                                                                    soulng::parser::Match match(false);
                                                                                    if (*lexer == STAR)
                                                                                    {
                                                                                        ++lexer;
                                                                                        match.hit = true;
                                                                                    }
                                                                                    if (match.hit)
                                                                                    {
                                                                                        lexer.parsingOperatorFunctionGroupId = false;
                                                                                        {
                                                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator*"));
                                                                                        }
                                                                                    }
                                                                                    *parentMatch66 = match;
                                                                                }
                                                                                *parentMatch65 = match;
                                                                            }
                                                                            *parentMatch17 = match;
                                                                        }
                                                                    }
                                                                    *parentMatch16 = match;
                                                                    if (!match.hit)
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch67 = &match;
                                                                        lexer.SetPos(save);
                                                                        {
                                                                            soulng::parser::Match match(false);
                                                                            soulng::parser::Match* parentMatch68 = &match;
                                                                            {
                                                                                int64_t pos = lexer.GetPos();
                                                                                soulng::parser::Match match(false);
                                                                                if (*lexer == DIV)
                                                                                {
                                                                                    ++lexer;
                                                                                    match.hit = true;
                                                                                }
                                                                                if (match.hit)
                                                                                {
                                                                                    lexer.parsingOperatorFunctionGroupId = false;
                                                                                    {
                                                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator/"));
                                                                                    }
                                                                                }
                                                                                *parentMatch68 = match;
                                                                            }
                                                                            *parentMatch67 = match;
                                                                        }
                                                                        *parentMatch16 = match;
                                                                    }
                                                                }
                                                                *parentMatch15 = match;
                                                                if (!match.hit)
                                                                {
                                                                    soulng::parser::Match match(false);
                                                                    soulng::parser::Match* parentMatch69 = &match;
                                                                    lexer.SetPos(save);
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch70 = &match;
                                                                        {
                                                                            int64_t pos = lexer.GetPos();
                                                                            soulng::parser::Match match(false);
                                                                            if (*lexer == REM)
                                                                            {
                                                                                ++lexer;
                                                                                match.hit = true;
                                                                            }
                                                                            if (match.hit)
                                                                            {
                                                                                lexer.parsingOperatorFunctionGroupId = false;
                                                                                {
                                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator%"));
                                                                                }
                                                                            }
                                                                            *parentMatch70 = match;
                                                                        }
                                                                        *parentMatch69 = match;
                                                                    }
                                                                    *parentMatch15 = match;
                                                                }
                                                            }
                                                            *parentMatch14 = match;
                                                            if (!match.hit)
                                                            {
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch71 = &match;
                                                                lexer.SetPos(save);
                                                                {
                                                                    soulng::parser::Match match(false);
                                                                    soulng::parser::Match* parentMatch72 = &match;
                                                                    {
                                                                        int64_t pos = lexer.GetPos();
                                                                        soulng::parser::Match match(false);
                                                                        if (*lexer == AMP)
                                                                        {
                                                                            ++lexer;
                                                                            match.hit = true;
                                                                        }
                                                                        if (match.hit)
                                                                        {
                                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                                            {
                                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator&"));
                                                                            }
                                                                        }
                                                                        *parentMatch72 = match;
                                                                    }
                                                                    *parentMatch71 = match;
                                                                }
                                                                *parentMatch14 = match;
                                                            }
                                                        }
                                                        *parentMatch13 = match;
                                                        if (!match.hit)
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch73 = &match;
                                                            lexer.SetPos(save);
                                                            {
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch74 = &match;
                                                                {
                                                                    int64_t pos = lexer.GetPos();
                                                                    soulng::parser::Match match(false);
                                                                    if (*lexer == BITOR)
                                                                    {
                                                                        ++lexer;
                                                                        match.hit = true;
                                                                    }
                                                                    if (match.hit)
                                                                    {
                                                                        lexer.parsingOperatorFunctionGroupId = false;
                                                                        {
                                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator|"));
                                                                        }
                                                                    }
                                                                    *parentMatch74 = match;
                                                                }
                                                                *parentMatch73 = match;
                                                            }
                                                            *parentMatch13 = match;
                                                        }
                                                    }
                                                    *parentMatch12 = match;
                                                    if (!match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch75 = &match;
                                                        lexer.SetPos(save);
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch76 = &match;
                                                            {
                                                                int64_t pos = lexer.GetPos();
                                                                soulng::parser::Match match(false);
                                                                if (*lexer == BITXOR)
                                                                {
                                                                    ++lexer;
                                                                    match.hit = true;
                                                                }
                                                                if (match.hit)
                                                                {
                                                                    lexer.parsingOperatorFunctionGroupId = false;
                                                                    {
                                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator^"));
                                                                    }
                                                                }
                                                                *parentMatch76 = match;
                                                            }
                                                            *parentMatch75 = match;
                                                        }
                                                        *parentMatch12 = match;
                                                    }
                                                }
                                                *parentMatch11 = match;
                                                if (!match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch77 = &match;
                                                    lexer.SetPos(save);
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch78 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::parser::Match match(false);
                                                            if (*lexer == EXCLAMATION)
                                                            {
                                                                ++lexer;
                                                                match.hit = true;
                                                            }
                                                            if (match.hit)
                                                            {
                                                                lexer.parsingOperatorFunctionGroupId = false;
                                                                {
                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator!"));
                                                                }
                                                            }
                                                            *parentMatch78 = match;
                                                        }
                                                        *parentMatch77 = match;
                                                    }
                                                    *parentMatch11 = match;
                                                }
                                            }
                                            *parentMatch10 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch79 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch80 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == CPL)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        if (match.hit)
                                                        {
                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                            {
                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator~"));
                                                            }
                                                        }
                                                        *parentMatch80 = match;
                                                    }
                                                    *parentMatch79 = match;
                                                }
                                                *parentMatch10 = match;
                                            }
                                        }
                                        *parentMatch9 = match;
                                        if (!match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch81 = &match;
                                            lexer.SetPos(save);
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch82 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == LBRACKET)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
                                                    }
                                                    *parentMatch82 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch83 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch84 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::parser::Match match(false);
                                                            if (*lexer == RBRACKET)
                                                            {
                                                                ++lexer;
                                                                match.hit = true;
                                                            }
                                                            if (match.hit)
                                                            {
                                                                lexer.parsingOperatorFunctionGroupId = false;
                                                                {
                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator[]"));
                                                                }
                                                            }
                                                            *parentMatch84 = match;
                                                        }
                                                        *parentMatch83 = match;
                                                    }
                                                    *parentMatch82 = match;
                                                }
                                                *parentMatch81 = match;
                                            }
                                            *parentMatch9 = match;
                                        }
                                    }
                                    *parentMatch8 = match;
                                    if (!match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch85 = &match;
                                        lexer.SetPos(save);
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch86 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                if (*lexer == LPAREN)
                                                {
                                                    ++lexer;
                                                    match.hit = true;
                                                }
                                                *parentMatch86 = match;
                                            }
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch87 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch88 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == RPAREN)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        if (match.hit)
                                                        {
                                                            lexer.parsingOperatorFunctionGroupId = false;
                                                            {
                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(U"operator()"));
                                                            }
                                                        }
                                                        *parentMatch88 = match;
                                                    }
                                                    *parentMatch87 = match;
                                                }
                                                *parentMatch86 = match;
                                            }
                                            *parentMatch85 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                }
                if (match.hit)
                {
                    lexer.parsingOperatorFunctionGroupId = false;
                }
                else
                {
                    lexer.parsingOperatorFunctionGroupId = false;
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("OperatorFunctionGroupId"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
