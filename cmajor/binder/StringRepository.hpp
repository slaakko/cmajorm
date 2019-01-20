// =================================
// Copyright (c) 2019 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMAJOR_BINDER_STRING_REPOSITORY_INCLUDED
#define CMAJOR_BINDER_STRING_REPOSITORY_INCLUDED
#include <cmajor/util/Error.hpp>
#include <unordered_map>
#include <vector>

namespace cmajor { namespace binder {

template<class StringT, class CharPtrT>
class StringRepository
{
public:
    int Install(const StringT& str);
    const StringT& GetString(int id) const;
    CharPtrT CharPtr(int id) const;
private:
    std::unordered_map<StringT, int> stringMap;
    std::vector<StringT> strings;
};

template<class StringT, class CharPtrT>
int StringRepository<StringT, CharPtrT>::Install(const StringT& str)
{
    auto it = stringMap.find(str);
    if (it != stringMap.cend())
    {
        return it->second;
    }
    else
    {
        int id = strings.size();
        stringMap[str] = id;
        strings.push_back(str);
        return id;
    }
}

template<class StringT, class CharPtrT>
const StringT& StringRepository<StringT, CharPtrT>::GetString(int id) const
{
    Assert(id >= 0 && id < strings.size(), "invalid string id");
    return strings[id];
}

template<class StringT, class CharPtrT>
CharPtrT StringRepository<StringT, CharPtrT>::CharPtr(int id) const
{
    Assert(id >= 0 && id < strings.size(), "invalid string id");
    return reinterpret_cast<CharPtrT>(strings[id].c_str());
}

} } // namespace cmajor::binder

#endif // CMAJOR_BINDER_STRING_REPOSITORY_INCLUDED
