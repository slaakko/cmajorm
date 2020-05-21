#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP
#include <sngjson/json/JsonApi.hpp>
#include <soulng/util/Json.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonParser.parser' using soulng parser generator spg version 3.0.0

class JsonLexer;

struct SNGJSON_JSON_API JsonParser
{
    static std::unique_ptr<soulng::util::JsonValue> Parse(JsonLexer& lexer);
    static soulng::parser::Match Value(JsonLexer& lexer);
    static soulng::parser::Match Object(JsonLexer& lexer);
    static soulng::parser::Match Field(JsonLexer& lexer, soulng::util::JsonObject* o);
    static soulng::parser::Match Array(JsonLexer& lexer);
};

#endif // JSONPARSER_HPP
