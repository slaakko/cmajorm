// =================================
// Copyright (c) 2022 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <system-x/sxbs/tokenizer.hpp>
#include <soulng/util/Util.hpp>

namespace sxbs {

Tokenizer::Tokenizer(const std::string& messageStr) : pos(0), farthestPos(0)
{
    words = soulng::util::Split(messageStr, ' ');
    tokenMap["build"] = BUILD;
    tokenMap["line"] = LINE;
    tokenMap["exit"] = EXIT;
    tokenMap["stop"] = STOP;
    tokenMap["server"] = SERVER;
    tokenMap["request"] = REQUEST;
    tokenMap["reply"] = REPLY;
    tokenMap["debug"] = DEBUG;
    tokenMap["release"] = RELEASE;
    tokenMap["error"] = ERROR;
}

int32_t Tokenizer::operator*() const
{
    if (pos >= words.size())
    {
        return END;
    }
    else
    {
        auto it = tokenMap.find(words[pos]);
        if (it != tokenMap.cend())
        {
            return it->second;
        }
        else
        {
            return ANY;
        }
    }
}

void Tokenizer::operator++()
{
    if (pos > farthestPos)
    {
        farthestPos = pos;
    }
    ++pos;
}

std::string Tokenizer::GetWord(int32_t pos) const
{
    if (pos >= words.size())
    {
        return std::string();
    }
    else
    {
        return words[pos];
    }
}

std::string Tokenizer::ErrorLines() const
{
    std::string errorLines;
    bool first = true;
    for (const std::string& word : words)
    {
        if (first)
        {
            first = false;
        }
        else
        {
            errorLines.append(1, ' ');
        }
        errorLines.append(word);
    }
    errorLines.append("\n");
    std::string s;
    for (int i = 0; i < farthestPos; ++i)
    {
        if (i > 0)
        {
            s.append(1, ' ');
        }
        s.append(' ', words[i].length());
    }
    errorLines.append(s).append(1, '^').append("\n");
    return errorLines;
}

} // sxbs
