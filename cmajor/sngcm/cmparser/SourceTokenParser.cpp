// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <sngcm/cmparser/SourceTokenParser.hpp>
#include <sngcm/cmlexer/TokenLexer.hpp>
#include <sngcm/cmlexer/TokenLexerTokens.hpp>
#include <soulng/util/TextUtils.hpp>
#include <soulng/util/Unicode.hpp>

namespace sngcm { namespace parser {

void ParseSourceLine(const std::u32string& sourceLine, sngcm::ast::SourceTokenFormatter* formatter, bool& inBlockComment)
{
    using namespace TokenLexerTokens;
    using namespace soulng::unicode;

    std::u32string line(sourceLine);
    line.append(U"\n");
    TokenLexer lexer(line, "", 0);
    lexer.inBlockComment = inBlockComment;
    lexer.SetSeparatorChar('\n');
    ++lexer;
    int i = 0;
    formatter->BeginFormat();
    while (*lexer != END)
    {
        switch (*lexer)
        {
            case WS: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Spaces(token.match.ToString()); break; }
            case LINECOMMENT: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Comment(token.match.ToString()); return; }
            case BLOCKCOMMENT:
            {
                soulng::lexer::Token token = lexer.GetToken(i);
                formatter->Comment(token.match.ToString());
                if (!soulng::util::EndsWith(ToUtf8(token.match.ToString()), "*/"))
                {
                    inBlockComment = true;
                    return;
                }
                break;
            }
            case BLOCKCOMMENTLINE:
            {
                soulng::lexer::Token token = lexer.GetToken(i);
                formatter->Comment(token.match.ToString());
                if (token.match.ToString().find(U"*/") != std::u32string::npos)
                {
                    inBlockComment = false;
                }
                return;
            }
            case KEYWORD: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Keyword(token.match.ToString()); break; }
            case ID: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Identifier(token.match.ToString()); break; }
            case CHARLIT: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Char(token.match.ToString()); break; }
            case STRINGLIT: { soulng::lexer::Token token = lexer.GetToken(i); formatter->String(token.match.ToString()); break; }
            case NUMBER: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Number(token.match.ToString()); break; }
            case OTHER: { soulng::lexer::Token token = lexer.GetToken(i); formatter->Other(token.match.ToString()); break; }
        }
        ++lexer;
        ++i;
    }
    formatter->EndFormat();
}

} } // namespace sngcm::parser
