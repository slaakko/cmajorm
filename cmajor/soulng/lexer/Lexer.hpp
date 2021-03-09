// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_LEXER_INCLUDED
#define SOULNG_LEXER_LEXER_INCLUDED
#include <soulng/lexer/Keyword.hpp>
#include <soulng/lexer/Span.hpp>
#include <soulng/lexer/ParsingLog.hpp>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <stdint.h>

namespace soulng { namespace lexer {

enum class LexerFlags : int8_t
{
    none = 0, synchronize = 1 << 0, synchronized = 1 << 1, synchronizedAtLeastOnce = 1 << 2, cursorSeen = 1 << 3
};

inline LexerFlags operator|(LexerFlags left, LexerFlags right)
{
    return static_cast<LexerFlags>(static_cast<int8_t>(left) | static_cast<int8_t>(right));
}

inline LexerFlags operator&(LexerFlags left, LexerFlags right)
{
    return static_cast<LexerFlags>(static_cast<int8_t>(left) & static_cast<int8_t>(right));
}

inline LexerFlags operator~(LexerFlags flag)
{
    return static_cast<LexerFlags>(~static_cast<int8_t>(flag));
}

class SOULNG_LEXER_API Lexer
{
public:
    Lexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    Lexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    Lexer(const Lexer&) = delete;
    Lexer(Lexer&&) = delete;
    Lexer& operator=(const Lexer&) = delete;
    Lexer& operator=(Lexer&&) = delete;
    virtual ~Lexer();
    int operator*() const { return current->id; }
    void operator++();
    int64_t GetPos() const;
    void SetPos(int64_t pos);
    virtual int NextState(int state, char32_t c);
    void SetKeywordMap(KeywordMap* keywordMap_) { keywordMap = keywordMap_; }
    KeywordMap* GetKeywordMap() { return keywordMap; }
    int GetKeywordToken(const Lexeme& lexeme) const;
    void Retract() { token.match.end = pos; }
    const std::string& FileName() const { return fileName; }
    Span GetSpan() const { return Span(fileIndex, line, static_cast<int32_t>(GetPos())); }
    void ConvertExternal(Span& span);
    Token GetToken(int64_t pos) const;
    void SetTokens(const std::vector<Token>& tokens_);
    void SetLine(int line_) { line = line_; }
    void SetCountLines(bool countLines_) { countLines = countLines_; }
    Token token;
    std::u32string GetMatch(const Span& span) const;
    std::u32string ErrorLines(const Token& token) const;
    std::u32string ErrorLines(const Span& span) const;
    void GetColumns(const Span& span, int32_t& startCol, int32_t& endCol) const;
    void ThrowExpectationFailure(const Span& span, const std::u32string& name);
    void AddError(const Span& span, const std::u32string& name);
    std::vector<std::unique_ptr<std::exception>> Errors() { return std::move(errors); }
    const char32_t* Start() const { return start; }
    const char32_t* End() const { return end; }
    const char32_t* Pos() const { return pos; }
    void SetLog(ParsingLog* log_) { log = log_; }
    ParsingLog* Log() const { return log; }
    std::u32string RestOfLine(int maxLineLength);
    void SetSeparatorChar(char32_t separatorChar_) { separatorChar = separatorChar_; }
    TokenLine TokenizeLine(const std::u32string& line, int lineNumber, int startState);
    void SetSyncTokens(const std::vector<int>& syncTokens_);
    bool Synchronize();
    void SetBlockCommentStates(const std::set<int>& blockCommentStates_);
    const std::set<int>& BlockCommentStates() const;
    void SetCommentTokenId(int commentTokenId_) { commentTokenId = commentTokenId_; }
    LexerFlags Flags() const { return flags; }
    bool GetFlag(LexerFlags flag) const { return (flags & flag) != LexerFlags::none; }
    void SetFlag(LexerFlags flag) { flags = flags | flag; }
    void ResetFlag(LexerFlags flag) { flags = flags & ~flag; }
protected:
    Lexeme lexeme;
    int32_t line;
    virtual int GetCommentTokenId() const { return -1; }
private:
    std::u32string content;
    std::string fileName;
    int fileIndex;
    KeywordMap* keywordMap;
    const char32_t* start;
    const char32_t* end;
    const char32_t* pos;
    std::vector<Token> tokens;
    std::vector<Token>::iterator current;
    std::vector<std::unique_ptr<std::exception>> errors;
    std::vector<int> syncTokens;
    ParsingLog* log;
    bool countLines;
    char32_t separatorChar;
    LexerFlags flags;
    std::set<int> blockCommentStates;
    int commentTokenId;
    void NextToken();
};

SOULNG_LEXER_API std::u32string GetErrorLines(const char32_t* start, const char32_t* end, const Span& externalSpan);
SOULNG_LEXER_API void GetColumns(const char32_t* start, const char32_t* end, const Span& externalSpan, int32_t& startCol, int32_t& endCol);
SOULNG_LEXER_API void WriteBeginRuleToLog(Lexer& lexer, const std::u32string& ruleName);
SOULNG_LEXER_API void WriteSuccessToLog(Lexer& lexer, const Span& matchSpan, const std::u32string& ruleName);
SOULNG_LEXER_API void WriteFailureToLog(Lexer& lexer, const std::u32string& ruleName);

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_LEXER_INCLUDED
