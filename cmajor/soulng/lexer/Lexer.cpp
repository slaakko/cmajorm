// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <soulng/lexer/Lexer.hpp>
#include <soulng/lexer/ParsingException.hpp>
#include <soulng/util/Unicode.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <algorithm>
#include <limits>

namespace soulng { namespace lexer {

using namespace soulng::unicode;

int8_t ToSByte(int64_t value)
{
    if (value < std::numeric_limits<int8_t>::min() || value > std::numeric_limits<int8_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<int8_t>(value);
}

uint8_t ToByte(int64_t value)
{
    if (value < std::numeric_limits<uint8_t>::min() || value > std::numeric_limits<uint8_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<uint8_t>(value);
}

int16_t ToShort(int64_t value)
{
    if (value < std::numeric_limits<int16_t>::min() || value > std::numeric_limits<int16_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<int16_t>(value);
}

uint16_t ToUShort(int64_t value)
{
    if (value < std::numeric_limits<uint16_t>::min() || value > std::numeric_limits<uint16_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<uint16_t>(value);
}

int32_t ToInt(int64_t value)
{
    if (value < std::numeric_limits<int32_t>::min() || value > std::numeric_limits<int32_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<int32_t>(value);
}

uint32_t ToUInt(int64_t value)
{
    if (value < std::numeric_limits<uint32_t>::min() || value > std::numeric_limits<uint32_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<uint32_t>(value);
}

int64_t ToLong(int64_t value)
{
    if (value < std::numeric_limits<int64_t>::min() || value > std::numeric_limits<int64_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<int64_t>(value);
}

uint64_t ToULong(uint64_t value)
{
    if (value < std::numeric_limits<uint64_t>::min() || value > std::numeric_limits<uint64_t>::max())
    {
        throw std::runtime_error("range error: value=" + std::to_string(value));
    }
    return static_cast<uint64_t>(value);
}

LexerState::LexerState() : token(), line(0), lexeme(), pos(), tokens(), flags(), recordedPosPair(), farthestPos(), currentPos()
{
}

SourceInfo::SourceInfo() : fileName(), lineNumber(0), sourceLine()
{
}

LineMapper::~LineMapper()
{
}

Lexer::Lexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_) :
    content(content_), fileName(fileName_), fileIndex(fileIndex_), line(1), keywordMap(nullptr), start(content.c_str()), end(content.c_str() + content.length()), pos(start), current(tokens.end()),
    log(nullptr), countLines(true), separatorChar('\0'), flags(), commentTokenId(-1), farthestPos(GetPos()), ruleNameVecPtr(nullptr), lineMapper(nullptr), recovered(false)
{
    CalculateLineStarts();
}

Lexer::Lexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_) :
    content(), fileName(fileName_), fileIndex(fileIndex_), line(1), keywordMap(nullptr), start(start_), end(end_), pos(start), current(tokens.end()),
    log(nullptr), countLines(true), separatorChar('\0'), flags(), commentTokenId(-1), farthestPos(GetPos()), ruleNameVecPtr(nullptr), lineMapper(nullptr), recovered(false)
{
    CalculateLineStarts();
}

Lexer::~Lexer()
{
}

void Lexer::operator++()
{
    if (GetFlag(LexerFlags::recordedParse))
    {
        // precondition: !tokens.empty() && tokens.back().id == END_TOKEN
        int64_t currentPos = GetPos();
        if (currentPos == recordedPosPair.end)
        {
            current = tokens.end() - 1; // set current to last token whose id is END_TOKEN
            pos = tokens.back().match.end;
        }
        else
        {
            ++current;
        }
        line = current->line;
    }
    else
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
        if (GetFlag(LexerFlags::farthestError))
        {
            int64_t p = GetPos();
            if (p > farthestPos)
            {
                farthestPos = p;
                farthestRuleContext = ruleContext;
            }
        }
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

SourcePos Lexer::GetSourcePos(int64_t pos) const
{
    const char32_t* s = start;
    int line = GetLine(pos);
    if (line < lineStarts.size())
    {
        s = lineStarts[line];
    }
    Token token = GetToken(pos);
    int col = token.match.begin - s + 1;
    return SourcePos(pos, line, col);
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

std::string Lexer::MappedFileName(const SourcePos& sourcePos) const
{
    SourceInfo sourceInfo;
    if (lineMapper)
    {
        sourceInfo = lineMapper->GetSourceInfo(sourcePos);
    }
    if (!sourceInfo.fileName.empty())
    {
        return sourceInfo.fileName;
    }
    else
    {
        return FileName();
    }
}

std::string Lexer::MappedFileLine(const SourcePos& sourcePos) const
{
    SourceInfo sourceInfo;
    if (lineMapper)
    {
        sourceInfo = lineMapper->GetSourceInfo(sourcePos);
    }
    if (sourceInfo.lineNumber && !sourceInfo.fileName.empty())
    {
        return sourceInfo.fileName + ":" + std::to_string(sourceInfo.lineNumber);
    }
    else
    {
        return FileName() + ":" + std::to_string(line);
    }
}

int Lexer::MappedLineNumber(const SourcePos& sourcePos) const
{
    SourceInfo sourceInfo;
    if (lineMapper)
    {
        sourceInfo = lineMapper->GetSourceInfo(sourcePos);
    }
    if (sourceInfo.lineNumber)
    {
        return sourceInfo.lineNumber;
    }
    else
    {
        return line;
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

std::string Lexer::GetString(int64_t pos) const
{
    return ToUtf8(GetToken(pos).match.ToString());
}

int8_t Lexer::GetSByte(int64_t pos) const
{
    try
    {
        return ToSByte(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("sbyte expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

uint8_t Lexer::GetByte(int64_t pos) const
{
    try
    {
        return ToByte(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("byte expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

int16_t Lexer::GetShort(int64_t pos) const
{
    try
    {
        return ToShort(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("short expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

uint16_t Lexer::GetUShort(int64_t pos) const
{
    try
    {
        return ToUShort(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("ushort expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

int32_t Lexer::GetInt(int64_t pos) const
{
    try
    {
        return ToInt(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("int expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

uint32_t Lexer::GetUInt(int64_t pos) const
{
    try
    {
        return ToUInt(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("uint expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

int64_t Lexer::GetLong(int64_t pos) const
{
    try
    {
        return ToLong(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("long expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

uint64_t Lexer::GetULong(int64_t pos) const
{
    try
    {
        return ToULong(boost::lexical_cast<int64_t>(GetString(pos)));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("ulong expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

float Lexer::GetFloat(int64_t pos) const
{
    try
    {
        return boost::lexical_cast<float>(GetString(pos));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("float expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
    }
}

double Lexer::GetDouble(int64_t pos) const
{
    try
    {
        return boost::lexical_cast<double>(GetString(pos));
    }
    catch (const std::exception& ex)
    {
        throw std::runtime_error("float expected in '" + fileName + "' at line " + std::to_string(line) + ": " + ex.what());
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

std::u32string Lexer::GetMatch(int64_t pos) const
{
    return GetToken(pos).match.ToString();
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
    if (token.match.begin && token.match.end)
    {
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
    }
    return lines;
}

std::string Lexer::MappedErrorLines(const Token& token) const
{
    SourceInfo sourceInfo;
    if (lineMapper)
    {
        const char32_t* lineStart = LineStart(start, token.match.begin);
        SourcePos sourcePos(-1, token.line, token.match.begin - lineStart);
        sourceInfo = lineMapper->GetSourceInfo(sourcePos);
    }
    if (sourceInfo.lineNumber)
    {
        const char32_t* lineStart = LineStart(start, token.match.begin);
        std::string caretLine = std::string(token.match.begin - lineStart, ' ') + std::string(std::max(static_cast<int64_t>(1), token.match.end - token.match.begin), '^');
        return sourceInfo.sourceLine + caretLine;
    }
    else
    {
        return ToUtf8(ErrorLines(token));
    }
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

std::u32string Lexer::ErrorLines(const SourcePos& sourcePos) const
{
    std::u32string lines;
    const char32_t* lineStart = lineStarts[sourcePos.line];
    const char32_t* lineEnd = LineEnd(end, lineStart);
    lines.append(std::u32string(lineStart, lineEnd));
    lines.append(1, '\n');
    std::u32string caretLine;
    if (sourcePos.col > 1)
    {
        caretLine.append(sourcePos.col - 1, ' ');
    }
    caretLine.append(1, '^');
    lines.append(caretLine);
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
    int spanCols = std::max(static_cast<int>(1), std::min(span.end - span.start, lineLength - cols));
    endCol = startCol + spanCols;
}

void Lexer::ThrowExpectationFailure(const Span& span, const std::u32string& name)
{
    Token token = GetToken(span.start);
    throw ParsingException("parsing error at '" + fileName + ":" + std::to_string(token.line) + "': " + ToUtf8(name) + " expected:\n" + ToUtf8(ErrorLines(span)), fileName, span);
}

std::string Lexer::GetFarthestError() const
{
    Token token = GetToken(farthestPos);
    std::string parserStateStr = GetParserStateStr();
    if (lineMapper)
    {
        const char32_t* lineStart = LineStart(start, token.match.begin);
        SourcePos sourcePos(-1, token.line, std::max(1, static_cast<int>(token.match.begin - lineStart)));
        SourceInfo sourceInfo = lineMapper->GetSourceInfo(sourcePos);
        std::string caretLine = std::string(sourcePos.col - 1, ' ') + std::string(std::max(static_cast<int64_t>(1), token.match.end - token.match.begin), '^');
        return "parsing error at '" + sourceInfo.fileName + ":" + std::to_string(sourceInfo.lineNumber) + ":\n" +
            sourceInfo.sourceLine + caretLine + "\n" + parserStateStr;
    }
    else
    {
        return "parsing error at '" + fileName + ":" + std::to_string(token.line) + "':\n" + ToUtf8(ErrorLines(token)) + parserStateStr;
    }
}

void Lexer::ThrowFarthestError()
{
    throw ParsingException(GetFarthestError(), fileName);
}

void Lexer::AddError(const Span& span, const std::u32string& name)
{
    Token token = GetToken(span.start);
    ParsingException error("parsing error at '" + fileName + ":" + std::to_string(token.line) + "': " + ToUtf8(name) + " expected:\n" + ToUtf8(ErrorLines(span)), fileName, span);
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
    pos = line.c_str();
    end = line.c_str() + line.length();
    TokenLine tokenLine;
    tokenLine.startState = startState;
    lexeme.begin = pos;
    lexeme.end = end;
    token.match = lexeme;
    token.id = INVALID_TOKEN;
    token.line = lineNumber;
    int state = startState;
    int prevState = 0;
    int prevPrevState = 0;
    bool cont = false;
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
        prevPrevState = prevState;
        prevState = state;
        state = NextState(state, c);
        if (state == -1)
        {
            if (prevState == 0)
            {
                break;
            }
            state = 0;
            pos = token.match.end;
            tokenLine.tokens.push_back(token);
            if (pos + 1 < end && *pos == '\"' && *(pos + 1) == '\\' && prevPrevState == 13 && prevState == 71)
            {
                Token tok;
                tok.match.begin = pos;
                tok.match.end = pos + 2;
                tokenLine.tokens.push_back(tok);
                pos += 2;
            }
            lexeme.begin = lexeme.end;
        }
        else
        {
            ++pos;
        }
    }
    if (state != 0 && state != -1)
    {
        state = NextState(state, '\r');
    }
    if (state != 0 && state != -1)
    {
        state = NextState(state, '\n');
    }
    if (state != 0 && state != -1)
    {
        if (blockCommentStates.find(state) != blockCommentStates.cend())
        {
            token.id = commentTokenId;
            token.match.end = end;
            tokenLine.tokens.push_back(token);
            tokenLine.endState = state;
            return tokenLine;
        }
    }
    if (lexeme.begin != lexeme.end)
    {
        token.match = lexeme;
        tokenLine.tokens.push_back(token);
    }
    if (state == -1)
    {
        state = 0;
    }
    tokenLine.endState = state;
    return tokenLine;
}

void Lexer::SetSyncTokens(const std::vector<int>& syncTokens_)
{
    syncTokens = syncTokens_;
}

bool Lexer::Synchronize()
{
    if (GetFlag(LexerFlags::synchronize))
    {
        if (GetFlag(LexerFlags::synchronized)) return false;
        SetFlag(LexerFlags::synchronized);
        while (pos != end)
        {
            int curToken = token.id;
            for (int syncToken : syncTokens)
            {
                if (curToken == syncToken)
                {
                    return true;
                }
            }
            ++* this;
        }
    }
    return false;
}

void Lexer::SetBlockCommentStates(const std::set<int>& blockCommentStates_)
{
    blockCommentStates = blockCommentStates_;
}

const std::set<int>& Lexer::BlockCommentStates() const
{
    return blockCommentStates;
}

LexerState Lexer::GetState() const
{
    LexerState state;
    state.token = token;
    state.line = line;
    state.lexeme = lexeme;
    state.pos = pos;
    state.tokens = tokens;
    state.flags = flags;
    state.recordedPosPair = recordedPosPair;
    state.farthestPos = farthestPos;
    state.ruleContext = ruleContext;
    state.farthestRuleContext = farthestRuleContext;
    state.currentPos = GetPos();
    return state;
}

void Lexer::SetState(const LexerState& state)
{
    token = state.token;
    line = state.line;
    lexeme = state.lexeme;
    pos = state.pos;
    tokens = state.tokens;
    flags = state.flags;
    recordedPosPair = state.recordedPosPair;
    farthestPos = state.farthestPos;
    ruleContext = state.ruleContext;
    farthestRuleContext = state.farthestRuleContext;
    SetPos(state.currentPos);
}

void Lexer::PushState()
{
    stateStack.push(GetState());
}

void Lexer::PopState()
{
    SetState(stateStack.top());
    stateStack.pop();
}

void Lexer::BeginRecordedParse(const LexerPosPair& recordedPosPair_)
{
    PushState();
    if (tokens.empty() || tokens.back().id != END_TOKEN)
    {
        Token endToken(END_TOKEN);
        endToken.match.begin = end;
        endToken.match.end = end;
        tokens.push_back(endToken);
    }
    recordedPosPair = recordedPosPair_;
    SetPos(recordedPosPair.start);
    SetFlag(LexerFlags::recordedParse);
}

void Lexer::EndRecordedParse()
{
    PopState();
}

void Lexer::PushRule(int ruleId)
{
    ruleContext.push_back(ruleId);
}

void Lexer::PopRule()
{
    ruleContext.pop_back();
}

void Lexer::SetGlobalRuleContext()
{
    if (GetFlag(LexerFlags::gcc))
    {
        if (ruleContext.size() > cursorRuleContext.size())
        {
            cursorRuleContext = ruleContext;
        }
    }
}

void Lexer::SetLocalRuleContext()
{
    if (GetFlag(LexerFlags::lcc))
    {
        cursorRuleContext = ruleContext;
    }
}

void Lexer::ResetGlobalRuleContext()
{
    if (GetFlag(LexerFlags::gcc))
    {
        cursorRuleContext.clear();
    }
}

std::string Lexer::GetParserStateStr() const
{
    std::string parserStateStr;
    int n = farthestRuleContext.size();
    if (ruleNameVecPtr && n > 0)
    {
        parserStateStr.append("\nParser state:\n");
        for (int i = 0; i < n; ++i)
        {
            int ruleId = farthestRuleContext[i];
            if (ruleId >= 0 && ruleId < ruleNameVecPtr->size())
            {
                std::string ruleName = (*ruleNameVecPtr)[ruleId];
                parserStateStr.append(ruleName.append("\n"));
            }
        }
    }
    return parserStateStr;
}

void Lexer::CalculateLineStarts()
{
    lineStarts.push_back(pos);
    const char32_t* p = pos;
    bool startOfLine = true;
    while (p != end)
    {
        if (startOfLine)
        {
            lineStarts.push_back(p);
        }
        startOfLine = *p == '\n';
        ++p;
    }
    lineStarts.push_back(end);
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
