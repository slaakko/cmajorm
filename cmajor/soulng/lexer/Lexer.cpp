// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/Lexer.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/Unicode.hpp>
#include <string>
#include <algorithm>

namespace soulng { namespace lexer {

using namespace soulng::unicode;

Lexer::Lexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) :
    content(content_), fileName(fileName_), fileIndex(fileIndex_), line(1), keywordMap(nullptr), start(content.c_str()), end(content.c_str() + content.length()), pos(start), current(tokens.end()),
    log(nullptr), countLines(true), separatorChar('\0'), flags(), cursorNode(nullptr)
{
}

Lexer::Lexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) :
    content(), fileName(fileName_), fileIndex(fileIndex_), line(1), keywordMap(nullptr), start(start_), end(end_), pos(start), current(tokens.end()),
    log(nullptr), countLines(true), separatorChar('\0'), flags(), cursorNode(nullptr)
{
}

Lexer::~Lexer()
{
}

void Lexer::operator++()
{
    if (current != tokens.end())
    {
        ++current;
    }
    if (current == tokens.end())
    {
        NextToken();
    }
    else
    {
        line = current->line;
    }
}

int64_t Lexer::GetPos() const
{
    int32_t p = static_cast<int32_t>(current - tokens.begin());
    return (static_cast<int64_t>(line) << 32) | static_cast<int64_t>(p);
}

void Lexer::SetPos(int64_t pos)
{
    current = tokens.begin() + static_cast<int32_t>(pos);
    line = static_cast<int32_t>(pos >> 32);
}

void Lexer::NextToken()
{
    int state = 0;
    while (true)
    {
        char32_t c = separatorChar;
        if (pos != end)
        {
            c = *pos;
        }
        else if (c == '\0')
        {
            break;
        }
        if (state == 0)
        {
            lexeme.begin = pos;
            token.id = INVALID_TOKEN;
            token.line = line;
        }
        if (pos == end)
        {
            lexeme.end = end;
        }
        else
        {
            lexeme.end = pos + 1;
        }
        state = NextState(state, c);
        if (state == -1)
        {
            if (token.id == CONTINUE_TOKEN)
            {
                if (pos == end)
                {
                    break;
                }
                else
                {
                    pos = token.match.end;
                }
                state = 0;
                continue;
            }
            else if (token.id == INVALID_TOKEN)
            {
                if (pos == end)
                {
                    break;
                }
                else
                {
                    throw std::runtime_error("soulng::lexer::Lexer::NextToken(): error: invalid character '" + ToUtf8(std::u32string(1, c)) + "' in file '" + fileName + "' at line " + std::to_string(line));
                }
            }
            else
            {
                tokens.push_back(token);
                current = tokens.end() - 1;
                pos = token.match.end;
                return;
            }
        }
        if (c == '\n' && countLines)
        {
            ++line;
        }
        ++pos;
    }
    token.id = INVALID_TOKEN;
    state = NextState(state, '\0');
    int64_t p = -1;
    if (token.id != INVALID_TOKEN && token.id != CONTINUE_TOKEN)
    {
        tokens.push_back(token);
        current = tokens.end() - 1;
        p = GetPos();
    }
    Token endToken(END_TOKEN);
    endToken.match.begin = end;
    endToken.match.end = end;
    tokens.push_back(endToken);
    if (p == -1)
    {
        current = tokens.end() - 1;
        p = GetPos();
    }
    SetPos(p);
}

int Lexer::NextState(int state, char32_t c)
{
    return -1;
}

int Lexer::GetKeywordToken(const Lexeme& lexeme) const
{
    if (keywordMap)
    {
        return keywordMap->GetKeywordToken(lexeme);
    }
    else
    {
        return INVALID_TOKEN;
    }
}

void Lexer::ConvertExternal(Span& span)
{
    Token startToken = GetToken(span.start);
    span.start = static_cast<int>(startToken.match.begin - start);
    Token endToken = GetToken(span.end);
    span.end = static_cast<int>(endToken.match.end - start);
}

Token Lexer::GetToken(int64_t pos) const
{
    int32_t tokenIndex = static_cast<int32_t>(pos);
    if (tokenIndex >= 0 && tokenIndex < tokens.size())
    {
        return tokens[tokenIndex];
    }
    else
    {
        throw std::runtime_error("invalid token index");
    }
}

void Lexer::SetTokens(const std::vector<Token>& tokens_)
{
    if (!tokens_.empty())
    {
        tokens.push_back(tokens_.front());
    }
    else
    {
        tokens.push_back(Token(END_TOKEN, Lexeme(end, end), 1));
    }
    for (const Token& token : tokens_)
    {
        tokens.push_back(token);
    }
    tokens.push_back(Token(END_TOKEN, Lexeme(end, end), 1));
    current = tokens.begin();
}

std::u32string Lexer::GetMatch(const Span& span) const
{
    std::u32string match;
    Token startToken = GetToken(span.start);
    match.append(startToken.match.ToString());
    const char32_t* e = startToken.match.end;
    for (int i = span.start + 1; i <= span.end; ++i)
    {
        Token token = GetToken(i);
        match.append(std::u32string(token.match.begin - e, ' '));
        match.append(token.match.ToString());
        e = token.match.end;
    }
    return match;
}

const char32_t* LineStart(const char32_t* start, const char32_t* p)
{
    while (p != start && *p != '\n' && *p != '\r')
    {
        --p;
    }
    if (p != start)
    {
        ++p;
    }
    return p;
}

const char32_t* LineEnd(const char32_t* end, const char32_t* p)
{
    while (p != end && *p != '\n' && *p != '\r')
    {
        ++p;
    }
    return p;
}

std::u32string GetErrorLines(const char32_t* start, const char32_t* end, const Span& externalSpan)
{
    const char32_t* startPos = start + externalSpan.start;
    if (startPos < start || startPos >= end)
    {
        return std::u32string();
    }
    const char32_t* lineStart = LineStart(start, startPos);
    int cols = static_cast<int>(startPos - lineStart);
    if (cols < 0)
    {
        cols = 0;
    }
    const char32_t* lineEnd = LineEnd(end, startPos);
    if (lineEnd < lineStart)
    {
        lineEnd = lineStart;
    }
    int lineLength = static_cast<int>(lineEnd - lineStart);
    std::u32string lines(lineStart, lineEnd);
    int spanCols = std::max(static_cast<int>(1), std::min(externalSpan.end - externalSpan.start, lineLength - cols));
    lines.append(1, '\n').append(std::u32string(cols, ' ')).append(spanCols, '^');
    return lines;
}

void GetColumns(const char32_t* start, const char32_t* end, const Span& externalSpan, int32_t& startCol, int32_t& endCol)
{
    startCol = 0;
    endCol = 0;
    const char32_t* startPos = start + externalSpan.start;
    if (startPos < start || startPos >= end)
    {
        return;
    }
    const char32_t* lineStart = LineStart(start, startPos);
    int cols = static_cast<int>(startPos - lineStart);
    if (cols < 0)
    {
        cols = 0;
    }
    startCol = cols + 1;
    const char32_t* lineEnd = LineEnd(end, startPos);
    if (lineEnd < lineStart)
    {
        lineEnd = lineStart;
    }
    int lineLength = static_cast<int>(lineEnd - lineStart);
    int spanCols = std::max(static_cast<int>(1), std::min(externalSpan.end - externalSpan.start, lineLength - cols));
    endCol = startCol + spanCols;
}

std::u32string Lexer::ErrorLines(const Token& token) const
{
    std::u32string lines;
    const char32_t* lineStart = LineStart(start, token.match.begin);
    const char32_t* lineEnd = LineEnd(end, token.match.end);
    lines.append(std::u32string(lineStart, token.match.begin));
    lines.append(token.match.ToString());
    lines.append(std::u32string(token.match.end, lineEnd));
    lines.append(1, '\n');
    lines.append(token.match.begin - lineStart, ' ');
    lines.append(std::max(static_cast<int64_t>(1), token.match.end - token.match.begin), '^');
    lines.append(lineEnd - token.match.end, ' ');
    lines.append(1, '\n');
    return lines;
}

std::u32string Lexer::ErrorLines(const Span& span) const
{
    std::u32string lines;
    Token startToken = GetToken(span.start);
    Token endToken = startToken;
    const char32_t* lineStart = LineStart(start, startToken.match.begin);
    if (span.end != span.start)
    {
        endToken = GetToken(span.end);
    }
    const char32_t* lineEnd = LineEnd(end, endToken.match.end);
    lines.append(std::u32string(lineStart, startToken.match.begin));
    lines.append(startToken.match.ToString());
    const char32_t* s = startToken.match.begin;
    const char32_t* e = startToken.match.end;
    for (int i = span.start + 1; i <= span.end; ++i)
    {
        Token token = GetToken(i);
        lines.append(std::u32string(token.match.begin - e, ' '));
        lines.append(token.match.ToString());
        e = token.match.end;
    }
    lines.append(std::u32string(e, lineEnd));
    lines.append(1, '\n');
    lines.append(s - lineStart, ' ');
    lines.append(std::max(static_cast<int64_t>(1), e - s), '^');
    lines.append(lineEnd - e, ' ');
    lines.append(1, '\n');
    return lines;
}

void Lexer::GetColumns(const Span& span, int32_t& startCol, int32_t& endCol) const
{
    Token startToken = GetToken(span.start);
    Token endToken = startToken;
    const char32_t* lineStart = LineStart(start, startToken.match.begin);
    if (span.end != span.start)
    {
        endToken = GetToken(span.end);
    }
    int cols = static_cast<int>(startToken.match.begin - lineStart);
    if (cols < 0)
    {
        cols = 0;
    }
    startCol = cols + 1;
    const char32_t* lineEnd = LineEnd(end, endToken.match.end);
    if (lineEnd < lineStart)
    {
        lineEnd = lineStart;
    }
    int lineLength = static_cast<int>(lineEnd - lineStart);
    int spanCols = std::max(static_cast<int>(1), std::min(static_cast<int>(endToken.match.end - startToken.match.begin), lineLength - cols));
    endCol = startCol + spanCols;
}

void Lexer::ThrowExpectationFailure(const Span& span, const std::u32string& name)
{
    Token token = GetToken(span.start);
    throw ParsingException("parsing error in '" + fileName + ":" + std::to_string(token.line) + "': " + ToUtf8(name) + " expected:\n" + ToUtf8(ErrorLines(span)), fileName, span);
}

void Lexer::AddError(const Span& span, const std::u32string& name)
{
    Token token = GetToken(span.start);
    ParsingException error("parsing error in '" + fileName + ":" + std::to_string(token.line) + "': " + ToUtf8(name) + " expected:\n" + ToUtf8(ErrorLines(span)), fileName, span);
    errors.push_back(std::move(error));
}

std::u32string Lexer::RestOfLine(int maxLineLength)
{
    std::u32string restOfLine(current->match.ToString() + std::u32string(current->match.end, pos) + std::u32string(pos, LineEnd(end, pos)));
    if (maxLineLength != 0)
    {
        restOfLine = restOfLine.substr(0, maxLineLength);
    }
    return restOfLine;
}

TokenLine Lexer::TokenizeLine(const std::u32string& line, int lineNumber, int startState)
{
    const char32_t* pos = line.c_str();
    const char32_t* end = line.c_str() + line.length();
    TokenLine tokenLine;
    lexeme.begin = end;
    lexeme.end = end;
    token.match = lexeme;
    token.id = INVALID_TOKEN;
    token.line = lineNumber;
    int state = startState;
    while (pos != end)
    {
        char32_t c = *pos;
        if (state == 0)
        {
            lexeme.begin = pos;
            token.id = INVALID_TOKEN;
            token.line = lineNumber;
        }
        lexeme.end = pos + 1;
        state = NextState(state, c);
        if (state == -1)
        {
            state = 0;
            pos = token.match.end;
            tokenLine.tokens.push_back(token);
        }
        ++pos;
    }
    if (token.match.begin != token.match.end)
    {
        tokenLine.tokens.push_back(token);
    }
    tokenLine.endState = state;
    return tokenLine;
}

void Lexer::SetSyncTokens(const std::vector<int>& syncTokens_)
{
    syncTokens = syncTokens_;
}

void Lexer::Synchronize()
{
    while (pos != end)
    {
        int curToken = token.id;
        for (int syncToken : syncTokens)
        {
            if (curToken == syncToken)
            {
                return;
            }
        }
        ++*this;
    }
}

void WriteBeginRuleToLog(Lexer& lexer, const std::u32string& ruleName)
{
    lexer.Log()->WriteBeginRule(ruleName);
    lexer.Log()->IncIndent();
    lexer.Log()->WriteTry(lexer.RestOfLine(lexer.Log()->MaxLineLength()));
    lexer.Log()->IncIndent();
}

void WriteSuccessToLog(Lexer& lexer, const Span& matchSpan, const std::u32string& ruleName)
{
    lexer.Log()->DecIndent();
    lexer.Log()->WriteSuccess(lexer.GetMatch(matchSpan));
    lexer.Log()->DecIndent();
    lexer.Log()->WriteEndRule(ruleName);
}

void WriteFailureToLog(Lexer& lexer, const std::u32string& ruleName)
{
    lexer.Log()->DecIndent();
    lexer.Log()->WriteFail();
    lexer.Log()->DecIndent();
    lexer.Log()->WriteEndRule(ruleName);
}

} } // namespace soulng::lexer
