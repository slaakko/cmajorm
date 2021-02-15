#include "Concept.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Parameter.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowparser/Function.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Concept.parser' using soulng parser generator spg version 3.10.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowConceptParser::Concept(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Concept"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConceptNode> conceptNode = std::unique_ptr<ConceptNode>();
    std::unique_ptr<WhereConstraintNode> constraint = std::unique_ptr<WhereConstraintNode>();
    Span s = Span();
    Span beginBraceSpan = Span();
    Span endBraceSpan = Span();
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> conceptName;
    std::unique_ptr<IdentifierNode> typeParam;
    std::unique_ptr<ConceptIdNode> refinement;
    std::unique_ptr<WhereConstraintNode> c;
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
                ctx->BeginParsingConcept();
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
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch10 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch11 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch12 = &match;
                                                {
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::lexer::Span span = lexer.GetSpan();
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch13 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch14 = &match;
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch15 = &match;
                                                            {
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch16 = &match;
                                                                {
                                                                    int64_t pos = lexer.GetPos();
                                                                    soulng::lexer::Span span = lexer.GetSpan();
                                                                    soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                                    specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                                    if (match.hit)
                                                                    {
                                                                        s = span;
                                                                    }
                                                                    *parentMatch16 = match;
                                                                }
                                                                *parentMatch15 = match;
                                                            }
                                                            if (match.hit)
                                                            {
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch17 = &match;
                                                                {
                                                                    soulng::parser::Match match(false);
                                                                    if (*lexer == CONCEPT)
                                                                    {
                                                                        ++lexer;
                                                                        match.hit = true;
                                                                    }
                                                                    *parentMatch17 = match;
                                                                }
                                                                *parentMatch15 = match;
                                                            }
                                                            *parentMatch14 = match;
                                                        }
                                                        if (match.hit)
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch18 = &match;
                                                            {
                                                                soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                                                conceptName.reset(static_cast<IdentifierNode*>(match.value));
                                                                *parentMatch18 = match;
                                                            }
                                                            *parentMatch14 = match;
                                                        }
                                                        *parentMatch13 = match;
                                                    }
                                                    if (match.hit)
                                                    {
                                                        conceptNode.reset(new ConceptNode(span, *moduleId, specifiers->value, conceptName.release()));
                                                    }
                                                    *parentMatch12 = match;
                                                }
                                                *parentMatch11 = match;
                                            }
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch19 = &match;
                                                {
                                                    soulng::parser::Match match(true);
                                                    soulng::parser::Match* parentMatch20 = &match;
                                                    {
                                                        soulng::lexer::Span span = lexer.GetSpan();
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == LANGLE)
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
                                                            lexer.AddError(span, ToUtf32(GetTokenInfo(LANGLE)));
                                                        }
                                                    }
                                                    *parentMatch19 = match;
                                                }
                                                *parentMatch11 = match;
                                            }
                                            *parentMatch10 = match;
                                        }
                                        if (match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch21 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch22 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch23 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch24 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                                            typeParam.reset(static_cast<IdentifierNode*>(match.value));
                                                            if (match.hit)
                                                            {
                                                                conceptNode->AddTypeParameter(typeParam.release());
                                                            }
                                                            *parentMatch24 = match;
                                                        }
                                                        *parentMatch23 = match;
                                                    }
                                                    if (match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch25 = &match;
                                                        {
                                                            soulng::parser::Match match(true);
                                                            soulng::parser::Match* parentMatch26 = &match;
                                                            {
                                                                while (true)
                                                                {
                                                                    int64_t save = lexer.GetPos();
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch27 = &match;
                                                                        {
                                                                            soulng::parser::Match match(false);
                                                                            if (*lexer == COMMA)
                                                                            {
                                                                                ++lexer;
                                                                                match.hit = true;
                                                                            }
                                                                            *parentMatch27 = match;
                                                                        }
                                                                        if (match.hit)
                                                                        {
                                                                            soulng::parser::Match match(false);
                                                                            soulng::parser::Match* parentMatch28 = &match;
                                                                            {
                                                                                soulng::parser::Match match(false);
                                                                                soulng::parser::Match* parentMatch29 = &match;
                                                                                {
                                                                                    int64_t pos = lexer.GetPos();
                                                                                    soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                                                                    typeParam.reset(static_cast<IdentifierNode*>(match.value));
                                                                                    if (match.hit)
                                                                                    {
                                                                                        conceptNode->AddTypeParameter(typeParam.release());
                                                                                    }
                                                                                    *parentMatch29 = match;
                                                                                }
                                                                                *parentMatch28 = match;
                                                                            }
                                                                            *parentMatch27 = match;
                                                                        }
                                                                        if (match.hit)
                                                                        {
                                                                            *parentMatch26 = match;
                                                                        }
                                                                        else
                                                                        {
                                                                            lexer.SetPos(save);
                                                                            break;
                                                                        }
                                                                    }
                                                                }
                                                            }
                                                            *parentMatch25 = match;
                                                        }
                                                        *parentMatch23 = match;
                                                    }
                                                    *parentMatch22 = match;
                                                }
                                                *parentMatch21 = match;
                                            }
                                            *parentMatch10 = match;
                                        }
                                        *parentMatch9 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch30 = &match;
                                        {
                                            soulng::parser::Match match(true);
                                            soulng::parser::Match* parentMatch31 = &match;
                                            {
                                                soulng::lexer::Span span = lexer.GetSpan();
                                                soulng::parser::Match match(false);
                                                if (*lexer == RANGLE)
                                                {
                                                    ++lexer;
                                                    match.hit = true;
                                                }
                                                if (match.hit)
                                                {
                                                    *parentMatch31 = match;
                                                }
                                                else
                                                {
                                                    match.hit = true;
                                                    *parentMatch31 = match;
                                                    lexer.AddError(span, ToUtf32(GetTokenInfo(RANGLE)));
                                                }
                                            }
                                            *parentMatch30 = match;
                                        }
                                        *parentMatch9 = match;
                                    }
                                    *parentMatch8 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch32 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch33 = &match;
                                        {
                                            int64_t pos = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch34 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch35 = &match;
                                                {
                                                    soulng::parser::Match match(true);
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match* parentMatch36 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch37 = &match;
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch38 = &match;
                                                            {
                                                                int64_t pos = lexer.GetPos();
                                                                soulng::parser::Match match = NothrowConceptParser::Refinement(lexer, moduleId);
                                                                refinement.reset(static_cast<ConceptIdNode*>(match.value));
                                                                if (match.hit)
                                                                {
                                                                    conceptNode->SetRefinement(refinement.release());
                                                                }
                                                                *parentMatch38 = match;
                                                            }
                                                            *parentMatch37 = match;
                                                        }
                                                        if (match.hit)
                                                        {
                                                            *parentMatch36 = match;
                                                        }
                                                        else
                                                        {
                                                            lexer.SetPos(save);
                                                        }
                                                    }
                                                    *parentMatch35 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch39 = &match;
                                                    {
                                                        soulng::parser::Match match(true);
                                                        int64_t save = lexer.GetPos();
                                                        soulng::parser::Match* parentMatch40 = &match;
                                                        {
                                                            soulng::parser::Match match(false);
                                                            soulng::parser::Match* parentMatch41 = &match;
                                                            {
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch42 = &match;
                                                                {
                                                                    int64_t pos = lexer.GetPos();
                                                                    soulng::lexer::Span span = lexer.GetSpan();
                                                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                                                    c.reset(static_cast<WhereConstraintNode*>(match.value));
                                                                    if (match.hit)
                                                                    {
                                                                        s.end = span.end;
                                                                        constraint.reset(c.release());
                                                                        constraint->SetHeaderConstraint();
                                                                        conceptNode->AddConstraint(constraint.release());
                                                                    }
                                                                    *parentMatch42 = match;
                                                                }
                                                                *parentMatch41 = match;
                                                            }
                                                            if (match.hit)
                                                            {
                                                                *parentMatch40 = match;
                                                            }
                                                            else
                                                            {
                                                                lexer.SetPos(save);
                                                            }
                                                        }
                                                        *parentMatch39 = match;
                                                    }
                                                    *parentMatch35 = match;
                                                }
                                                *parentMatch34 = match;
                                            }
                                            if (match.hit)
                                            {
                                                conceptNode->SetSpan(s);
                                            }
                                            *parentMatch33 = match;
                                        }
                                        *parentMatch32 = match;
                                    }
                                    *parentMatch8 = match;
                                }
                                *parentMatch7 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch43 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch44 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(true);
                                        soulng::parser::Match* parentMatch45 = &match;
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
                                                *parentMatch45 = match;
                                            }
                                            else
                                            {
                                                match.hit = true;
                                                *parentMatch45 = match;
                                                lexer.AddError(span, ToUtf32(GetTokenInfo(LBRACE)));
                                            }
                                        }
                                        if (match.hit)
                                        {
                                            beginBraceSpan = span;
                                        }
                                        *parentMatch44 = match;
                                    }
                                    *parentMatch43 = match;
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch46 = &match;
                            {
                                soulng::parser::Match match = NothrowConceptParser::ConceptBody(lexer, moduleId, ctx, conceptNode.get());
                                *parentMatch46 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch5 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch47 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch48 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch49 = &match;
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
                                        *parentMatch49 = match;
                                    }
                                    else
                                    {
                                        match.hit = true;
                                        *parentMatch49 = match;
                                        lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACE)));
                                    }
                                }
                                if (match.hit)
                                {
                                    endBraceSpan = span;
                                }
                                *parentMatch48 = match;
                            }
                            *parentMatch47 = match;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                }
                if (match.hit)
                {
                    ctx->EndParsingConcept();
                    conceptNode->SetBeginBraceSpan(beginBraceSpan);
                    conceptNode->SetEndBraceSpan(endBraceSpan);
                    conceptNode->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Concept"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, conceptNode.release());
                    }
                }
                else
                {
                    ctx->EndParsingConcept();
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Concept"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Concept"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::Refinement(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Refinement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConceptIdNode> conceptId = std::unique_ptr<ConceptIdNode>();
    std::unique_ptr<IdentifierNode> refinedConcept;
    std::unique_ptr<IdentifierNode> typeParam;
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
                            if (*lexer == COLON)
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
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match = NothrowIdentifierParser::QualifiedId(lexer, moduleId);
                                    refinedConcept.reset(static_cast<IdentifierNode*>(match.value));
                                    if (match.hit)
                                    {
                                        conceptId.reset(new ConceptIdNode(span, *moduleId, refinedConcept.release()));
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        {
                            soulng::parser::Match match(true);
                            soulng::parser::Match* parentMatch9 = &match;
                            {
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match(false);
                                if (*lexer == LANGLE)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                if (match.hit)
                                {
                                    *parentMatch9 = match;
                                }
                                else
                                {
                                    match.hit = true;
                                    *parentMatch9 = match;
                                    lexer.AddError(span, ToUtf32(GetTokenInfo(LANGLE)));
                                }
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch12 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch13 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                    typeParam.reset(static_cast<IdentifierNode*>(match.value));
                                    if (match.hit)
                                    {
                                        conceptId->AddTypeParameter(typeParam.release());
                                    }
                                    *parentMatch13 = match;
                                }
                                *parentMatch12 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch14 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch15 = &match;
                                    {
                                        while (true)
                                        {
                                            int64_t save = lexer.GetPos();
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch16 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == COMMA)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
                                                    }
                                                    *parentMatch16 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch17 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch18 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                                            typeParam.reset(static_cast<IdentifierNode*>(match.value));
                                                            if (match.hit)
                                                            {
                                                                conceptId->AddTypeParameter(typeParam.release());
                                                            }
                                                            *parentMatch18 = match;
                                                        }
                                                        *parentMatch17 = match;
                                                    }
                                                    *parentMatch16 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    *parentMatch15 = match;
                                                }
                                                else
                                                {
                                                    lexer.SetPos(save);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    *parentMatch14 = match;
                                }
                                *parentMatch12 = match;
                            }
                            *parentMatch11 = match;
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
                soulng::parser::Match* parentMatch19 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch20 = &match;
                    {
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(false);
                        if (*lexer == RANGLE)
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
                            lexer.AddError(span, ToUtf32(GetTokenInfo(RANGLE)));
                        }
                    }
                    *parentMatch19 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            conceptId->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Refinement"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, conceptId.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Refinement"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Refinement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConceptBody(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConceptBody"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
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
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match match = NothrowConceptParser::ConceptBodyConstraint(lexer, moduleId, ctx, conceptNode);
                        *parentMatch2 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch3 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match = NothrowConceptParser::Axiom(lexer, moduleId, ctx, conceptNode);
                                *parentMatch3 = match;
                            }
                            *parentMatch2 = match;
                        }
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConceptBody"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConceptBody"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConceptBodyConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConceptBodyConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> typeNameConstraint;
    std::unique_ptr<ConstraintNode> signatureConstraint;
    std::unique_ptr<WhereConstraintNode> embeddedConstraint;
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
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match = NothrowConceptParser::TypeNameConstraint(lexer, moduleId, ctx);
                typeNameConstraint.reset(static_cast<ConstraintNode*>(match.value));
                if (match.hit)
                {
                    conceptNode->AddConstraint(typeNameConstraint.release());
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = NothrowConceptParser::SignatureConstraint(lexer, moduleId, ctx, conceptNode->TypeParameters()[0]);
                        signatureConstraint.reset(static_cast<ConstraintNode*>(match.value));
                        if (match.hit)
                        {
                            conceptNode->AddConstraint(signatureConstraint.release());
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match = NothrowConceptParser::EmbeddedConstraint(lexer, moduleId, ctx);
                    embeddedConstraint.reset(static_cast<WhereConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        conceptNode->AddConstraint(embeddedConstraint.release());
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConceptBodyConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConceptBodyConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::TypeNameConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("TypeNameConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<sngcm::ast::Node> type;
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
                if (*lexer == TYPENAME)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    s = span;
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
                soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                type.reset(static_cast<sngcm::ast::Node*>(match.value));
                *parentMatch3 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
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
                soulng::parser::Match* parentMatch6 = &match;
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
                        *parentMatch6 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch6 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                    }
                }
                if (match.hit)
                {
                    s.end = span.end;
                    TypeNameConstraintNode * node = new TypeNameConstraintNode(s, *moduleId, type.release());
                    node->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("TypeNameConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, node);
                    }
                }
                *parentMatch5 = match;
            }
            *parentMatch4 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("TypeNameConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("TypeNameConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::SignatureConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("SignatureConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> constructorConstraint;
    std::unique_ptr<ConstraintNode> destructorConstraint;
    std::unique_ptr<ConstraintNode> memberFunctionConstraint;
    std::unique_ptr<ConstraintNode> functionConstraint;
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
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowConceptParser::ConstructorConstraint(lexer, moduleId, ctx, firstTypeParameter);
                    constructorConstraint.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SignatureConstraint"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, constructorConstraint.release());
                        }
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::parser::Match match = NothrowConceptParser::DestructorConstraint(lexer, moduleId, ctx, firstTypeParameter);
                            destructorConstraint.reset(static_cast<ConstraintNode*>(match.value));
                            if (match.hit)
                            {
                                {
                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SignatureConstraint"));
                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                    return soulng::parser::Match(true, destructorConstraint.release());
                                }
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = NothrowConceptParser::MemberFunctionConstraint(lexer, moduleId, ctx);
                        memberFunctionConstraint.reset(static_cast<ConstraintNode*>(match.value));
                        if (match.hit)
                        {
                            {
                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SignatureConstraint"));
                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                return soulng::parser::Match(true, memberFunctionConstraint.release());
                            }
                        }
                        *parentMatch7 = match;
                    }
                    *parentMatch6 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match = NothrowConceptParser::FunctionConstraint(lexer, moduleId, ctx);
                    functionConstraint.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SignatureConstraint"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, functionConstraint.release());
                        }
                    }
                    *parentMatch9 = match;
                }
                *parentMatch8 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SignatureConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("SignatureConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConstructorConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConstructorConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> ctorConstraint = std::unique_ptr<ConstraintNode>();
    Span e = Span();
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
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == EXPLICIT)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                *parentMatch5 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch4 = match;
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
                                soulng::lexer::Span span = lexer.GetSpan();
                                bool pass = true;
                                soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                id.reset(static_cast<IdentifierNode*>(match.value));
                                if (match.hit)
                                {
                                    std::u32string className = lexer.GetMatch(span);
                                    pass = className == firstTypeParameter->Str();
                                    if (pass) ctorConstraint.reset(new ConstructorConstraintNode(span, *moduleId, id.release()));
                                }
                                if (match.hit && !pass)
                                {
                                    match = soulng::parser::Match(false);
                                }
                                *parentMatch7 = match;
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
                        soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, ctorConstraint.get());
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
                            soulng::parser::Match match(false);
                            if (*lexer == SEMICOLON)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                *parentMatch11 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch11 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                            }
                        }
                        if (match.hit)
                        {
                            e = span;
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
            ctorConstraint->SetSpanEnd(e.end);
            ctorConstraint->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConstructorConstraint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, ctorConstraint.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConstructorConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConstructorConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::DestructorConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::IdentifierNode* firstTypeParameter)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DestructorConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
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
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match(false);
                                if (*lexer == CPL)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
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
                                soulng::parser::Match* parentMatch8 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    bool pass = true;
                                    soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                    id.reset(static_cast<IdentifierNode*>(match.value));
                                    if (match.hit)
                                    {
                                        std::u32string className = lexer.GetMatch(span);
                                        pass = className == firstTypeParameter->Str();
                                    }
                                    if (match.hit && !pass)
                                    {
                                        match = soulng::parser::Match(false);
                                    }
                                    *parentMatch8 = match;
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
                        soulng::parser::Match* parentMatch9 = &match;
                        {
                            soulng::parser::Match match(true);
                            soulng::parser::Match* parentMatch10 = &match;
                            {
                                soulng::lexer::Span span = lexer.GetSpan();
                                soulng::parser::Match match(false);
                                if (*lexer == LPAREN)
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
                                    lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
                                }
                            }
                            *parentMatch9 = match;
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
                        soulng::parser::Match* parentMatch12 = &match;
                        {
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match(false);
                            if (*lexer == RPAREN)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                *parentMatch12 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch12 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
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
                            if (*lexer == SEMICOLON)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            if (match.hit)
                            {
                                *parentMatch15 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch15 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                            }
                        }
                        if (match.hit)
                        {
                            s.end = span.end;
                        }
                        *parentMatch14 = match;
                    }
                    *parentMatch13 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            DestructorConstraintNode * node = new DestructorConstraintNode(s, *moduleId, id.release());
            node->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DestructorConstraint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, node);
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DestructorConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DestructorConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::MemberFunctionConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("MemberFunctionConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> memberFunctionConstraint = std::unique_ptr<ConstraintNode>();
    std::unique_ptr<sngcm::ast::Node> returnType;
    std::unique_ptr<IdentifierNode> typeParam;
    std::unique_ptr<soulng::parser::Value<std::u32string>> functionGroupId;
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
                                soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                returnType.reset(static_cast<sngcm::ast::Node*>(match.value));
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                    typeParam.reset(static_cast<IdentifierNode*>(match.value));
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch8 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == DOT)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch8 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch9 = &match;
                        {
                            soulng::parser::Match match = NothrowFunctionParser::FunctionGroupId(lexer, moduleId, ctx);
                            functionGroupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch9 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    memberFunctionConstraint.reset(new MemberFunctionConstraintNode(span, *moduleId, returnType.release(), typeParam.release(), functionGroupId->value));
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch10 = &match;
            {
                soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, memberFunctionConstraint.get());
                *parentMatch10 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch13 = &match;
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
                        *parentMatch13 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch13 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                    }
                }
                if (match.hit)
                {
                    memberFunctionConstraint->SetSpanEnd(span.end);
                    memberFunctionConstraint->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberFunctionConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, memberFunctionConstraint.release());
                    }
                }
                *parentMatch12 = match;
            }
            *parentMatch11 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberFunctionConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("MemberFunctionConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::FunctionConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("FunctionConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> functionConstraint = std::unique_ptr<ConstraintNode>();
    std::unique_ptr<sngcm::ast::Node> returnType;
    std::unique_ptr<soulng::parser::Value<std::u32string>> functionGroupId;
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
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                        returnType.reset(static_cast<sngcm::ast::Node*>(match.value));
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match = NothrowFunctionParser::FunctionGroupId(lexer, moduleId, ctx);
                            functionGroupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    functionConstraint.reset(new FunctionConstraintNode(span, *moduleId, returnType.release(), functionGroupId->value));
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch6 = &match;
            {
                soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, functionConstraint.get());
                *parentMatch6 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch9 = &match;
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
                        *parentMatch9 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch9 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                    }
                }
                if (match.hit)
                {
                    functionConstraint->SetSpanEnd(span.end);
                    functionConstraint->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("FunctionConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, functionConstraint.release());
                    }
                }
                *parentMatch8 = match;
            }
            *parentMatch7 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("FunctionConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("FunctionConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::EmbeddedConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EmbeddedConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<WhereConstraintNode> constraint = std::unique_ptr<WhereConstraintNode>();
    std::unique_ptr<WhereConstraintNode> c;
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
                soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                c.reset(static_cast<WhereConstraintNode*>(match.value));
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch4 = &match;
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
                            *parentMatch4 = match;
                        }
                        else
                        {
                            match.hit = true;
                            *parentMatch4 = match;
                            lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                        }
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            constraint.reset(c.release());
            constraint->SetSemicolon();
            constraint->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EmbeddedConstraint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, constraint.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EmbeddedConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EmbeddedConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::WhereConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("WhereConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<ConstraintNode> constraint;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::lexer::Span span = lexer.GetSpan();
            soulng::parser::Match match(false);
            if (*lexer == WHERE)
            {
                ++lexer;
                match.hit = true;
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
        soulng::parser::Match* parentMatch2 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match = NothrowConceptParser::ConstraintExpr(lexer, moduleId, ctx);
                constraint.reset(static_cast<ConstraintNode*>(match.value));
                if (match.hit)
                {
                    s.end = span.end;
                    WhereConstraintNode * node = new WhereConstraintNode(s, *moduleId, constraint.release());
                    node->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("WhereConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, node);
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("WhereConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("WhereConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConstraintExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> disjunctiveConstraint;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match = NothrowConceptParser::DisjunctiveConstraintExpr(lexer, moduleId, ctx);
        disjunctiveConstraint.reset(static_cast<ConstraintNode*>(match.value));
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConstraintExpr"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, disjunctiveConstraint.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConstraintExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::DisjunctiveConstraintExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DisjunctiveConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<ConstraintNode> constraint = std::unique_ptr<ConstraintNode>();
    std::unique_ptr<ConstraintNode> left;
    std::unique_ptr<ConstraintNode> right;
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
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowConceptParser::ConjunctiveConstraintExpr(lexer, moduleId, ctx);
                    left.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        constraint.reset(left.release());
                        s = span;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch7 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        if (*lexer == OR)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        *parentMatch7 = match;
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
                                                soulng::parser::Match match = NothrowConceptParser::ConjunctiveConstraintExpr(lexer, moduleId, ctx);
                                                right.reset(static_cast<ConstraintNode*>(match.value));
                                                if (match.hit)
                                                {
                                                    s.end = span.end;
                                                    constraint.reset(new DisjunctiveConstraintNode(s, *moduleId, constraint.release(), right.release()));
                                                }
                                                *parentMatch9 = match;
                                            }
                                            *parentMatch8 = match;
                                        }
                                        *parentMatch7 = match;
                                    }
                                    *parentMatch6 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch5 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                    break;
                                }
                            }
                        }
                    }
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            constraint->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DisjunctiveConstraintExpr"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, constraint.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DisjunctiveConstraintExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DisjunctiveConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConjunctiveConstraintExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConjunctiveConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<ConstraintNode> constraint = std::unique_ptr<ConstraintNode>();
    std::unique_ptr<ConstraintNode> left;
    std::unique_ptr<ConstraintNode> right;
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
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowConceptParser::PrimaryConstraintExpr(lexer, moduleId, ctx);
                    left.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        constraint.reset(left.release());
                        s = span;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch5 = &match;
                    {
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch7 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        if (*lexer == AND)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        *parentMatch7 = match;
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
                                                soulng::parser::Match match = NothrowConceptParser::PrimaryConstraintExpr(lexer, moduleId, ctx);
                                                right.reset(static_cast<ConstraintNode*>(match.value));
                                                if (match.hit)
                                                {
                                                    s.end = span.end;
                                                    constraint.reset(new ConjunctiveConstraintNode(s, *moduleId, constraint.release(), right.release()));
                                                }
                                                *parentMatch9 = match;
                                            }
                                            *parentMatch8 = match;
                                        }
                                        *parentMatch7 = match;
                                    }
                                    *parentMatch6 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch5 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                    break;
                                }
                            }
                        }
                    }
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            constraint->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConjunctiveConstraintExpr"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, constraint.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConjunctiveConstraintExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConjunctiveConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::PrimaryConstraintExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PrimaryConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<ConstraintNode> constraint;
    std::unique_ptr<ConstraintNode> atomicConstraint;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == LPAREN)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    if (match.hit)
                    {
                        s = span;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match = NothrowConceptParser::ConstraintExpr(lexer, moduleId, ctx);
                    constraint.reset(static_cast<ConstraintNode*>(match.value));
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
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
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(false);
                        if (*lexer == RPAREN)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        if (match.hit)
                        {
                            *parentMatch7 = match;
                        }
                        else
                        {
                            match.hit = true;
                            *parentMatch7 = match;
                            lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                        }
                    }
                    if (match.hit)
                    {
                        s.end = span.end;
                        ParenthesizedConstraintNode * node = new ParenthesizedConstraintNode(s, *moduleId, constraint.release());
                        node->SetLexerFlags(lexer.Flags());
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryConstraintExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, node);
                        }
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match = NothrowConceptParser::AtomicConstraintExpr(lexer, moduleId, ctx);
                    atomicConstraint.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryConstraintExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, atomicConstraint.release());
                        }
                    }
                    *parentMatch9 = match;
                }
                *parentMatch8 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PrimaryConstraintExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PrimaryConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::AtomicConstraintExpr(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstraintNode> predicateConstraint;
    std::unique_ptr<ConstraintNode> isConstraint;
    std::unique_ptr<ConstraintNode> multiParamConstraint;
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
                int64_t pos = lexer.GetPos();
                soulng::parser::Match match = NothrowConceptParser::PredicateConstraint(lexer, moduleId, ctx);
                predicateConstraint.reset(static_cast<ConstraintNode*>(match.value));
                if (match.hit)
                {
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, predicateConstraint.release());
                    }
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = NothrowConceptParser::IsConstraint(lexer, moduleId, ctx);
                        isConstraint.reset(static_cast<ConstraintNode*>(match.value));
                        if (match.hit)
                        {
                            {
                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                return soulng::parser::Match(true, isConstraint.release());
                            }
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match = NothrowConceptParser::MultiParamConstraint(lexer, moduleId, ctx);
                    multiParamConstraint.reset(static_cast<ConstraintNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, multiParamConstraint.release());
                        }
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AtomicConstraintExpr"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::PredicateConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PredicateConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<Node> invokeExpr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::lexer::Span span = lexer.GetSpan();
        soulng::parser::Match match = NothrowExpressionParser::InvokeExpr(lexer, moduleId, ctx);
        invokeExpr.reset(static_cast<Node*>(match.value));
        if (match.hit)
        {
            PredicateConstraintNode * node = new PredicateConstraintNode(span, *moduleId, invokeExpr.release());
            node->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PredicateConstraint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, node);
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PredicateConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PredicateConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::IsConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("IsConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<sngcm::ast::Node> type;
    std::unique_ptr<Node> conceptOrTypeName;
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
                soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                type.reset(static_cast<sngcm::ast::Node*>(match.value));
                if (match.hit)
                {
                    s = span;
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
                soulng::parser::Match match(false);
                if (*lexer == IS)
                {
                    ++lexer;
                    match.hit = true;
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
        soulng::parser::Match* parentMatch4 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch5 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match = NothrowConceptParser::ConceptOrTypeName(lexer, moduleId, ctx);
                conceptOrTypeName.reset(static_cast<Node*>(match.value));
                if (match.hit)
                {
                    s.end = span.end;
                    IsConstraintNode * node = new IsConstraintNode(s, *moduleId, type.release(), conceptOrTypeName.release());
                    node->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("IsConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, node);
                    }
                }
                *parentMatch5 = match;
            }
            *parentMatch4 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("IsConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("IsConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::ConceptOrTypeName(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConceptOrTypeName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<sngcm::ast::Node> conceptOrTypeName;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
        conceptOrTypeName.reset(static_cast<sngcm::ast::Node*>(match.value));
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConceptOrTypeName"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, conceptOrTypeName.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConceptOrTypeName"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConceptOrTypeName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::MultiParamConstraint(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("MultiParamConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<MultiParamConstraintNode> constraint = std::unique_ptr<MultiParamConstraintNode>();
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<sngcm::ast::Node> typeExpr;
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
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowIdentifierParser::QualifiedId(lexer, moduleId);
                    id.reset(static_cast<IdentifierNode*>(match.value));
                    if (match.hit)
                    {
                        constraint.reset(new MultiParamConstraintNode(span, *moduleId, id.release()));
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == LANGLE)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
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
                            soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                            typeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                            if (match.hit)
                            {
                                constraint->AddTypeExpr(typeExpr.release());
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
                            soulng::parser::Match* parentMatch10 = &match;
                            {
                                while (true)
                                {
                                    int64_t save = lexer.GetPos();
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch11 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            if (*lexer == COMMA)
                                            {
                                                ++lexer;
                                                match.hit = true;
                                            }
                                            *parentMatch11 = match;
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
                                                    soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                                    typeExpr.reset(static_cast<sngcm::ast::Node*>(match.value));
                                                    if (match.hit)
                                                    {
                                                        constraint->AddTypeExpr(typeExpr.release());
                                                    }
                                                    *parentMatch13 = match;
                                                }
                                                *parentMatch12 = match;
                                            }
                                            *parentMatch11 = match;
                                        }
                                        if (match.hit)
                                        {
                                            *parentMatch10 = match;
                                        }
                                        else
                                        {
                                            lexer.SetPos(save);
                                            break;
                                        }
                                    }
                                }
                            }
                            *parentMatch9 = match;
                        }
                        *parentMatch7 = match;
                    }
                    *parentMatch6 = match;
                }
                *parentMatch5 = match;
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
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch16 = &match;
                {
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(false);
                    if (*lexer == RANGLE)
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
                        lexer.AddError(span, ToUtf32(GetTokenInfo(RANGLE)));
                    }
                }
                if (match.hit)
                {
                    constraint->SetSpanEnd(span.end);
                    constraint->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MultiParamConstraint"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, constraint.release());
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MultiParamConstraint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("MultiParamConstraint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::Axiom(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ConceptNode* conceptNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Axiom"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<AxiomNode> axiomNode = std::unique_ptr<AxiomNode>();
    Span axiomSpan = Span();
    Span beginBraceSpan = Span();
    Span endBraceSpan = Span();
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
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(false);
                                        if (*lexer == AXIOM)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        if (match.hit)
                                        {
                                            axiomSpan = span;
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
                                            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                            id.reset(static_cast<IdentifierNode*>(match.value));
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
                                        soulng::parser::Match match(true);
                                        if (match.hit)
                                        {
                                            axiomNode.reset(new AxiomNode(axiomSpan, *moduleId, id.release()));
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
                                soulng::parser::Match match(true);
                                int64_t save = lexer.GetPos();
                                soulng::parser::Match* parentMatch14 = &match;
                                {
                                    soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, axiomNode.get());
                                    if (match.hit)
                                    {
                                        *parentMatch14 = match;
                                    }
                                    else
                                    {
                                        lexer.SetPos(save);
                                    }
                                }
                                *parentMatch13 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
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
                                        match.hit = true;
                                        *parentMatch17 = match;
                                        lexer.AddError(span, ToUtf32(GetTokenInfo(LBRACE)));
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch18 = &match;
                    {
                        soulng::parser::Match match = NothrowConceptParser::AxiomBody(lexer, moduleId, ctx, axiomNode.get());
                        *parentMatch18 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
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
                                match.hit = true;
                                *parentMatch21 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACE)));
                            }
                        }
                        if (match.hit)
                        {
                            endBraceSpan = span;
                        }
                        *parentMatch20 = match;
                    }
                    *parentMatch19 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            axiomNode->SetBeginBraceSpan(beginBraceSpan);
            axiomNode->SetEndBraceSpan(endBraceSpan);
            axiomNode->SetLexerFlags(lexer.Flags());
            conceptNode->AddAxiom(axiomNode.release());
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Axiom"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Axiom"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::AxiomBody(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::AxiomNode* axiom)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AxiomBody"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<AxiomStatementNode> stmt;
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
                        soulng::parser::Match match = NothrowConceptParser::AxiomStatement(lexer, moduleId, ctx);
                        stmt.reset(static_cast<AxiomStatementNode*>(match.value));
                        if (match.hit)
                        {
                            axiom->AddStatement(stmt.release());
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AxiomBody"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AxiomBody"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowConceptParser::AxiomStatement(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AxiomStatement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<Node> expr;
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
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match = NothrowExpressionParser::Expression(lexer, moduleId, ctx);
                    expr.reset(static_cast<Node*>(match.value));
                    if (match.hit)
                    {
                        s = span;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
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
                        soulng::parser::Match* parentMatch6 = &match;
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
                                *parentMatch6 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch6 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(SEMICOLON)));
                            }
                        }
                        if (match.hit)
                        {
                            s.end = span.end;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            AxiomStatementNode * node = new AxiomStatementNode(s, *moduleId, expr.release(), lexer.GetMatch(s));
            node->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AxiomStatement"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, node);
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AxiomStatement"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AxiomStatement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
