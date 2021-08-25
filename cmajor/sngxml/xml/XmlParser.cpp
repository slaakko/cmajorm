#include "XmlParser.hpp"
#include <soulng/util/Unicode.hpp>
#include <soulng/lexer/TrivialLexer.hpp>
#include <soulng/parser/Range.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngxml/xml/XmlParser.parser' using soulng parser generator spg version 4.0.0

static const soulng::parser::Range s0[] = {{9, 9}, {10, 10}, {13, 13}, {32, 55295}, {57344, 65533}, {65536, 1114111}};

static const soulng::parser::Range s1[] = {{32, 32}, {9, 9}, {13, 13}, {10, 10}};

static const soulng::parser::Range s2[] = {{58, 58}, {65, 90}, {95, 95}, {97, 122}, {192, 214}, {216, 246}, {248, 767}, {880, 893}, {895, 8191}, {8204, 8205}, {8304, 8591}, {11264, 12271}, {12289, 55295}, {63744, 64975}, {65008, 65533}, {65536, 983039}};

static const soulng::parser::Range s3[] = {{45, 45}, {46, 46}, {48, 57}, {183, 183}, {768, 879}, {8255, 8256}};

static const int s4[] = {60, 63, 120, 109, 108};

static const int s5[] = {63, 62};

static const int s6[] = {118, 101, 114, 115, 105, 111, 110};

static const soulng::parser::Range s7[] = {{48, 57}};

static const int s8[] = {101, 110, 99, 111, 100, 105, 110, 103};

static const soulng::parser::Range s9[] = {{65, 90}, {97, 122}};

static const soulng::parser::Range s10[] = {{65, 90}, {97, 122}, {48, 57}, {46, 46}, {95, 95}, {45, 45}};

static const int s11[] = {115, 116, 97, 110, 100, 97, 108, 111, 110, 101};

static const int s12[] = {60, 33, 68, 79, 67, 84, 89, 80, 69};

static const int s13[] = {83, 89, 83, 84, 69, 77};

static const int s14[] = {80, 85, 66, 76, 73, 67};

static const soulng::parser::Range s15[] = {{34, 34}};

static const soulng::parser::Range s16[] = {{39, 39}};

static const soulng::parser::Range s17[] = {{32, 32}, {13, 13}, {10, 10}};

static const soulng::parser::Range s18[] = {{97, 122}, {65, 90}, {48, 57}};

static const soulng::parser::Range s19[] = {{45, 45}, {39, 39}, {40, 40}, {41, 41}, {43, 43}, {44, 44}, {46, 46}, {47, 47}, {58, 58}, {61, 61}, {63, 63}, {59, 59}, {33, 33}, {42, 42}, {35, 35}, {64, 64}, {36, 36}, {95, 95}, {37, 37}};

static const int s20[] = {60, 33, 69, 76, 69, 77, 69, 78, 84};

static const int s21[] = {69, 77, 80, 84, 89};

static const int s22[] = {65, 78, 89};

static const int s23[] = {35, 80, 67, 68, 65, 84, 65};

static const int s24[] = {41, 42};

static const int s25[] = {35, 80, 67, 68, 65, 84, 65};

static const int s26[] = {60, 33, 65, 84, 84, 76, 73, 83, 84};

static const int s27[] = {67, 68, 65, 84, 65};

static const int s28[] = {73, 68};

static const int s29[] = {73, 68, 82, 69, 70};

static const int s30[] = {73, 68, 82, 69, 70, 83};

static const int s31[] = {69, 78, 84, 73, 84, 89};

static const int s32[] = {69, 78, 84, 73, 84, 73, 69, 83};

static const int s33[] = {78, 77, 84, 79, 75, 69, 78};

static const int s34[] = {78, 77, 84, 79, 75, 69, 78, 83};

static const int s35[] = {78, 79, 84, 65, 84, 73, 79, 78};

static const int s36[] = {35, 82, 69, 81, 85, 73, 82, 69, 68};

static const int s37[] = {35, 73, 77, 80, 76, 73, 69, 68};

static const int s38[] = {35, 70, 73, 88, 69, 68};

static const int s39[] = {60, 33, 69, 78, 84, 73, 84, 89};

static const int s40[] = {60, 33, 69, 78, 84, 73, 84, 89};

static const soulng::parser::Range s41[] = {{37, 37}, {38, 38}, {34, 34}};

static const soulng::parser::Range s42[] = {{37, 37}, {38, 38}, {39, 39}};

static const int s43[] = {78, 68, 65, 84, 65};

static const int s44[] = {60, 33, 78, 79, 84, 65, 84, 73, 79, 78};

static const int s45[] = {80, 85, 66, 76, 73, 67};

static const int s46[] = {47, 62};

static const int s47[] = {60, 47};

static const soulng::parser::Range s48[] = {{60, 60}, {38, 38}};

static const soulng::parser::Range s49[] = {{60, 60}, {38, 38}};

static const int s50[] = {93, 93, 62};

static const soulng::parser::Range s51[] = {{60, 60}, {38, 38}};

static const int s52[] = {60, 33, 91, 67, 68, 65, 84, 65, 91};

static const int s53[] = {93, 93, 62};

static const int s54[] = {93, 93, 62};

static const soulng::parser::Range s55[] = {{60, 60}, {38, 38}, {34, 34}};

static const soulng::parser::Range s56[] = {{60, 60}, {38, 38}, {39, 39}};

static const soulng::parser::Range s57[] = {{48, 57}};

static const soulng::parser::Range s58[] = {{48, 57}, {97, 102}, {65, 70}};

static const int s59[] = {38, 35};

static const int s60[] = {38, 35, 120};

static const int s61[] = {60, 33, 45, 45};

static const int s62[] = {45, 45, 62};

static const int s63[] = {60, 63};

static const int s64[] = {63, 62};

static const int s65[] = {63, 62};

static const soulng::parser::Range s66[] = {{120, 120}, {88, 88}};

static const soulng::parser::Range s67[] = {{109, 109}, {77, 77}};

static const soulng::parser::Range s68[] = {{108, 108}, {76, 76}};

static const int s69[] = {34, 121, 101, 115, 34};

static const int s70[] = {34, 110, 111, 34};

using namespace soulng::unicode;
using namespace soulng::lexer;

void XmlParser::Parse(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (lexer.Log())
    {
        lexer.Log()->WriteBeginRule(soulng::unicode::ToUtf32("parse"));
        lexer.Log()->IncIndent();
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    ++lexer;
    soulng::lexer::Span span = lexer.GetSpan();
    soulng::parser::Match match = XmlParser::Document(lexer, processor);
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
            return;
        }
        else
        {
            lexer.ThrowFarthestError();
        }
    }
    else
    {
        lexer.ThrowFarthestError();
    }
    return;
}

soulng::parser::Match XmlParser::Document(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Document"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 0);
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
                processor->StartDocument();
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
                            soulng::parser::Match match = XmlParser::Prolog(lexer, processor);
                            *parentMatch6 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                soulng::parser::Match match = XmlParser::Element(lexer, processor);
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
                                            soulng::parser::Match match = XmlParser::Misc(lexer, processor);
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
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                }
                if (match.hit)
                {
                    processor->EndDocument();
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Document"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Document"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Char(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Char"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 1);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
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
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            Token token = lexer.GetToken(pos);
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Char"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<char32_t>(static_cast<char32_t>(token.id)));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Char"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Char"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::S(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("S"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 2);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        for (const soulng::parser::Range& range : s1)
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
                for (const soulng::parser::Range& range : s1)
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("S"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("S"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::NameStartChar(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("NameStartChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 3);
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
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("NameStartChar"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("NameStartChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::NameChar(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("NameChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 4);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::NameStartChar(lexer);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
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
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("NameChar"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("NameChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Name(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Name"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 5);
    Span s = Span();
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
                    soulng::parser::Match match = XmlParser::NameStartChar(lexer);
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
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match = XmlParser::NameChar(lexer);
                                        if (match.hit)
                                        {
                                            s.end = span.end;
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
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Name"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(lexer.GetMatch(s)));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Name"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Name"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Names(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Names"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 6);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name1;
    std::unique_ptr<soulng::parser::Value<std::u32string>> name2;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match = XmlParser::Name(lexer);
        name1.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch2 = &match;
            {
                while (true)
                {
                    int64_t save = lexer.GetPos();
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch3 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch4 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == 32)
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
                                    soulng::parser::Match match = XmlParser::Name(lexer);
                                    name2.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                    *parentMatch5 = match;
                                }
                                *parentMatch4 = match;
                            }
                            *parentMatch3 = match;
                        }
                        if (match.hit)
                        {
                            *parentMatch2 = match;
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Names"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Names"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Nmtoken(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Nmtoken"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 7);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match = XmlParser::NameChar(lexer);
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
                soulng::parser::Match match = XmlParser::NameChar(lexer);
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Nmtoken"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Nmtoken"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Nmtokens(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Nmtokens"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 8);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match = XmlParser::Nmtoken(lexer);
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(true);
            soulng::parser::Match* parentMatch2 = &match;
            {
                while (true)
                {
                    int64_t save = lexer.GetPos();
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch3 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch4 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == 32)
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
                                    soulng::parser::Match match = XmlParser::Nmtoken(lexer);
                                    *parentMatch5 = match;
                                }
                                *parentMatch4 = match;
                            }
                            *parentMatch3 = match;
                        }
                        if (match.hit)
                        {
                            *parentMatch2 = match;
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Nmtokens"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Nmtokens"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Prolog(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Prolog"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 9);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(true);
            int64_t save = lexer.GetPos();
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::XMLDecl(lexer, processor);
                if (match.hit)
                {
                    *parentMatch2 = match;
                }
                else
                {
                    lexer.SetPos(save);
                }
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
                            soulng::parser::Match match = XmlParser::Misc(lexer, processor);
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
            soulng::parser::Match match(true);
            int64_t save = lexer.GetPos();
            soulng::parser::Match* parentMatch6 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch7 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch8 = &match;
                    {
                        soulng::parser::Match match = XmlParser::DocTypeDecl(lexer, processor);
                        *parentMatch8 = match;
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
                                        soulng::parser::Match match = XmlParser::Misc(lexer, processor);
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
                        *parentMatch8 = match;
                    }
                    *parentMatch7 = match;
                }
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Prolog"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Prolog"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::XMLDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("XMLDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 10);
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
                        soulng::parser::Match match(true);
                        for (int i : s4)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match = XmlParser::VersionInfo(lexer, processor);
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
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
                            soulng::parser::Match match = XmlParser::EncodingDecl(lexer, processor);
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
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        soulng::parser::Match match = XmlParser::SDDecl(lexer, processor);
                        if (match.hit)
                        {
                            *parentMatch9 = match;
                        }
                        else
                        {
                            lexer.SetPos(save);
                        }
                    }
                    *parentMatch8 = match;
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch11 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch12 = &match;
        {
            soulng::parser::Match match(true);
            for (int i : s5)
            {
                if (*lexer == i)
                {
                    ++lexer;
                }
                else
                {
                    match.hit = false;
                    break;
                }
            }
            *parentMatch12 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("XMLDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("XMLDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::VersionInfo(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("VersionInfo"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 11);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s6)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
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
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch4 = &match;
            {
                soulng::parser::Match match = XmlParser::Eq(lexer);
                *parentMatch4 = match;
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
            soulng::parser::Match match = XmlParser::VersionNumber(lexer, processor);
            *parentMatch5 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("VersionInfo"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("VersionInfo"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::VersionNum(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("VersionNum"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 12);
    Span s = Span();
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
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match(false);
                        if (*lexer == 49)
                        {
                            ++lexer;
                            match.hit = true;
                        }
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
                        if (*lexer == 46)
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
                                soulng::parser::Match match(false);
                                for (const soulng::parser::Range& range : s7)
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
                                    s.end = span.end;
                                }
                                *parentMatch9 = match;
                            }
                            *parentMatch8 = match;
                        }
                        *parentMatch7 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch10 = &match;
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
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(false);
                                        for (const soulng::parser::Range& range : s7)
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
                                            s.end = span.end;
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
                    *parentMatch6 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            processor->Version(lexer.GetMatch(s));
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("VersionNum"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("VersionNum"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::VersionNumDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("VersionNumDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 13);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 34)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::VersionNum(lexer, processor);
                *parentMatch2 = match;
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
            if (*lexer == 34)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("VersionNumDQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("VersionNumDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::VersionNumSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("VersionNumSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 14);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 39)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::VersionNum(lexer, processor);
                *parentMatch2 = match;
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
            if (*lexer == 39)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("VersionNumSQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("VersionNumSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::VersionNumber(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("VersionNumber"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 15);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::VersionNumDQ(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::VersionNumSQ(lexer, processor);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("VersionNumber"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("VersionNumber"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EncodingDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EncodingDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 16);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s8)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
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
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch4 = &match;
            {
                soulng::parser::Match match = XmlParser::Eq(lexer);
                *parentMatch4 = match;
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
            soulng::parser::Match match = XmlParser::EncName(lexer, processor);
            *parentMatch5 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EncodingDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EncodingDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EncodingName(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EncodingName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 17);
    Span s = Span();
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
                    soulng::parser::Match match(false);
                    for (const soulng::parser::Range& range : s9)
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
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(false);
                                        for (const soulng::parser::Range& range : s10)
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
                                            s.end = span.end;
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
            processor->Encoding(lexer.GetMatch(s));
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EncodingName"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EncodingName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EncNameDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EncNameDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 18);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 34)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::EncodingName(lexer, processor);
                *parentMatch2 = match;
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
            if (*lexer == 34)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EncNameDQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EncNameDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EncNameSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EncNameSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 19);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 39)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::EncodingName(lexer, processor);
                *parentMatch2 = match;
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
            if (*lexer == 39)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EncNameSQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EncNameSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EncName(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EncName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 20);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::EncNameDQ(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::EncNameSQ(lexer, processor);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EncName"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EncName"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::SDDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("SDDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 21);
    std::unique_ptr<soulng::parser::Value<bool>> yn;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s11)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
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
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch4 = &match;
            {
                soulng::parser::Match match = XmlParser::Eq(lexer);
                *parentMatch4 = match;
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
                soulng::parser::Match match = XmlParser::YesNo(lexer);
                yn.reset(static_cast<soulng::parser::Value<bool>*>(match.value));
                if (match.hit)
                {
                    processor->Standalone(yn->value);
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SDDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("SDDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::DocTypeDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DocTypeDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 22);
    std::unique_ptr<soulng::parser::Value<std::u32string>> rootElementName;
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
                            soulng::parser::Match match(true);
                            for (int i : s12)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            rootElementName.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch7 = match;
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
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch9 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch10 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch11 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::S(lexer);
                                    *parentMatch11 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch12 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::ExternalID(lexer);
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
                            }
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
                soulng::parser::Match* parentMatch13 = &match;
                {
                    soulng::parser::Match match(true);
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match* parentMatch14 = &match;
                    {
                        soulng::parser::Match match = XmlParser::S(lexer);
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
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
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
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch17 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch18 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch19 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch20 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == 91)
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
                                        soulng::parser::Match match = XmlParser::IntSubset(lexer, processor);
                                        *parentMatch21 = match;
                                    }
                                    *parentMatch20 = match;
                                }
                                *parentMatch19 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch22 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == 93)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch22 = match;
                                }
                                *parentMatch19 = match;
                            }
                            *parentMatch18 = match;
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
                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                            *parentMatch18 = match;
                        }
                        *parentMatch17 = match;
                    }
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch25 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch25 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DocTypeDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DocTypeDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::ExternalID(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ExternalID"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 23);
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
                    soulng::parser::Match match(true);
                    for (int i : s13)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match = XmlParser::S(lexer);
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch5 = &match;
                {
                    soulng::parser::Match match = XmlParser::SystemLiteral(lexer);
                    *parentMatch5 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch6 = &match;
            lexer.SetPos(save);
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
                                    soulng::parser::Match match(true);
                                    for (int i : s14)
                                    {
                                        if (*lexer == i)
                                        {
                                            ++lexer;
                                        }
                                        else
                                        {
                                            match.hit = false;
                                            break;
                                        }
                                    }
                                    *parentMatch11 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch12 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::S(lexer);
                                        *parentMatch12 = match;
                                    }
                                    *parentMatch11 = match;
                                }
                                *parentMatch10 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch13 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::PubidLiteral(lexer);
                                    *parentMatch13 = match;
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
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch14 = match;
                            }
                            *parentMatch9 = match;
                        }
                        *parentMatch8 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch15 = &match;
                        {
                            soulng::parser::Match match = XmlParser::SystemLiteral(lexer);
                            *parentMatch15 = match;
                        }
                        *parentMatch8 = match;
                    }
                    *parentMatch7 = match;
                }
                *parentMatch6 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ExternalID"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ExternalID"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::SystemLiteral(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("SystemLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 24);
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
                    soulng::parser::Match match(false);
                    if (*lexer == 34)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(true);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                while (true)
                                {
                                    int64_t save = lexer.GetPos();
                                    {
                                        soulng::parser::Match match(lexer.Pos() != lexer.End());
                                        for (const soulng::parser::Range& range : s15)
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
                                            *parentMatch6 = match;
                                        }
                                        else
                                        {
                                            lexer.SetPos(save);
                                            break;
                                        }
                                    }
                                }
                            }
                            *parentMatch5 = match;
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
                soulng::parser::Match* parentMatch7 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == 34)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch7 = match;
                }
                *parentMatch2 = match;
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
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch10 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == 39)
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
                                    soulng::parser::Match match(true);
                                    soulng::parser::Match* parentMatch14 = &match;
                                    {
                                        while (true)
                                        {
                                            int64_t save = lexer.GetPos();
                                            {
                                                soulng::parser::Match match(lexer.Pos() != lexer.End());
                                                for (const soulng::parser::Range& range : s16)
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
                                                    *parentMatch14 = match;
                                                }
                                                else
                                                {
                                                    lexer.SetPos(save);
                                                    break;
                                                }
                                            }
                                        }
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch15 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == 39)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            *parentMatch15 = match;
                        }
                        *parentMatch10 = match;
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("SystemLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("SystemLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PubidLiteral(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PubidLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 25);
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
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
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
                                    soulng::parser::Match match = XmlParser::PubidChar(lexer);
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
                    *parentMatch3 = match;
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
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                *parentMatch6 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch7 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch8 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == 39)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch9 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch11 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch12 = &match;
                                {
                                    while (true)
                                    {
                                        int64_t save = lexer.GetPos();
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch13 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch14 = &match;
                                                int64_t save = lexer.GetPos();
                                                {
                                                    soulng::parser::Match match = XmlParser::PubidChar(lexer);
                                                    *parentMatch14 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch15 = &match;
                                                    {
                                                        int64_t tmp = lexer.GetPos();
                                                        lexer.SetPos(save);
                                                        save = tmp;
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == 39)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        *parentMatch15 = match;
                                                    }
                                                    if (!match.hit)
                                                    {
                                                        lexer.SetPos(save);
                                                    }
                                                    *parentMatch14 = soulng::parser::Match(!match.hit, match.value);
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
                                                break;
                                            }
                                        }
                                    }
                                }
                                *parentMatch11 = match;
                            }
                            *parentMatch10 = match;
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
                        if (*lexer == 39)
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
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PubidLiteral"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PubidLiteral"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PubidChar(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PubidChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 26);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match(false);
            for (const soulng::parser::Range& range : s17)
            {
                if (*lexer >= range.first && *lexer <= range.last)
                {
                    match.hit = true;
                    ++lexer;
                    break;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch2 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(false);
                    for (const soulng::parser::Range& range : s18)
                    {
                        if (*lexer >= range.first && *lexer <= range.last)
                        {
                            match.hit = true;
                            ++lexer;
                            break;
                        }
                    }
                    *parentMatch2 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                for (const soulng::parser::Range& range : s19)
                {
                    if (*lexer >= range.first && *lexer <= range.last)
                    {
                        match.hit = true;
                        ++lexer;
                        break;
                    }
                }
                *parentMatch3 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PubidChar"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PubidChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::IntSubset(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("IntSubset"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 27);
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
                        soulng::parser::Match match = XmlParser::MarkupDecl(lexer, processor);
                        *parentMatch2 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch3 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match = XmlParser::DeclSep(lexer, processor);
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("IntSubset"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("IntSubset"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::MarkupDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("MarkupDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 28);
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
                        soulng::parser::Match match = XmlParser::ElementDecl(lexer, processor);
                        *parentMatch4 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch5 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match = XmlParser::AttlistDecl(lexer, processor);
                                *parentMatch5 = match;
                            }
                            *parentMatch4 = match;
                        }
                    }
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch6 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match = XmlParser::EntityDecl(lexer, processor);
                            *parentMatch6 = match;
                        }
                        *parentMatch3 = match;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match = XmlParser::NotationDecl(lexer, processor);
                        *parentMatch7 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch8 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match = XmlParser::PI(lexer, processor);
                    *parentMatch8 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch9 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::Comment(lexer, processor);
                *parentMatch9 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("MarkupDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("MarkupDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::DeclSep(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DeclSep"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 29);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::PEReference(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DeclSep"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DeclSep"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::ElementDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ElementDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 30);
    std::unique_ptr<soulng::parser::Value<std::u32string>> elementName;
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
                            soulng::parser::Match match(true);
                            for (int i : s20)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            elementName.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch7 = match;
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
                        soulng::parser::Match match = XmlParser::S(lexer);
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
                    soulng::parser::Match match = XmlParser::ContentSpec(lexer);
                    *parentMatch9 = match;
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch11 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch12 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ElementDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ElementDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::ContentSpec(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ContentSpec"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 31);
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
                soulng::parser::Match match(true);
                for (int i : s21)
                {
                    if (*lexer == i)
                    {
                        ++lexer;
                    }
                    else
                    {
                        match.hit = false;
                        break;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch3 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match(true);
                        for (int i : s22)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
                        }
                        *parentMatch3 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch4 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match = XmlParser::Mixed(lexer);
                    *parentMatch4 = match;
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
                soulng::parser::Match match = XmlParser::Children(lexer);
                *parentMatch5 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ContentSpec"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ContentSpec"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Children(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Children"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 32);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                int64_t save = lexer.GetPos();
                soulng::parser::Match match = XmlParser::Choice(lexer);
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch3 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match = XmlParser::Seq(lexer);
                        *parentMatch3 = match;
                    }
                    *parentMatch2 = match;
                }
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
            soulng::parser::Match match(true);
            int64_t save = lexer.GetPos();
            soulng::parser::Match* parentMatch5 = &match;
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            if (*lexer == 63)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            *parentMatch8 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch9 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == 42)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch9 = match;
                                }
                                *parentMatch8 = match;
                            }
                        }
                        *parentMatch7 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch10 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == 43)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch10 = match;
                            }
                            *parentMatch7 = match;
                        }
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
                }
            }
            *parentMatch4 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Children"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Children"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::CP(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CP"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 33);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
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
                    soulng::parser::Match match = XmlParser::Name(lexer);
                    name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch4 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match = XmlParser::Choice(lexer);
                            *parentMatch4 = match;
                        }
                        *parentMatch3 = match;
                    }
                }
                *parentMatch2 = match;
                if (!match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch5 = &match;
                    lexer.SetPos(save);
                    {
                        soulng::parser::Match match = XmlParser::Seq(lexer);
                        *parentMatch5 = match;
                    }
                    *parentMatch2 = match;
                }
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch8 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match(false);
                            if (*lexer == 63)
                            {
                                ++lexer;
                                match.hit = true;
                            }
                            *parentMatch10 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch11 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == 42)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch11 = match;
                                }
                                *parentMatch10 = match;
                            }
                        }
                        *parentMatch9 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch12 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == 43)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch12 = match;
                            }
                            *parentMatch9 = match;
                        }
                    }
                    *parentMatch8 = match;
                }
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CP"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CP"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Choice(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Choice"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 34);
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
                        if (*lexer == 40)
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
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        soulng::parser::Match match = XmlParser::CP(lexer);
                        *parentMatch7 = match;
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
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch13 = &match;
                                    {
                                        soulng::parser::Match match(true);
                                        int64_t save = lexer.GetPos();
                                        soulng::parser::Match* parentMatch14 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::S(lexer);
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
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch15 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            if (*lexer == 124)
                                            {
                                                ++lexer;
                                                match.hit = true;
                                            }
                                            *parentMatch15 = match;
                                        }
                                        *parentMatch13 = match;
                                    }
                                    *parentMatch12 = match;
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
                                            soulng::parser::Match match = XmlParser::S(lexer);
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
                                    *parentMatch12 = match;
                                }
                                *parentMatch11 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch18 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::CP(lexer);
                                    *parentMatch18 = match;
                                }
                                *parentMatch11 = match;
                            }
                            *parentMatch10 = match;
                        }
                        *parentMatch9 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(true);
                        soulng::parser::Match* parentMatch19 = &match;
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch20 = &match;
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
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch24 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch25 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == 124)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
                                                    }
                                                    *parentMatch25 = match;
                                                }
                                                *parentMatch23 = match;
                                            }
                                            *parentMatch22 = match;
                                        }
                                        if (match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch26 = &match;
                                            {
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch27 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
                                                    if (match.hit)
                                                    {
                                                        *parentMatch27 = match;
                                                    }
                                                    else
                                                    {
                                                        lexer.SetPos(save);
                                                    }
                                                }
                                                *parentMatch26 = match;
                                            }
                                            *parentMatch22 = match;
                                        }
                                        *parentMatch21 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch28 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::CP(lexer);
                                            *parentMatch28 = match;
                                        }
                                        *parentMatch21 = match;
                                    }
                                    *parentMatch20 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch19 = match;
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
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch29 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch30 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
                    if (match.hit)
                    {
                        *parentMatch30 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch29 = match;
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
            if (*lexer == 41)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch31 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Choice"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Choice"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Seq(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Seq"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 35);
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
                        if (*lexer == 40)
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
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        soulng::parser::Match match = XmlParser::CP(lexer);
                        *parentMatch7 = match;
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
                                    soulng::parser::Match* parentMatch11 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch12 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch13 = &match;
                                            {
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch14 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch15 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == 44)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
                                                    }
                                                    *parentMatch15 = match;
                                                }
                                                *parentMatch13 = match;
                                            }
                                            *parentMatch12 = match;
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
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            *parentMatch12 = match;
                                        }
                                        *parentMatch11 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch18 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::CP(lexer);
                                            *parentMatch18 = match;
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
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch19 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch20 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
        soulng::parser::Match* parentMatch21 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 41)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch21 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Seq"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Seq"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Mixed(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Mixed"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 36);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
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
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match(false);
                            if (*lexer == 40)
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
                                soulng::parser::Match match(true);
                                int64_t save = lexer.GetPos();
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                            for (int i : s23)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch13 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch14 = &match;
                                                {
                                                    soulng::parser::Match match(true);
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match* parentMatch15 = &match;
                                                    {
                                                        soulng::parser::Match match = XmlParser::S(lexer);
                                                        if (match.hit)
                                                        {
                                                            *parentMatch15 = match;
                                                        }
                                                        else
                                                        {
                                                            lexer.SetPos(save);
                                                        }
                                                    }
                                                    *parentMatch14 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch16 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == 124)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        *parentMatch16 = match;
                                                    }
                                                    *parentMatch14 = match;
                                                }
                                                *parentMatch13 = match;
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
                                                        soulng::parser::Match match = XmlParser::S(lexer);
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
                                                *parentMatch13 = match;
                                            }
                                            *parentMatch12 = match;
                                        }
                                        if (match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch19 = &match;
                                            {
                                                soulng::parser::Match match = XmlParser::Name(lexer);
                                                name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                                *parentMatch19 = match;
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
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
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
                        soulng::parser::Match match = XmlParser::S(lexer);
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
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch22 = &match;
            {
                soulng::parser::Match match(true);
                for (int i : s24)
                {
                    if (*lexer == i)
                    {
                        ++lexer;
                    }
                    else
                    {
                        match.hit = false;
                        break;
                    }
                }
                *parentMatch22 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch23 = &match;
            lexer.SetPos(save);
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
                                soulng::parser::Match match(false);
                                if (*lexer == 40)
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
                                    soulng::parser::Match match(true);
                                    int64_t save = lexer.GetPos();
                                    soulng::parser::Match* parentMatch29 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::S(lexer);
                                        if (match.hit)
                                        {
                                            *parentMatch29 = match;
                                        }
                                        else
                                        {
                                            lexer.SetPos(save);
                                        }
                                    }
                                    *parentMatch28 = match;
                                }
                                *parentMatch27 = match;
                            }
                            *parentMatch26 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch30 = &match;
                            {
                                soulng::parser::Match match(true);
                                for (int i : s25)
                                {
                                    if (*lexer == i)
                                    {
                                        ++lexer;
                                    }
                                    else
                                    {
                                        match.hit = false;
                                        break;
                                    }
                                }
                                *parentMatch30 = match;
                            }
                            *parentMatch26 = match;
                        }
                        *parentMatch25 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch31 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch32 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
                                if (match.hit)
                                {
                                    *parentMatch32 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                }
                            }
                            *parentMatch31 = match;
                        }
                        *parentMatch25 = match;
                    }
                    *parentMatch24 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch33 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == 41)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch33 = match;
                    }
                    *parentMatch24 = match;
                }
                *parentMatch23 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Mixed"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Mixed"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttlistDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttlistDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 37);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
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
                        soulng::parser::Match match(true);
                        for (int i : s26)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match = XmlParser::S(lexer);
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch6 = &match;
                    {
                        soulng::parser::Match match = XmlParser::Name(lexer);
                        name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                        *parentMatch6 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
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
                                soulng::parser::Match match = XmlParser::AttDef(lexer, processor);
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
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
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
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch11 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttlistDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttlistDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 38);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
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
                        soulng::parser::Match match = XmlParser::S(lexer);
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch6 = &match;
                    {
                        soulng::parser::Match match = XmlParser::S(lexer);
                        *parentMatch6 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch7 = &match;
                {
                    soulng::parser::Match match = XmlParser::AttType(lexer);
                    *parentMatch7 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch8 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch8 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch9 = &match;
        {
            soulng::parser::Match match = XmlParser::DefaultDecl(lexer, processor);
            *parentMatch9 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttDef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttType(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 39);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match = XmlParser::StringType(lexer);
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch2 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match = XmlParser::TokenizedType(lexer);
                    *parentMatch2 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::EnumeratedType(lexer);
                *parentMatch3 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::StringType(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("StringType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 40);
    soulng::parser::Match match(true);
    for (int i : s27)
    {
        if (*lexer == i)
        {
            ++lexer;
        }
        else
        {
            match.hit = false;
            break;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("StringType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("StringType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::TokenizedType(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("TokenizedType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 41);
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
                            soulng::parser::Match match(true);
                            for (int i : s28)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match(true);
                                    for (int i : s29)
                                    {
                                        if (*lexer == i)
                                        {
                                            ++lexer;
                                        }
                                        else
                                        {
                                            match.hit = false;
                                            break;
                                        }
                                    }
                                    *parentMatch6 = match;
                                }
                                *parentMatch5 = match;
                            }
                        }
                        *parentMatch4 = match;
                        if (!match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            lexer.SetPos(save);
                            {
                                soulng::parser::Match match(true);
                                for (int i : s30)
                                {
                                    if (*lexer == i)
                                    {
                                        ++lexer;
                                    }
                                    else
                                    {
                                        match.hit = false;
                                        break;
                                    }
                                }
                                *parentMatch7 = match;
                            }
                            *parentMatch4 = match;
                        }
                    }
                    *parentMatch3 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(true);
                            for (int i : s31)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch8 = match;
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
                        soulng::parser::Match match(true);
                        for (int i : s32)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
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
                soulng::parser::Match* parentMatch10 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(true);
                    for (int i : s33)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch10 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch11 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(true);
                for (int i : s34)
                {
                    if (*lexer == i)
                    {
                        ++lexer;
                    }
                    else
                    {
                        match.hit = false;
                        break;
                    }
                }
                *parentMatch11 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("TokenizedType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("TokenizedType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EnumeratedType(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EnumeratedType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 42);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::NotationType(lexer);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::Enumeration(lexer);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EnumeratedType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EnumeratedType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::NotationType(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("NotationType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 43);
    std::unique_ptr<soulng::parser::Value<std::u32string>> f;
    std::unique_ptr<soulng::parser::Value<std::u32string>> n;
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
                                soulng::parser::Match match(true);
                                for (int i : s35)
                                {
                                    if (*lexer == i)
                                    {
                                        ++lexer;
                                    }
                                    else
                                    {
                                        match.hit = false;
                                        break;
                                    }
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch7 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                if (*lexer == 40)
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
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch10 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch11 = &match;
                    {
                        soulng::parser::Match match = XmlParser::Name(lexer);
                        f.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                        *parentMatch11 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch12 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch13 = &match;
                    {
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
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
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch17 = &match;
                                            {
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch18 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch19 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == 124)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
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
                                            soulng::parser::Match* parentMatch20 = &match;
                                            {
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch21 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            *parentMatch16 = match;
                                        }
                                        *parentMatch15 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch22 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::Name(lexer);
                                            n.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                            *parentMatch22 = match;
                                        }
                                        *parentMatch15 = match;
                                    }
                                    *parentMatch14 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch13 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                    break;
                                }
                            }
                        }
                    }
                    *parentMatch12 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
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
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch25 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 41)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch25 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("NotationType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("NotationType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Enumeration(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Enumeration"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 44);
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
                        if (*lexer == 40)
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
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
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
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        soulng::parser::Match match = XmlParser::Nmtoken(lexer);
                        *parentMatch7 = match;
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
                                    soulng::parser::Match* parentMatch11 = &match;
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch12 = &match;
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch13 = &match;
                                            {
                                                soulng::parser::Match match(true);
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match* parentMatch14 = &match;
                                                {
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            if (match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch15 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    if (*lexer == 124)
                                                    {
                                                        ++lexer;
                                                        match.hit = true;
                                                    }
                                                    *parentMatch15 = match;
                                                }
                                                *parentMatch13 = match;
                                            }
                                            *parentMatch12 = match;
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
                                                    soulng::parser::Match match = XmlParser::S(lexer);
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
                                            *parentMatch12 = match;
                                        }
                                        *parentMatch11 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch18 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::Nmtoken(lexer);
                                            *parentMatch18 = match;
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
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch19 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch20 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
        soulng::parser::Match* parentMatch21 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 41)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch21 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Enumeration"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Enumeration"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::DefaultDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DefaultDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 45);
    std::unique_ptr<soulng::parser::Value<std::u32string>> attVAlue;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match(true);
            for (int i : s36)
            {
                if (*lexer == i)
                {
                    ++lexer;
                }
                else
                {
                    match.hit = false;
                    break;
                }
            }
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch2 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match(true);
                    for (int i : s37)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch2 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
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
                        soulng::parser::Match match(true);
                        int64_t save = lexer.GetPos();
                        soulng::parser::Match* parentMatch6 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch7 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch8 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    for (int i : s38)
                                    {
                                        if (*lexer == i)
                                        {
                                            ++lexer;
                                        }
                                        else
                                        {
                                            match.hit = false;
                                            break;
                                        }
                                    }
                                    *parentMatch8 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch9 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::S(lexer);
                                        *parentMatch9 = match;
                                    }
                                    *parentMatch8 = match;
                                }
                                *parentMatch7 = match;
                            }
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
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            soulng::parser::Match match = XmlParser::AttValue(lexer, processor);
                            attVAlue.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch10 = match;
                        }
                        *parentMatch5 = match;
                    }
                    *parentMatch4 = match;
                }
                *parentMatch3 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DefaultDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DefaultDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EntityDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EntityDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 46);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::GEDecl(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::PEDecl(lexer, processor);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EntityDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EntityDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::GEDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("GEDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 47);
    std::unique_ptr<soulng::parser::Value<std::u32string>> entityName;
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
                            soulng::parser::Match match(true);
                            for (int i : s39)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            entityName.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch7 = match;
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
                        soulng::parser::Match match = XmlParser::S(lexer);
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
                    soulng::parser::Match match = XmlParser::EntityDef(lexer, processor);
                    *parentMatch9 = match;
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch11 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch12 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("GEDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("GEDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PEDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PEDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 48);
    std::unique_ptr<soulng::parser::Value<std::u32string>> peName;
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
                                    soulng::parser::Match match(true);
                                    for (int i : s40)
                                    {
                                        if (*lexer == i)
                                        {
                                            ++lexer;
                                        }
                                        else
                                        {
                                            match.hit = false;
                                            break;
                                        }
                                    }
                                    *parentMatch7 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch8 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::S(lexer);
                                        *parentMatch8 = match;
                                    }
                                    *parentMatch7 = match;
                                }
                                *parentMatch6 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch9 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    if (*lexer == 37)
                                    {
                                        ++lexer;
                                        match.hit = true;
                                    }
                                    *parentMatch9 = match;
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
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch10 = match;
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
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            peName.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch11 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch12 = &match;
                    {
                        soulng::parser::Match match = XmlParser::S(lexer);
                        *parentMatch12 = match;
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
                    soulng::parser::Match match = XmlParser::PEDef(lexer, processor);
                    *parentMatch13 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch14 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch15 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
                    if (match.hit)
                    {
                        *parentMatch15 = match;
                    }
                    else
                    {
                        lexer.SetPos(save);
                    }
                }
                *parentMatch14 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch16 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch16 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PEDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PEDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EntityDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EntityDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 49);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::EntityValue(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch2 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch3 = &match;
                    {
                        soulng::parser::Match match = XmlParser::ExternalID(lexer);
                        *parentMatch3 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch4 = &match;
                        {
                            soulng::parser::Match match(true);
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match* parentMatch5 = &match;
                            {
                                soulng::parser::Match match = XmlParser::NDataDecl(lexer);
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
                        *parentMatch3 = match;
                    }
                    *parentMatch2 = match;
                }
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EntityDef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EntityDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PEDef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PEDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 50);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::EntityValue(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::ExternalID(lexer);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PEDef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PEDef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EntityValue(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EntityValue"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 51);
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
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
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
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch5 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch6 = &match;
                                    {
                                        int64_t save = lexer.GetPos();
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch7 = &match;
                                        {
                                            int64_t save = lexer.GetPos();
                                            soulng::parser::Match match(lexer.Pos() != lexer.End());
                                            for (const soulng::parser::Range& range : s41)
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
                                            *parentMatch7 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch8 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match = XmlParser::PEReference(lexer, processor);
                                                    *parentMatch8 = match;
                                                }
                                                *parentMatch7 = match;
                                            }
                                        }
                                        *parentMatch6 = match;
                                        if (!match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch9 = &match;
                                            lexer.SetPos(save);
                                            {
                                                soulng::parser::Match match = XmlParser::Reference(lexer, processor);
                                                *parentMatch9 = match;
                                            }
                                            *parentMatch6 = match;
                                        }
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
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch10 = &match;
            {
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                *parentMatch10 = match;
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch11 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch12 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch13 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == 39)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch13 = match;
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
                                            soulng::parser::Match* parentMatch17 = &match;
                                            {
                                                int64_t save = lexer.GetPos();
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch18 = &match;
                                                {
                                                    int64_t save = lexer.GetPos();
                                                    soulng::parser::Match match(lexer.Pos() != lexer.End());
                                                    for (const soulng::parser::Range& range : s42)
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
                                                    *parentMatch18 = match;
                                                    if (!match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch19 = &match;
                                                        lexer.SetPos(save);
                                                        {
                                                            soulng::parser::Match match = XmlParser::PEReference(lexer, processor);
                                                            *parentMatch19 = match;
                                                        }
                                                        *parentMatch18 = match;
                                                    }
                                                }
                                                *parentMatch17 = match;
                                                if (!match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch20 = &match;
                                                    lexer.SetPos(save);
                                                    {
                                                        soulng::parser::Match match = XmlParser::Reference(lexer, processor);
                                                        *parentMatch20 = match;
                                                    }
                                                    *parentMatch17 = match;
                                                }
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
                        *parentMatch13 = match;
                    }
                    *parentMatch12 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch21 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == 39)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch21 = match;
                    }
                    *parentMatch12 = match;
                }
                *parentMatch11 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EntityValue"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EntityValue"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::NDataDecl(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("NDataDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 52);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s43)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
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
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch4 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch4 = match;
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
            soulng::parser::Match match = XmlParser::Name(lexer);
            name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
            *parentMatch5 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("NDataDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("NDataDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PEReference(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PEReference"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 53);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 37)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::Name(lexer);
                name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                *parentMatch2 = match;
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
            if (*lexer == 59)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PEReference"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PEReference"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::NotationDecl(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("NotationDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 54);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
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
                            soulng::parser::Match match(true);
                            for (int i : s44)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::S(lexer);
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            *parentMatch7 = match;
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
                        soulng::parser::Match match = XmlParser::S(lexer);
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
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch11 = &match;
                        {
                            int64_t save = lexer.GetPos();
                            soulng::parser::Match match = XmlParser::ExternalID(lexer);
                            *parentMatch11 = match;
                            if (!match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch12 = &match;
                                lexer.SetPos(save);
                                {
                                    soulng::parser::Match match = XmlParser::PublicID(lexer);
                                    *parentMatch12 = match;
                                }
                                *parentMatch11 = match;
                            }
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
            soulng::parser::Match* parentMatch13 = &match;
            {
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch14 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
    }
    if (match.hit)
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch15 = &match;
        {
            soulng::parser::Match match(false);
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch15 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("NotationDecl"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("NotationDecl"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PublicID(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PublicID"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 55);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(true);
            for (int i : s45)
            {
                if (*lexer == i)
                {
                    ++lexer;
                }
                else
                {
                    match.hit = false;
                    break;
                }
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch2 = match;
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
            soulng::parser::Match match = XmlParser::PubidLiteral(lexer);
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PublicID"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PublicID"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Element(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Element"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 56);
    std::u32string tagName = std::u32string();
    SourcePos sourcePos = SourcePos();
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
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
                    if (*lexer == 60)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch3 = match;
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
                            soulng::lexer::SourcePos sourcePos = lexer.GetSourcePos(pos);
                            soulng::parser::Match match = XmlParser::Name(lexer);
                            name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                            if (match.hit)
                            {
                                sourcePos = lexer.GetSourcePos(pos);
                                tagName = name->value;
                                processor->BeginStartTag(tagName);
                            }
                            *parentMatch5 = match;
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
                soulng::parser::Match* parentMatch6 = &match;
                {
                    soulng::parser::Match match(true);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        while (true)
                        {
                            int64_t save = lexer.GetPos();
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch8 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch9 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::S(lexer);
                                        *parentMatch9 = match;
                                    }
                                    if (match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch10 = &match;
                                        {
                                            soulng::parser::Match match = XmlParser::Attribute(lexer, processor);
                                            *parentMatch10 = match;
                                        }
                                        *parentMatch9 = match;
                                    }
                                    *parentMatch8 = match;
                                }
                                if (match.hit)
                                {
                                    *parentMatch7 = match;
                                }
                                else
                                {
                                    lexer.SetPos(save);
                                    break;
                                }
                            }
                        }
                    }
                    *parentMatch6 = match;
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch12 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch15 = &match;
                {
                    int64_t save = lexer.GetPos();
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch16 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::lexer::SourcePos sourcePos = lexer.GetSourcePos(pos);
                        soulng::parser::Match match(true);
                        for (int i : s46)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
                        }
                        if (match.hit)
                        {
                            processor->EndStartTag(span, sourcePos, lexer.FileName());
                            processor->EndTag(tagName, span, lexer.FileName());
                        }
                        *parentMatch16 = match;
                    }
                    *parentMatch15 = match;
                    if (!match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch17 = &match;
                        lexer.SetPos(save);
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch18 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch19 = &match;
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch20 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::lexer::SourcePos sourcePos = lexer.GetSourcePos(pos);
                                        soulng::parser::Match match(false);
                                        if (*lexer == 62)
                                        {
                                            ++lexer;
                                            match.hit = true;
                                        }
                                        if (match.hit)
                                        {
                                            processor->EndStartTag(span, sourcePos, lexer.FileName());
                                        }
                                        *parentMatch20 = match;
                                    }
                                    *parentMatch19 = match;
                                }
                                if (match.hit)
                                {
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch21 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::Content(lexer, processor);
                                        *parentMatch21 = match;
                                    }
                                    *parentMatch19 = match;
                                }
                                *parentMatch18 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch22 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::ETag(lexer, processor);
                                    *parentMatch22 = match;
                                }
                                *parentMatch18 = match;
                            }
                            *parentMatch17 = match;
                        }
                        *parentMatch15 = match;
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Element"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Element"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::ETag(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("ETag"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 57);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(true);
                for (int i : s47)
                {
                    if (*lexer == i)
                    {
                        ++lexer;
                    }
                    else
                    {
                        match.hit = false;
                        break;
                    }
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        int64_t pos = lexer.GetPos();
                        soulng::lexer::Span span = lexer.GetSpan();
                        soulng::parser::Match match = XmlParser::Name(lexer);
                        name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                        if (match.hit)
                        {
                            processor->EndTag(name->value, span, lexer.FileName());
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
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
                soulng::parser::Match match(true);
                int64_t save = lexer.GetPos();
                soulng::parser::Match* parentMatch6 = &match;
                {
                    soulng::parser::Match match = XmlParser::S(lexer);
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
            if (*lexer == 62)
            {
                ++lexer;
                match.hit = true;
            }
            *parentMatch7 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("ETag"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("ETag"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Content(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Content"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 58);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(true);
        int64_t save = lexer.GetPos();
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match = XmlParser::CharData(lexer, processor);
            if (match.hit)
            {
                *parentMatch1 = match;
            }
            else
            {
                lexer.SetPos(save);
            }
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
                            soulng::parser::Match* parentMatch5 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
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
                                                    soulng::parser::Match match = XmlParser::Element(lexer, processor);
                                                    *parentMatch10 = match;
                                                    if (!match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch11 = &match;
                                                        lexer.SetPos(save);
                                                        {
                                                            soulng::parser::Match match = XmlParser::Reference(lexer, processor);
                                                            *parentMatch11 = match;
                                                        }
                                                        *parentMatch10 = match;
                                                    }
                                                }
                                                *parentMatch9 = match;
                                                if (!match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch12 = &match;
                                                    lexer.SetPos(save);
                                                    {
                                                        soulng::parser::Match match = XmlParser::CDSect(lexer, processor);
                                                        *parentMatch12 = match;
                                                    }
                                                    *parentMatch9 = match;
                                                }
                                            }
                                            *parentMatch8 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch13 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match = XmlParser::PI(lexer, processor);
                                                    *parentMatch13 = match;
                                                }
                                                *parentMatch8 = match;
                                            }
                                        }
                                        *parentMatch7 = match;
                                        if (!match.hit)
                                        {
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch14 = &match;
                                            lexer.SetPos(save);
                                            {
                                                soulng::parser::Match match = XmlParser::Comment(lexer, processor);
                                                *parentMatch14 = match;
                                            }
                                            *parentMatch7 = match;
                                        }
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
                                    soulng::parser::Match match(true);
                                    int64_t save = lexer.GetPos();
                                    soulng::parser::Match* parentMatch16 = &match;
                                    {
                                        soulng::parser::Match match = XmlParser::CharData(lexer, processor);
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Content"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Content"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::CharDataChar(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CharDataChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 59);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match(lexer.Pos() != lexer.End());
        for (const soulng::parser::Range& range : s48)
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
            Token token = lexer.GetToken(pos);
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CharDataChar"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<char32_t>(static_cast<char32_t>(token.id)));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CharDataChar"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CharDataChar"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::CharData(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CharData"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 60);
    std::u32string s = std::u32string();
    std::unique_ptr<soulng::parser::Value<char32_t>> chr;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            int64_t save = lexer.GetPos();
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
                                soulng::parser::Match* parentMatch5 = &match;
                                {
                                    int64_t pos = lexer.GetPos();
                                    soulng::parser::Match match = XmlParser::CharDataChar(lexer);
                                    chr.reset(static_cast<soulng::parser::Value<char32_t>*>(match.value));
                                    if (match.hit)
                                    {
                                        s.append(1, chr->value);
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
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch6 = &match;
                {
                    int64_t tmp = lexer.GetPos();
                    lexer.SetPos(save);
                    save = tmp;
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch7 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch8 = &match;
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
                                            soulng::parser::Match match(lexer.Pos() != lexer.End());
                                            for (const soulng::parser::Range& range : s49)
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
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch11 = &match;
                                {
                                    soulng::parser::Match match(true);
                                    for (int i : s50)
                                    {
                                        if (*lexer == i)
                                        {
                                            ++lexer;
                                        }
                                        else
                                        {
                                            match.hit = false;
                                            break;
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
                            soulng::parser::Match* parentMatch12 = &match;
                            {
                                soulng::parser::Match match(true);
                                soulng::parser::Match* parentMatch13 = &match;
                                {
                                    while (true)
                                    {
                                        int64_t save = lexer.GetPos();
                                        {
                                            soulng::parser::Match match(lexer.Pos() != lexer.End());
                                            for (const soulng::parser::Range& range : s51)
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
                                                *parentMatch13 = match;
                                            }
                                            else
                                            {
                                                lexer.SetPos(save);
                                                break;
                                            }
                                        }
                                    }
                                }
                                *parentMatch12 = match;
                            }
                            *parentMatch8 = match;
                        }
                        *parentMatch7 = match;
                    }
                    *parentMatch6 = match;
                }
                if (!match.hit)
                {
                    lexer.SetPos(save);
                }
                *parentMatch2 = soulng::parser::Match(!match.hit, match.value);
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            processor->Text(s);
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CharData"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CharData"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::CDSect(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CDSect"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 61);
    std::u32string s = std::u32string();
    std::unique_ptr<soulng::parser::Value<char32_t>> chr;
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
                    soulng::parser::Match match(true);
                    for (int i : s52)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch3 = match;
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
                                            int64_t pos = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch8 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch9 = &match;
                                                int64_t save = lexer.GetPos();
                                                {
                                                    soulng::parser::Match match = XmlParser::Char(lexer);
                                                    chr.reset(static_cast<soulng::parser::Value<char32_t>*>(match.value));
                                                    *parentMatch9 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch10 = &match;
                                                    {
                                                        int64_t tmp = lexer.GetPos();
                                                        lexer.SetPos(save);
                                                        save = tmp;
                                                        soulng::parser::Match match(true);
                                                        for (int i : s53)
                                                        {
                                                            if (*lexer == i)
                                                            {
                                                                ++lexer;
                                                            }
                                                            else
                                                            {
                                                                match.hit = false;
                                                                break;
                                                            }
                                                        }
                                                        *parentMatch10 = match;
                                                    }
                                                    if (!match.hit)
                                                    {
                                                        lexer.SetPos(save);
                                                    }
                                                    *parentMatch9 = soulng::parser::Match(!match.hit, match.value);
                                                }
                                                *parentMatch8 = match;
                                            }
                                            if (match.hit)
                                            {
                                                s.append(1, chr->value);
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
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch11 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s54)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch11 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            processor->CDataSection(s);
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CDSect"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CDSect"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Attribute(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Attribute"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 62);
    std::unique_ptr<soulng::parser::Value<std::u32string>> attName;
    std::unique_ptr<soulng::parser::Value<std::u32string>> attValue;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match = XmlParser::Name(lexer);
            attName.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::Eq(lexer);
                *parentMatch2 = match;
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
                int64_t pos = lexer.GetPos();
                soulng::lexer::Span span = lexer.GetSpan();
                soulng::parser::Match match = XmlParser::AttValue(lexer, processor);
                attValue.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                if (match.hit)
                {
                    processor->AddAttribute(attName->value, attValue->value, span, lexer.FileName());
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Attribute"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Attribute"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttValueDQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttValueDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 63);
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
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    processor->BeginAttributeValue();
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
                            soulng::parser::Match* parentMatch5 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    int64_t save = lexer.GetPos();
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch7 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(lexer.Pos() != lexer.End());
                                        for (const soulng::parser::Range& range : s55)
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
                                            processor->AttValue().append(lexer.GetMatch(span));
                                        }
                                        *parentMatch7 = match;
                                    }
                                    *parentMatch6 = match;
                                    if (!match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch8 = &match;
                                        lexer.SetPos(save);
                                        {
                                            soulng::parser::Match match = XmlParser::Reference(lexer, processor);
                                            *parentMatch8 = match;
                                        }
                                        *parentMatch6 = match;
                                    }
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(false);
                if (*lexer == 34)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    std::u32string value = processor->AttValue();
                    processor->EndAttributeValue();
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValueDQ"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(value));
                    }
                }
                *parentMatch10 = match;
            }
            *parentMatch9 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValueDQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttValueDQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttValueSQ(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttValueSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 64);
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
                soulng::parser::Match match(false);
                if (*lexer == 39)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    processor->BeginAttributeValue();
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
                            soulng::parser::Match* parentMatch5 = &match;
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch6 = &match;
                                {
                                    int64_t save = lexer.GetPos();
                                    soulng::parser::Match match(false);
                                    soulng::parser::Match* parentMatch7 = &match;
                                    {
                                        int64_t pos = lexer.GetPos();
                                        soulng::lexer::Span span = lexer.GetSpan();
                                        soulng::parser::Match match(lexer.Pos() != lexer.End());
                                        for (const soulng::parser::Range& range : s56)
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
                                            processor->AttValue().append(lexer.GetMatch(span));
                                        }
                                        *parentMatch7 = match;
                                    }
                                    *parentMatch6 = match;
                                    if (!match.hit)
                                    {
                                        soulng::parser::Match match(false);
                                        soulng::parser::Match* parentMatch8 = &match;
                                        lexer.SetPos(save);
                                        {
                                            soulng::parser::Match match = XmlParser::Reference(lexer, processor);
                                            *parentMatch8 = match;
                                        }
                                        *parentMatch6 = match;
                                    }
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
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                soulng::parser::Match match(false);
                if (*lexer == 39)
                {
                    ++lexer;
                    match.hit = true;
                }
                if (match.hit)
                {
                    std::u32string value = processor->AttValue();
                    processor->EndAttributeValue();
                    {
                        #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                        if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValueSQ"));
                        #endif // SOULNG_PARSER_DEBUG_SUPPORT
                        return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(value));
                    }
                }
                *parentMatch10 = match;
            }
            *parentMatch9 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValueSQ"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttValueSQ"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::AttValue(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("AttValue"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 65);
    std::unique_ptr<soulng::parser::Value<std::u32string>> attValueDQ;
    std::unique_ptr<soulng::parser::Value<std::u32string>> attValueSQ;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match = XmlParser::AttValueDQ(lexer, processor);
            attValueDQ.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValue"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(attValueDQ->value));
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
                    soulng::parser::Match match = XmlParser::AttValueSQ(lexer, processor);
                    attValueSQ.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValue"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(attValueSQ->value));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("AttValue"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("AttValue"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::EntityRef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("EntityRef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 66);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::lexer::Span span = lexer.GetSpan();
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
                    if (*lexer == 38)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch4 = &match;
                    {
                        soulng::parser::Match match = XmlParser::Name(lexer);
                        name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch5 = &match;
                {
                    soulng::parser::Match match(false);
                    if (*lexer == 59)
                    {
                        ++lexer;
                        match.hit = true;
                    }
                    *parentMatch5 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            processor->EntityRef(name->value, span, lexer.FileName());
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("EntityRef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("EntityRef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::DecCodePoint(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("DecCodePoint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 67);
    uint32_t val = uint32_t();
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
                        soulng::parser::Match match(false);
                        for (const soulng::parser::Range& range : s57)
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
                            Token token = lexer.GetToken(pos);
                            val = 10 * val + token.id - '0';
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
                                soulng::parser::Match match(false);
                                for (const soulng::parser::Range& range : s57)
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
                                    Token token = lexer.GetToken(pos);
                                    val = 10 * val + token.id - '0';
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
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DecCodePoint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<uint32_t>(val));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("DecCodePoint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("DecCodePoint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::HexCodePoint(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("HexCodePoint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 68);
    uint32_t val = uint32_t();
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
                        soulng::parser::Match match(false);
                        for (const soulng::parser::Range& range : s58)
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
                            Token token = lexer.GetToken(pos);
                            if (token.id >= '0' && token.id <= '9') val = 16 * val + token.id - '0';
                            else if (token.id >= 'a' && token.id <= 'f') val = 16 * val + 10 + token.id - 'a';
                            else if (token.id >= 'A' && token.id <= 'F') val = 16 * val + 10 + token.id - 'A';
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
                                soulng::parser::Match match(false);
                                for (const soulng::parser::Range& range : s58)
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
                                    Token token = lexer.GetToken(pos);
                                    if (token.id >= '0' && token.id <= '9') val = 16 * val + token.id - '0';
                                    else if (token.id >= 'a' && token.id <= 'f') val = 16 * val + 10 + token.id - 'a';
                                    else if (token.id >= 'A' && token.id <= 'F') val = 16 * val + 10 + token.id - 'A';
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
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("HexCodePoint"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<uint32_t>(val));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("HexCodePoint"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("HexCodePoint"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::CharRef(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("CharRef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 69);
    std::unique_ptr<soulng::parser::Value<uint32_t>> decCodePoint;
    std::unique_ptr<soulng::parser::Value<uint32_t>> hexCodePoint;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
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
                        for (int i : s59)
                        {
                            if (*lexer == i)
                            {
                                ++lexer;
                            }
                            else
                            {
                                match.hit = false;
                                break;
                            }
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch5 = &match;
                        {
                            soulng::parser::Match match = XmlParser::DecCodePoint(lexer);
                            decCodePoint.reset(static_cast<soulng::parser::Value<uint32_t>*>(match.value));
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    *parentMatch3 = match;
                }
                if (match.hit)
                {
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch6 = &match;
                    {
                        soulng::parser::Match match(false);
                        if (*lexer == 59)
                        {
                            ++lexer;
                            match.hit = true;
                        }
                        *parentMatch6 = match;
                    }
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                processor->Text(std::u32string(1, static_cast<char32_t>(decCodePoint->value)));
            }
            *parentMatch1 = match;
        }
        *parentMatch0 = match;
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
                    soulng::parser::Match match(false);
                    soulng::parser::Match* parentMatch9 = &match;
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch10 = &match;
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch11 = &match;
                            {
                                soulng::parser::Match match(true);
                                for (int i : s60)
                                {
                                    if (*lexer == i)
                                    {
                                        ++lexer;
                                    }
                                    else
                                    {
                                        match.hit = false;
                                        break;
                                    }
                                }
                                *parentMatch11 = match;
                            }
                            if (match.hit)
                            {
                                soulng::parser::Match match(false);
                                soulng::parser::Match* parentMatch12 = &match;
                                {
                                    soulng::parser::Match match = XmlParser::HexCodePoint(lexer);
                                    hexCodePoint.reset(static_cast<soulng::parser::Value<uint32_t>*>(match.value));
                                    *parentMatch12 = match;
                                }
                                *parentMatch11 = match;
                            }
                            *parentMatch10 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch13 = &match;
                            {
                                soulng::parser::Match match(false);
                                if (*lexer == 59)
                                {
                                    ++lexer;
                                    match.hit = true;
                                }
                                *parentMatch13 = match;
                            }
                            *parentMatch10 = match;
                        }
                        *parentMatch9 = match;
                    }
                    if (match.hit)
                    {
                        processor->Text(std::u32string(1, static_cast<char32_t>(hexCodePoint->value)));
                    }
                    *parentMatch8 = match;
                }
                *parentMatch7 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("CharRef"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("CharRef"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Reference(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Reference"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 70);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match = XmlParser::EntityRef(lexer, processor);
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch1 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::CharRef(lexer, processor);
                *parentMatch1 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Reference"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Reference"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Misc(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Misc"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 71);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t save = lexer.GetPos();
            soulng::parser::Match match = XmlParser::Comment(lexer, processor);
            *parentMatch1 = match;
            if (!match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch2 = &match;
                lexer.SetPos(save);
                {
                    soulng::parser::Match match = XmlParser::PI(lexer, processor);
                    *parentMatch2 = match;
                }
                *parentMatch1 = match;
            }
        }
        *parentMatch0 = match;
        if (!match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            lexer.SetPos(save);
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                *parentMatch3 = match;
            }
            *parentMatch0 = match;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Misc"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Misc"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Comment(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Comment"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 72);
    std::u32string s = std::u32string();
    std::unique_ptr<soulng::parser::Value<char32_t>> chr;
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
                    soulng::parser::Match match(true);
                    for (int i : s61)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch3 = match;
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
                                            int64_t save = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch8 = &match;
                                            {
                                                int64_t pos = lexer.GetPos();
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch9 = &match;
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch10 = &match;
                                                    int64_t save = lexer.GetPos();
                                                    {
                                                        soulng::parser::Match match = XmlParser::Char(lexer);
                                                        chr.reset(static_cast<soulng::parser::Value<char32_t>*>(match.value));
                                                        *parentMatch10 = match;
                                                    }
                                                    if (match.hit)
                                                    {
                                                        soulng::parser::Match match(false);
                                                        soulng::parser::Match* parentMatch11 = &match;
                                                        {
                                                            int64_t tmp = lexer.GetPos();
                                                            lexer.SetPos(save);
                                                            save = tmp;
                                                            soulng::parser::Match match(false);
                                                            if (*lexer == 45)
                                                            {
                                                                ++lexer;
                                                                match.hit = true;
                                                            }
                                                            *parentMatch11 = match;
                                                        }
                                                        if (!match.hit)
                                                        {
                                                            lexer.SetPos(save);
                                                        }
                                                        *parentMatch10 = soulng::parser::Match(!match.hit, match.value);
                                                    }
                                                    *parentMatch9 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    s.append(1, chr->value);
                                                }
                                                *parentMatch8 = match;
                                            }
                                            *parentMatch7 = match;
                                            if (!match.hit)
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch12 = &match;
                                                lexer.SetPos(save);
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch13 = &match;
                                                    {
                                                        soulng::parser::Match match(false);
                                                        if (*lexer == 45)
                                                        {
                                                            ++lexer;
                                                            match.hit = true;
                                                        }
                                                        *parentMatch13 = match;
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
                                                                soulng::parser::Match match(false);
                                                                soulng::parser::Match* parentMatch16 = &match;
                                                                {
                                                                    soulng::parser::Match match(false);
                                                                    soulng::parser::Match* parentMatch17 = &match;
                                                                    int64_t save = lexer.GetPos();
                                                                    {
                                                                        soulng::parser::Match match = XmlParser::Char(lexer);
                                                                        chr.reset(static_cast<soulng::parser::Value<char32_t>*>(match.value));
                                                                        *parentMatch17 = match;
                                                                    }
                                                                    if (match.hit)
                                                                    {
                                                                        soulng::parser::Match match(false);
                                                                        soulng::parser::Match* parentMatch18 = &match;
                                                                        {
                                                                            int64_t tmp = lexer.GetPos();
                                                                            lexer.SetPos(save);
                                                                            save = tmp;
                                                                            soulng::parser::Match match(false);
                                                                            if (*lexer == 45)
                                                                            {
                                                                                ++lexer;
                                                                                match.hit = true;
                                                                            }
                                                                            *parentMatch18 = match;
                                                                        }
                                                                        if (!match.hit)
                                                                        {
                                                                            lexer.SetPos(save);
                                                                        }
                                                                        *parentMatch17 = soulng::parser::Match(!match.hit, match.value);
                                                                    }
                                                                    *parentMatch16 = match;
                                                                }
                                                                if (match.hit)
                                                                {
                                                                    s.append(1, '-').append(1, chr->value);
                                                                }
                                                                *parentMatch15 = match;
                                                            }
                                                            *parentMatch14 = match;
                                                        }
                                                        *parentMatch13 = match;
                                                    }
                                                    *parentMatch12 = match;
                                                }
                                                *parentMatch7 = match;
                                            }
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
                    for (int i : s62)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
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
            processor->Comment(s);
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Comment"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Comment"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PI(TrivialLexer& lexer, sngxml::xml::XmlProcessor* processor)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PI"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 73);
    std::u32string data = std::u32string();
    std::unique_ptr<soulng::parser::Value<std::u32string>> target;
    std::unique_ptr<soulng::parser::Value<char32_t>> chr;
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
                            for (int i : s63)
                            {
                                if (*lexer == i)
                                {
                                    ++lexer;
                                }
                                else
                                {
                                    match.hit = false;
                                    break;
                                }
                            }
                            *parentMatch5 = match;
                        }
                        if (match.hit)
                        {
                            soulng::parser::Match match(false);
                            soulng::parser::Match* parentMatch6 = &match;
                            {
                                soulng::parser::Match match = XmlParser::PITarget(lexer);
                                target.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                                *parentMatch6 = match;
                            }
                            *parentMatch5 = match;
                        }
                        *parentMatch4 = match;
                    }
                    if (match.hit)
                    {
                        soulng::parser::Match match(false);
                        soulng::parser::Match* parentMatch7 = &match;
                        {
                            soulng::parser::Match match = XmlParser::S(lexer);
                            *parentMatch7 = match;
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
                                        soulng::parser::Match* parentMatch11 = &match;
                                        {
                                            int64_t pos = lexer.GetPos();
                                            soulng::parser::Match match(false);
                                            soulng::parser::Match* parentMatch12 = &match;
                                            {
                                                soulng::parser::Match match(false);
                                                soulng::parser::Match* parentMatch13 = &match;
                                                int64_t save = lexer.GetPos();
                                                {
                                                    soulng::parser::Match match = XmlParser::Char(lexer);
                                                    chr.reset(static_cast<soulng::parser::Value<char32_t>*>(match.value));
                                                    *parentMatch13 = match;
                                                }
                                                if (match.hit)
                                                {
                                                    soulng::parser::Match match(false);
                                                    soulng::parser::Match* parentMatch14 = &match;
                                                    {
                                                        int64_t tmp = lexer.GetPos();
                                                        lexer.SetPos(save);
                                                        save = tmp;
                                                        soulng::parser::Match match(true);
                                                        for (int i : s64)
                                                        {
                                                            if (*lexer == i)
                                                            {
                                                                ++lexer;
                                                            }
                                                            else
                                                            {
                                                                match.hit = false;
                                                                break;
                                                            }
                                                        }
                                                        *parentMatch14 = match;
                                                    }
                                                    if (!match.hit)
                                                    {
                                                        lexer.SetPos(save);
                                                    }
                                                    *parentMatch13 = soulng::parser::Match(!match.hit, match.value);
                                                }
                                                *parentMatch12 = match;
                                            }
                                            if (match.hit)
                                            {
                                                data.append(1, chr->value);
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
                    *parentMatch3 = match;
                }
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch15 = &match;
                {
                    soulng::parser::Match match(true);
                    for (int i : s65)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    *parentMatch15 = match;
                }
                *parentMatch2 = match;
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            processor->PI(target->value, data);
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PI"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PI"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::PITarget(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("PITarget"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 74);
    std::unique_ptr<soulng::parser::Value<std::u32string>> name;
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t pos = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            int64_t save = lexer.GetPos();
            {
                soulng::parser::Match match = XmlParser::Name(lexer);
                name.reset(static_cast<soulng::parser::Value<std::u32string>*>(match.value));
                *parentMatch2 = match;
            }
            if (match.hit)
            {
                soulng::parser::Match match(false);
                soulng::parser::Match* parentMatch3 = &match;
                {
                    int64_t tmp = lexer.GetPos();
                    lexer.SetPos(save);
                    save = tmp;
                    soulng::parser::Match match = XmlParser::Xml(lexer);
                    *parentMatch3 = match;
                }
                if (!match.hit)
                {
                    lexer.SetPos(save);
                }
                *parentMatch2 = soulng::parser::Match(!match.hit, match.value);
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            {
                #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PITarget"));
                #endif // SOULNG_PARSER_DEBUG_SUPPORT
                return soulng::parser::Match(true, new soulng::parser::Value<std::u32string>(name->value));
            }
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("PITarget"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("PITarget"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Xml(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Xml"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 75);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(false);
            for (const soulng::parser::Range& range : s66)
            {
                if (*lexer >= range.first && *lexer <= range.last)
                {
                    match.hit = true;
                    ++lexer;
                    break;
                }
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match(false);
                for (const soulng::parser::Range& range : s67)
                {
                    if (*lexer >= range.first && *lexer <= range.last)
                    {
                        match.hit = true;
                        ++lexer;
                        break;
                    }
                }
                *parentMatch2 = match;
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
            for (const soulng::parser::Range& range : s68)
            {
                if (*lexer >= range.first && *lexer <= range.last)
                {
                    match.hit = true;
                    ++lexer;
                    break;
                }
            }
            *parentMatch3 = match;
        }
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Xml"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Xml"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::Eq(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("Eq"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 76);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            soulng::parser::Match match(true);
            int64_t save = lexer.GetPos();
            soulng::parser::Match* parentMatch2 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
                if (match.hit)
                {
                    *parentMatch2 = match;
                }
                else
                {
                    lexer.SetPos(save);
                }
            }
            *parentMatch1 = match;
        }
        if (match.hit)
        {
            soulng::parser::Match match(false);
            soulng::parser::Match* parentMatch3 = &match;
            {
                soulng::parser::Match match(false);
                if (*lexer == 61)
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
            soulng::parser::Match match(true);
            int64_t save = lexer.GetPos();
            soulng::parser::Match* parentMatch5 = &match;
            {
                soulng::parser::Match match = XmlParser::S(lexer);
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
        *parentMatch0 = match;
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("Eq"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("Eq"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}

soulng::parser::Match XmlParser::YesNo(TrivialLexer& lexer)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("YesNo"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 77);
    soulng::parser::Match match(false);
    soulng::parser::Match* parentMatch0 = &match;
    {
        int64_t save = lexer.GetPos();
        soulng::parser::Match match(false);
        soulng::parser::Match* parentMatch1 = &match;
        {
            int64_t pos = lexer.GetPos();
            soulng::parser::Match match(true);
            for (int i : s69)
            {
                if (*lexer == i)
                {
                    ++lexer;
                }
                else
                {
                    match.hit = false;
                    break;
                }
            }
            if (match.hit)
            {
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("YesNo"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, new soulng::parser::Value<bool>(true));
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
                    soulng::parser::Match match(true);
                    for (int i : s70)
                    {
                        if (*lexer == i)
                        {
                            ++lexer;
                        }
                        else
                        {
                            match.hit = false;
                            break;
                        }
                    }
                    if (match.hit)
                    {
                        {
                            #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                            if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("YesNo"));
                            #endif // SOULNG_PARSER_DEBUG_SUPPORT
                            return soulng::parser::Match(true, new soulng::parser::Value<bool>(false));
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
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("YesNo"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("YesNo"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
