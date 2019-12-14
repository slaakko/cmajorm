#ifndef TokenLexer_HPP
#define TokenLexer_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/TokenLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGCM_LEXER_API TokenLexer : public soulng::lexer::Lexer
{
public:
    TokenLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    TokenLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
    bool inBlockComment;
private:
    int GetTokenId(int statementIndex);
};
#endif
