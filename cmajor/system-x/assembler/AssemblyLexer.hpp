#ifndef AssemblyLexer_HPP
#define AssemblyLexer_HPP

// this file has been automatically generated from 'C:/work/cmajorm/cmajor/system-x/assembler/AssemblyLexer.lexer' using soulng lexer generator slg version 3.1.0

#include <system-x/assembler/api.hpp>
#include <soulng/lexer/Lexer.hpp>

class CMSX_ASSEMBLER_API AssemblyLexer : public soulng::lexer::Lexer
{
public:
    AssemblyLexer(const std::u32string& content_, const std::string& fileName_, int fileIndex_);
    AssemblyLexer(const char32_t* start_, const char32_t* end_, const std::string& fileName_, int fileIndex_);
    int NextState(int state, char32_t c) override;
private:
    int GetTokenId(int statementIndex);
};
#endif
