#ifndef RexLexer_HPP
#define RexLexer_HPP

// this file has been automatically generated from 'D:/work/soulng-project/soulng/rex/RexLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <soulng/rex/RexApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SOULNG_REX_API RexLexer : public soulng::lexer::Lexer
{
public:
    RexLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    RexLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
