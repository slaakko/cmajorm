// =================================
// Copyright (c) 2020 Seppo Laakko
// Distributed under the MIT license
// =================================

#include <cmajor/binder/UuidRepository.hpp>

namespace cmajor { namespace binder {

int UuidRepository::Install(const boost::uuids::uuid& uuid)
{
    auto it = uuidMap.find(uuid);
    if (it != uuidMap.cend())
    {
        return it->second;
    }
    else
    {
        int id = uuids.size();
        uuidMap[uuid] = id;
        uuids.push_back(uuid);
        return id;
    }
}

const boost::uuids::uuid& UuidRepository::GetUuid(int id) const
{
    Assert(id >= 0 && id < uuids.size(), "invalid uuid id");
    return uuids[id];
}

} } // namespace cmajor::binder
