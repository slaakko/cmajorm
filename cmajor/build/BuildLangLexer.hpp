#ifndef BuildLangLexer_HPP
#define BuildLangLexer_HPP

// this file has been automatically generated from 'D:/work/cmajorm/cmajor/build/BuildLang.lexer' using soulng lexer generator slg version 3.0.0

#include <cmajor/build/BuildApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class BUILD_API BuildLangLexer : public soulng::lexer::Lexer
{
public:
    BuildLangLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    BuildLangLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
