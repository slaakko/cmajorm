#ifndef LibraryFile_hpp_5581
#define LibraryFile_hpp_5581

#include <cmajor/parsing/Grammar.hpp>
#include <cmajor/parsing/Keyword.hpp>
#include <cmajor/parsing/ParsingDomain.hpp>

namespace cmajor { namespace syntax {

class LibraryFile : public cmajor::parsing::Grammar
{
public:
    static LibraryFile* Create();
    static LibraryFile* Create(cmajor::parsing::ParsingDomain* parsingDomain);
    void Parse(const char32_t* start, const char32_t* end, int fileIndex, const std::string& fileName, cmajor::parsing::ParsingDomain* parsingDomain);
private:
    LibraryFile(cmajor::parsing::ParsingDomain* parsingDomain_);
    void CreateRules() override;
    void GetReferencedGrammars() override;
    class LibraryFileRule;
    class NamespaceContentRule;
    class NamespaceRule;
    class GrammarRule;
    class GrammarContentRule;
    class RuleRule;
};

} } // namespace cmajor.syntax

#endif // LibraryFile_hpp_5581
