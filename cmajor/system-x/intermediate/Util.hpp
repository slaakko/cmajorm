// =================================
// Copyright (c) 2021 Seppo Laakko
// Distributed under the MIT license
// =================================

#ifndef CMSX_INTERMEDIATE_UTIL_INCLUDED
#define CMSX_INTERMEDIATE_UTIL_INCLUDED
#include <system-x/intermediate/Api.hpp>
#include <algorithm>
#include <vector>

namespace cmsx::intermediate {

class Function;

template<typename PtrT>
CMSX_INTERMEDIATE_API inline void AddPtrToSet(PtrT* ptr, std::vector<PtrT*>& set)
{
    if (std::find(set.cbegin(), set.cend(), ptr) == set.cend())
    {
        set.push_back(ptr);
    }
}

template<typename PtrT>
CMSX_INTERMEDIATE_API bool RemovePtrFromSet(PtrT* ptr, std::vector<PtrT*>& set)
{
    auto it = std::remove(set.begin(), set.end(), ptr);
    if (it == set.cend())
    {
        return false;
    }
    else
    {
        set.erase(it, set.end());
        return true;
    }
}

template<typename PtrT>
CMSX_INTERMEDIATE_API inline bool PtrInSet(PtrT* ptr, const std::vector<PtrT*>& set)
{
    return std::find(set.cbegin(), set.cend(), ptr) != set.cend();
}

} // cmsx::intermediate

#endif // CMSX_INTERMEDIATE_UTIL_INCLUDED
