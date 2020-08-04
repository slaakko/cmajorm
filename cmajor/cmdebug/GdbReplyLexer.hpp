#ifndef GdbReplyLexer_HPP
#define GdbReplyLexer_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/cmdebug/GdbReply.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/cmdebug/DebugApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class DEBUG_API GdbReplyLexer : public soulng::lexer::Lexer
{
public:
    GdbReplyLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    GdbReplyLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif