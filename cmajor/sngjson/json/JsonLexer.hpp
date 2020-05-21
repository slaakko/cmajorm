#ifndef JsonLexer_HPP
#define JsonLexer_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGJSON_JSON_API JsonLexer : public soulng::lexer::Lexer
{
public:
    JsonLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    JsonLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
