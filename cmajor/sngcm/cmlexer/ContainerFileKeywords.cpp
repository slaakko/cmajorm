
// this file has been automatically generated from 'D:/work/soulng-project/sngcm/cmlexer/ContainerFileLexer.lexer' using soulng lexer generator slg version 3.0.0

#include <sngcm/cmlexer/ContainerFileKeywords.hpp>
#include <sngcm/cmlexer/ContainerFileTokens.hpp>

namespace ContainerFileKeywords
{
    using namespace ContainerFileTokens;

    Keyword keywords[] =
    {
        {U"project", PROJECT}, 
        {U"solution", SOLUTION}, 
        {U"reference", REFERENCE}, 
        {U"source", SOURCE}, 
        {U"text", TEXT}, 
        {U"target", TARGET}, 
        {U"program", PROGRAM}, 
        {U"winapp", WINAPP}, 
        {U"library", LIBRARY}, 
        {U"winlib", WINLIB}, 
        {U"unitTest", UNITTEST}, 
        {U"activeProject", ACTIVEPROJECT},
        {nullptr, -1}
    };

    KeywordMap keywordMap(keywords);

    KeywordMap* GetKeywordMap()
    {
        return &keywordMap;
    }
}
