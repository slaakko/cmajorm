#include "CommandLine.hpp"
#include <soulng/util/Unicode.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/parser/Range.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmparser/CommandLine.parser' using soulng parser generator spg version 3.10.0

static const soulng::parser::Range s0[] = {{9, 9}, {32, 32}};

static const soulng::parser::Range s1[] = {{9, 9}, {10, 10}, {13, 13}, {32, 32}};

static const soulng::parser::Range s2[] = {{92, 92}};

static const soulng::parser::Range s3[] = {{92, 92}};

static const soulng::parser::Range s4[] = {{92, 92}};

static const soulng::parser::Range s5[] = {{10, 10}, {13, 13}};

using namespace soulng::unicode;
using namespace soulng::lexer;

std::vector<std::string> CommandLineParser::Parse(TrivialLexer& lexer)
{
    std::unique_ptr<soulng::parser::Value<std::vector<std::string>>> value;
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (lexer.Log())
    {
        lexer.Log()->WriteBeginRule(soulng::unicode::ToUtf32("parse"));
        lexer.Log()->IncIndent();
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    ++lexer;
    soulng::lexer::Span span = lexer.GetSpan();
    soulng::parser::Match match = CommandLineParser::CommandLine(lexer);
    value.reset(static_cast<soulng::parser::Value<std::vector<std::string>>*>(match.value));
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (lexer.Log())
    {
        lexer.Log()->DecIndent();
        lexer.Log()->WriteEndRule(soulng::unicode::ToUtf32("parse"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (match.hit)
    {
        if (*lexer == soulng::lexer::END_TOKEN)
        {
            return value->value;
        }
        else
        {
            lexer.ThrowExpectationFailure(lexer.GetSpan(), ToUtf32(soulng::lexer::GetEndTokenInfo()));
        }
    }
    else
    {
        lexer.ThrowExpectationFailure(span, U"CommandLine");
    }
    return value->value;
}

soulng::parser::Match CommandLineParser::CommandLine(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CommandLine"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::vector<std::string> value = std::vector<std::string>();
    std::unique_ptr<soulng::parser::Value<std::string>> first;
    std::unique_ptr<soulng::parser::Value<std::string>> next;
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
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = CommandLineParser::Spaces(lexer);
                                if (match.hit)
                                {
                                    *parentMatch6 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
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
                                    soulng::parser::Match match = CommandLineParser::Argument(lexer);
                                    first.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                    if (match.hit)
                                    {
                                        value.push_back(first->value);
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
                                while (true)
                                {
                                    int64_t save = lexer.GetPos();
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch11 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch12 = &match;
                                            {
                                                soulng::parser::Match match = CommandLineParser::Spaces(lexer);
                                                *parentMatch12 = match;
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
                                                        soulng::parser::Match match = CommandLineParser::Argument(lexer);
                                                        next.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                                        if (match.hit)
                                                        {
                                                            value.push_back(next->value);
                                                        }
                                                        *parentMatch14 = match;
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch15 = &match;
                    {
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch16 = &match;
                        {
                            soulng::parser::Match match = CommandLineParser::Spaces(lexer);
                            if (match.hit)
                            {
                                *parentMatch16 = match;
                            }
                            else
                            {
                                lexer.SetPos(save);
                            }
                        }
                        *parentMatch15 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch17 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == 10)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch17 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CommandLine"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<std::vector<std::string>>(value));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CommandLine"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CommandLine"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::Spaces(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Spaces"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        for (const soulng::parser::Range& range : s0)
        {
            if (*lexer >= range.first && *lexer <= range.last)
            {
                match.hit = true;
                ++lexer;
                break;
            }
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(true);
        soulng::parser::Match* parentMatch1 = &match;
        while (true)
        {
            int64_t save = lexer.GetPos();
            {
                soulng::parser::Match match(false);
                for (const soulng::parser::Range& range : s0)
                {
                    if (*lexer >= range.first && *lexer <= range.last)
                    {
                        match.hit = true;
                        ++lexer;
                        break;
                    }
                }
                if (match.hit)
                {
                    *parentMatch1 = match;
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Spaces"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Spaces"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::Argument(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Argument"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::string value = std::string();
    std::unique_ptr<soulng::parser::Value<std::string>> element;
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
                        int64_t pos = lexer.GetPos();
                        soulng::parser::Match match = CommandLineParser::ArgElement(lexer);
                        element.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                        if (match.hit)
                        {
                            value.append(element->value);
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch5 = &match;
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
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = CommandLineParser::ArgElement(lexer);
                                element.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                if (match.hit)
                                {
                                    value.append(element->value);
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
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Argument"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<std::string>(value));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Argument"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Argument"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::ArgElement(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ArgElement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    std::string value = std::string();
    std::unique_ptr<soulng::parser::Value<std::string>> o;
    std::unique_ptr<soulng::parser::Value<std::string>> s;
    std::unique_ptr<soulng::parser::Value<std::string>> sc;
    std::unique_ptr<soulng::parser::Value<std::string>> e;
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
                soulng::parser::Match match = CommandLineParser::OddBackslashesAndLiteralQuotationMark(lexer);
                o.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                if (match.hit)
                {
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArgElement"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, new soulng::parser::Value<std::string>(o->value));
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = CommandLineParser::EvenBackslashesAndQuotationMark(lexer);
                                s.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                if (match.hit)
                                {
                                    value.append(s->value);
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
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch8 = &match;
                                {
                                    while (true)
                                    {
                                        int64_t save = lexer.GetPos();
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch9 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch10 = &match;
                                                {
                                                    int64_t pos = lexer.GetPos();
                                                    soulng::parser::Match match = CommandLineParser::StringChar(lexer);
                                                    sc.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                                    if (match.hit)
                                                    {
                                                        value.append(sc->value);
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
                                                break;
                                            }
                                        }
                                    }
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
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch12 = &match;
                            {
                                int64_t pos = lexer.GetPos();
                                soulng::parser::Match match = CommandLineParser::EvenBackslashesAndQuotationMark(lexer);
                                e.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                if (match.hit)
                                {
                                    value.append(e->value);
                                    {
                                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArgElement"));
                                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                                        return soulng::parser::Match(true, new soulng::parser::Value<std::string>(value));
                                    }
                                }
                                *parentMatch12 = match;
                            }
                            *parentMatch11 = match;
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
            soulng::parser::Match* parentMatch13 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch14 = &match;
                {
                    int64_t pos = lexer.GetPos();
                    soulng::lexer::Span span = lexer.GetSpan();
                    soulng::parser::Match match(lexer.Pos() != lexer.End());
                    for (const soulng::parser::Range& range : s1)
                    {
                        if (*lexer >= range.first && *lexer <= range.last)
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    if (match.hit)
                    {
                        ++lexer;
                    }
                    if (match.hit)
                    {
                        value.append(ToUtf8(lexer.GetMatch(span)));
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArgElement"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::string>(value));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ArgElement"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ArgElement"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::OddBackslashesAndLiteralQuotationMark(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("OddBackslashesAndLiteralQuotationMark"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::string value = std::string();
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
                for (const soulng::parser::Range& range : s2)
                {
                    if (*lexer >= range.first && *lexer <= range.last)
                    {
                        match.hit = true;
                        ++lexer;
                        break;
                    }
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
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    while (true)
                    {
                        int64_t save = lexer.GetPos();
                        {
                            soulng::parser::Match match(false);
                            for (const soulng::parser::Range& range : s3)
                            {
                                if (*lexer >= range.first && *lexer <= range.last)
                                {
                                    match.hit = true;
                                    ++lexer;
                                    break;
                                }
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                bool pass = true;
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    s.end = span.end;
                    std::u32string bs = lexer.GetMatch(s);
                    int n = bs.length() - 1;
                    pass = n % 2 == 1;
                    if (pass)
                    {
                        value = std::string(n / 2, '\\');
                        value.append(1, '"');
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OddBackslashesAndLiteralQuotationMark"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::string>(value));
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("OddBackslashesAndLiteralQuotationMark"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("OddBackslashesAndLiteralQuotationMark"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::EvenBackslashesAndQuotationMark(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EvenBackslashesAndQuotationMark"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::string value = std::string();
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
                soulng::parser::Match match(true);
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
                soulng::parser::Match match(true);
                soulng::parser::Match* parentMatch4 = &match;
                {
                    while (true)
                    {
                        int64_t save = lexer.GetPos();
                        {
                            soulng::parser::Match match(false);
                            for (const soulng::parser::Range& range : s4)
                            {
                                if (*lexer >= range.first && *lexer <= range.last)
                                {
                                    match.hit = true;
                                    ++lexer;
                                    break;
                                }
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                bool pass = true;
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    s.end = span.end;
                    std::u32string bs = lexer.GetMatch(s);
                    int n = bs.length() - 1;
                    pass = n % 2 == 0;
                    if (pass)
                    {
                        value = std::string(n / 2, '\\');
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EvenBackslashesAndQuotationMark"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::string>(value));
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EvenBackslashesAndQuotationMark"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EvenBackslashesAndQuotationMark"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match CommandLineParser::StringChar(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("StringChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    Span s = Span();
    std::unique_ptr<soulng::parser::Value<std::string>> o;
    std::unique_ptr<soulng::parser::Value<std::string>> m;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = CommandLineParser::OddBackslashesAndLiteralQuotationMark(lexer);
            o.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StringChar"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, new soulng::parser::Value<std::string>(o->value));
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
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        int64_t save = lexer.GetPos();
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::lexer::Span span = lexer.GetSpan();
                                    soulng::parser::Match match(lexer.Pos() != lexer.End());
                                    for (const soulng::parser::Range& range : s5)
                                    {
                                        if (*lexer >= range.first && *lexer <= range.last)
                                        {
                                            match.hit = false;
                                            break;
                                        }
                                    }
                                    if (match.hit)
                                    {
                                        ++lexer;
                                    }
                                    if (match.hit)
                                    {
                                        s = span;
                                    }
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
                                int64_t tmp = lexer.GetPos();
                                lexer.SetPos(save);
                                save = tmp;
                                soulng::parser::Match match = CommandLineParser::EvenBackslashesAndQuotationMark(lexer);
                                m.reset(static_cast<soulng::parser::Value<std::string>*>(match.value));
                                *parentMatch8 = match;
                            }
                            if (!match.hit)
                            {
                                lexer.SetPos(save);
                            }
                            *parentMatch5 = soulng::parser::Match(!match.hit, match.value);
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StringChar"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::string>(ToUtf8(lexer.GetMatch(s))));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StringChar"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("StringChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
