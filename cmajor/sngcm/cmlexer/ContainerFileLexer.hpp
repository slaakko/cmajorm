#ifndef ContainerFileLexer_HPP
#define ContainerFileLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 4.0.0

#include <sngcm/cmlexer/CmajorLexerApi.hpp>
#include <sngcm/cmlexer/TokenValueParsers.hpp>
#include <soulng/lexer/Lexer.hpp>

class SNGCM_LEXER_API ContainerFileLexer : public soulng::lexer::Lexer
{
public:
    ContainerFileLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    ContainerFileLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
    std::string filePath;
private:
    int GetTokenId(int statementIndex);
};
#endif
