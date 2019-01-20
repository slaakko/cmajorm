// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_PARSING_MATCH_INCLUDED
#define CMAJOR_PARSING_MATCH_INCLUDED

namespace cmajor { namespace parsing {

class Match
{
public:
    Match(bool hit_, int length_): hit(hit_), length(length_) {}
    bool Hit() const { return hit; }
    int Length() const { return length; }
    void Concatenate(const Match& that)
    {
        length += that.length;
    }
    static Match Empty() 
    {
        return Match(true, 0);
    }
    static Match One()
    {
        return Match(true, 1);
    }
    static Match Nothing()
    {
        return Match(false, -1);
    }
private:
    bool hit;
    int length;
};

} } // namespace cmajor::parsing

#endif // CMAJOR_PARSING_MATCH_INCLUDED

