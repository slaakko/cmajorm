#include "Class.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowparser/Attribute.hpp>
#include <sngcm/cmnothrowparser/Specifier.hpp>
#include <sngcm/cmnothrowparser/Identifier.hpp>
#include <sngcm/cmnothrowparser/TypeExpr.hpp>
#include <sngcm/cmnothrowparser/Concept.hpp>
#include <sngcm/cmnothrowparser/Expression.hpp>
#include <sngcm/cmnothrowparser/Statement.hpp>
#include <sngcm/cmnothrowparser/Parameter.hpp>
#include <sngcm/cmnothrowparser/Function.hpp>
#include <sngcm/cmnothrowparser/Template.hpp>
#include <sngcm/cmnothrowparser/Typedef.hpp>
#include <sngcm/cmnothrowparser/Enumeration.hpp>
#include <sngcm/cmnothrowparser/Constant.hpp>
#include <sngcm/cmnothrowparser/Delegate.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/Class.parser' using soulng parser generator spg version 3.10.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowClassParser::Class(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Class"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ClassNode> cls = std::unique_ptr<ClassNode>();
    Span s = Span();
    Span specifierSpan = Span();
    Span classSpan = Span();
    Span beginBraceSpan = Span();
    Span endBraceSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<WhereConstraintNode> constraint;
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
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch9 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::lexer::Span span = lexer.GetSpan();
                                                soulng::parser::Match match(true);
                                                if (match.hit)
                                                {
                                                    s = span;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            *parentMatch9 = match;
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
                                                    soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                                    attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
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
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
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
                                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                if (match.hit)
                                                {
                                                    specifierSpan = span;
                                                }
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch13 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
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
                                            soulng::parser::Match match(false);
                                            if (*lexer == CLASS)
                                            {
                                                ++lexer;
                                                match.hit = true;
                                            }
                                            if (match.hit)
                                            {
                                                classSpan = span;
                                            }
                                            *parentMatch16 = match;
                                        }
                                        *parentMatch15 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
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
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                        id.reset(static_cast<IdentifierNode*>(match.value));
                                        if (match.hit)
                                        {
                                            s.end = span.end;
                                            cls.reset(new ClassNode(s, *moduleId, specifiers->value, id.release(), attrs.release()));
                                            cls->SetSpecifierSpan(specifierSpan);
                                            cls->SetClassSpan(classSpan);
                                        }
                                        *parentMatch18 = match;
                                    }
                                    *parentMatch17 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
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
                                    soulng::parser::Match match = NothrowTemplateParser::TemplateParameterList(lexer, moduleId, ctx, cls.get());
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
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch21 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch22 = &match;
                            {
                                soulng::parser::Match match = NothrowClassParser::InheritanceAndInterfaces(lexer, moduleId, ctx, cls.get());
                                if (match.hit)
                                {
                                    *parentMatch22 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch21 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch23 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch24 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch25 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch26 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        cls->SetConstraint(constraint.release());
                                    }
                                    *parentMatch26 = match;
                                }
                                *parentMatch25 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch24 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch23 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch27 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch28 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch29 = &match;
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
                                *parentMatch29 = match;
                            }
                            else
                            {
                                match.hit = true;
                                *parentMatch29 = match;
                                lexer.AddError(span, ToUtf32(GetTokenInfo(LBRACE)));
                            }
                        }
                        if (match.hit)
                        {
                            beginBraceSpan = span;
                        }
                        *parentMatch28 = match;
                    }
                    *parentMatch27 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch30 = &match;
            {
                soulng::parser::Match match = NothrowClassParser::ClassContent(lexer, moduleId, ctx, cls.get());
                *parentMatch30 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch31 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch32 = &match;
            {
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch33 = &match;
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
                        *parentMatch33 = match;
                    }
                    else
                    {
                        match.hit = true;
                        *parentMatch33 = match;
                        lexer.AddError(span, ToUtf32(GetTokenInfo(RBRACE)));
                    }
                }
                if (match.hit)
                {
                    endBraceSpan = span;
                    cls->SetBeginBraceSpan(beginBraceSpan);
                    cls->SetEndBraceSpan(endBraceSpan);
                    cls->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Class"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, cls.release());
                    }
                }
                *parentMatch32 = match;
            }
            *parentMatch31 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Class"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Class"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::InheritanceAndInterfaces(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("InheritanceAndInterfaces"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<Node> baseOrIntf;
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
                            soulng::parser::Match match(true);
                            if (match.hit)
                            {
                                ctx->BeginParsingTypeExpr();
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
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowClassParser::BaseClassOrInterface(lexer, moduleId, ctx);
                                baseOrIntf.reset(static_cast<Node*>(match.value));
                                if (match.hit)
                                {
                                    classNode->AddBaseClassOrInterface(baseOrIntf.release());
                                    ctx->EndParsingTypeExpr();
                                }
                                else
                                {
                                    ctx->EndParsingTypeExpr();
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch6 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch13 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch14 = &match;
                                                {
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::parser::Match match(true);
                                                    if (match.hit)
                                                    {
                                                        ctx->BeginParsingTypeExpr();
                                                    }
                                                    *parentMatch14 = match;
                                                }
                                                *parentMatch13 = match;
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
                                                        soulng::parser::Match match = NothrowClassParser::BaseClassOrInterface(lexer, moduleId, ctx);
                                                        baseOrIntf.reset(static_cast<Node*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            classNode->AddBaseClassOrInterface(baseOrIntf.release());
                                                            ctx->EndParsingTypeExpr();
                                                        }
                                                        else
                                                        {
                                                            ctx->EndParsingTypeExpr();
                                                        }
                                                        *parentMatch16 = match;
                                                    }
                                                    *parentMatch15 = match;
                                                }
                                                *parentMatch13 = match;
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
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("InheritanceAndInterfaces"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("InheritanceAndInterfaces"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::BaseClassOrInterface(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("BaseClassOrInterface"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<Node> templateId;
    std::unique_ptr<IdentifierNode> qid;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = NothrowTemplateParser::TemplateId(lexer, moduleId, ctx);
            templateId.reset(static_cast<Node*>(match.value));
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BaseClassOrInterface"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, templateId.release());
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
                    soulng::parser::Match match = NothrowIdentifierParser::QualifiedId(lexer, moduleId);
                    qid.reset(static_cast<IdentifierNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BaseClassOrInterface"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, qid.release());
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BaseClassOrInterface"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("BaseClassOrInterface"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::ClassContent(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ClassContent"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<Node> classMember;
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
                        soulng::parser::Match match = NothrowClassParser::ClassMember(lexer, moduleId, ctx, classNode);
                        classMember.reset(static_cast<Node*>(match.value));
                        if (match.hit)
                        {
                            classNode->AddMember(classMember.release());
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassContent"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ClassContent"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::ClassMember(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ClassMember"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<StaticConstructorNode> staticConstructor;
    std::unique_ptr<Node> constructor;
    std::unique_ptr<Node> destructor;
    std::unique_ptr<Node> memberFunction;
    std::unique_ptr<Node> conversionFunction;
    std::unique_ptr<MemberVariableNode> memberVariable;
    std::unique_ptr<TypedefNode> typedefinition;
    std::unique_ptr<ClassNode> cls;
    std::unique_ptr<EnumTypeNode> enumType;
    std::unique_ptr<ConstantNode> constant;
    std::unique_ptr<DelegateNode> delegate;
    std::unique_ptr<ClassDelegateNode> classDelegate;
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                int64_t save = lexer.GetPos();
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    int64_t save = lexer.GetPos();
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
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::parser::Match match = NothrowClassParser::StaticConstructor(lexer, moduleId, ctx, classNode);
                                                    staticConstructor.reset(static_cast<StaticConstructorNode*>(match.value));
                                                    if (match.hit)
                                                    {
                                                        {
                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                            return soulng::parser::Match(true, staticConstructor.release());
                                                        }
                                                    }
                                                    *parentMatch11 = match;
                                                }
                                                *parentMatch10 = match;
                                                if (!match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch12 = &match;
                                                    lexer.SetPos(save);
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch13 = &match;
                                                        {
                                                            int64_t pos = lexer.GetPos();
                                                            soulng::parser::Match match = NothrowClassParser::Constructor(lexer, moduleId, ctx, classNode);
                                                            constructor.reset(static_cast<Node*>(match.value));
                                                            if (match.hit)
                                                            {
                                                                {
                                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                    return soulng::parser::Match(true, constructor.release());
                                                                }
                                                            }
                                                            *parentMatch13 = match;
                                                        }
                                                        *parentMatch12 = match;
                                                    }
                                                    *parentMatch10 = match;
                                                }
                                            }
                                            *parentMatch9 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch14 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch15 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match = NothrowClassParser::Destructor(lexer, moduleId, ctx, classNode);
                                                        destructor.reset(static_cast<Node*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            {
                                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                                return soulng::parser::Match(true, destructor.release());
                                                            }
                                                        }
                                                        *parentMatch15 = match;
                                                    }
                                                    *parentMatch14 = match;
                                                }
                                                *parentMatch9 = match;
                                            }
                                        }
                                        *parentMatch8 = match;
                                        if (!match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch16 = &match;
                                            lexer.SetPos(save);
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch17 = &match;
                                                {
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::parser::Match match = NothrowClassParser::MemberFunction(lexer, moduleId, ctx);
                                                    memberFunction.reset(static_cast<Node*>(match.value));
                                                    if (match.hit)
                                                    {
                                                        {
                                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                            return soulng::parser::Match(true, memberFunction.release());
                                                        }
                                                    }
                                                    *parentMatch17 = match;
                                                }
                                                *parentMatch16 = match;
                                            }
                                            *parentMatch8 = match;
                                        }
                                    }
                                    *parentMatch7 = match;
                                    if (!match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch18 = &match;
                                        lexer.SetPos(save);
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch19 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::parser::Match match = NothrowClassParser::ConversionFunction(lexer, moduleId, ctx);
                                                conversionFunction.reset(static_cast<Node*>(match.value));
                                                if (match.hit)
                                                {
                                                    {
                                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                        return soulng::parser::Match(true, conversionFunction.release());
                                                    }
                                                }
                                                *parentMatch19 = match;
                                            }
                                            *parentMatch18 = match;
                                        }
                                        *parentMatch7 = match;
                                    }
                                }
                                *parentMatch6 = match;
                                if (!match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch20 = &match;
                                    lexer.SetPos(save);
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch21 = &match;
                                        {
                                            int64_t pos = lexer.GetPos();
                                            soulng::parser::Match match = NothrowClassParser::MemberVariable(lexer, moduleId, ctx);
                                            memberVariable.reset(static_cast<MemberVariableNode*>(match.value));
                                            if (match.hit)
                                            {
                                                {
                                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                    return soulng::parser::Match(true, memberVariable.release());
                                                }
                                            }
                                            *parentMatch21 = match;
                                        }
                                        *parentMatch20 = match;
                                    }
                                    *parentMatch6 = match;
                                }
                            }
                            *parentMatch5 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch22 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch23 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::parser::Match match = NothrowTypedefParser::Typedef(lexer, moduleId, ctx);
                                        typedefinition.reset(static_cast<TypedefNode*>(match.value));
                                        if (match.hit)
                                        {
                                            {
                                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                                return soulng::parser::Match(true, typedefinition.release());
                                            }
                                        }
                                        *parentMatch23 = match;
                                    }
                                    *parentMatch22 = match;
                                }
                                *parentMatch5 = match;
                            }
                        }
                        *parentMatch4 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch24 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch25 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowClassParser::Class(lexer, moduleId, ctx);
                                    cls.reset(static_cast<ClassNode*>(match.value));
                                    if (match.hit)
                                    {
                                        {
                                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                            return soulng::parser::Match(true, cls.release());
                                        }
                                    }
                                    *parentMatch25 = match;
                                }
                                *parentMatch24 = match;
                            }
                            *parentMatch4 = match;
                        }
                    }
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch26 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch27 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowEnumerationParser::EnumType(lexer, moduleId, ctx);
                                enumType.reset(static_cast<EnumTypeNode*>(match.value));
                                if (match.hit)
                                {
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, enumType.release());
                                    }
                                }
                                *parentMatch27 = match;
                            }
                            *parentMatch26 = match;
                        }
                        *parentMatch3 = match;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch28 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch29 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::parser::Match match = NothrowConstantParser::Constant(lexer, moduleId, ctx);
                            constant.reset(static_cast<ConstantNode*>(match.value));
                            if (match.hit)
                            {
                                {
                                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                    return soulng::parser::Match(true, constant.release());
                                }
                            }
                            *parentMatch29 = match;
                        }
                        *parentMatch28 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch30 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch31 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = NothrowDelegateParser::Delegate(lexer, moduleId, ctx);
                        delegate.reset(static_cast<DelegateNode*>(match.value));
                        if (match.hit)
                        {
                            {
                                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                return soulng::parser::Match(true, delegate.release());
                            }
                        }
                        *parentMatch31 = match;
                    }
                    *parentMatch30 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch32 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch33 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::parser::Match match = NothrowDelegateParser::ClassDelegate(lexer, moduleId, ctx);
                    classDelegate.reset(static_cast<ClassDelegateNode*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, classDelegate.release());
                        }
                    }
                    *parentMatch33 = match;
                }
                *parentMatch32 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ClassMember"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ClassMember"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::StaticConstructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("StaticConstructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<StaticConstructorNode> staticConstructor = std::unique_ptr<StaticConstructorNode>();
    Span s = Span();
    Span specifierSpan = Span();
    std::u32string className = std::u32string();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<InitializerNode> initializer;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::lexer::Span span = lexer.GetSpan();
                                                soulng::parser::Match match(true);
                                                if (match.hit)
                                                {
                                                    s = span;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            *parentMatch9 = match;
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
                                                    soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                                    attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
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
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
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
                                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                if (match.hit)
                                                {
                                                    specifierSpan = span;
                                                }
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch13 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
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
                                            soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                            id.reset(static_cast<IdentifierNode*>(match.value));
                                            if (match.hit)
                                            {
                                                className = lexer.GetMatch(span);
                                            }
                                            *parentMatch16 = match;
                                        }
                                        *parentMatch15 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch17 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == LPAREN)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch17 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
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
                                    bool pass = true;
                                    soulng::parser::Match match(false);
                                    if (*lexer == RPAREN)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    if (match.hit)
                                    {
                                        s.end = span.end;
                                        if (className != classNode->Id()->Str()) pass = false;
                                        else if (!StaticConstructorSpecifiers(specifiers->value)) pass = false;
                                        else
                                        {
                                            staticConstructor.reset(new StaticConstructorNode(s, *moduleId, specifiers->value, attrs.release()));
                                            staticConstructor->SetClassId(id.release());
                                            staticConstructor->SetSpecifierSpan(specifierSpan);
                                        }
                                    }
                                    if (match.hit && !pass)
                                    {
                                        match = soulng::parser::Match(false);
                                    }
                                    *parentMatch19 = match;
                                }
                                *parentMatch18 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch20 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch21 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch22 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch23 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        if (*lexer == COLON)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        *parentMatch23 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch24 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch25 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch26 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch27 = &match;
                                                    {
                                                        int64_t pos = lexer.GetPos();
                                                        soulng::parser::Match match = NothrowClassParser::Initializer(lexer, moduleId, ctx);
                                                        initializer.reset(static_cast<InitializerNode*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            staticConstructor->AddInitializer(initializer.release());
                                                        }
                                                        *parentMatch27 = match;
                                                    }
                                                    *parentMatch26 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch28 = &match;
                                                    {
                                                        soulng::parser::Match match(true);
                                                        soulng::parser::Match* parentMatch29 = &match;
                                                        {
                                                            while (true)
                                                            {
                                                                int64_t save = lexer.GetPos();
                                                                {
                                                                    soulng::parser::Match match(false);
                                                                    soulng::parser::Match* parentMatch30 = &match;
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        if (*lexer == COMMA)
                                                                        {
                                                                            ++lexer;
                                                                            match.hit = true;
                                                                        }
                                                                        *parentMatch30 = match;
                                                                    }
                                                                    if (match.hit)
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch31 = &match;
                                                                        {
                                                                            soulng::parser::Match match(false);
                                                                            soulng::parser::Match* parentMatch32 = &match;
                                                                            {
                                                                                int64_t pos = lexer.GetPos();
                                                                                soulng::parser::Match match = NothrowClassParser::Initializer(lexer, moduleId, ctx);
                                                                                initializer.reset(static_cast<InitializerNode*>(match.value));
                                                                                if (match.hit)
                                                                                {
                                                                                    staticConstructor->AddInitializer(initializer.release());
                                                                                }
                                                                                *parentMatch32 = match;
                                                                            }
                                                                            *parentMatch31 = match;
                                                                        }
                                                                        *parentMatch30 = match;
                                                                    }
                                                                    if (match.hit)
                                                                    {
                                                                        *parentMatch29 = match;
                                                                    }
                                                                    else
                                                                    {
                                                                        lexer.SetPos(save);
                                                                        break;
                                                                    }
                                                                }
                                                            }
                                                        }
                                                        *parentMatch28 = match;
                                                    }
                                                    *parentMatch26 = match;
                                                }
                                                *parentMatch25 = match;
                                            }
                                            *parentMatch24 = match;
                                        }
                                        *parentMatch23 = match;
                                    }
                                    *parentMatch22 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch21 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch20 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch33 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch34 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch35 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch36 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        staticConstructor->SetConstraint(constraint.release());
                                    }
                                    *parentMatch36 = match;
                                }
                                *parentMatch35 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch34 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch33 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch37 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch38 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch39 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch40 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                                body.reset(static_cast<CompoundStatementNode*>(match.value));
                                if (match.hit)
                                {
                                    staticConstructor->SetBody(body.release());
                                }
                                *parentMatch40 = match;
                            }
                            *parentMatch39 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch41 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == SEMICOLON)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch41 = match;
                                }
                                *parentMatch39 = match;
                            }
                        }
                        *parentMatch38 = match;
                    }
                    *parentMatch37 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            staticConstructor->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StaticConstructor"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, staticConstructor.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StaticConstructor"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("StaticConstructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::Constructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Constructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConstructorNode> ctor = std::unique_ptr<ConstructorNode>();
    Span s = Span();
    Span specifierSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<InitializerNode> initializer;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                                soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
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
                                            soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
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
                                    soulng::parser::Match* parentMatch15 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        bool pass = true;
                                        soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                        id.reset(static_cast<IdentifierNode*>(match.value));
                                        if (match.hit)
                                        {
                                            std::u32string className = lexer.GetMatch(span);
                                            s.end = span.end;
                                            if (className != classNode->Id()->Str()) pass = false;
                                            else
                                            {
                                                ctor.reset(new ConstructorNode(s, *moduleId, specifiers->value, attrs.release()));
                                                ctor->SetClassId(id.release());
                                                ctor->SetSpecifierSpan(specifierSpan);
                                            }
                                        }
                                        if (match.hit && !pass)
                                        {
                                            match = soulng::parser::Match(false);
                                        }
                                        *parentMatch15 = match;
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
                            soulng::parser::Match* parentMatch16 = &match;
                            {
                                soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, ctor.get());
                                *parentMatch16 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch17 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch18 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch19 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch20 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        if (*lexer == COLON)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        *parentMatch20 = match;
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
                                                        soulng::parser::Match match = NothrowClassParser::Initializer(lexer, moduleId, ctx);
                                                        initializer.reset(static_cast<InitializerNode*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            ctor->AddInitializer(initializer.release());
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
                                                                                soulng::parser::Match match = NothrowClassParser::Initializer(lexer, moduleId, ctx);
                                                                                initializer.reset(static_cast<InitializerNode*>(match.value));
                                                                                if (match.hit)
                                                                                {
                                                                                    ctor->AddInitializer(initializer.release());
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
                                        *parentMatch20 = match;
                                    }
                                    *parentMatch19 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch18 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch17 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch30 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch31 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch32 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch33 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        ctor->SetConstraint(constraint.release());
                                    }
                                    *parentMatch33 = match;
                                }
                                *parentMatch32 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch31 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch30 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch34 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch35 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch36 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch37 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                                body.reset(static_cast<CompoundStatementNode*>(match.value));
                                if (match.hit)
                                {
                                    ctor->SetBody(body.release());
                                }
                                *parentMatch37 = match;
                            }
                            *parentMatch36 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch38 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == SEMICOLON)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch38 = match;
                                }
                                *parentMatch36 = match;
                            }
                        }
                        *parentMatch35 = match;
                    }
                    *parentMatch34 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            ctor->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Constructor"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, ctor.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Constructor"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Constructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::Destructor(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx, sngcm::ast::ClassNode* classNode)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Destructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<DestructorNode> dtor = std::unique_ptr<DestructorNode>();
    Span s = Span();
    Span specifierSpan = Span();
    std::u32string className = std::u32string();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<IdentifierNode> id;
    std::unique_ptr<WhereConstraintNode> contraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::lexer::Span span = lexer.GetSpan();
                                                soulng::parser::Match match(true);
                                                if (match.hit)
                                                {
                                                    s = span;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            *parentMatch9 = match;
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
                                                    soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                                    attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
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
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
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
                                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                if (match.hit)
                                                {
                                                    specifierSpan = span;
                                                }
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch13 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch15 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        if (*lexer == CPL)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        *parentMatch15 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch16 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch17 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                        id.reset(static_cast<IdentifierNode*>(match.value));
                                        if (match.hit)
                                        {
                                            className = lexer.GetMatch(span);
                                        }
                                        *parentMatch17 = match;
                                    }
                                    *parentMatch16 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch18 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch19 = &match;
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
                                        *parentMatch19 = match;
                                    }
                                    else
                                    {
                                        match.hit = true;
                                        *parentMatch19 = match;
                                        lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
                                    }
                                }
                                *parentMatch18 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch20 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch21 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::lexer::Span span = lexer.GetSpan();
                                bool pass = true;
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch22 = &match;
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
                                        *parentMatch22 = match;
                                    }
                                    else
                                    {
                                        match.hit = true;
                                        *parentMatch22 = match;
                                        lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                                    }
                                }
                                if (match.hit)
                                {
                                    s.end = span.end;
                                    if (className != classNode->Id()->Str()) pass = false;
                                    else
                                    {
                                        dtor.reset(new DestructorNode(s, *moduleId, specifiers->value, attrs.release()));
                                        dtor->SetClassId(id.release());
                                        dtor->SetSpecifierSpan(specifierSpan);
                                    }
                                }
                                if (match.hit && !pass)
                                {
                                    match = soulng::parser::Match(false);
                                }
                                *parentMatch21 = match;
                            }
                            *parentMatch20 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch23 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch24 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch25 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch26 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    contraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        dtor->SetConstraint(contraint.release());
                                    }
                                    *parentMatch26 = match;
                                }
                                *parentMatch25 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch24 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch23 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch27 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch28 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch29 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch30 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                                body.reset(static_cast<CompoundStatementNode*>(match.value));
                                if (match.hit)
                                {
                                    dtor->SetBody(body.release());
                                }
                                *parentMatch30 = match;
                            }
                            *parentMatch29 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch31 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == SEMICOLON)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch31 = match;
                                }
                                *parentMatch29 = match;
                            }
                        }
                        *parentMatch28 = match;
                    }
                    *parentMatch27 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            dtor->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Destructor"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, dtor.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Destructor"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Destructor"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::Initializer(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Initializer"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<InitializerNode> initializer = std::unique_ptr<InitializerNode>();
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
                int64_t save = lexer.GetPos();
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t save = lexer.GetPos();
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
                                    int64_t pos = lexer.GetPos();
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match(false);
                                    if (*lexer == THIS)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    if (match.hit)
                                    {
                                        initializer.reset(new ThisInitializerNode(span, *moduleId));
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
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(false);
                                        if (*lexer == LPAREN)
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
                                            lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
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
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch10 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch11 = &match;
                                {
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match = NothrowExpressionParser::ArgumentList(lexer, moduleId, ctx, initializer.get());
                                    if (match.hit)
                                    {
                                        *parentMatch11 = match;
                                    }
                                    else
                                    {
                                        match.hit = true;
                                        *parentMatch11 = match;
                                        lexer.AddError(span, U"argument list");
                                    }
                                }
                                *parentMatch10 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
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
                                soulng::parser::Match match(false);
                                if (*lexer == RPAREN)
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
                                    lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                                }
                            }
                            *parentMatch12 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch14 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch15 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch16 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch17 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch18 = &match;
                                        {
                                            int64_t pos = lexer.GetPos();
                                            soulng::lexer::Span span = lexer.GetSpan();
                                            soulng::parser::Match match(false);
                                            if (*lexer == BASE)
                                            {
                                                ++lexer;
                                                match.hit = true;
                                            }
                                            if (match.hit)
                                            {
                                                initializer.reset(new BaseInitializerNode(span, *moduleId));
                                            }
                                            *parentMatch18 = match;
                                        }
                                        *parentMatch17 = match;
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
                                                if (*lexer == LPAREN)
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
                                                    lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
                                                }
                                            }
                                            *parentMatch19 = match;
                                        }
                                        *parentMatch17 = match;
                                    }
                                    *parentMatch16 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch21 = &match;
                                    {
                                        soulng::parser::Match match(true);
                                        soulng::parser::Match* parentMatch22 = &match;
                                        {
                                            soulng::lexer::Span span = lexer.GetSpan();
                                            soulng::parser::Match match = NothrowExpressionParser::ArgumentList(lexer, moduleId, ctx, initializer.get());
                                            if (match.hit)
                                            {
                                                *parentMatch22 = match;
                                            }
                                            else
                                            {
                                                match.hit = true;
                                                *parentMatch22 = match;
                                                lexer.AddError(span, U"argument list");
                                            }
                                        }
                                        *parentMatch21 = match;
                                    }
                                    *parentMatch16 = match;
                                }
                                *parentMatch15 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch23 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch24 = &match;
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
                                            *parentMatch24 = match;
                                        }
                                        else
                                        {
                                            match.hit = true;
                                            *parentMatch24 = match;
                                            lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                                        }
                                    }
                                    *parentMatch23 = match;
                                }
                                *parentMatch15 = match;
                            }
                            *parentMatch14 = match;
                        }
                        *parentMatch3 = match;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch25 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch26 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch27 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch28 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch29 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                                        id.reset(static_cast<IdentifierNode*>(match.value));
                                        if (match.hit)
                                        {
                                            initializer.reset(new MemberInitializerNode(span, *moduleId, id.release()));
                                        }
                                        *parentMatch29 = match;
                                    }
                                    *parentMatch28 = match;
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
                                            if (*lexer == LPAREN)
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
                                                lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
                                            }
                                        }
                                        *parentMatch30 = match;
                                    }
                                    *parentMatch28 = match;
                                }
                                *parentMatch27 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch32 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch33 = &match;
                                    {
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowExpressionParser::ArgumentList(lexer, moduleId, ctx, initializer.get());
                                        if (match.hit)
                                        {
                                            *parentMatch33 = match;
                                        }
                                        else
                                        {
                                            match.hit = true;
                                            *parentMatch33 = match;
                                            lexer.AddError(span, U"argument list");
                                        }
                                    }
                                    *parentMatch32 = match;
                                }
                                *parentMatch27 = match;
                            }
                            *parentMatch26 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch34 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch35 = &match;
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
                                        *parentMatch35 = match;
                                    }
                                    else
                                    {
                                        match.hit = true;
                                        *parentMatch35 = match;
                                        lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                                    }
                                }
                                *parentMatch34 = match;
                            }
                            *parentMatch26 = match;
                        }
                        *parentMatch25 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            initializer->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Initializer"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, initializer.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Initializer"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Initializer"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::MemberFunction(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("MemberFunction"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<MemberFunctionNode> memFun = std::unique_ptr<MemberFunctionNode>();
    Span s = Span();
    Span specifierSpan = Span();
    Span groupIdSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> returnType;
    std::unique_ptr<soulng::parser::Value<std::u32string>> groupId;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::lexer::Span span = lexer.GetSpan();
                                                soulng::parser::Match match(true);
                                                if (match.hit)
                                                {
                                                    s = span;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            *parentMatch9 = match;
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
                                                    soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                                    attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
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
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
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
                                                soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                if (match.hit)
                                                {
                                                    specifierSpan = span;
                                                }
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch13 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch15 = &match;
                                    {
                                        soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                        returnType.reset(static_cast<sngcm::ast::Node*>(match.value));
                                        *parentMatch15 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch16 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch17 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = NothrowFunctionParser::FunctionGroupId(lexer, moduleId, ctx);
                                        groupId.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                        if (match.hit)
                                        {
                                            s.end = span.end;
                                            groupIdSpan = span;
                                            memFun.reset(new MemberFunctionNode(s, *moduleId, specifiers->value, returnType.release(), groupId->value, attrs.release()));
                                            memFun->SetSpecifierSpan(specifierSpan);
                                            memFun->SetGroupIdSpan(groupIdSpan);
                                        }
                                        *parentMatch17 = match;
                                    }
                                    *parentMatch16 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch18 = &match;
                            {
                                soulng::parser::Match match = NothrowParameterParser::ParameterList(lexer, moduleId, ctx, memFun.get());
                                *parentMatch18 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
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
                                        soulng::parser::Match match(false);
                                        if (*lexer == CONST)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        if (match.hit)
                                        {
                                            memFun->SetConst();
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch23 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch24 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch25 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch26 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        memFun->SetConstraint(constraint.release());
                                    }
                                    *parentMatch26 = match;
                                }
                                *parentMatch25 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch24 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch23 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch27 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch28 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch29 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch30 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                                body.reset(static_cast<CompoundStatementNode*>(match.value));
                                if (match.hit)
                                {
                                    memFun->SetBody(body.release());
                                }
                                *parentMatch30 = match;
                            }
                            *parentMatch29 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch31 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == SEMICOLON)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch31 = match;
                                }
                                *parentMatch29 = match;
                            }
                        }
                        *parentMatch28 = match;
                    }
                    *parentMatch27 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            memFun->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberFunction"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, memFun.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberFunction"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("MemberFunction"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::ConversionFunction(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ConversionFunction"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::unique_ptr<ConversionFunctionNode> conversionFun = std::unique_ptr<ConversionFunctionNode>();
    Span s = Span();
    Span specifierSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
    std::unique_ptr<soulng::parser::Value<sngcm::ast::Specifiers>> specifiers;
    std::unique_ptr<sngcm::ast::Node> type;
    std::unique_ptr<WhereConstraintNode> constraint;
    std::unique_ptr<CompoundStatementNode> body;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch10 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch11 = &match;
                                                {
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::lexer::Span span = lexer.GetSpan();
                                                    soulng::parser::Match match(true);
                                                    if (match.hit)
                                                    {
                                                        s = span;
                                                    }
                                                    *parentMatch11 = match;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch12 = &match;
                                                {
                                                    soulng::parser::Match match(true);
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match* parentMatch13 = &match;
                                                    {
                                                        soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
                                                        attrs.reset(static_cast<sngcm::ast::Attributes*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            *parentMatch13 = match;
                                                        }
                                                        else
                                                        {
                                                            lexer.SetPos(save);
                                                        }
                                                    }
                                                    *parentMatch12 = match;
                                                }
                                                *parentMatch10 = match;
                                            }
                                            *parentMatch9 = match;
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
                                                    soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                                                    specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                                                    if (match.hit)
                                                    {
                                                        specifierSpan = span;
                                                    }
                                                    *parentMatch15 = match;
                                                }
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch9 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch16 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            if (*lexer == OPERATOR)
                                            {
                                                ++lexer;
                                                match.hit = true;
                                            }
                                            *parentMatch16 = match;
                                        }
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch17 = &match;
                                    {
                                        soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                                        type.reset(static_cast<sngcm::ast::Node*>(match.value));
                                        *parentMatch17 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch18 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch19 = &match;
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
                                            *parentMatch19 = match;
                                        }
                                        else
                                        {
                                            match.hit = true;
                                            *parentMatch19 = match;
                                            lexer.AddError(span, ToUtf32(GetTokenInfo(LPAREN)));
                                        }
                                    }
                                    *parentMatch18 = match;
                                }
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch20 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch21 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch22 = &match;
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
                                            *parentMatch22 = match;
                                        }
                                        else
                                        {
                                            match.hit = true;
                                            *parentMatch22 = match;
                                            lexer.AddError(span, ToUtf32(GetTokenInfo(RPAREN)));
                                        }
                                    }
                                    if (match.hit)
                                    {
                                        s.end = span.end;
                                        conversionFun.reset(new ConversionFunctionNode(s, *moduleId, specifiers->value, type.release(), attrs.release()));
                                        conversionFun->SetSpecifierSpan(specifierSpan);
                                    }
                                    *parentMatch21 = match;
                                }
                                *parentMatch20 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch23 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch24 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch25 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch26 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::parser::Match match(false);
                                        if (*lexer == CONST)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        if (match.hit)
                                        {
                                            conversionFun->SetConst();
                                        }
                                        *parentMatch26 = match;
                                    }
                                    *parentMatch25 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch24 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch23 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch27 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch28 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch29 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch30 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = NothrowConceptParser::WhereConstraint(lexer, moduleId, ctx);
                                    constraint.reset(static_cast<WhereConstraintNode*>(match.value));
                                    if (match.hit)
                                    {
                                        conversionFun->SetConstraint(constraint.release());
                                    }
                                    *parentMatch30 = match;
                                }
                                *parentMatch29 = match;
                            }
                            if (match.hit)
                            {
                                *parentMatch28 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch27 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch31 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch32 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch33 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch34 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = NothrowStatementParser::CompoundStatement(lexer, moduleId, ctx);
                                body.reset(static_cast<CompoundStatementNode*>(match.value));
                                if (match.hit)
                                {
                                    conversionFun->SetBody(body.release());
                                }
                                *parentMatch34 = match;
                            }
                            *parentMatch33 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch35 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == SEMICOLON)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch35 = match;
                                }
                                *parentMatch33 = match;
                            }
                        }
                        *parentMatch32 = match;
                    }
                    *parentMatch31 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            conversionFun->SetLexerFlags(lexer.Flags());
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConversionFunction"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, conversionFun.release());
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ConversionFunction"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ConversionFunction"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match NothrowClassParser::MemberVariable(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId, NothrowParsingContext* ctx)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("MemberVariable"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    Span specifierSpan = Span();
    std::unique_ptr<sngcm::ast::Attributes> attrs;
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
                                soulng::parser::Match match = NothrowAttributeParser::Attributes(lexer, moduleId);
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch9 = &match;
                        {
                            int64_t pos = lexer.GetPos();
                            soulng::lexer::Span span = lexer.GetSpan();
                            soulng::parser::Match match = NothrowSpecifierParser::Specifiers(lexer);
                            specifiers.reset(static_cast<soulng::parser::Value<sngcm::ast::Specifiers>*>(match.value));
                            if (match.hit)
                            {
                                specifierSpan = span;
                            }
                            *parentMatch9 = match;
                        }
                        *parentMatch8 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch10 = &match;
                {
                    soulng::parser::Match match = NothrowTypeExprParser::TypeExpr(lexer, moduleId, ctx);
                    type.reset(static_cast<sngcm::ast::Node*>(match.value));
                    *parentMatch10 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch11 = &match;
            {
                soulng::parser::Match match = NothrowIdentifierParser::Identifier(lexer, moduleId);
                id.reset(static_cast<IdentifierNode*>(match.value));
                *parentMatch11 = match;
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
                    MemberVariableNode * value = new MemberVariableNode(s, *moduleId, specifiers->value, type.release(), id.release(), attrs.release());
                    value->SetSpecifierSpan(specifierSpan);
                    value->SetLexerFlags(lexer.Flags());
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberVariable"));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MemberVariable"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("MemberVariable"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
