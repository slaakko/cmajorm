#ifndef JsonSchemaLexer_HPP
#define JsonSchemaLexer_HPP

// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonSchemaLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngjson/json/JsonApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGJSON_JSON_API JsonSchemaLexer : public soulng::lexer::Lexer
{
public:
    JsonSchemaLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    JsonSchemaLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
    bool parsingIncludeDeclaration;
private:
    int GetTokenId(int statementIndex);
};
#endif
