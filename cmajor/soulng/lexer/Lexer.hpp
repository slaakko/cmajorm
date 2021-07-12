// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef SOULNG_LEXER_LEXER_INCLUDED
#define SOULNG_LEXER_LEXER_INCLUDED
#include <soulng/lexer/Keyword.hpp>
#include <soulng/lexer/Span.hpp>
#include <soulng/lexer/SourcePos.hpp>
#include <soulng/lexer/ParsingLog.hpp>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <stdint.h>

namespace soulng { namespace lexer {

enum class LexerFlags : int
{
    none = 0, synchronize = 1 << 0, synchronized = 1 << 1, synchronizedAtLeastOnce = 1 << 2, cursorSeen = 1 << 3, recordedParse = 1 << 4, farthestError = 1 << 5, gcc = 1 << 6, lcc = 1 << 7
};

inline LexerFlags operator|(LexerFlags left, LexerFlags right)
{
    return static_cast<LexerFlags>(static_cast<int>(left) | static_cast<int>(right));
}

inline LexerFlags operator&(LexerFlags left, LexerFlags right)
{
    return static_cast<LexerFlags>(static_cast<int>(left) & static_cast<int>(right));
}

inline LexerFlags operator~(LexerFlags flag)
{
    return static_cast<LexerFlags>(~static_cast<int>(flag));
}

struct SOULNG_LEXER_API LexerPosPair
{
    LexerPosPair() : start(-1), end(-1) {}
    LexerPosPair(int64_t start_, int64_t end_) { start = start_; end = end_; }
    bool IsValid() const { return start != -1 && end != -1; }
    int64_t start;
    int64_t end;
};

struct SOULNG_LEXER_API LexerState
{
    LexerState();
    Token token;
    int line;
    Lexeme lexeme;
    const char32_t* pos;
    std::vector<Token> tokens;
    LexerFlags flags;
    LexerPosPair recordedPosPair;
    int64_t farthestPos;
    std::vector<int> ruleContext;
    std::vector<int> farthestRuleContext;
    int64_t currentPos;
};

SOULNG_LEXER_API inline int GetLine(int64_t pos)
{
    return pos >> 32;
}

struct SOULNG_LEXER_API SourceInfo
{
    SourceInfo();
    std::string fileName;
    int lineNumber;
    std::string sourceLine;
};

class SOULNG_LEXER_API LineMapper
{
public:
    virtual ~LineMapper();
    virtual SourceInfo GetSourceInfo(const SourcePos& sourcePos) = 0;
};

class SOULNG_LEXER_API Lexer
{
public:
    Lexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    Lexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    virtual ~Lexer();
    int operator*() const { return current->id; }
    void operator++();
    int64_t GetPos() const;
    void SetPos(int64_t pos);
    SourcePos GetSourcePos(int64_t pos) const;
    int64_t FarthestPos() const { return farthestPos; }
    virtual int NextState(int state, char32_t c);
    void SetKeywordMap(KeywordMap* keywordMap_) { keywordMap = keywordMap_; }
    KeywordMap* GetKeywordMap() { return keywordMap; }
    int GetKeywordToken(const Lexeme& lexeme) const;
    void Retract() { token.match.end = pos; }
    const std::string& FileName() const { return fileName; }
    std::string MappedFileName(const SourcePos& sourcePos) const;
    std::string MappedFileLine(const SourcePos& sourcePos) const;
    int MappedLineNumber(const SourcePos& sourcePos) const;
    Span GetSpan() const { return Span(fileIndex, line, static_cast<int32_t>(GetPos())); }
    void ConvertExternal(Span& span);
    Token GetToken(int64_t pos) const;
    void SetTokens(const std::vector<Token>& tokens_);
    void SetLine(int line_) { line = line_; }
    void SetCountLines(bool countLines_) { countLines = countLines_; }
    Token token;
    std::u32string GetMatch(const Span& span) const;
    std::u32string GetMatch(int64_t pos) const;
    std::u32string ErrorLines(const Token& token) const;
    std::string MappedErrorLines(const Token& token) const;
    std::u32string ErrorLines(const Span& span) const;
    void GetColumns(const Span& span, int32_t& startCol, int32_t& endCol) const;
    void ThrowExpectationFailure(const Span& span, const std::u32string& name);
    std::string GetFarthestError() const;
    void ThrowFarthestError();
    void AddError(const Span& span, const std::u32string& name);
    const std::vector<std::exception>& Errors() const { return errors; }
    const char32_t* Start() const { return start; }
    const char32_t* End() const { return end; }
    const char32_t* Pos() const { return pos; }
    void SetPos(const char32_t* pos_) { pos = pos_; }
    void SetLog(ParsingLog* log_) { log = log_; }
    ParsingLog* Log() const { return log; }
    std::u32string RestOfLine(int maxLineLength);
    void SetSeparatorChar(char32_t separatorChar_) { separatorChar = separatorChar_; }
    TokenLine TokenizeLine(const std::u32string& line, int lineNumber, int startState);
    void SetSyncTokens(const std::vector<int>& syncTokens_);
    bool Synchronize();
    LexerState GetState() const;
    void SetState(const LexerState& state);
    void PushState();
    void PopState();
    void BeginRecordedParse(const LexerPosPair& recordedPosPair_);
    void EndRecordedParse();
    void PushRule(int ruleId);
    void PopRule();
    const std::vector<int>& RuleContext() const { return ruleContext; }
    const std::vector<int>& FarthestRuleContext() const { return farthestRuleContext; }
    const std::vector<int>& CursorRuleContext() const { return cursorRuleContext; }
    void SetGlobalRuleContext();
    void SetLocalRuleContext();
    void ResetGlobalRuleContext();
    void SetRuleNameVecPtr(std::vector<const char*>* ruleNameVecPtr_) { ruleNameVecPtr = ruleNameVecPtr_; }
    std::string GetParserStateStr() const;
    void SetBlockCommentStates(const std::set<int>& blockCommentStates_);
    const std::set<int>& BlockCommentStates() const;
    void SetCommentTokenId(int commentTokenId_) { commentTokenId = commentTokenId_; }
    LexerFlags Flags() const { return flags; }
    bool GetFlag(LexerFlags flag) const { return (flags & flag) != LexerFlags::none; }
    void SetFlag(LexerFlags flag) { flags = flags | flag; }
    void ResetFlag(LexerFlags flag) { flags = flags & ~flag; }
    int32_t line;
    Lexeme lexeme;
    std::vector<Token>::iterator Current() { return current; }
    std::vector<Token>& Tokens() { return tokens; }
    LineMapper* GetLineMapper() { return lineMapper; }
    void SetLineMapper(LineMapper* lineMapper_) { lineMapper = lineMapper_; }
    virtual int GetCommentTokenId() const { return -1; }
    bool Recovered() const { return recovered; }
    void SetRecovered() { recovered = true; }
    void ResetRecovered() { recovered = false; }
private:
    std::u32string content;
    std::string fileName;
    int fileIndex;
    KeywordMap* keywordMap;
    const char32_t* start;
    const char32_t* end;
    const char32_t* pos;
    std::vector<const char32_t*> lineStarts;
    std::vector<Token> tokens;
    std::vector<Token>::iterator current;
    std::vector<std::exception> errors;
    std::vector<int> syncTokens;
    ParsingLog* log;
    bool countLines;
    char32_t separatorChar;
    LexerFlags flags;
    LexerPosPair recordedPosPair;
    std::stack<LexerState> stateStack;
    int64_t farthestPos;
    std::vector<int> ruleContext;
    std::vector<int> farthestRuleContext;
    std::vector<int> cursorRuleContext;
    std::vector<const char*>* ruleNameVecPtr;
    LineMapper* lineMapper;
    std::set<int> blockCommentStates;
    int commentTokenId;
    bool recovered;
    void NextToken();
    void CalculateLineStarts();
};

struct SOULNG_LEXER_API RuleGuard
{
    RuleGuard(Lexer& lexer_, int ruleId_) : lexer(lexer_)
    {
        lexer.PushRule(ruleId_);
        if (lexer.GetFlag(LexerFlags::cursorSeen))
        {
            lexer.SetGlobalRuleContext();
        }
    }
    ~RuleGuard()
    {
        lexer.PopRule();
    }
    Lexer& lexer;
};

SOULNG_LEXER_API std::u32string GetErrorLines(const char32_t* start, const char32_t* end, const Span& externalSpan);
SOULNG_LEXER_API void GetColumns(const char32_t* start, const char32_t* end, const Span& externalSpan, int32_t& startCol, int32_t& endCol);
SOULNG_LEXER_API void WriteBeginRuleToLog(Lexer& lexer, const std::u32string& ruleName);
SOULNG_LEXER_API void WriteSuccessToLog(Lexer& lexer, const Span& matchSpan, const std::u32string& ruleName);
SOULNG_LEXER_API void WriteFailureToLog(Lexer& lexer, const std::u32string& ruleName);

} } // namespace soulng::lexer

#endif // SOULNG_LEXER_LEXER_INCLUDED
