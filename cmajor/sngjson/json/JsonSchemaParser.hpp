#ifndef JSONSCHEMAPARSER_HPP
#define JSONSCHEMAPARSER_HPP
#include <sngjson/json/JsonApi.hpp>
#include <sngjson/json/JsonSchema.hpp>
#include <sngjson/json/JsonSchemaLexer.hpp>
#include <soulng/lexer/Token.hpp>
#include <soulng/parser/Match.hpp>
#include <soulng/parser/Value.hpp>

// this file has been automatically generated from 'D:/work/soulng-project/sngjson/json/JsonSchemaParser.parser' using soulng parser generator spg version 3.0.0

class JsonSchemaLexer;

struct SNGJSON_JSON_API JsonSchemaParser
{
    static void Parse(JsonSchemaLexer& lexer, sngjson::json::Context* context);
    static soulng::parser::Match JsonSchemaFile(JsonSchemaLexer& lexer, sngjson::json::Context* context);
    static soulng::parser::Match IncludeDirectives(JsonSchemaLexer& lexer, sngjson::json::Context* context);
    static soulng::parser::Match StructDeclaration(JsonSchemaLexer& lexer, sngjson::json::Context* context);
    static soulng::parser::Match Api(JsonSchemaLexer& lexer);
    static soulng::parser::Match FieldDeclarations(JsonSchemaLexer& lexer, sngjson::json::Context* context, sngjson::json::StructType* s);
    static soulng::parser::Match FieldDeclaration(JsonSchemaLexer& lexer, sngjson::json::Context* context, sngjson::json::StructType* s);
    static soulng::parser::Match TypeName(JsonSchemaLexer& lexer, sngjson::json::Context* context);
    static soulng::parser::Match Type(JsonSchemaLexer& lexer, sngjson::json::Context* context);
};

#endif // JSONSCHEMAPARSER_HPP
