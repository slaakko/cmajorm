#ifndef TraceLexer_HPP
#define TraceLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/cmtrace/TraceLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <soulng/lexer/Lexer.hpp>

class TraceLexer : public soulng::lexer::Lexer
{
public:
    TraceLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    TraceLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
