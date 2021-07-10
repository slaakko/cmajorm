#include "BasicType.hpp"
#include <soulng/util/Unicode.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowLexer.hpp>
#include <sngcm/cmnothrowlexer/CmajorNothrowTokens.hpp>

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmnothrowparser/BasicType.parser' using soulng parser generator spg version 4.0.0

using namespace soulng::unicode;
using namespace sngcm::ast;
using namespace CmajorNothrowTokens;

soulng::parser::Match NothrowBasicTypeParser::BasicType(CmajorNothrowLexer& lexer, boost::uuids::uuid* moduleId)
{
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::Span parser_debug_match_span;
    bool parser_debug_write_to_log = lexer.Log() != nullptr;
    if (parser_debug_write_to_log)
    {
        parser_debug_match_span = lexer.GetSpan();
        soulng::lexer::WriteBeginRuleToLog(lexer, soulng::unicode::ToUtf32("BasicType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    soulng::lexer::RuleGuard ruleGuard(lexer, 2);
    soulng::parser::Match match(false);
    int64_t pos = lexer.GetPos();
    soulng::lexer::SourcePos sourcePos = lexer.GetSourcePos(pos);
    soulng::lexer::Span span = lexer.GetSpan();
    switch (*lexer)
    {
        case BOOL:
        {
            ++lexer;
            {
                BoolNode * node = new BoolNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case SBYTE:
        {
            ++lexer;
            {
                SByteNode * node = new SByteNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case BYTE:
        {
            ++lexer;
            {
                ByteNode * node = new ByteNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case SHORT:
        {
            ++lexer;
            {
                ShortNode * node = new ShortNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case USHORT:
        {
            ++lexer;
            {
                UShortNode * node = new UShortNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case INT:
        {
            ++lexer;
            {
                IntNode * node = new IntNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case UINT:
        {
            ++lexer;
            {
                UIntNode * node = new UIntNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case LONG:
        {
            ++lexer;
            {
                LongNode * node = new LongNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case ULONG:
        {
            ++lexer;
            {
                ULongNode * node = new ULongNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case FLOAT:
        {
            ++lexer;
            {
                FloatNode * node = new FloatNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case DOUBLE:
        {
            ++lexer;
            {
                DoubleNode * node = new DoubleNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case CHAR:
        {
            ++lexer;
            {
                CharNode * node = new CharNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case WCHAR:
        {
            ++lexer;
            {
                WCharNode * node = new WCharNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case UCHAR:
        {
            ++lexer;
            {
                UCharNode * node = new UCharNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
        case VOID:
        {
            ++lexer;
            {
                VoidNode * node = new VoidNode(span, *moduleId);
                node->SetLexerFlags(lexer.Flags());
                {
                    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
                    if (parser_debug_write_to_log) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
                    #endif // SOULNG_PARSER_DEBUG_SUPPORT
                    return soulng::parser::Match(true, node);
                }
            }
            break;
        }
    }
    #ifdef SOULNG_PARSER_DEBUG_SUPPORT
    if (parser_debug_write_to_log)
    {
        if (match.hit) soulng::lexer::WriteSuccessToLog(lexer, parser_debug_match_span, soulng::unicode::ToUtf32("BasicType"));
        else soulng::lexer::WriteFailureToLog(lexer, soulng::unicode::ToUtf32("BasicType"));
    }
    #endif // SOULNG_PARSER_DEBUG_SUPPORT
    if (!match.hit)
    {
        match.value = nullptr;
    }
    return match;
}
