#ifndef XmlSerLexer_HPP
#define XmlSerLexer_HPP

// this file has been automatically generated from 'C:/work/soulng/sngxml/serialization/XmlSerLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngxml/serialization/XmlSerApi.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGXML_SERIALIZATION_API XmlSerLexer : public soulng::lexer::Lexer
{
public:
    XmlSerLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    XmlSerLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
    bool parsingIncludeDeclaration;
private:
    int GetTokenId(int statementIndex);
};
#endif
